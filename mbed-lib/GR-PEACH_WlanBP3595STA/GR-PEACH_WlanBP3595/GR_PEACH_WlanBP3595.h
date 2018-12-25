/* GR_PEACH_WlanBP3595.h */
/* Copyright (C) 2016 Grape Systems, Inc. */
/* The base file is EthernetInterface.h. */

/* EthernetInterface.h */
/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GR_PEACH_WLANBP3595_H_
#define GR_PEACH_WLANBP3595_H_

#if !defined(TARGET_RZ_A1H)
#error The GR_PEACH_WlanBP3595 is not supported on this target
#endif

#include "rtos.h"
#include "lwip/netif.h"

/** Enum of WiFi encryption types
 *
 *  The security type specifies a particular security to use when
 *  connected to a WiFi network
 *
 *  @enum nsapi_protocol_t
 */
enum nsapi_security_t {
    NSAPI_SECURITY_NONE = 0,   /*!< open access point */
    NSAPI_SECURITY_WEP,        /*!< phrase conforms to WEP */
    NSAPI_SECURITY_WPA,        /*!< phrase conforms to WPA */
    NSAPI_SECURITY_WPA2,       /*!< phrase conforms to WPA2 */
};

 /** Interface using Wlan to connect to an IP-based network
 *
 */
class GR_PEACH_WlanBP3595 {
public:
    /** Initialize the interface with DHCP.
     * Initialize the interface and configure it to use DHCP (no connection at this point).
     * @return 0 on success, a negative number on failure
     */
    static int init(); //With DHCP

    /** Initialize the interface with a static IP address.
     * Initialize the interface and configure it with the following static configuration (no connection at this point).
     * @param ip the IP address to use
     * @param mask the IP address mask
     * @param gateway the gateway to use
     * @return 0 on success, a negative number on failure
     */
    static int init(const char* ip, const char* mask, const char* gateway);

    /** Connect
     * Bring the interface up, start DHCP if needed.
     * @param ssid Name of the network to connect to
     * @param pass Security passphrase to connect to the network
     * @param security Type of encryption to connect with
     * @param timeout_ms  timeout in ms (default: (15)s).
     * @return 0 on success, a negative number on failure
     */
    static int connect(
        const char *ssid,
        const char *pass,
        nsapi_security_t security = NSAPI_SECURITY_WPA2,
        unsigned int timeout_ms=1800000);

    /** Disconnect
     * Bring the interface down
     * @return 0 on success, a negative number on failure
     */
    static int disconnect();

    /** Get the MAC address of your Wlan interface
     * @return a pointer to a string containing the MAC address
     */
    static char* getMACAddress();

    /** Get the IP address of your Wlan interface
     * @return a pointer to a string containing the IP address
     */
    static char* getIPAddress();

    /** Get the Gateway address of your Wlan interface
     * @return a pointer to a string containing the Gateway address
     */
    static char* getGateway();

    /** Get the Network mask of your Wlan interface
     * @return a pointer to a string containing the Network mask
     */
    static char* getNetworkMask();

    /** Execute WID function
    * Please see "WID" of document "Software development specs of BP3595" for the details. "https://developer.mbed.org/teams/Rohm/wiki/Datasheet"
    * @param ulFunc A function number
    * @param pvParam The data buffer (the parameter)
    * @return 0 on success, "The others" on failure
    */
    static int WlanIoctl(uint32_t ulFunc, void *pvParam);

    /** Set WLAN Information callback function
    * @param fptr WLAN Information callback function
    */
    static void setWlanCbFunction(void(*fptr)(uint8_t ucType, uint16_t usWid, uint16_t usSize, uint8_t *pucData));

};

#include "TCPSocketConnection.h"
#include "TCPSocketServer.h"

#include "Endpoint.h"
#include "UDPSocket.h"

#endif /* GR_PEACH_WLANBP3595_H_ */
