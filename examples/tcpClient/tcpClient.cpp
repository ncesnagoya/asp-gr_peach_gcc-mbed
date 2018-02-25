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
#include    <tcpClient.h>

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

//#define SERVER "www.wolfssl.com"
//#define HTTP_REQ "GET /wolfSSL/Home.html HTTP/1.0\r\nhost: www.wolfssl.com\r\n\r\n"
//#define SERVER "www.google.com"
//#define HTTP_REQ "GET / HTTP/1.0\r\nhost: www.google.com\r\n\r\n"
//#define HTTP_PORT 80

#define SERVER "192.168.0.3"
#define HTTP_REQ "GET /iisstart.htm HTTP/1.0\r\nhost: 192.168.0.3\r\n\r\n"
#define HTTPS_PORT 443

/*
 *  clients initial contact with server. Socket to connect to: sock
 */
int ClientGreet(TCPSocketConnection *sock)
{
   	#define MAXDATASIZE (1024*4)
    char       rcvBuff[MAXDATASIZE] = {0};
    int        ret ;

    if (sock->send((char *)HTTP_REQ, strlen(HTTP_REQ)) < 0)  {
        ret = wolfSSL_get_error(ssl, 0);
        syslog(LOG_EMERG, "Write error[%d]:%s\n", ret, wc_GetErrorString(ret));
        return EXIT_FAILURE;		
    }
    syslog(LOG_NOTICE, "Recieved:\n");
    while ((ret = sock->receive(rcvBuff, sizeof(rcvBuff)-1)) > 0)  {
        rcvBuff[ret] = '\0';
        syslog(LOG_NOTICE, "%s", rcvBuff);
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

void
tcpClient_main(intptr_t exinf) {

	EthernetInterface network;
	TCPSocketConnection socket;

	/* syslogの設定 */
    SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));

	syslog(LOG_NOTICE, "tcpClient:");
    syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);
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

    syslog(LOG_NOTICE, "MAC Address is %s", network.getMACAddress());
    syslog(LOG_NOTICE, "IP Address is %s", network.getIPAddress());
    syslog(LOG_NOTICE, "NetMask is %s", network.getNetworkMask());
    syslog(LOG_NOTICE, "Gateway Address is %s", network.getGateway());
    syslog(LOG_NOTICE, "Network Setup OK...");
	
    while (socket.connect(SERVER, HTTP_PORT) < 0) {
        syslog(LOG_NOTICE, "Unable to connect to (%s) on port (%d)\n", SERVER, HTTP_PORT);
        wait(1.0);
    }

    ClientGreet(&socket);
    socket.close();
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
