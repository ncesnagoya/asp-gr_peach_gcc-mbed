/****************************************************************************/
/* Copyright (c) 2015 GRAPE SYSTEMS INC.                                    */
/*                                                                          */
/* Licensed under the Apache License, Version 2.0 (the "License");          */
/* you may not use this file except in compliance with the License.         */
/* You may obtain a copy of the License at                                  */
/*                                                                          */
/*     http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                          */
/* Unless required by applicable law or agreed to in writing, software      */
/* distributed under the License is distributed on an "AS IS" BASIS,        */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/* See the License for the specific language governing permissions and      */
/* limitations under the License.                                           */
/*                                                                          */
/* [Additional license terms]                                               */
/*                                                                          */
/*     This library is free for non-commercial purposes.                    */
/*     To use this commercially, contact us at                              */
/*                                                                          */
/*         sales@info.grape.co.jp                                           */
/*                                                                          */
/****************************************************************************/

/** @file
 *
 * WlanBP3595 library.
 * This library only works with GR-PEACH.
 *
 * @note
 *      Please refer to ROHM's "Software Development Specs" for the information of WID.
 */

#ifndef _WLANBP3595_H_
#define _WLANBP3595_H_

#include "rza1_bp3595_emac.h"
#include "lwip/opt.h"
#include "lwip/pbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**** CONSTANT DEFINES **************************************************************************/

/** @name Status of WlanBP3595
 *
 */
/*@{*/
#define WLAN_BP3595_NONE    0   /*!< BP3595 is not connected, or the firmware is not downloaded */
#define WLAN_BP3595_FW_DL   1   /*!< The firmware is being downloaded */
#define WLAN_BP3595_START   2   /*!< The firmware was downloaded and is working */
/*@}*/

/**** FUNCTION PROTOTYPE & MACRO FUNCTION  ******************************************************/

/** Initialize WlanBP3595
 *
 * @note
 *      After executing this function, please wait until the state becomes WLAN_BP3595_START.
 *      You can get the status by WlanBP3595_GetWlanSts().
 *
 * @param   pfnWlanInfoNotication   The information callback function
 *                                  - 1st param : The message type ('I' or 'N')
 *                                  - 2nd param : WID number
 *                                  - 3rd param : The data size
 *                                  - 4th param : The data buffer
 * @retval  0                       Success
 * @retval  "The others"            Failure
 */
int WlanBP3595_Init(void (*pfnWlanInfoNotication)(uint8_t, uint16_t, uint16_t, uint8_t *));

/** Get the status of WlanBP3595
 *
 * @retval  WLAN_BP3595_NONE        BP3595 is not connected, or the firmware is not downloaded
 * @retval  WLAN_BP3595_FW_DL       The firmware is being downloaded
 * @retval  WLAN_BP3595_START       The firmware was downloaded and is working
 */
uint32_t WlanBP3595_GetWlanSts(void);

/** Execute WID function
 *
 * @param   ulFunc                  A function number
 * @param   pvParam                 The data buffer (the parameter)
 * @retval  0                       Success
 * @retval  "The others"            Failure
 */
int WlanBP3595_Ioctl(uint32_t ulFunc, void *pvParam);

/** Output an ethernet frame
 *
 * @note
 *      This function is used in EthernetInterface driver.
 *
 * @param   p                       A pbuf to output
 * @retval  0                       Success
 * @retval  "The others"            Failure
 */
int WlanBP3595_Output(struct pbuf *p);

/** Enable receiving
 *
 * @note
 *      This function is used in EthernetInterface driver.
 */
void WlanBP3595_RecvEnable(void);

/** Disable receiving
 *
 * @note
 *      This function is used in EthernetInterface driver.
 */
void WlanBP3595_RecvDisable(void);

/** @name Macro Function
 *
 */
/*@{*/

/** Notify the EthernetInterface driver that WLAN was connected
 *
 * When WLAN was connected,
 * please call this macro function from your application program.
 * The types of the argument and the return value are "void".
 * You can know the change of the connection state by the information callback.
 *
 * @note
 *      When this function is called before executing EthernetInterface::init(), this function does nothing.
 *      If you use WlanBP3595_Disconnected() in a different thread, please perform exclusive control.
 */
#define WlanBP3595_Connected()      rza1_bp3595_connected()

/** Notify the EthernetInterface driver that WLAN was disconnected
 *
 * When WLAN was disconnected,
 * please call this macro function from your application program.
 * The types of the argument and the return value are "void".
 * You can know the change of the connection state by the information callback.
 *
 * @note
 *      When this function is called before executing EthernetInterface::init(), this function does nothing.
 *      If you use WlanBP3595_Connected() in a different thread, please perform exclusive control.
 */
#define WlanBP3595_Disconnected()   rza1_bp3595_disconnected()

/*@}*/

/**** TYPE DEFINES ******************************************************************************/
typedef unsigned int    grp_ui;
typedef signed int      grp_si;

typedef unsigned long   grp_u32;
typedef signed long     grp_s32;

typedef unsigned short  grp_u16;
typedef signed short    grp_s16;

typedef unsigned char   grp_u8;
typedef signed char     grp_s8;

/**** SIZE DEFINES ******************************************************************************/
#define     GRP_WLD_SSID_SIZE                               32
#define     GRP_WLD_BSSID_SIZE                              6
#define     GRP_WLD_MAC_ADDRESS_SIZE                        6
#define     GRP_WLD_WEP_KEY_SIZE_128                        13

/**** IOCTL FUNCTION PARAMETER DEFINES **********************************************************/
#define     GRP_WLD_IOCTL_SITE_SURVEY_RESULT_MAX            8

/**** IOCTL FUNCTION BASE NUMBER DEFINES ********************************************************/
#define     GRP_WLD_IOCTL_FUNCTION_KIND_MASK                0xc000
#define     GRP_WLD_IOCTL_GET_FUNCTION                      0x8000
#define     GRP_WLD_IOCTL_SET_FUNCTION                      0x4000

/**** IOCTL FUNCTION NUMBER DEFINES *************************************************************/

/** @name Function code
 *
 */
/*@{*/
#define     GRP_WLD_IOCTL_GET_BSS_TYPE                      GRP_WLD_IOCTL_GET_FUNCTION + 0x0001 /*!< The function code of WID_BSS_TYPE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_BSS_TYPE                      GRP_WLD_IOCTL_SET_FUNCTION + 0x0001 /*!< The function code of WID_BSS_TYPE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_CHANNEL                       GRP_WLD_IOCTL_GET_FUNCTION + 0x0002 /*!< The function code of WID_CURRENT_CHANNEL for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_CHANNEL                       GRP_WLD_IOCTL_SET_FUNCTION + 0x0002 /*!< The function code of WID_CURRENT_CHANNEL for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_ENABLE_CHANNEL                GRP_WLD_IOCTL_GET_FUNCTION + 0x0003 /*!< The function code of WID_ENABLE_CHANNEL for getting. The parameter type is "grp_u32". */
#define     GRP_WLD_IOCTL_SET_ENABLE_CHANNEL                GRP_WLD_IOCTL_SET_FUNCTION + 0x0003 /*!< The function code of WID_ENABLE_CHANNEL for setting. The parameter type is "grp_u32". */
#define     GRP_WLD_IOCTL_GET_11I_MODE                      GRP_WLD_IOCTL_GET_FUNCTION + 0x0004 /*!< The function code of WID_11I_MODE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_11I_MODE                      GRP_WLD_IOCTL_SET_FUNCTION + 0x0004 /*!< The function code of WID_11I_MODE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_11I_PSK                       GRP_WLD_IOCTL_GET_FUNCTION + 0x0005 /*!< The function code of WID_11I_PSK for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_SET_11I_PSK                       GRP_WLD_IOCTL_SET_FUNCTION + 0x0005 /*!< The function code of WID_11I_PSK for setting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_SCAN_FILTER                   GRP_WLD_IOCTL_GET_FUNCTION + 0x0006 /*!< The function code of WID_SCAN_FILTER for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_SCAN_FILTER                   GRP_WLD_IOCTL_SET_FUNCTION + 0x0006 /*!< The function code of WID_SCAN_FILTER for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_RSSI                          GRP_WLD_IOCTL_GET_FUNCTION + 0x000f /*!< The function code of WID_RSSI for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_POWER_MANAGEMENT              GRP_WLD_IOCTL_GET_FUNCTION + 0x0010
#define     GRP_WLD_IOCTL_SET_POWER_MANAGEMENT              GRP_WLD_IOCTL_SET_FUNCTION + 0x0010
#define     GRP_WLD_IOCTL_GET_POWER_SAVE                    GRP_WLD_IOCTL_GET_FUNCTION + 0x0011
#define     GRP_WLD_IOCTL_SET_POWER_SAVE                    GRP_WLD_IOCTL_SET_FUNCTION + 0x0011
#define     GRP_WLD_IOCTL_GET_MAC_STATUS                    GRP_WLD_IOCTL_GET_FUNCTION + 0x0012 /*!< The function code of WID_CURRENT_MAC_STATUS for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_SSID                          GRP_WLD_IOCTL_GET_FUNCTION + 0x0013 /*!< The function code of WID_SSID for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_SET_SSID                          GRP_WLD_IOCTL_SET_FUNCTION + 0x0013 /*!< The function code of WID_SSID for setting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_BCAST_SSID                    GRP_WLD_IOCTL_GET_FUNCTION + 0x0014 /*!< The function code of WID_BCAST_SSID for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_BCAST_SSID                    GRP_WLD_IOCTL_SET_FUNCTION + 0x0014 /*!< The function code of WID_BCAST_SSID for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_BSSID                         GRP_WLD_IOCTL_GET_FUNCTION + 0x0015 /*!< The function code of WID_BSSID for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_WEP_KEY                       GRP_WLD_IOCTL_GET_FUNCTION + 0x0016 /*!< The function code of WID_WEP_KEY_VALUE for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_SET_WEP_KEY                       GRP_WLD_IOCTL_SET_FUNCTION + 0x0016 /*!< The function code of WID_WEP_KEY_VALUE for setting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_WEP_KEY_INDEX                 GRP_WLD_IOCTL_GET_FUNCTION + 0x0017 /*!< The function code of WID_KEY_ID for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_WEP_KEY_INDEX                 GRP_WLD_IOCTL_SET_FUNCTION + 0x0017 /*!< The function code of WID_KEY_ID for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_LISTEN_INTERVAL               GRP_WLD_IOCTL_GET_FUNCTION + 0x0019 /*!< The function code of WID_LISTEN_INTERVAL for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_LISTEN_INTERVAL               GRP_WLD_IOCTL_SET_FUNCTION + 0x0019 /*!< The function code of WID_LISTEN_INTERVAL for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_SITE_SURVEY_RESULT            GRP_WLD_IOCTL_GET_FUNCTION + 0x001a /*!< The function code of WID_SITE_SURVEY_RESULTS for getting. The parameter type is "grp_wld_site_survey_result_array". */
#define     GRP_WLD_IOCTL_GET_DEVICE_STRINGS                GRP_WLD_IOCTL_GET_FUNCTION + 0x001b /*!< The function code of WID_FIRMWARE_VERSION & WID_SERIAL_NUMBER for getting. The parameter type is "grp_wld_dev_strings". */
#define     GRP_WLD_IOCTL_GET_MAC_ADDRESS                   GRP_WLD_IOCTL_GET_FUNCTION + 0x001c /*!< The function code of WID_MAC_ADDR for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_SCAN_TYPE                     GRP_WLD_IOCTL_GET_FUNCTION + 0x0022 /*!< The function code of WID_SCAN_TYPE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_SCAN_TYPE                     GRP_WLD_IOCTL_SET_FUNCTION + 0x0022 /*!< The function code of WID_SCAN_TYPE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_AUTH_TYPE                     GRP_WLD_IOCTL_GET_FUNCTION + 0x0026 /*!< The function code of WID_AUTH_TYPE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_AUTH_TYPE                     GRP_WLD_IOCTL_SET_FUNCTION + 0x0026 /*!< The function code of WID_AUTH_TYPE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_SEL_ANTENNA                   GRP_WLD_IOCTL_GET_FUNCTION + 0x0030 /*!< The function code of WID_ANTENNA_SELECTION for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_SEL_ANTENNA                   GRP_WLD_IOCTL_SET_FUNCTION + 0x0030 /*!< The function code of WID_ANTENNA_SELECTION for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_WPS_START                     GRP_WLD_IOCTL_GET_FUNCTION + 0x0035 /*!< The function code of WID_WPS_START for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_WPS_START                     GRP_WLD_IOCTL_SET_FUNCTION + 0x0035 /*!< The function code of WID_WPS_START for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_WPS_DEV_MODE                  GRP_WLD_IOCTL_GET_FUNCTION + 0x0036 /*!< The function code of WID_WPS_DEV_MODE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_WPS_DEV_MODE                  GRP_WLD_IOCTL_SET_FUNCTION + 0x0036 /*!< The function code of WID_WPS_DEV_MODE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_BEACON_INTERVAL               GRP_WLD_IOCTL_GET_FUNCTION + 0x004a /*!< The function code of WID_BEACON_INTERVAL for getting. The parameter type is "grp_u16". */
#define     GRP_WLD_IOCTL_SET_BEACON_INTERVAL               GRP_WLD_IOCTL_SET_FUNCTION + 0x004a /*!< The function code of WID_BEACON_INTERVAL for setting. The parameter type is "grp_u16". */
#define     GRP_WLD_IOCTL_GET_WPS_PIN                       GRP_WLD_IOCTL_GET_FUNCTION + 0x0064 /*!< The function code of WID_WPS_PIN for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_SET_WPS_PIN                       GRP_WLD_IOCTL_SET_FUNCTION + 0x0064 /*!< The function code of WID_WPS_PIN for setting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_WPS_CRED_LIST                 GRP_WLD_IOCTL_GET_FUNCTION + 0x006a /*!< The function code of WID_WPS_CRED_LIST for getting. The parameter type is "grp_wld_wps_cred_list". */
#define     GRP_WLD_IOCTL_SET_WPS_CRED_LIST                 GRP_WLD_IOCTL_SET_FUNCTION + 0x006a /*!< The function code of WID_WPS_CRED_LIST for setting. The parameter type is "grp_wld_wps_cred_list". */
#define     GRP_WLD_IOCTL_GET_USB_IN_XFER_MODE              GRP_WLD_IOCTL_GET_FUNCTION + 0x006b /*!< The function code of WID_USB_IN_XFER_MODE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_USB_IN_XFER_MODE              GRP_WLD_IOCTL_SET_FUNCTION + 0x006b /*!< The function code of WID_USB_IN_XFER_MODE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_TX_POWER_RATE                 GRP_WLD_IOCTL_GET_FUNCTION + 0x006c /*!< The function code of WID_TX_POWER_RATE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_TX_POWER_RATE                 GRP_WLD_IOCTL_SET_FUNCTION + 0x006c /*!< The function code of WID_TX_POWER_RATE for setting. The parameter type is "grp_u8". */

#define     GRP_WLD_IOCTL_GET_VSIE_FRAME                    GRP_WLD_IOCTL_GET_FUNCTION + 0x006d /*!< The function code of WID_VSIE_FRAME for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_VSIE_FRAME                    GRP_WLD_IOCTL_SET_FUNCTION + 0x006d /*!< The function code of WID_VSIE_FRAME for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_VSIE_INFO_ENABLE              GRP_WLD_IOCTL_GET_FUNCTION + 0x006e /*!< The function code of WID_VSIE_INFO_ENABLE for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_VSIE_INFO_ENABLE              GRP_WLD_IOCTL_SET_FUNCTION + 0x006e /*!< The function code of WID_VSIE_INFO_ENABLE for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_VSIE_RX_OUI                   GRP_WLD_IOCTL_GET_FUNCTION + 0x006f /*!< The function code of WID_VSIE_RX_OUI for getting. The parameter type is "grp_u32". */
#define     GRP_WLD_IOCTL_SET_VSIE_RX_OUI                   GRP_WLD_IOCTL_SET_FUNCTION + 0x006f /*!< The function code of WID_VSIE_RX_OUI for setting. The parameter type is "grp_u32". */
#define     GRP_WLD_IOCTL_GET_VSIE_TX_DATA                  GRP_WLD_IOCTL_GET_FUNCTION + 0x0070 /*!< The function code of WID_VSIE_TX_DATA for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_SET_VSIE_TX_DATA                  GRP_WLD_IOCTL_SET_FUNCTION + 0x0070 /*!< The function code of WID_VSIE_TX_DATA for setting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_VSIE_RX_DATA                  GRP_WLD_IOCTL_GET_FUNCTION + 0x0071 /*!< The function code of WID_VSIE_RX_DATA for getting. The parameter type is "grp_wld_byte_array". */
#define     GRP_WLD_IOCTL_GET_DTIM_PERIOD                   GRP_WLD_IOCTL_GET_FUNCTION + 0x0072 /*!< The function code of WID_DTIM_PERIOD for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_DTIM_PERIOD                   GRP_WLD_IOCTL_SET_FUNCTION + 0x0072 /*!< The function code of WID_DTIM_PERIOD for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_DISCONNECT                    GRP_WLD_IOCTL_SET_FUNCTION + 0x0073 /*!< The function code of WID_DISCONNECT for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_REKEY_PERIOD                  GRP_WLD_IOCTL_GET_FUNCTION + 0x0074 /*!< The function code of WID_REKEY_PERIOD for getting. The parameter type is "grp_u32". */
#define     GRP_WLD_IOCTL_SET_REKEY_PERIOD                  GRP_WLD_IOCTL_SET_FUNCTION + 0x0074 /*!< The function code of WID_REKEY_PERIOD for setting. The parameter type is "grp_u32". */
#define     GRP_WLD_IOCTL_GET_WPS_PASS_ID                   GRP_WLD_IOCTL_GET_FUNCTION + 0x0075 /*!< The function code of WID_WPS_PASS_ID for getting. The parameter type is "grp_u16". */
#define     GRP_WLD_IOCTL_SET_WPS_PASS_ID                   GRP_WLD_IOCTL_SET_FUNCTION + 0x0075 /*!< The function code of WID_WPS_PASS_ID for setting. The parameter type is "grp_u16". */
#define     GRP_WLD_IOCTL_GET_REKEY_POLICY                  GRP_WLD_IOCTL_GET_FUNCTION + 0x0076 /*!< The function code of WID_REKEY_POLICY for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_REKEY_POLICY                  GRP_WLD_IOCTL_SET_FUNCTION + 0x0076 /*!< The function code of WID_REKEY_POLICY for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_CONNECTED_STA_LIST            GRP_WLD_IOCTL_GET_FUNCTION + 0x0077 /*!< The function code of WID_CONNECTED_STA_LIST for getting. The parameter type is "grp_wld_connected_sta_list_info". */

#define     GRP_WLD_IOCTL_SET_JOIN_REQUEST                  GRP_WLD_IOCTL_SET_FUNCTION + 0x0902 /*!< The function code of WID_JOIN_REQ for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_START_SCAN_REQ                GRP_WLD_IOCTL_SET_FUNCTION + 0x0905 /*!< The function code of WID_START_SCAN_REQ for setting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_CHECK_SCAN_END                GRP_WLD_IOCTL_GET_FUNCTION + 0x0906 /*!< The function code of WID_START_SCAN_REQ for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_GET_SITE_SURVEY                   GRP_WLD_IOCTL_GET_FUNCTION + 0x0907 /*!< The function code of WID_SITE_SURVEY for getting. The parameter type is "grp_u8". */
#define     GRP_WLD_IOCTL_SET_SITE_SURVEY                   GRP_WLD_IOCTL_SET_FUNCTION + 0x0907 /*!< The function code of WID_SITE_SURVEY for setting. The parameter type is "grp_u8". */
/*@}*/

/**** STRUCTURE DEFINES  *************************************************************************/

/** Byte array structure
 *
 */
typedef struct GRP_WLD_BYTE_ARRAY_T {                           /* Byte array structure */
    grp_u32                             ulSize;                 /*!< Array size */
    grp_u8                              *pucData;               /*!< Byte array buffer pointer */
} grp_wld_byte_array;

/** Device strings structure
 *
 */
typedef struct GRP_WLD_DEV_STRINGS_T {                          /* Device strings structure  */
    grp_u32                             ulFirmwareVersionSize;  /*!< buffer size */
    grp_u8                              *pucFirmwareVersionBuf; /*!< buffer pointer */
    grp_u32                             ulSerialNumberSize;     /*!< buffer size */
    grp_u8                              *pucSerialNumberBuf;    /*!< buffer pointer */
} grp_wld_dev_strings;

/** Site survey result structure
 *
 */
typedef struct GRP_WLD_SITE_SURVEY_RESULT_T {                   /* Site survey result structure */
    grp_u8                              aucSsid[GRP_WLD_SSID_SIZE + 1]; /*!< SSID */
    grp_u8                              ucBssType;              /*!< BSS type */
    grp_u8                              ucChannel;              /*!< Channel */
    grp_u8                              ucSecurity;             /*!< Security */
    grp_u8                              aucBssid[GRP_WLD_BSSID_SIZE];   /*!< BSSID */
    grp_u8                              ucRxPower;              /*!< Rx power */
} grp_wld_site_survey_result;

/** Site survey result array structure
 *
 */
typedef struct GRP_WLD_SITE_SURVEY_RESULT_ARRAY_T {             /* Site survey result array structure */
    grp_si                              iCnt;                   /*!< Valid data count */
    grp_wld_site_survey_result          atResult[GRP_WLD_IOCTL_SITE_SURVEY_RESULT_MAX]; /*!< result */
} grp_wld_site_survey_result_array;

/** WPS cred list structure
 *
 */
typedef struct GRP_WLD_WPS_CRED_T {                             /* WPS cred list structure */
    grp_u8                              aucSSID[GRP_WLD_SSID_SIZE+1];   /*!< SSID */
    grp_u8                              ucAuthType;             /*!< auth type */
    grp_u8                              uc11imode;              /*!< 11i mode */
    grp_u8                              ucWEPKeyId;             /*!< WEP key ID */
    grp_u8                              aucWEPKey[GRP_WLD_WEP_KEY_SIZE_128+1];  /*!< WEP key value */
    grp_u8                              ucWEPKeyLen;            /*!< WEP key length */
    grp_u8                              aucPSK[64+1];           /*!< PSK */
    grp_u8                              ucPSKLen;               /*!< PSK Length */
    grp_u8                              aucBSSID[GRP_WLD_BSSID_SIZE];   /*!< BSSID */
} grp_wld_wps_cred;

/** WPS cred list structure
 *
 */
typedef struct GRP_WLD_WPS_CRED_LIST_T {                        /* WPS cred list structure */
    grp_si                              iNum;                   /*!< Cred info number */
    grp_wld_wps_cred                    *ptCredBuf;             /*!< Cred info buffer */
} grp_wld_wps_cred_list;

/** STA join information
 *
 */
typedef struct GRP_WLD_STA_JOIN_INFO_T {                        /* STA join information */
    grp_u8                              ucAID;                  /*!< Association ID lower 8bit */
    grp_u8                              ucSTAType;              /*!< STA type */
    grp_u8                              uc11gInfo;              /*!< 11g information */
    grp_u8                              ucSecInfo;              /*!< Security information */
    grp_u16                             usWMMInfo;              /*!< WMM information */
    grp_u16                             us11nInfo;              /*!< 11N information */
    grp_u8                              aucMacAddress[GRP_WLD_MAC_ADDRESS_SIZE];
                                                                /*!< MAC address */
} grp_wld_sta_join_info;

/** Connected Sta list information
 *
 */
typedef struct GRP_WLD_CONNECTED_STA_LIST_INFO_T {              /* Connected Sta list information */
    grp_si                              iNum;                   /*!< Connected sta info number */
    grp_wld_sta_join_info               *ptCnctStaBuf;          /*!< Connected sta info buffer */
} grp_wld_connected_sta_list_info;

#ifdef __cplusplus
}
#endif

#endif /* _WLANBP3595_H_ */
