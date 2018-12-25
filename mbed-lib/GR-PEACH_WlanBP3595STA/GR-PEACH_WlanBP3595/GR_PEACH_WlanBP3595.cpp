/* GR_PEACH_WlanBP3595.cpp */
/* Copyright (C) 2016 Grape Systems, Inc. */
/* The base file is EthernetInterface.cpp. */

/* EthernetInterface.cpp */
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
#include "GR_PEACH_WlanBP3595.h"
#include "GR_PEACH_WlanBP3595_BssType.h"
#include "WlanBP3595.h"

#include "lwip/inet.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "wifi_arch.h"
#include "lwip/tcpip.h"

#include "mbed.h"

/* TCP/IP and Network Interface Initialisation */
static struct netif netif;

static char mac_addr[19];
static char ip_addr[17] = "\0";
static char gateway[17] = "\0";
static char networkmask[17] = "\0";
static bool use_dhcp = false;

static Semaphore tcpip_inited(0);
static Semaphore netif_linked(0);
static Semaphore netif_up(0);
static void (*_wlan_callback_func)(uint8_t ucType, uint16_t usWid, uint16_t usSize, uint8_t *pucData) = NULL;

static void tcpip_init_done(void *arg) {
    tcpip_inited.release();
}

static void netif_link_callback(struct netif *netif) {
    if (netif_is_link_up(netif)) {
        netif_linked.release();
    }
}

static void netif_status_callback(struct netif *netif) {
    if (netif_is_up(netif)) {
        strcpy(ip_addr, inet_ntoa(netif->ip_addr));
        strcpy(gateway, inet_ntoa(netif->gw));
        strcpy(networkmask, inet_ntoa(netif->netmask));
        netif_up.release();
    }
}

static void init_netif(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw) {
    tcpip_init(tcpip_init_done, NULL);
    tcpip_inited.wait();
    
    memset((void*) &netif, 0, sizeof(netif));
    netif_add(&netif, ipaddr, netmask, gw, NULL, wifi_arch_enetif_init, tcpip_input);
    netif_set_default(&netif);
    
    netif_set_link_callback  (&netif, netif_link_callback);
    netif_set_status_callback(&netif, netif_status_callback);
}

static void set_mac_address(void) {
    snprintf(mac_addr, 19, "%02x:%02x:%02x:%02x:%02x:%02x", netif.hwaddr[0], netif.hwaddr[1],
             netif.hwaddr[2], netif.hwaddr[3], netif.hwaddr[4], netif.hwaddr[5]);
}

static void _wlan_inf_callback(uint8_t ucType, uint16_t usWid, uint16_t usSize, uint8_t *pucData) {
    if ((ucType == 'I') && (usWid == 0x0005)) {
        if (pucData[0] == 0x01) {     // CONNECTED
            /* Notify the EthernetInterface driver that WLAN was connected */
            WlanBP3595_Connected();
        } else {
            /* Notify the EthernetInterface driver that WLAN was disconnected */
            WlanBP3595_Disconnected();
        }
    }
    if (_wlan_callback_func != NULL) {
        _wlan_callback_func(ucType, usWid, usSize, pucData);
    }
}

static int _wlan_init() {
    uint32_t status;

    /* Initialize WlanBP3595 */
    if (WlanBP3595_Init(&_wlan_inf_callback) != 0) {
        return -1;
    }

    /* Wait until WLAN_BP3595_START  timeout 60s */
    while (1) {
        Thread::wait(200);
        status = WlanBP3595_GetWlanSts();
        if (status == WLAN_BP3595_START) {
            break;
        }
    }

    return 0;
}

static int _wlan_setting(const char *ssid, const char *pass, nsapi_security_t security)
{
    int     ret;
    grp_u8  ucWidData8;     // 8bit wid data
    grp_wld_byte_array  tBAWidData;     // byte array wid data

    // Set BSS type
    ucWidData8 = BSS_TYPE;
    ret = WlanBP3595_Ioctl(GRP_WLD_IOCTL_SET_BSS_TYPE, &ucWidData8);
    if (ret != 0) {
        return -1;
    }

    // Set SSID
    tBAWidData.pucData = (grp_u8 *)ssid;
    tBAWidData.ulSize  = strlen((char *)tBAWidData.pucData);
    ret = WlanBP3595_Ioctl(GRP_WLD_IOCTL_SET_SSID, &tBAWidData);
    if (ret != 0) {
        return -1;
    }

    if ((security == NSAPI_SECURITY_WPA) || (security == NSAPI_SECURITY_WPA2)) {
        // Set PSK
        tBAWidData.pucData = (grp_u8 *)pass;
        tBAWidData.ulSize  = strlen((char *)tBAWidData.pucData);
        ret = WlanBP3595_Ioctl(GRP_WLD_IOCTL_SET_11I_PSK, &tBAWidData);
        if (ret != 0) {
            return -1;
        }
    }

    // Set 11i mode
    switch (security) {
        case NSAPI_SECURITY_WEP:
            ret = strlen(pass);
            if (ret == 5) {
                ucWidData8 = 0x03;  // WEP64
            } else if (ret == 13) {
                ucWidData8 = 0x07;  // WEP128
            } else {
                return -1;
            }
            break;
        case NSAPI_SECURITY_WPA:
        case NSAPI_SECURITY_WPA2:
            ucWidData8 = 0x79;  // WPA/WPA2 Mixed
            break;
        case NSAPI_SECURITY_NONE:
        default:
            ucWidData8 = 0x00;
            break;
    }
    ret = WlanBP3595_Ioctl(GRP_WLD_IOCTL_SET_11I_MODE, &ucWidData8);
    if (ret != 0) {
        return -1;
    }

    if (security == NSAPI_SECURITY_WEP) {
        // Set WEP KEY
        tBAWidData.pucData = (grp_u8 *)pass;
        tBAWidData.ulSize  = strlen((char *)tBAWidData.pucData);
        ret = WlanBP3595_Ioctl(GRP_WLD_IOCTL_SET_WEP_KEY, &tBAWidData);
        if (ret != 0) {
            return -1;
        }
    }

    return 0;
}

int GR_PEACH_WlanBP3595::init() {
    _wlan_init();
    use_dhcp = true;
    init_netif(NULL, NULL, NULL);
    set_mac_address();
    return 0;
}

int GR_PEACH_WlanBP3595::init(const char* ip, const char* mask, const char* gateway) {
    _wlan_init();
    use_dhcp = false;
    
    strcpy(ip_addr, ip);
    
    ip_addr_t ip_n, mask_n, gateway_n;
    inet_aton(ip, &ip_n);
    inet_aton(mask, &mask_n);
    inet_aton(gateway, &gateway_n);
    init_netif(&ip_n, &mask_n, &gateway_n);
    set_mac_address();
    
    return 0;
}

int GR_PEACH_WlanBP3595::connect(const char *ssid, 
    const char *pass, 
    nsapi_security_t security,
    unsigned int timeout_ms)
{
    _wlan_setting(ssid, pass, security);
    wifi_arch_enable_interrupts();

    int inited;
    if (use_dhcp) {
        dhcp_start(&netif);
        
        // Wait for an IP Address
        // -1: error, 0: timeout
        inited = netif_up.wait(timeout_ms);
    } else {
        netif_set_up(&netif);
        
        // Wait for the link up
        inited = netif_linked.wait(timeout_ms);
    }
    
    return (inited > 0) ? (0) : (-1);
}

int GR_PEACH_WlanBP3595::disconnect() {
    if (use_dhcp) {
        dhcp_release(&netif);
        dhcp_stop(&netif);
    } else {
        netif_set_down(&netif);
    }
    
    wifi_arch_disable_interrupts();
    
    return 0;
}

char* GR_PEACH_WlanBP3595::getMACAddress() {
    return mac_addr;
}

char* GR_PEACH_WlanBP3595::getIPAddress() {
    return ip_addr;
}

char* GR_PEACH_WlanBP3595::getGateway() {
    return gateway;
}

char* GR_PEACH_WlanBP3595::getNetworkMask() {
    return networkmask;
}

int GR_PEACH_WlanBP3595::WlanIoctl(uint32_t ulFunc, void *pvParam) {
    return WlanBP3595_Ioctl(ulFunc, pvParam);
}

void GR_PEACH_WlanBP3595::setWlanCbFunction(void(*fptr)(uint8_t ucType, uint16_t usWid, uint16_t usSize, uint8_t *pucData)) {
    _wlan_callback_func = fptr;
}
