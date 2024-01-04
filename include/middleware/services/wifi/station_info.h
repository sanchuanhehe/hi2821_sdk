/**
* @file station_info.h
*
* Copyright (c) CompanyNameMagicTag 2021-2021.All rights reserved. \n
* Description: header file for station information. \n
* Author: CompanyName \n
* History: \n
* 2022-02-22，更新文件注释 \n
* 2021-12-31，初始化该文件 \n
*/

/**
* @defgroup middleware_service_wifi_station_info WiFi Station Info
* @ingroup middleware_service_wifi
* @{
*/

#ifndef SERVICE_WIFI_STATION_INFO_H
#define SERVICE_WIFI_STATION_INFO_H
#include "wifi_device_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  Indicates the rate of STA packets.
 * @else
 * @brief  STA报文的速率值。
 * @endif
 */
typedef struct {
    union {
        struct {
            uint8_t  he_mcs        : 4;
            uint8_t  nss_mode      : 2;
            uint8_t  protocol_mode : 2;
        } he_nss_mcs;                   /*!< @if Eng Definition of the 11ax rate set.
                                             @else  11ax的速率集定义。  @endif */
        struct {
            uint8_t   vht_mcs      : 4;
            uint8_t   nss_mode     : 2;
            uint8_t   protocol_mode: 2;
        } vht_nss_mcs;                  /*!< @if Eng Definition of the 11ac rate set.
                                             @else  11ac的速率集定义。  @endif */
        struct {
            uint8_t   ht_mcs       : 6;
            uint8_t   protocol_mode: 2;
        } ht_rate;                      /*!< @if Eng Definition of the 11n rate set.
                                             @else  11n的速率集定义。  @endif */
        struct {
            uint8_t   legacy_rate  : 4;
            uint8_t   reserved1    : 2;
            uint8_t   protocol_mode: 2;
        } legacy_rate_st;               /*!< @if Eng Definition of the 11a/b/g rate set.
                                             @else  11a/b/g的速率集定义。  @endif */
    } nss_rate;
} wifi_rate_stru;

/**
 * @if Eng
 * @brief  Returns the information about the STA connected to the AP.
 * @else
 * @brief  返回与AP相连的STA信息。
 * @endif
 */
typedef struct {
    uint8_t mac_addr[WIFI_MAC_LEN];     /*!< @if Eng MAC address.
                                             @else  MAC地址。  @endif */
    int8_t rssi;                        /*!< @if Eng Received signal strength indicator (RSSI).
                                             @else  RSSI。  @endif */
    wifi_rate_stru rate;                /*!< @if Eng Rate at which the softap receives packets from the connected station last time.
                                             @else  softap上一次接收相连的station报文速率值。  @endif */
} wifi_sta_info_stru;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_WIFI_STATION_INFO_H
/** @} */
