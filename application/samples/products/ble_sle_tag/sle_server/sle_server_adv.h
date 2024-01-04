/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE Server adv config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */

/**
 * @defgroup bluetooth_sle_adv API
 * @ingroup
 * @{
 */

#ifndef SLE_SERVER_ADV_H
#define SLE_SERVER_ADV_H

/**
 * @if Eng
 * @brief Definitaion of BLE ADV 通用广播结构.
 * @else
 * @brief SLE 广播普通数据结构。
 * @endif
 */
struct sle_adv_common_value {
    uint8_t length;
    uint8_t type;
    uint8_t value;
};

/**
 * @if Eng
 * @brief Definitaion of BLE ADV Channel mapping.
 * @else
 * @brief SLE 广播信道映射。
 * @endif
 */
typedef enum {
    SLE_ADV_CHANNEL_MAP_77                 = 0x01,
    SLE_ADV_CHANNEL_MAP_78                 = 0x02,
    SLE_ADV_CHANNEL_MAP_79                 = 0x04,
    SLE_ADV_CHANNEL_MAP_DEFAULT            = 0x07
} sle_adv_channel_map;

/**
 * @if Eng
 * @brief Definitaion of SLE ADV Data Type.
 * @else
 * @brief SLE 广播数据类型
 * @endif
 */
typedef enum {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL                              = 0x01,   /*!< 发现等级 */
    SLE_ADV_DATA_TYPE_ACCESS_MODE                                  = 0x02,   /*!< 接入层能力 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID                      = 0x03,   /*!< 标准服务数据信息 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID                     = 0x04,   /*!< 自定义服务数据信息 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS         = 0x05,   /*!< 完整标准服务标识列表 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS        = 0x06,   /*!< 完整自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS       = 0x07,   /*!< 部分标准服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS      = 0x08,   /*!< 部分自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE                 = 0x09,   /*!< 服务结构散列值 */
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME                         = 0x0A,   /*!< 设备缩写本地名称 */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME                          = 0x0B,   /*!< 设备完整本地名称 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL                               = 0x0C,   /*!< 广播发送功率 */
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN                     = 0x0D,   /*!< SLB通信域域名 */
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID                    = 0x0E,   /*!< SLB媒体接入层标识 */
    SLE_ADV_DATA_TYPE_EXTENDED                                     = 0xFE,   /*!< 数据类型扩展 */
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA                   = 0xFF    /*!< 厂商自定义信息 */
} sle_adv_data_type;


/**
 * @if Eng
 * @brief  sle adv data config.
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    Excute successfully
 * @retval ERRCODE_SLE_FAIL       Execute fail
 * @par Dependency:
 * @li NULL
 * @else
 * @brief  sle广播数据配置。
 * @attention  NULL
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 * @par 依赖:
 * @li NULL
 * @endif
 */
errcode_t sle_uuid_server_adv_init(void);

/* SLE使能状态 */
#define SLE_ENABLE_STATUS_OK 1

uint8_t get_g_sle_enable_status(void);
#endif