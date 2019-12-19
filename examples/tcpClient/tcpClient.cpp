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
#define USE_DHCP               (1)                 /* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
  #define IP_ADDRESS           ("192.168.3.2")     /* IP address      */
  #define SUBNET_MASK          ("255.255.255.0")   /* Subnet mask     */
  #define DEFAULT_GATEWAY      ("192.168.3.1")     /* Default gateway */
#endif

#define NETWORK_TYPE			(0)					/* Select  0(EthernetInterface) or 1(GR_PEACH_WlanBP3595) */
#if (NETWORK_TYPE == 1)
	#define WLAN_SSID			("ssid")	/* SSID */
	#define WLAN_PSK			("pass")		/* PSK(Pre-Shared Key) */

	#define WLAN_SECURITY		NSAPI_SECURITY_WPA2	/* NSAPI_SECURITY_NONE, NSAPI_SECURITY_WEP, NSAPI_SECURITY_WPA or NSAPI_SECURITY_WPA2 */
#endif

#if (NETWORK_TYPE == 0)
  #include "EthernetInterface.h"
  EthernetInterface network;
#elif (NETWORK_TYPE == 1)
  #include "GR_PEACH_WlanBP3595.h"
  GR_PEACH_WlanBP3595 network;
  DigitalOut usb1en(P3_8);
#else
  #error NETWORK_TYPE error
#endif /* NETWORK_TYPE */

//#define SERVER "www.wolfssl.com"
//#define HTTP_REQ "GET /wolfSSL/Home.html HTTP/1.0\r\nhost: www.wolfssl.com\r\n\r\n"

//#define SERVER "os.mbed.com"
//#define HTTP_REQ "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\nhost: os.mbed.com\r\n\r\n"

#define SERVER "192.168.137.1"
#define HTTP_REQ "GET /iisstart.htm HTTP/1.0\r\nhost: 192.168.137.1r\n\r\n"
#define HTTP_PORT 80

/*
 *  clients initial contact with server. Socket to connect to: sock
 */
int ClientGreet(TCPSocketConnection *sock)
{
   	#define MAXDATASIZE (1024*4)
    char       rcvBuff[MAXDATASIZE] = {0};
    int        ret ;

    if (sock->send((char *)HTTP_REQ, strlen(HTTP_REQ)) < 0)  {
        syslog(LOG_EMERG, "Write error");
        return EXIT_FAILURE;		
    }
    syslog(LOG_NOTICE, "Received:");
    while ((ret = sock->receive(rcvBuff, sizeof(rcvBuff)-1)) > 0)  {
        rcvBuff[ret] = '\0';
        syslog(LOG_NOTICE, "%s", rcvBuff);
    }

    return ret;
}

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

	dly_tsk(1000);
	
    syslog(LOG_NOTICE, "MAC Address is %s", network.getMACAddress());
    syslog(LOG_NOTICE, "IP Address is %s", network.getIPAddress());
    syslog(LOG_NOTICE, "NetMask is %s", network.getNetworkMask());
    syslog(LOG_NOTICE, "Gateway Address is %s", network.getGateway());
    syslog(LOG_NOTICE, "Network Setup OK...");
	
    while (socket.connect(SERVER, HTTP_PORT) < 0) {
        syslog(LOG_EMERG, "Unable to connect to (%s) on port (%d)", SERVER, HTTP_PORT);
        dly_tsk(1000);
    }
    ClientGreet(&socket);
    socket.close();
    syslog(LOG_NOTICE, "program end");
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
