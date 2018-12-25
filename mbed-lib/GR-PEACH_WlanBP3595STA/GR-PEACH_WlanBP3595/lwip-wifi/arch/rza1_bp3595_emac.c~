#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "netif/ppp_oe.h"
#include "mbed_interface.h"

#include <string.h>
#include "rza1_bp3595_emac.h"
#include "WlanBP3595.h"

/* Static variable */
static struct netif * volatile target_netif = NULL;
static volatile int init_sts = 0;           /* 0: not initialized, 1:initialized */
static int          connect_sts = 0;        /* 0: disconnected, 1:connected */

/* Static function  */
static err_t rza1_bp3595_etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr);
static err_t rza1_bp3595_low_level_output(struct netif *netif, struct pbuf *p);

/* This function is called from the receiving thread of WlanBP3595 library. */
void rza1_bp3595_input(void *buff, u16_t recv_size) {
    struct eth_hdr  *ethhdr;
    struct pbuf     *p;

    if (recv_size != 0) {
        p = pbuf_alloc(PBUF_RAW, recv_size, PBUF_RAM);
        if (p != NULL) {
            /* Copy data */
            memcpy(p->payload, buff, recv_size);
            /* Check Ethernet frame type */
            ethhdr = p->payload;
            switch (htons(ethhdr->type)) {
                case ETHTYPE_IP:
                case ETHTYPE_ARP:
#if PPPOE_SUPPORT
                case ETHTYPE_PPPOEDISC:
                case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
                    /* full packet send to tcpip_thread to process */
                    if (target_netif->input(p, target_netif) != ERR_OK) {
                        /* Free buffer */
                        pbuf_free(p);
                    }
                    break;
                default:
                    /* Free buffer */
                    pbuf_free(p);
                    break;
            }
        }
    }
}

void rza1_bp3595_connected(void) {
    /* 0: not initialized, 1:initialized */
    if (init_sts == 1) {
        /* 0: disconnected, 1:connected */
        if (connect_sts == 0) {
            tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_up, (void*)target_netif, 1);
            connect_sts = 1;
        }
    }
}

void rza1_bp3595_disconnected(void) {
    /* 0: not initialized, 1:initialized */
    if (init_sts == 1) {
        /* 0: disconnected, 1:connected */
        if (connect_sts == 1) {
            tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_down, (void*)target_netif, 1);
            connect_sts = 0;
        }
    }
}

static err_t rza1_bp3595_etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr) {
    if (netif->flags & NETIF_FLAG_LINK_UP) {
        return etharp_output(netif, q, ipaddr);
    }

    return ERR_CONN;
}

static err_t rza1_bp3595_low_level_output(struct netif *netif, struct pbuf *p) {
    err_t   err = ERR_MEM;
    int     ret;

    ret = WlanBP3595_Output(p);
    if (ret == 0) {
        err = ERR_OK;
    }

    return err;
}

err_t wifi_arch_enetif_init(struct netif *netif) {
    grp_wld_byte_array  tBAWidData;     /* byte array wid data */
    int                 ret;

    /* Set MAC hardware address */
    tBAWidData.pucData = (grp_u8 *)netif->hwaddr;
    tBAWidData.ulSize  = 6;

    ret = WlanBP3595_Ioctl(GRP_WLD_IOCTL_GET_MAC_ADDRESS, &tBAWidData);
    if (ret != 0) {
        /* error(return a default MAC hardware address) */
        netif->hwaddr[0] = 0x00;
        netif->hwaddr[1] = 0x02;
        netif->hwaddr[2] = 0xF7;
        netif->hwaddr[3] = 0xF0;
        netif->hwaddr[4] = 0x00;
        netif->hwaddr[5] = 0x00;
    }

    /* Set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* Set maximum transfer unit */
    netif->mtu = 1500;

    /* Set device capabilities */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwiprza1";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = 'e';
    netif->name[1] = 'n';

    netif->output     = rza1_bp3595_etharp_output;
    netif->linkoutput = rza1_bp3595_low_level_output;

    target_netif = netif;

    init_sts = 1;   /* 0: not initialized, 1:initialized */

    return ERR_OK;
}

void wifi_arch_enable_interrupts(void) {
    WlanBP3595_RecvEnable();
}

void wifi_arch_disable_interrupts(void) {
    WlanBP3595_RecvDisable();
}

