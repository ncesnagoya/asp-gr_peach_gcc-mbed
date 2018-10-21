#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

/* GR-PEACH Sketch Template V1.00 */
#include <Arduino.h>

/*
#include "EthernetInterface.h"
#include "HTTPServer.h"
#include "mbed_rpc.h"
#include "RomRamFileSystem.h"
#include "file_table.h"        //Binary data of web pages
#include "i2c_setting.h"
*/

#include "syssvc/logtask.h"
#include "arduino_app.h"

/*
 *  Error check of service calls
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/* This is a sample application program for GR-PEACH_WlanBP3595AP library. */
/* GR-PEACH_WlanBP3595AP library only works with GR-PEACH. */
/* This sample works as TCP socket sever, and this program sends a message */
/* when a connection is accepted. */

/*
This works with the following library.
  mbed-rtos : revision 115
*/

#include "mbed.h"
#include "rtos.h"
#include "GR_PEACH_WlanBP3595.h"

/* Please change the following macro definition to your setting. */
#define WLAN_SSID               ("GR-PEACH_WlanTest")           // SSID
#define WLAN_PSK                ("WlanBP3595_PreSharedKey")     // PSK(Pre-Shared Key)
#define SERVER_IP               ("192.168.1.200")               // Server IP address
#define SERVER_PORT             (50000)                         // TCP server socket port number
#define SUBNET_MASK             ("255.255.255.0")               // Subnet mask
#define DEFAULT_GATEWAY         ("192.168.1.1")                 // Default gateway
#define SEND_MESSAGE            ("Hello, world!\r\n")           // Send-message

static void _wlan_inf_callback(uint8_t ucType, uint16_t usWid, uint16_t usSize, uint8_t *pucData);

DigitalOut  red_led(LED1);              // On: error
DigitalOut  green_led(LED2);            // On: WLAN has been connected

/*
 *  Main task
 */
void
main_task(intptr_t exinf) {
    GR_PEACH_WlanBP3595   wlan;
    TCPSocketServer     server;
    TCPSocketConnection connection;
    int                 ret;

    wlan.setWlanCbFunction(_wlan_inf_callback);

    /* Initialize GR_PEACH_WlanBP3595 */
    ret = wlan.init(SERVER_IP, SUBNET_MASK, DEFAULT_GATEWAY);
    if (ret != 0) {
        /* error */
        red_led = 1;
        while (1) { Thread::wait(1000); }
    }

    /* Connect(GR_PEACH_WlanBP3595) */
    ret = wlan.connect(WLAN_SSID, WLAN_PSK);
    if (ret != 0) {
        /* error */
        red_led = 1;
        while (1) { Thread::wait(1000); }
    }

    /* Bind and listen */
    server.bind(SERVER_PORT);
    server.listen();

    /* Loop */
    while (1) {
        /* Accept */
        server.accept(connection);
        printf("Connection from: %s\n", connection.get_address());

        /* Send a message */
        connection.send_all((char *)SEND_MESSAGE, sizeof(SEND_MESSAGE)-1);

        /* Close */
        connection.close();
    }
}

/** WLAN Information callback function
 *
 */
static void _wlan_inf_callback(uint8_t ucType, uint16_t usWid, uint16_t usSize, uint8_t *pucData)
{
    if (ucType == 'I') {
        if (usWid == 0x0005) {    // WID_STATUS
            if (pucData[0] == 0x01) {     // CONNECTED
                green_led = 1;
            } else {
                green_led = 0;
            }
        }
    }
}

