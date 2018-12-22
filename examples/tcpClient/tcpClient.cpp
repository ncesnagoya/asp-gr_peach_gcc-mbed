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

#include <tcpClient.h>

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

#define SERVER "www.google.com"
#define HTTP_REQ "GET / HTTP/1.0\r\nhost: www.google.com\r\n\r\n"
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
        /* the message is not able to send, or error trying */
        syslog(LOG_NOTICE, "Write error\n");
        return EXIT_FAILURE;
    }
    syslog(LOG_NOTICE, "Recieved:\n");
    while ((ret = sock->receive(rcvBuff, sizeof(rcvBuff)-1)) > 0)  {
        rcvBuff[ret] = '\0';
        printf("%s", rcvBuff);
    }

    return ret;
}

void
tcpClient_main(intptr_t exinf) {

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

	syslog(LOG_NOTICE, "MAC Address is %s\r\n", network.getMACAddress());
    syslog(LOG_NOTICE, "IP Address is %s\r\n", network.getIPAddress());
    syslog(LOG_NOTICE, "NetMask is %s\r\n", network.getNetworkMask());
    syslog(LOG_NOTICE, "Gateway Address is %s\r\n", network.getGateway());
    syslog(LOG_NOTICE, "Network Setup OK...\r\n");

    while(i < 5000){
    	i++;
		while (socket.connect(SERVER, HTTP_PORT) < 0) {
			syslog(LOG_NOTICE, "Unable to connect to (%s) on port (%d)\n", SERVER, HTTP_PORT);
			wait(1.0);
		}
		ClientGreet(&socket);
		syslog(LOG_NOTICE, "end%d:\n",i);
		socket.close();
    }
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
