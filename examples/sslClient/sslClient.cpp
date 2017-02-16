#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sil.h"
#include "scif.h"

#include "mbed.h"
#include "EthernetInterface.h"
#include "syssvc/logtask.h"

#include    <wolfssl/ssl.h>          /* wolfSSL security library */
#include    <wolfssl/wolfcrypt/error-crypt.h>
#include    <user_settings.h>
#include    <sslClient.h>

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
#define USE_DHCP               (1)                 /* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
  #define IP_ADDRESS           ("192.168.0.2")     /* IP address      */
  #define SUBNET_MASK          ("255.255.255.0")   /* Subnet mask     */
  #define DEFAULT_GATEWAY      ("192.168.0.3")     /* Default gateway */
#endif

static int SocketReceive(WOLFSSL* ssl, char *buf, int sz, void *sock)
{
    return ((TCPSocketConnection *)sock)->receive(buf, sz) ;
}

static int SocketSend(WOLFSSL* ssl, char *buf, int sz, void *sock)
{
    return ((TCPSocketConnection *)sock)->send(buf, sz);
}

#define SERVER "www.wolfssl.com"
#define HTTP_REQ "GET /wolfSSL/Home.html HTTP/1.0\r\nhost: www.wolfssl.com\r\n\r\n"
#define HTTPS_PORT 443

/*
 *  clients initial contact with server. Socket to connect to: sock
 */
int ClientGreet(WOLFSSL *ssl)
{
   	#define MAXDATASIZE (1024*4)
    char       rcvBuff[MAXDATASIZE] = {0};
    int        ret ;

    if (wolfSSL_write(ssl, HTTP_REQ, strlen(HTTP_REQ)) < 0) {
        /* the message is not able to send, or error trying */
        ret = wolfSSL_get_error(ssl, 0);
        printf("Write error[%d]:%s\n", ret, wc_GetErrorString(ret));
        return EXIT_FAILURE;
    }
    printf("Recieved:\n");
    while ((ret = wolfSSL_read(ssl, rcvBuff, sizeof(rcvBuff)-1)) > 0) {
        rcvBuff[ret] = '\0' ;
        printf("%s", rcvBuff);
    }

    return ret;
}


/*
 * applies TLS 1.2 security layer to data being sent.
 */
#define STATIC_BUFFER
#ifdef  STATIC_BUFFER
static byte memory[1024*256];
static byte memoryIO[1024*72];
#endif

int Security(TCPSocketConnection *socket)
{
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL*     ssl;    /* create WOLFSSL object */
    int          ret = 0;

#ifdef  STATIC_BUFFER
    printf("wolfSSL_CTX_load_static_memory\n");
    /* set up static memory */
    if (wolfSSL_CTX_load_static_memory(&ctx, wolfTLSv1_2_client_method_ex, memory, sizeof(memory),0,1)
            != SSL_SUCCESS){
        printf("unable to load static memory and create ctx");
        return  EXIT_FAILURE;
    }

    /* load in a buffer for IO */
    printf("wolfSSL_CTX_load_static_memory\n");
    if (wolfSSL_CTX_load_static_memory(&ctx, NULL, memoryIO, sizeof(memoryIO),
                                 WOLFMEM_IO_POOL_FIXED | WOLFMEM_TRACK_STATS, 1)
            != SSL_SUCCESS){
        printf("unable to load static memory and create ctx");
	    return  EXIT_FAILURE;
    }
#else
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        printf("wolfSSL_new error.\n");
        return EXIT_FAILURE;
    }
#endif

    wolfSSL_SetIORecv(ctx, SocketReceive) ;
    wolfSSL_SetIOSend(ctx, SocketSend) ;
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);

    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        printf("wolfSSL_new error.\n");
        return EXIT_FAILURE;
    }

    wolfSSL_SetIOReadCtx(ssl, (void *)socket) ;
    wolfSSL_SetIOWriteCtx(ssl, (void *)socket) ;

    ret = wolfSSL_connect(ssl);
    if (ret == SSL_SUCCESS) {
        printf("TLS Connected\n") ;
        ret = ClientGreet(ssl);
    } else {
        ret = wolfSSL_get_error(ssl, 0);
        printf("TLS Connect error[%d], %s\n", ret, wc_GetErrorString(ret));
        return EXIT_FAILURE;
    }
    /* frees all data before client termination */
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    return ret;
}


void
sslClient_main(intptr_t exinf) {
    EthernetInterface network;
    TCPSocketConnection socket;

	  /* syslogの設定 */
    SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));

    syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);
    printf("Network Setting up...\r\n");
    syslog(LOG_NOTICE, "LOG_NOTICE: Network Setting up...");

#if (USE_DHCP == 1)
    if (network.init() != 0) {                             //for DHCP Server
#else
    if (network.init(IP_ADDRESS, SUBNET_MASK, DEFAULT_GATEWAY) != 0) { //for Static IP Address (IPAddress, NetMasks, Gateway)
#endif
		syslog(LOG_NOTICE, "Network Initialize Error");
        return;
    }
    syslog(LOG_NOTICE, "Network was initialized successfully");
    while (network.connect(5000) != 0) {
        syslog(LOG_NOTICE, "LOG_NOTICE: Network Connect Error");
    }

    printf("MAC Address is %s\r\n", network.getMACAddress());
    printf("IP Address is %s\r\n", network.getIPAddress());
    printf("NetMask is %s\r\n", network.getNetworkMask());
    printf("Gateway Address is %s\r\n", network.getGateway());
    printf("Network Setup OK...\r\n");

    while (socket.connect(SERVER, HTTPS_PORT) < 0) {
        printf("Unable to connect to (%s) on port (%d)\n", SERVER, HTTPS_PORT);
        wait(1.0);
    }
    Security(&socket);
    socket.close();
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
