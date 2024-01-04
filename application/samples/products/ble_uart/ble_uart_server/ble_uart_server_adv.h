/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE UART Server adv config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-26, Create file. \n
 */

#ifndef BLE_UART_SERVER_ADV_H
#define BLE_UART_SERVER_ADV_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Ble Adv type Flag length */
#define BLE_ADV_FLAG_LEN 0x03
/* Ble Adv data length */
#define BLE_GENERAL_BYTE_1 1
/* Ble Adv appearance length */
#define BLE_ADV_APPEARANCE_LENGTH 4
/* Ble Adv appearance type */
#define BLE_ADV_APPEARANCE_DATA_TYPE 0x19
/* Ble uart appearance type */
#define BLE_ADV_CATEGORY_UART_VALUE 0x0080
/* Ble uart categorylength */
#define BLE_ADV_CATEGORY_LEN 2
/* Ble name adv param type length */
#define BLE_ADV_PARAM_DATATYPE_LENGTH 1
/* Ble name adv name type */
#define BLE_ADV_LOCAL_NAME_DATA_TYPE 0x09
/* Ble name adv tx power type */
#define BLE_ADV_TX_POWER_LEVEL       0x0A
/* Ble name adv tx power response type */
#define BLE_SCAN_RSP_TX_POWER_LEVEL_LEN 0x03
/* Ble adv flag data */
#define BLE_ADV_FLAG_DATA 0x05

typedef struct {
    uint8_t length;             /* 广播数据类型长度 */
    uint8_t adv_data_type;      /* 广播数据类型 */
    uint8_t flags;              /* 广播数据标志 */
} ble_adv_flag;

typedef struct {
    uint8_t length;                             /* 设备外观数据类型长度 */
    uint8_t adv_data_type;                      /* 设备外观数据类型 */
    uint8_t catogory_id[BLE_ADV_CATEGORY_LEN];  /* 设备外观数据 */
} ble_appearance_t;

typedef struct {
    uint8_t length;             /* 广播设备名称类型长度 */
    uint8_t adv_data_type;      /* 设备名称类型 */
    int8_t *name;               /* 设备名称数据指针 */
} ble_local_name_t;

typedef struct {
    uint8_t length;             /* 广播发送功率长度 */
    uint8_t adv_data_type;      /* 广播发送数据类型 */
    uint8_t tx_power_value;     /* 广播发送数据 */
} ble_tx_power_level_t;

typedef enum ble_adv_filter_policy {
    BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_ANY =                     0x00,
    BLE_ADV_FILTER_POLICY_SCAN_WHITE_LIST_CONNECT_ANY =              0x01,
    BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_WHITE_LIST =              0x02,
    BLE_ADV_FILTER_POLICY_SCAN_WHITE_LIST_CONNECT_WHITE_LIST =       0x03
} ble_adv_filter_policy_t;

typedef enum ble_adverting_type {
    BLE_ADV_TYPE_CONNECTABLE_UNDIRECTED =                            0x00,
    BLE_ADV_TYPE_CONNECTABLE_HIGH_DUTY_CYCLE_DIRECTED =              0x01,
    BLE_ADV_TYPE_SCANNABLE_UNDIRECTED =                              0x02,
    BLE_ADV_TYPE_NON_CONNECTABLE_UNDIRECTED =                        0x03,
    BLE_ADV_TYPE_CONNECTABLE_LOW_DUTY_CYCLE_DIRECTED =               0x04
} ble_adverting_type_t;

typedef enum ble_adv_channel_map {
    BLE_ADV_CHANNEL_MAP_CH_37 =                      0x01,
    BLE_ADV_CHANNEL_MAP_CH_38 =                      0x02,
    BLE_ADV_CHANNEL_MAP_CH_39 =                      0x04,
    BLE_ADV_CHANNEL_MAP_CH_37_CH_38 =                0x03,
    BLE_ADV_CHANNEL_MAP_CH_37_CH_39 =                0x05,
    BLE_ADV_CHANNEL_MAP_CH_38_CH_39 =                0x06,
    BLE_ADV_CHANNEL_MAP_CH_DEFAULT =                 0x07
} ble_adv_channel_map_t;

typedef enum {
    BLE_PUBLIC_DEVICE_ADDRESS =                             0x00,
    BLE_RANDOM_DEVICE_ADDRESS =                             0x01,
    BLE_PUBLIC_IDENTITY_ADDRESS =                           0x02,
    BLE_RANDOM_STATIC_IDENTITY_ADDRESS =                    0x03
} ble_address_t;

/* Ble adv min interval */
#define BLE_ADV_MIN_INTERVAL 0x30
/* Ble adv max interval */
#define BLE_ADV_MAX_INTERVAL 0x60
/* Ble adv handle */
#define BTH_GAP_BLE_ADV_HANDLE_DEFAULT 0x01
/* Ble adv duration */
#define BTH_GAP_BLE_ADV_FOREVER_DURATION 0

uint8_t ble_uart_start_adv(void);

uint8_t ble_uart_set_adv_data(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif