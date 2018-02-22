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

/*mbed TLS library*/
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/net.h"

#include    <mbedClient.h>
#include    <histogram.h>

#define DEBUG_LEVEL 0
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

#define SVC_PERROR(expr)    svc_perror(__FILE__, __LINE__, #expr, (expr))

/**** User Selection *********/
/** Network setting **/
#define USE_DHCP               (1)                 /* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
  #define IP_ADDRESS           ("192.168.0.2")     /* IP address      */
  #define SUBNET_MASK          ("255.255.255.0")   /* Subnet mask     */
  #define DEFAULT_GATEWAY      ("192.168.0.3")     /* Default gateway */
#endif

static int TlsReceive(void *sock, unsigned char *buf, size_t sz)
{

    return ((TCPSocketConnection *)sock)->receive((char *) buf, (int)sz) ;
}

static int TlsSend(void *sock, const unsigned char *buf, size_t sz)
{
    return ((TCPSocketConnection *)sock)->send((char *) buf, (int)sz);
}

#if DEBUG_LEVEL > 0
static void my_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    const char *p, *basename;
    ((void) level);
    /* Extract basename from file */
    for(p = basename = file; *p != '\0'; p++) {
        if(*p == '/' || *p == '\\') {
            basename = p + 1;
        }
    }
    syslog(LOG_NOTICE, "%s:%04d: |%d| %s", basename, line, level, str);
    syslog_flush();
}

static int my_verify(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    const uint32_t buf_size = 1024;
    char *buf = new char[buf_size];
    (void) data;

    syslog(LOG_NOTICE, "\nVerifying certificate at depth %d:\n", depth);
    mbedtls_x509_crt_info(buf, buf_size - 1, "  ", crt);
    syslog(LOG_NOTICE, "%s", buf);
    syslog_flush();
    if (*flags == 0)
        syslog(LOG_NOTICE, "No verification issue for this certificate\n");
    else
    {
        mbedtls_x509_crt_verify_info(buf, buf_size, "  ! ", *flags);
        syslog(LOG_NOTICE, "%s\n", buf);
    }
    syslog_flush();
    delete[] buf;
    return 0;
}
#endif

#define SERVER "os.mbed.com"
#define HTTP_REQ "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\nhost: os.mbed.com\r\n\r\n"
#define HTTPS_PORT 443

const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n"
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n"
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n"
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n"
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n"
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n"
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n"
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n"
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n"
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n"
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n"
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n"
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n"
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n"
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n"
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n"
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n"
    "-----END CERTIFICATE-----\n";

/*
 *  clients initial contact with server. Socket to connect to: sock
 */
int ClientGreet(mbedtls_ssl_context ssl)
{
    #define MAXDATASIZE (1024*4)
    unsigned char       rcvBuff[MAXDATASIZE] = {0};
    int        ret ;

    while((ret = (mbedtls_ssl_write(&ssl , (unsigned char*)HTTP_REQ, strlen(HTTP_REQ) ))) <= 0){
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE){
            syslog(LOG_NOTICE, "Write error! returned %d\n", ret);
            return EXIT_FAILURE;
        }
    }

    syslog(LOG_NOTICE, "Recieved:\n");

    while ((ret = mbedtls_ssl_read(&ssl, rcvBuff, sizeof(rcvBuff)-1)) >0 ){
        rcvBuff[ret] = '\0';
        syslog(LOG_NOTICE, "%s", rcvBuff);
    }
    return ret;
}


/*
 * applies TLS 1.2 security layer to data being sent.
 */

int Security(TCPSocketConnection *socket)
{
    mbedtls_ssl_context         ssl;
    mbedtls_ssl_config          conf;
    mbedtls_x509_crt            cacert;
    mbedtls_ctr_drbg_context    ctr_drbg;   
    mbedtls_entropy_context     entropy;
    const char *pers = "mbedClient";
    int          ret = 0;

    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &cacert );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );

    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers, strlen( pers ) ) ) != 0 )
        {
    	syslog(LOG_NOTICE, " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
            return ret;
        }

    if ((ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *) SSL_CA_PEM,
                               sizeof (SSL_CA_PEM))) != 0) {
                syslog(LOG_NOTICE, "mbedtls_x509_crt_parse", ret);
                return ret;
            }
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    //mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_NONE );
    if( ( ret = mbedtls_ssl_config_defaults( &conf, MBEDTLS_SSL_IS_CLIENT,
                MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
    	syslog(LOG_NOTICE, " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        return ret;
    }

#if DEBUG_LEVEL > 0
    mbedtls_ssl_conf_verify(&conf, my_verify, NULL);
    mbedtls_ssl_conf_dbg( &conf, my_debug, NULL);
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
   

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 ){
    	syslog(LOG_NOTICE, " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        return ret;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, SERVER ) ) != 0 ){
    	syslog(LOG_NOTICE, " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        return ret;
    }

    mbedtls_ssl_set_bio( &ssl, (void *)socket, TlsSend, TlsReceive, NULL );

    while(( ret = mbedtls_ssl_handshake( &ssl )) != 0 ){
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
        	syslog(LOG_NOTICE, " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            return ret;
        }
    }

    syslog(LOG_NOTICE, "TLS Connected\n") ;
    ret = ClientGreet(ssl);
    
    /* frees all data before client termination */
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_x509_crt_free( &cacert );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    return ret;
}


void
mbedClient_main(intptr_t exinf) {

    EthernetInterface network;
    TCPSocketConnection socket;

    int i = 0;
         /* syslogの設定 */
    SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));

    syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);
    //printf( "Network Setting up...\r\n");
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

    //syslog(LOG_NOTICE, "MAC Address is %s\r\n", network.getMACAddress());
    //syslog(LOG_NOTICE, "IP Address is %s\r\n", network.getIPAddress());
    //syslog(LOG_NOTICE, "NetMask is %s\r\n", network.getNetworkMask());
    //syslog(LOG_NOTICE, "Gateway Address is %s\r\n", network.getGateway());
    //syslog(LOG_NOTICE, "Network Setup OK...\r\n");

    while(i < 10){

    syslog(LOG_NOTICE, "mbedTLS start:\n");
   	i++;
    while (socket.connect(SERVER, HTTPS_PORT) < 0) {
        syslog(LOG_NOTICE, "Unable to connect to (%s) on port (%d)\n", SERVER, HTTPS_PORT);
        wait(1.0);
    }

    Security(&socket);
    syslog(LOG_NOTICE, "end%d:\n",i);
    socket.close();
    }

    /*時間稼ぎ*/
	syslog(LOG_NOTICE, "program end\n");
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
