#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sil.h"
#include "scif.h"
#include "test_lib.h"

#include "mbed.h"
#include "EthernetInterface.h"
#include "syssvc/logtask.h"

#include "lwip/sockets.h"

#include  <wolfssl/ssl.h>          /* wolfSSL security library */
#include  <wolfssl/wolfcrypt/error-crypt.h>
#include  <wolfssl/wolfcrypt/logging.h>

#include "picotls.h"
#include "picotls/wolfcrypt.h"
#include <picotls_settings.h>
#include <picotlsClient.h>
#include <histogram.h>

/*
 *  サービスコールのエラーのログ出力
 */

Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/**** User Selection *********/
/** Network setting **/
#define USE_DHCP               (0)                 /* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
  #define IP_ADDRESS           ("10.0.0.2")     /* IP address      */
  #define SUBNET_MASK          ("255.255.255.0")   /* Subnet mask     */
  #define DEFAULT_GATEWAY      ("10.0.0.3")     /* Default gateway */
#endif

#define SERVER "10.0.0.1"
#define HTTP_REQ "GET /itest.html\r\n"
#define DEFAULT_SNI "www.test.com"

#define HTTPS_PORT "8443"

ptls_session_ticket_t SESSION_TICKET = {{0}, 0, 0};

int ptls_save_ticket_call_back(ptls_save_ticket_t* save_ticket_ctx, ptls_t* tls, ptls_iovec_t input)
{
    int ret = 0;
    const char* sni = ptls_get_server_name(tls);
    const char* alpn = ptls_get_negotiated_protocol(tls);

    memset(SESSION_TICKET.ticket, 0, 2048);
    SESSION_TICKET.ticket_length = 0;

    if (sni != NULL) {
        memcpy(SESSION_TICKET.ticket, input.base, input.len);
        SESSION_TICKET.ticket_length = input.len;
    } else {
        syslog(LOG_INFO, "Received incorrect session resume ticket, sni = %s, alpn = %s, length = %d\n",
            (sni == NULL) ? "NULL" : sni, (alpn == NULL) ? "NULL" : alpn, (int)input.len);
    }

    return ret;
}

int ptls_load_ticket(ptls_context_t *ctx, ptls_handshake_properties_t *hsprop)
{
    static ptls_save_ticket_t st;
    st.cb = ptls_save_ticket_call_back;
    ctx->save_ticket = &st;

    if (SESSION_TICKET.ticket_length == 0){
        hsprop->client.session_ticket = ptls_iovec_init(NULL, 0);
        return -1;
    } else{
        hsprop->client.session_ticket = ptls_iovec_init(SESSION_TICKET.ticket, SESSION_TICKET.ticket_length);
        static size_t max_early_data_size;
        hsprop->client.max_early_data_size = &max_early_data_size;
        return 0;
    }
}

#ifdef NO_FILESYSTEM
static int ptls_init_verify_cert(ptls_context_t *ctx, const void* SSL_CA, int cert_size){

    ptls_openssl_verify_certificate_t* verifier = NULL;
    verifier = (ptls_openssl_verify_certificate_t*)malloc(sizeof(ptls_openssl_verify_certificate_t));
    int ret = 0;

    if (verifier == NULL) {
        ctx->verify_certificate = NULL;
    } else {
        X509_STORE *store = NULL;
        store = X509_STORE_new();
        if (store != NULL){
            X509_LOOKUP *lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
            X509* x509 = NULL;
            int ca_ret = 0;
            x509 = wolfSSL_X509_load_certificate_buffer((const unsigned char*)SSL_CA, cert_size, SSL_FILETYPE_PEM);
            if(x509 == NULL){
                ret = MEMORY_ERROR;
            }
            if((ca_ret = X509_STORE_add_cert(lookup->store, x509)) != 1){
                syslog(LOG_NOTICE, "Cannot load X509 store (%s), ret = %d\n", SSL_CA_ECC_PEM, ca_ret);   
            }
            X509_free(x509);
        }
        ptls_openssl_init_verify_certificate(verifier, store);
        ctx->verify_certificate = &verifier->super;
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        if (store != NULL) {
            X509_STORE_free(store);
        }
#endif
    }
    return ret;
}
#endif

static inline int get_server_address(struct sockaddr *sa, socklen_t *salen, const char *host, const char *port, int family, int type,
                                  int proto)
{
    struct addrinfo hints, *res;
    int err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = proto;
    hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV | AI_PASSIVE;
    if ((err = lwip_getaddrinfo(host, port, &hints, &res)) != 0 || res == NULL) {
        syslog(LOG_EMERG, "failed to resolve address:%s:%s:%d\n", host, port);
        return -1;
    }

    memcpy(sa, res->ai_addr, res->ai_addrlen);
    *salen = res->ai_addrlen;

    lwip_freeaddrinfo(res);
    return 0;
}

#define MAX_DATA_SIZE (1024*4)

static int ptls_client(struct sockaddr *sa, socklen_t salen, ptls_context_t *ctx, const char *server_name,
                      ptls_handshake_properties_t *hsprop, int use_ticket)
{

    int sockfd;
    ptls_buffer_t recvbuf, encbuf, appbuf;
    char bytebuf[MAX_DATA_SIZE];
    enum { IN_HANDSHAKE, IN_1RTT, IN_SHUTDOWN } state = IN_HANDSHAKE;
    int ret = 0;
    ssize_t ioret, roff;
    char* nullbuf = "";
    int i = 0;

    if (use_ticket){
        ret = ptls_load_ticket(ctx, hsprop);
        if (ret != 0){
            syslog(LOG_INFO, "No session ticket is loaded\n");
        }
    }

    wolfSSL_RAND_seed(NULL, 0);
    ptls_t *tls = ptls_new(ctx, 0);
    ptls_buffer_init(&recvbuf, (void *)nullbuf, 0);
    ptls_buffer_init(&encbuf, (void *)nullbuf, 0);
    ptls_buffer_init(&appbuf, (void *)nullbuf, 0);

    ptls_set_server_name(tls, server_name, 0);

    if ((sockfd = lwip_socket(sa->sa_family, SOCK_STREAM, 0)) == 1) {
        syslog(LOG_ERROR, "socket(2) failed");
        return 1;
    }

    if (lwip_connect(sockfd, sa, salen) != 0) {
        syslog(LOG_ERROR, "connect(2) failed");
        return 1;
    }

    if ((ret = ptls_handshake(tls, &encbuf, NULL, NULL, hsprop)) != PTLS_ERROR_IN_PROGRESS) {
        syslog(LOG_ERROR, "ptls_handshake:%d\n", ret);
        ret = 1;
        goto Exit;
    }

    if (hsprop->client.max_early_data_size != NULL){
        ret = ptls_send(tls, &encbuf, HTTP_REQ, strlen(HTTP_REQ));
        if (ret != 0){
            syslog(LOG_ERROR, "Cannot Send Early data:%d\n", ret);
            goto Exit;
        }
    }

    if (!lwip_send(sockfd, encbuf.base, encbuf.off, 0)) {
        ptls_buffer_dispose(&encbuf);
        goto Exit;
    }

    do {
        // read data from socket
        while ((ioret = lwip_recv(sockfd, bytebuf, sizeof(bytebuf), 0)) == -1 && errno == EINTR);
        if (ioret == 0)
            goto Exit;
        // repeatedly call ptls_handshake (and send the output) until handshake
        // completes or when the function consumes all input
        roff = 0;
        size_t consumed = 0;
        do {
            ptls_buffer_init(&encbuf, (void *)nullbuf, 0);
            consumed = ioret - roff;
            ret = ptls_handshake(tls, &encbuf, bytebuf + roff, &consumed, hsprop);
            roff += consumed;
        } while (ret == PTLS_ERROR_IN_PROGRESS && ioret != roff);
        
        while (ret == 0 && (consumed = ioret - roff) > 0){
            ret = ptls_receive(tls, &recvbuf, bytebuf + roff, &consumed);
            roff += consumed;
            if (ret == 0){
                if (recvbuf.off != 0){
                    for (i = 0; i < recvbuf.off; i++){
                        syslog(LOG_INFO, "%c", recvbuf.base[i]);        
                    }
                } 
            } else if (ret != PTLS_ERROR_IN_PROGRESS){
                syslog(LOG_ERROR, "ptls_receive\n");
                goto Exit;
            }
        }

        if ((ret == 0 || ret == PTLS_ERROR_IN_PROGRESS) && encbuf.off != 0) {
            /* Send Finished */
            if (!lwip_send(sockfd, encbuf.base, encbuf.off, 0)) {
                ptls_buffer_dispose(&encbuf);
                goto Exit;
            }
        }
        ptls_buffer_dispose(&encbuf);
    } while (ret == PTLS_ERROR_IN_PROGRESS);

    if (ret == 0) {
        state = IN_1RTT;
    } else {
        // handshake failed
        syslog(LOG_ERROR, "handshake failed\n");
    }

    if (hsprop->client.max_early_data_size == NULL){
        ret = ptls_send(tls, &appbuf, HTTP_REQ, strlen(HTTP_REQ));
        lwip_send(sockfd, appbuf.base, appbuf.off, 0);        

        while (recvbuf.off == 0){
            if (ret == 0) {
                while ((ioret = lwip_recv(sockfd, bytebuf, sizeof(bytebuf), 0)) == -1 && errno == EINTR);
                if (ioret < 0){
                    syslog(LOG_ERROR, "Cannot read data\n");
                    goto Exit;
                }

                size_t offset = 0;

                while (ret == 0 && offset < ioret) {
                    size_t consumed = ioret - offset;
                    ret = ptls_receive(tls, &recvbuf, bytebuf + offset, &consumed);
                    offset += consumed;
                } 
            }
        }
    }

    for (i = 0; i < recvbuf.off; i++){
        syslog(LOG_INFO, "%c", recvbuf.base[i]);        
    }
    syslog(LOG_INFO, "\n");
    syslog_flush();

    /* close the sender side when necessary */
    if (state == IN_1RTT) {
        ptls_buffer_t wbuf;
        uint8_t wbuf_small[32];
        ptls_buffer_init(&wbuf, wbuf_small, sizeof(wbuf_small));
        if ((ret = ptls_send_alert(tls, &wbuf, PTLS_ALERT_LEVEL_WARNING, PTLS_ALERT_CLOSE_NOTIFY)) != 0) {
            syslog(LOG_ERROR, "ptls_send_alert:%d\n", ret);
        }
        if (wbuf.off != 0){
            lwip_send(sockfd, wbuf.base, wbuf.off, 0);
        }
        ptls_buffer_dispose(&wbuf);
        lwip_shutdown(sockfd, SHUT_WR);
        state = IN_SHUTDOWN;
    }

Exit:
    if (sockfd != -1)
        lwip_close(sockfd);
    ptls_buffer_dispose(&recvbuf);
    ptls_buffer_dispose(&encbuf);
    ptls_buffer_dispose(&appbuf);
    ptls_free(tls);
    ptls_openssl_dispose_verify_certificate((ptls_openssl_verify_certificate_t*)ctx->verify_certificate);
    wolfSSL_RAND_Cleanup();
    return ret != 0;
}

void
ptlsClient_main(intptr_t exinf) {
    EthernetInterface network;

    int ret = 0;

    int i = 0;

	/* syslogの設定 */
    SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG)));

    syslog(LOG_NOTICE, "picotlsClient:");
    syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);
    syslog(LOG_NOTICE, "LOG_NOTICE: Network Setting up...\n");

#if (USE_DHCP == 1)
    if (network.init() != 0) {                             //for DHCP Server
        syslog(LOG_ERROR, "Network Initialize Error\n");
        return;
    }
#else
    if (network.init(IP_ADDRESS, SUBNET_MASK, DEFAULT_GATEWAY) != 0) { //for Static IP Address (IPAddress, NetMasks, Gateway)
		syslog(LOG_ERROR, "Network Initialize Error\n");
        return;
    }
#endif

    syslog(LOG_NOTICE, "Network was initialized successfully\n");
    while (network.connect(5000) != 0) {
        syslog(LOG_NOTICE, "LOG_NOTICE: Network Connect Error\n");
    }
    wait(3.0);

    syslog(LOG_NOTICE, "MAC Address is %s\n", network.getMACAddress());
    syslog(LOG_NOTICE, "IP Address is %s\n", network.getIPAddress());
    syslog(LOG_NOTICE, "NetMask is %s\n", network.getNetworkMask());
    syslog(LOG_NOTICE, "Gateway Address is %s\n", network.getGateway());
    syslog(LOG_NOTICE, "Network Setup OK...\n");
    syslog(LOG_NOTICE, "Starting PicoTLS connection to (%s) on port (%d)\n\n", SERVER, HTTPS_PORT);
    syslog_flush();

    ptls_context_t ctx;
    memset (&ctx, 0, sizeof(ctx));
    ctx.random_bytes = ptls_wolfcrypt_random_bytes;
    ctx.get_time = &ptls_get_time;
    ctx.key_exchanges = ptls_wolfcrypt_key_exchanges;
    ctx.cipher_suites = ptls_wolfcrypt_cipher_suites;
    ptls_handshake_properties_t hsprop = {{{{NULL}}}};
    ctx.require_dhe_on_psk = 1;
    struct sockaddr_storage sa;
    socklen_t salen;
    int family = 0;
    int use_ticket = 1;

    ret = ptls_init_verify_cert(&ctx, SSL_CA_ECC_PEM, sizeof(SSL_CA_ECC_PEM));

    if (get_server_address((struct sockaddr *)&sa, &salen, SERVER, HTTPS_PORT, family, SOCK_STREAM, IPPROTO_TCP) != 0){
        syslog(LOG_ERROR, "Address Error\n");
        return;
    }

    ptls_client((struct sockaddr *)&sa, salen, &ctx, SERVER, &hsprop, use_ticket);

}

// set mac address
void mbed_mac_address(char *mac) {
	// PEACH1
    mac[0] = 0x00;
    mac[1] = 0x02;
    mac[2] = 0xF7;
    mac[3] = 0xF0;
    mac[4] = 0x00;
    mac[5] = 0x00;
}

 /*
 *  周期ハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
bool_t led_state = true;
void cyclic_handler(intptr_t exinf)
{
	if (led_state == true) {
		led_state = false;
	} else {
		led_state = true;
	}
	set_led(BLUE_LED, led_state);
}
