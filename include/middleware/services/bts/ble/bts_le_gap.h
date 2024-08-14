/**
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 *
 * Description: BTS GAP module.
 */

/**
 * @defgroup bluetooth_bts_gap BTS LE GAP API
 * @ingroup  bluetooth
 * @{
 */
#ifndef BTS_LE_GAP_H
#define BTS_LE_GAP_H

#include <stdint.h>
#include "errcode.h"
#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of BLE appearance type samples, see the Appearance Values file for details.
 * @else
 * @brief BLE显示类型部分举例,具体参考Appearance Values文件
 * @endif
 */
typedef enum {
    GAP_BLE_APPEARANCE_TYPE_UNKNOWN = 00,                  /*!< @if Eng Unknown
                                                                  @else   未知类型 @endif */
    GAP_BLE_APPEARANCE_TYPE_GENERIC_PHONE = 64,            /*!< @if Eng Generic Phone
                                                                  @else   通用手机 @endif */
    GAP_BLE_APPEARANCE_TYPE_GENERIC_COMPUTER = 128,        /*!< @if Eng Generic Computer
                                                                  @else   通用电脑 @endif */
    GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH = 192,           /*!< @if Eng Generic Watch
                                                                  @else   通用手表 @endif */
    GAP_BLE_APPEARANCE_TYPE_GENERIC_DISPLAY = 320,         /*!< @if Eng Generic Display
                                                                  @else   通用显示器 @endif */
    GAP_BLE_APPEARANCE_TYPE_GENERIC_HID = 960,             /*!< @if Eng Generic Human Interface Device
                                                                  @else   通用人机界面设备 @endif */
    GAP_BLE_APPEARANCE_TYPE_KEYBOARD = 961,                /*!< @if Eng Keyboard
                                                                  @else   键盘 @endif */
    GAP_BLE_APPEARANCE_TYPE_MOUSE = 962,                   /*!< @if Eng Mouse
                                                                  @else   鼠标 @endif */
    GAP_BLE_APPEARANCE_TYPE_DIGITAL_PEN = 967,             /*!< @if Eng Digital Pen
                                                                  @else   电子笔 @endif */
} gap_ble_appearance_type_t;

/**
 * @if Eng
 * @brief Enum of advertising filter parameters.
 * @else
 * @brief 广播过滤参数。
 * @endif
 */
typedef enum {
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,   /*!< @if Eng Accepts all scan and connect requests
                                                             @else   处理所有设备的扫描和连接请求 @endif */
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01,  /*!< @if Eng Accepts all connect but white list scan requests
                                                             @else   处理所有连接请求，仅处理白名单的扫描请求 @endif */
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02,  /*!< @if Eng Accepts all scan but white list connect requests
                                                             @else   处理所有扫描请求，仅处理白名单的连接请求 @endif */
    GAP_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03, /*!< @if Eng Accepts only white list connect and scan requests
                                                             @else   仅处理白名单中扫描请求和连接请求 @endif */
} gap_ble_adv_filter_allow_scan_t;

/**
 * @if Eng
 * @brief Enum of advertising type.
 * @else
 * @brief 广播类型。
 * @endif
 */
typedef enum {
    GAP_BLE_ADV_CONN_SCAN_UNDIR = 0,        /*!< @if Eng Connectable, scanable, undirected advertising(default).
                                                 @else   可连接可扫描非定向广播(默认)。 @endif */
    GAP_BLE_ADV_CONN_NONSCAN_DIR,           /*!< @if Eng Connectable, nonscanble, high duty directed advertising.
                                                 @else   可连接不可扫描高频定向广播。 @endif */
    GAP_BLE_ADV_NONCONN_SCAN_UNDIR,         /*!< @if Eng Nonconnectable, scanable, undirected advertising.
                                                 @else   不可连接可扫描非定向广播。 @endif */
    GAP_BLE_ADV_NONCONN_NONSCAN_UNDIR,      /*!< @if Eng Nonconnectable, nonscanable, undirected advertising.
                                                 @else   不可连接不可扫描非定向广播。 @endif */
    GAP_BLE_ADV_CONN_NONSCAN_DIR_LOW_DUTY,  /*!< @if Eng Connectable, nonscanble, low duty directed advertising.
                                                 @else   可连接不可扫描低频定向广播。 @endif */
} gap_ble_adv_type_t;

/**
 * @if Eng
 * @brief Enum of BLE scan type.
 * @else
 * @brief BLE扫描类型。
 * @endif
 */
typedef enum {
    GAP_BLE_SCAN_TYPE_PASSIVE = 0x00, /*!< @if Eng Passive scan
                                           @else   被动扫描 @endif */
    GAP_BLE_SCAN_TYPE_ACTIVE,         /*!< @if Eng Active scan
                                           @else   主动扫描 @endif */
} gap_ble_scan_type_t;

/**
 * @if Eng
 * @brief Enum of BLE scan filter policy.
 * @else
 * @brief BLE扫描过滤策略。
 * @endif
 */
typedef enum {
    GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL = 0x00,       /*!< @if Eng Accept all advertising packets except directed
                                                                     advertising packets not addressed to this device
                                                                     (default)
                                                             @else   接收所有广播不接收目标地址不是本设备地址的定向广播
                                                                    （默认）
                                                             @endif */
    GAP_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST,         /*!< @if Eng Accept only advertisement packets from white list
                                                                     devices. Directed advertising packets which are
                                                                     not addressed for this device shall be ignored
                                                             @else   只接收白名单里设备的广播，
                                                                     不接收目标地址不是本设备地址的定向广播 @endif */
    GAP_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL_AND_RPA,      /*!< @if Eng Accept all undirected advertisement packets, and
                                                                     all directed advertising packets where the
                                                                     initiator address is a resolvable private address,
                                                                     and all directed advertising packets addressed to
                                                                     this device
                                                             @else   接收所有的非定向广播、
                                                                     地址是可解析私有地址的广播方发送的定向广播、
                                                                     发给该设备的定向广播 @endif */
    GAP_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST_AND_RPA, /*!< @if Eng Accept all undirected advertisement packets from
                                                                     devices where the advertiser's address is in the
                                                                     White list, and all directed advertising packets
                                                                     where the initiator address is a resolvable
                                                                     private address, and all directed advertising
                                                                     packets addressed to this device
                                                             @else   接收白名单中的所有非定向广播、
                                                                     地址是可解析私有地址的广播方发送的定向广播、
                                                                     发给该设备的定向广播 @endif */
} gap_ble_scan_filter_policy_t;

/**
 * @if Eng
 * @brief Enum of BLE scan event type.
 * @else
 * @brief BLE扫描结果广播类型。
 * @endif
 */
typedef enum {
    GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE = 0x00,          /*!< @if Eng Non-connectable, non-scannable, undirected
                                                                    @else   扩展的不可连接不可扫描非定向 @endif */
    GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED = 0x04, /*!< @if Eng Non-connectable, non-scannable, directed
                                                                    @else   扩展的不可连接不可扫描定向 @endif */
    GAP_BLE_EVT_CONNECTABLE = 0x01,                            /*!< @if Eng Connectable and undirected
                                                                    @else   扩展的可连接非定向 @endif */
    GAP_BLE_EVT_CONNECTABLE_DIRECTED = 0x05,                   /*!< @if Eng Connectable and directed
                                                                    @else   扩展的可连接定向 @endif */
    GAP_BLE_EVT_SCANNABLE = 0x02,                              /*!< @if Eng Scannable and undirected
                                                                    @else   扩展的可扫描非定向 @endif */
    GAP_BLE_EVT_SCANNABLE_DIRECTED = 0x06,                     /*!< @if Eng Scannable and directed
                                                                    @else   扩展的可扫描定向 @endif */
    GAP_BLE_EVT_LEGACY_NON_CONNECTABLE = 0x10,                 /*!< @if Eng Legacy, non-connectable and undirected
                                                                    @else   传统的不可连接非定向 @endif */
    GAP_BLE_EVT_LEGACY_SCANNABLE = 0x12,                       /*!< @if Eng Legacy, scannable and undirected
                                                                    @else   传统的可扫描非定向 @endif */
    GAP_BLE_EVT_LEGACY_CONNECTABLE = 0x13,                     /*!< @if Eng Legacy, connectable, scannable and
                                                                            undirected
                                                                    @else   传统的可连接可扫描非定向 @endif */
    GAP_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED = 0x15,            /*!< @if Eng Legacy, connectable, and directed
                                                                    @else   传统的可连接定向 @endif */
    GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN = 0x1A,            /*!< @if Eng Legacy scan response corresponding to
                                                                            ADV_SCAN_IND
                                                                    @else   传统的与ADV_SCAN_IND对应的扫描响应 @endif */
    GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV = 0x1B                  /*!< @if Eng Legacy scan response corresponding to
                                                                            ADV_IND
                                                                    @else   传统的与ADV_IND对应的扫描响应 @endif */
} gap_ble_scan_result_evt_type_t;

/**
 * @if Eng
 * @brief Struct of ble connect parameter update struct.
 * @else
 * @brief ble连接参数更新数据结构。
 * @endif
 */
typedef struct {
    uint16_t conn_handle;        /*!< @if Eng connection id
                                      @else   连接 ID @endif */
    uint16_t interval_min;       /*!< @if Eng min interval
                                      @else   最小间隔 @endif */
    uint16_t interval_max;       /*!< @if Eng max interval
                                      @else   最大间隔 @endif */
    uint16_t slave_latency;      /*!< @if Eng slave reply min latency
                                      @else   从设备回复最小间隔 @endif */
    uint16_t timeout_multiplier; /*!< @if Eng interval for disconnection due to timeout
                                      @else   超时断连间隔 @endif */
} gap_conn_param_update_t;

/**
 * @if Eng
 * @brief Struct of BLE PHY parameters.
 * @else
 * @brief BLE PHY参数数据结构。
 * @endif
 */
typedef struct {
    uint16_t conn_handle;       /*!< @if Eng interval
                                    @else   连接句柄 @endif */
    uint8_t all_phys;           /*!< @if Eng interval
                                    @else   所有PHY @endif */
    uint8_t tx_phys;            /*!< @if Eng interval
                                    @else   发送PHY @endif */
    uint8_t rx_phys;            /*!< @if Eng interval
                                    @else   接收PHY @endif */
    uint16_t phy_options;       /*!< @if Eng interval
                                    @else   PHY选项 @endif */
} gap_le_set_phy_t;

/**
 * @if Eng
 * @brief Struct of BLE packet transmission parameters.
 * @else
 * @brief BLE发包参数数据结构。
 * @endif
 */
typedef struct {
    uint16_t conn_handle;       /*!< @if Eng interval
                                    @else   连接句柄 @endif */
    uint16_t maxtxoctets;           /*!< @if Eng interval
                                    @else   最大字节数 @endif */
    uint16_t maxtxtime;            /*!< @if Eng interval
                                    @else   最大发送时间 @endif */
} gap_le_set_data_length_t;

/**
 * @if Eng
 * @brief Enum of BLE scan result data status.
 * @else
 * @brief BLE扫描结果数据完整性。
 * @endif
 */
typedef enum {
    GAP_BLE_DATA_COMPLETE = 0x00,                /*!< @if Eng Complete, or last segment
                                                      @else   完整数据或最后一个片段 @endif */
    GAP_BLE_DATA_INCOMPLETE_MORE_TO_COME = 0x01, /*!< @if Eng Incomplete
                                                      @else   不完整的数据 @endif */
    GAP_BLE_DATA_INCOMPLETE_TRUNCATED = 0x02,    /*!< @if Eng Truncated
                                                      @else   被截断不完整的数据 @endif */
} gap_ble_scan_result_data_status_t;

/**
 * @if Eng
 * @brief Enum of BLE PHY type.
 * @else
 * @brief BLE PHY类型。
 * @endif
 */
typedef enum {
    GAP_BLE_PHY_NO_PACKET = 0x00, /*!< @if Eng No packet
                                       @else   无广播包 @endif */
    GAP_BLE_PHY_1M = 0x01,        /*!< @if Eng 1M PHY
                                       @else   1M PHY @endif */
    GAP_BLE_PHY_2M = 0x02,        /*!< @if Eng 2M PHY
                                       @else   2M PHY @endif */
    GAP_BLE_PHY_CODED = 0x03      /*!< @if Eng Coded PHY
                                       @else   Coded PHY @endif */
} gap_ble_phy_type_t;

/**
 * @if Eng
 * @brief Advtertising status
 * @else
 * @brief 广播状态
 * @endif
 */
typedef enum {
    ADV_STATUS_STOPPED = 0x00, /*!< @if Eng advertising stoped
                                    @else   广播停止 @endif */
    ADV_STATUS_ADVERTISING,    /*!< @if Eng advertising
                                    @else   正在广播 @endif */
} adv_status_t;

/**
 * @if Eng
 * @brief Struct of BLE advertising data.
 * @else
 * @brief BLE广播数据。
 * @endif
 */
typedef struct {
    uint16_t adv_length;      /*!< @if Eng Length of advertising data
                                   @else   广播数据长度 @endif */
    uint8_t *adv_data;        /*!< @if Eng Advertising data
                                   @else   广播数据 @endif */
    uint16_t scan_rsp_length; /*!< @if Eng Length of scan response data
                                   @else   扫描响应数据长度 @endif */
    uint8_t *scan_rsp_data;   /*!< @if Eng Scan response data
                                   @else   扫描响应数据 @endif */
} gap_ble_config_adv_data_t;

/**
 * @if Eng
 * @brief Struct of BLE advertising parameters.
 * @else
 * @brief BLE广播参数。
 * @endif
 */
typedef struct {
    uint32_t min_interval;     /*!< @if Eng Min interval[N * 0.625ms]
                                    @else   最小的广播间隔[N * 0.625ms] @endif */
    uint32_t max_interval;     /*!< @if Eng Max interval[N * 0.625ms]
                                    @else   最大的广播间隔[N * 0.625ms] @endif */
    uint8_t adv_type;          /*!< @if Eng Advertising type { @ref gap_ble_adv_type_t }
                                    @else   广播类型 { @ref gap_ble_adv_type_t } @endif */
    bd_addr_t own_addr;       /*!< @if Eng own address
                                    @else   本端地址 @endif */
    bd_addr_t peer_addr;       /*!< @if Eng Peer address
                                    @else   对端地址 @endif */
    uint8_t channel_map;       /*!< @if Eng channel bitmap
                                    @else   广播通道选择:
                                            0x01----使用37通道
                                            0x07----使用37/38/39三个通道 @endif */
    uint8_t adv_filter_policy; /*!< @if Eng Advertising filter policy
                                            { @ref gap_ble_adv_filter_allow_scan_t }
                                    @else   白名单过滤策略
                                            { @ref gap_ble_adv_filter_allow_scan_t } @endif */
    uint32_t tx_power;         /*!< @if Eng Transmissive power
                                    @else   发送功率,单位dbm,范围-127~20 @endif */
    uint32_t duration;         /*!< @if Eng Duration
                                    @else   广播持续发送时长,单位dbm @endif */
} gap_ble_adv_params_t;

/**
 * @if Eng
 * @brief Enum of advertising type.
 * @else
 * @brief 设备输入输出能力
 * @endif
 */
typedef enum {
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL1 = 0,     /*!< @if Eng No security
                                                    @else   没有安全能力 @endif */
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL2,         /*!< @if Eng Unauthenticated pairing and encryption
                                                    @else   不需要认证基于链路进行配对和加密 @endif */
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL3,         /*!< @if Eng Authenticated Pairing and encryption
                                                    @else   需要认证基于链路进行配对和加密 @endif */
    GAP_BLE_GAP_SECURITY_MODE1_LEVEL4,         /*!< @if Eng Authenticated ECDH Pairing and encryption
                                                    @else   需要认证基于链路采用ECDH算法进行加密和配对 @endif */
    GAP_BLE_GAP_SECURITY_MODE2_LEVEL1,         /*!< @if Eng Unauthenticated pairing and data signing
                                                    @else   不需要认证基于数据进行配对和加密 @endif */
    GAP_BLE_GAP_SECURITY_MODE2_LEVEL2,         /*!< @if Eng Authenticated pairing and data signing
                                                    @else   需要认证基于数据进行配对和加密 @endif */
} gap_ble_sec_mode_t;

/**
 * @if Eng
 * @brief Enum of advertising type.
 * @else
 * @brief 设备输入输出能力
 * @endif
 */
typedef enum {
    GAP_BLE_IO_CAPABILITY_DISPLAYONLY = 0,     /*!< @if Eng only display
                                                    @else   只展示 @endif */
    GAP_BLE_IO_CAPABILITY_DISPLAYYESNO,        /*!< @if Eng display and select yes or no
                                                    @else   展示，并且可以选择Yes或者No @endif */
    GAP_BLE_IO_CAPABILITY_KEYBOARDONLY,        /*!< @if Eng only keyboard
                                                    @else   只支持键盘 @endif */
    GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT,     /*!< @if Eng no input and no output
                                                    @else   没有输入输出 @endif */
    GAP_BLE_IO_CAPABILITY_KEYBOARDDISPLAY,     /*!< @if Eng display and keyboard
                                                    @else   支持键盘和展示 @endif */
} gap_ble_io_ability_t;

/**
 * @if Eng
 * @brief Struct of BLE scan parameters.
 * @else
 * @brief BLE扫描参数。
 * @endif
 */
typedef struct {
    uint16_t scan_interval;     /*!< @if Eng Scan interval[N * 0.625ms]
                                     @else   扫描间隔，[N * 0.625 ms] @endif */
    uint16_t scan_window;       /*!< @if Eng Scan window[N * 0.625ms]
                                     @else   扫描窗长，[N * 0.625 ms] @endif */
    uint8_t scan_type;          /*!< @if Eng Scan type { @ref gap_ble_scan_type_t }
                                     @else   扫描类型 { @ref gap_ble_scan_type_t } @endif */
    uint8_t scan_phy;           /*!< @if Eng PHY type { @ref gap_ble_phy_type_t }
                                     @else   PHY类型 { @ref gap_ble_phy_type_t } @endif */
    uint8_t scan_filter_policy; /*!< @if Eng Scan fileter policy { @ref gap_ble_scan_filter_policy_t }
                                     @else   扫描过滤策略 { @ref gap_ble_scan_filter_policy_t } @endif */
} gap_ble_scan_params_t;

/**
 * @if Eng
 * @brief Struct of scan result data.
 * @else
 * @brief 扫描结果数据。
 * @endif
 */
typedef struct {
    uint8_t event_type;             /*!< @if Eng Event type { @ref gap_ble_scan_result_evt_type_t }
                                         @else   广播类型 { @ref gap_ble_scan_result_evt_type_t } @endif */
    uint8_t data_status;            /*!< @if Eng Data status { @ref gap_ble_scan_result_data_status_t }
                                         @else   扫描结果数据状态 { @ref gap_ble_scan_result_data_status_t } @endif */
    bd_addr_t addr;                 /*!< @if Eng Address
                                         @else   地址 @endif */
    uint8_t primary_phy;            /*!< @if Eng primary PHY { @ref gap_ble_phy_type_t }
                                         @else   主广播PHY类型 { @ref gap_ble_phy_type_t } @endif */
    uint8_t secondary_phy;          /*!< @if Eng secondary PHY { @ref gap_ble_phy_type_t }
                                         @else   辅广播PHY类型 { @ref gap_ble_phy_type_t } @endif */
    uint8_t adv_sid;                /*!< @if Eng Value of the Advertising SID subfield in the ADI field of the PDU
                                         @else   广播SID @endif */
    int8_t tx_power;                /*!< @if Eng Transmissive power
                                         @else   发送功率，范围: -127 ~ +20dBm @endif */
    int8_t rssi;                    /*!< @if Eng RSSI
                                         @else   信号强度，范围: -127 ~ +20dBm @endif */
    uint16_t periodic_adv_interval; /*!< @if Eng Periodic advertising interval
                                         @else   周期广播间隔，[N * 1.25 ms] @endif */
    bd_addr_t direct_addr;          /*!< @if Eng Directed address
                                         @else   定向广播地址 @endif */
    uint8_t adv_len;                /*!< @if Eng Advertising data length
                                         @else   广播数据长度 @endif */
    uint8_t *adv_data;              /*!< @if Eng Advertising data
                                         @else   广播数据 @endif */
} gap_scan_result_data_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth pairing state.
 * @else
 * @brief 蓝牙配对状态。
 * @endif
 */
typedef enum {
    GAP_BLE_PAIR_NONE = 0x01,    /*!< @if Eng Pair state of none
                                      @else   未配对状态 @endif */
    GAP_BLE_PAIR_PAIRING = 0x02, /*!< @if Eng Pair state of pairing
                                      @else   正在配对 @endif */
    GAP_BLE_PAIR_PAIRED = 0x03   /*!< @if Eng Pair state of paired
                                      @else   已完成配对 @endif */
} gap_ble_pair_state_t;

/**
 * @if Eng
 * @brief Enum of disconnect reason.
 * @else
 * @brief 蓝牙断链原因。
 * @endif
 */
typedef enum {
    GAP_BLE_DISCONN_UNKNOWN              = 0x00,    /*!< @if Eng disconnect by local
                                                         @else   未知原因断链 @endif */
    GAP_BLE_ERR_CONN_TIMEOUT             = 0x8,     /*!< @if Eng disconnect by local
                                                         @else   连接超时断链 @endif */
    GAP_BLE_DICSCONNECT_BY_REMOTE_USER   = 0x13,    /*!< @if Eng disconnect by remote
                                                         @else   远端用户断链 @endif */
    GAP_BLE_CONN_TERMINATE_BY_LOCAL_HOST = 0x16,    /*!< @if Eng disconnect by remote
                                                         @else   本端HOST断链 @endif */
} gap_ble_disc_reason_t;

/**
 * @if Eng
 * @brief  Enum of acl state.
 * @else
 * @brief  定义acl链路状态。
 * @endif
 */
typedef enum {
    GAP_BLE_STATE_DISCONNECTED,/*!< @if Eng BLE GAP ACL state of disconnected
                                    @else   BLE GAP ACL 已断连 @endif */
    GAP_BLE_STATE_CONNECTED    /*!< @if Eng BLE GAP ACL state of connected
                                    @else   BLE GAP ACL 已连接 @endif */
} gap_ble_conn_state_t;


/**
 * @if Eng
 * @brief Enum of ble link update event parameters.
 * @else
 * @brief ACL链路更新事件参数
 * @endif
 */
typedef struct {
    uint16_t interval;          /*!< @if Eng interval
                                     @else   链路调度间隔，单位slot @endif */
    uint16_t latency;           /*!< @if Eng latency
                                     @else   延迟周期，单位slot @endif */
    uint16_t timeout;           /*!< @if Eng timeout
                                     @else   超时时间，单位10ms @endif */
} gap_ble_conn_param_update_t;

/**
 * @if Eng
 * @brief Struct of BLE advertising parameters.
 * @else
 * @brief BLE广播参数。
 * @endif
 */
typedef struct {
    uint8_t bondable;          /*!< @if Eng bondable
                                    @else   绑定能力选择：
                                            0x01----支持绑定
                                            0x00----不支持绑定 @endif */
    uint8_t io_capability;     /*!< @if Eng input and output type { @ref gap_ble_io_ability_t }
                                    @else   输入输出能力 { @ref gap_ble_io_ability_t } @endif */
    uint8_t sc_enable;         /*!< @if Eng security enable
                                    @else   安全配对能力选择：
                                            0x01----支持安全配对
                                            0x00----不支持安全配对 @endif */
    uint8_t sc_mode;           /*!< @if Eng security mode type { @ref gap_ble_sec_mode_t }
                                    @else   安全模式 { @ref gap_ble_sec_mode_t } @endif */
} gap_ble_sec_params_t;

/**
 * @if Eng
 * @brief  Length of device smp ltk.
 * @else
 * @brief  蓝牙设备配对的LTK长度。
 * @endif
 */
#define BLE_PAIRED_LTK_LEN 16

/**
 * @if Eng
 * @brief Enum of sle authentication result.
 * @else
 * @brief BLE认证结果。
 * @endif
 */
typedef struct {
    uint8_t ltk_len;                            /*!< @if Eng link key len.
                                                  @else   链路密钥长度。 @endif */
    uint8_t ltk[BLE_PAIRED_LTK_LEN];            /*!< @if Eng link key.
                                                  @else   链路密钥。 @endif */
} ble_auth_info_evt_t;

/**
 * @if Eng
 * @brief Callback invoked when ble enabled.
 * @par When registered, this callback notifies the upper layer when ble enabled.
 * @attention 1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] status enable ble status.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  ble协议栈启动回调函数。
 * @par    注册该回调函数之后，BTS在ble协议栈启动后上报启动结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] status enable协议栈状态
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_enable_callback)(errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when ble disabled.
 * @par When registered, this callback notifies the upper layer when ble disabled.
 * @attention 1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] status disable ble status.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  ble协议栈关闭回调函数。
 * @par    注册该回调函数之后，BTS在ble协议栈关闭后上报关闭结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] status 关闭ble协议栈状态。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_disable_callback)(errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked in response to advertising being enabled.
 * @par When registered, this callback notifies the upper layer whether advertising is enabled successfully.
 * @attention 1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] adv_id advertising ID.
 * @param  [in] status advertising status.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  启动广播回调函数。
 * @par    注册该回调函数之后，BTS在每次启动广播后调用该接口上报启动结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] adv_id 广播ID。
 * @param  [in] status 当前广播状态。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_start_adv_callback)(uint8_t adv_id, adv_status_t status);

/**
 * @if Eng
 * @brief Callback invoked in response to advertising being disabled.
 * @par When registered, this callback notifies the upper layer whether advertising is disabled successfully.
 * @attention 1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] adv_id advertising ID.
 * @param  [in] status advertising status.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  停止广播回调函数。
 * @par    注册该回调函数之后，BTS在每次停止广播后调用该接口上报停止结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] adv_id 广播ID。
 * @param  [in] status 当前广播状态。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_stop_adv_callback)(uint8_t adv_id, adv_status_t status);

/**
 * @if Eng
 * @brief Callback invoked in response to advertising data being configured.
 * @par When registered, this callback notifies the upper layer if advertising data is configured as expected.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] adv_id advertising ID.
 * @param  [in] status error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  设置广播数据后的回调函数。
 * @par    注册该回调函数之后，BTS在每次设置广播数据后调用该接口上报设置结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] adv_id 广播ID。
 * @param  [in] status 执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_set_adv_data_callback)(uint8_t adv_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked in response to advertising being set.
 * @par  When registered, this callback notifies the upper layer if advertising data is set successfully.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] adv_id advertising ID.
 * @param  [in] status error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  设置广播参数后的回调函数。
 * @par    注册该回调函数之后，BTS在每次设置广播参数后调用该接口上报更新结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] adv_id 广播ID。
 * @param  [in] status 执行结果错误码.
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_set_adv_param_callback)(uint8_t adv_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked in response to scan being enabled.
 * @par  When registered, this callback notifies the upper layer about the scan result data.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] gap_scan_result_data_t scan result data.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  启动扫描后的回调函数。
 * @par    注册该回调函数之后，BTS在每次收到扫描结果后调用该接口上报结果数据给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @attention  3. 暂不支持
 * @param  [in] gap_scan_result_data_t 扫描结果数据。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_scan_result_callback)(gap_scan_result_data_t *scan_result_data);

/**
 * @if Eng
 * @brief Callback invoked in response to scan parameter being set.
 * @par  When registered, this callback notifies the upper layer if the scan parameter is set as expected.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param  [in] status  #uint32_t.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  扫描参数设置完成后的回调函数。
 * @par    注册该回调函数之后，BTS在扫描参数设置完成后调用该接口上报设置状态给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] status 执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_set_scan_param_callback)(errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when connection completed.
 * @par When registered, this callback notifies the upper layer if a connnection is established.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param [in] conn_id connection ID.
 * @param [in] conn_state the connection state.
 * @param [in] pair_status the pair state.
 * @param [in] disc_reason the reason of disconnect.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  连接事件完成的回调函数。
 * @par    注册该回调函数之后，BTS在连接成后调用该接口上报连接状态信息给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] conn_id 连接ID。
 * @param  [in] conn_state 连接状态。
 * @param  [in] pair_status 配对状态。
 * @param  [in] disc_reason 断链原因。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_connect_state_changed_callback)(uint16_t conn_id, bd_addr_t *addr,
    gap_ble_conn_state_t conn_state, gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason);

/**
 * @if Eng
 * @brief Callback invoked when pair completed.
 * @par When registered, this callback notifies the upper layer if a connnection is established.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param [in] conn_id connection ID.
 * @param [in] conn_state the connection state.
 * @param [in] status the pair state.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  配对事件完成的回调函数。
 * @par    注册该回调函数之后，BTS在配对后调用该接口上报配对状态信息给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] conn_id 连接ID。
 * @param  [in] status 配对状态。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_paired_complete_callback)(uint16_t conn_id, const bd_addr_t *addr, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when advertising stoped.
 * @par When registered, this callback notifies the upper layer if an advertising is stoped.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of <devices> are requested and freed by the bts automatically.
 * @param [in] adv_id advertising ID.
 * @param [in] adv_status_t the advertising state.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  配对事件完成的回调函数。
 * @par    注册该回调函数之后，BTS在配对后调用该接口上报配对状态信息给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. <devices>由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in] adv_id 连接ID。
 * @param  [in] status 配对状态。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_terminate_adv_callback)(uint8_t adv_id, adv_status_t status);

/**
 * @if Eng
 * @brief Callback invoked when rssi read complete.
 * @par Callback invoked when rssi read complete.
 * @attention 1.This function is called in service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the BLE service automatically.
 * @param [in] conn_id connection ID.
 * @param [in] rssi    rssi.
 * @param [in] status  error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  读取rssi的回调函数。
 * @par    读取rssi的回调函数。
 * @attention  1. 该回调函数运行于 service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由 service申请内存，也由 service释放，回调中不应释放。
 * @param [in] conn_id 连接 ID。
 * @param [in] rssi    rssi。
 * @param [in] status  执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*gap_ble_read_rssi_callback)(uint16_t conn_id, int8_t rssi, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when authentication complete.
 * @par Callback invoked when authentication complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id connection ID.
 * @param [in] addr    address.
 * @param [in] status  error code.
 * @param [in] evt     authentication event.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  认证完成的回调函数。
 * @par    认证完成的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] conn_id 连接 ID。
 * @param [in] addr    地址。
 * @param [in] status  执行结果错误码。
 * @param [in] evt     认证事件。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*gap_ble_auth_complete_callback)(uint16_t conn_id, const bd_addr_t *addr, errcode_t status,
    const ble_auth_info_evt_t* evt);

/**
 * @if Eng
 * @brief Callback invoked when connect parameter updated.
 * @par Callback invoked when connect parameter updated.
 * @attention 1.This function is called in SLE service context, should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id    connection ID.
 * @param [in] status     error code.
 * @param [in] param      connection param.
 * @retval #void no return value.
 * @par Dependency:
 * @li  ble_common.h
 * @see ble_connection_callbacks_t
 * @else
 * @brief  连接参数更新的回调函数。
 * @par    连接参数更新的回调函数。
 * @attention  1. 该回调函数运行于BLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由BLE service申请内存，也由BLE service释放，回调中不应释放。
 * @param [in] conn_id    连接 ID。
 * @param [in] status     执行结果错误码。
 * @param [in] param      连接参数。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*gap_ble_connect_param_update_callback)(uint16_t conn_id, errcode_t status,
    const gap_ble_conn_param_update_t *param);

/**
 * @if Eng
 * @brief Struct of GAP BLE callback function.
 * @else
 * @brief GAP BLE回调函数接口定义。
 * @endif
 */
typedef struct {
    gap_ble_enable_callback ble_enable_cb;                       /*!< @if Eng Ble enable callback
                                                                      @else   BLE启动回调函数 @endif */
    gap_ble_disable_callback ble_disable_cb;                     /*!< @if Eng Ble disable callback
                                                                      @else   BLE关闭回调函数 @endif */
    gap_ble_set_adv_data_callback set_adv_data_cb;               /*!< @if Eng Set advertising data callback
                                                                      @else   设置广播数据回调函数 @endif */
    gap_ble_set_adv_param_callback set_adv_param_cb;             /*!< @if Eng Set advertising parameter callback
                                                                      @else   设置广播参数回调函数 @endif */
    gap_ble_set_scan_param_callback set_scan_param_cb;           /*!< @if Eng Set scan parameter callback
                                                                      @else   设置扫描参数回调函数 @endif */
    gap_ble_start_adv_callback start_adv_cb;                     /*!< @if Eng Start advertising callback
                                                                      @else   开启广播回调函数 @endif */
    gap_ble_stop_adv_callback stop_adv_cb;                       /*!< @if Eng Stop advertising callback
                                                                      @else   关闭广播回调函数 @endif */
    gap_ble_scan_result_callback scan_result_cb;                 /*!< @if Eng Scan result callback
                                                                      @else   扫描结果回调函数 @endif */
    gap_ble_connect_state_changed_callback conn_state_change_cb; /*!< @if Eng Connect state changed callback
                                                                      @else   连接状态改变回调函数 @endif */
    gap_ble_paired_complete_callback pair_result_cb;             /*!< @if Eng pair complete callback
                                                                      @else   连接状态改变回调函数 @endif */
    gap_ble_read_rssi_callback read_rssi_cb;                     /*!< @if Eng Read rssi callback.
                                                                      @else   读取rssi回调函数。 @endif */
    gap_ble_terminate_adv_callback terminate_adv_cb;             /*!< @if Eng terminate adv callback
                                                                      @else   被动中止广播回调函数 @endif */
    gap_ble_auth_complete_callback auth_complete_cb;             /*!< @if Eng authentication complete callback
                                                                      @else   认证完成回调函数 @endif */
    gap_ble_connect_param_update_callback conn_param_update_cb;  /*!< @if Eng connect param update callback
                                                                      @else   连接参数更新回调函数 @endif */
} gap_ble_callbacks_t;

/**
 * @if Eng
 * @brief Use this funtion to enable BLE.
 * @par  Use this funtion to enable BLE.
 * @attention NULL
 * @param  NULL
 * @retval error code, ble enable result will be returned at { @ref gap_ble_enable_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  使能BLE协议栈。
 * @par    使能BLE协议栈。
 * @attention 无
 * @param  无
 * @retval 执行结果错误码，执行结果将在 { @ref gap_ble_enable_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t enable_ble(void);

/**
 * @if Eng
 * @brief Use this funtion to disable BLE stack.
 * @par  Use this funtion to disable BLE stack.
 * @attention NULL
 * @param  NULL
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  去使能BLE协议栈。
 * @par    去使能BLE协议栈。
 * @attention 无
 * @param  无
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t disable_ble(void);

/**
 * @if Eng
 * @brief Use this funtion to set local device address.
 * @par   Use this funtion to set local device address.
 * @attention NULL
 * @param  [in] addr local device address.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置本地设备地址。
 * @par    设置本地设备地址。
 * @attention 无
 * @param  [in] addr 本地设备地址。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_local_addr(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to get local device address.
 * @par   Use this funtion to get local device address.
 * @attention NULL
 * @param  [out] addr local device address
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  获取本地设备地址。
 * @par    获取本地设备地址。
 * @attention 无
 * @param  [out] addr 本地设备地址
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_get_local_addr(bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to set local device appearance.
 * @par   Use this funtion to set local device appearance.
 * @attention NULL
 * @param  [in] addr local device appearance { @ref gap_ble_appearance_type_t }.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置本地设备地址。
 * @par    设置本地设备地址。
 * @attention 无
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_local_appearance(uint16_t appearance);

/**
 * @if Eng
 * @brief Use this funtion to set local device name.
 * @par   Use this funtion to set local device name.
 * @attention NULL
 * @param  [out] name the name of local host bluetooth device.
 * @param  [in]  len  the length of device name, including the endding 0.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置本地设备名称。
 * @par    设置本地设备名称。
 * @attention 无
 * @param  [out] 设备名称。
 * @param  [in]  名称长度，包括结束符\0。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_local_name(const uint8_t *name, const uint8_t len);

/**
 * @if Eng
 * @brief Use this funtion to get local device name.
 * @par   Use this funtion to get local device name.
 * @attention NULL
 * @param   [out]   name local device name.
 * @param   [inout] len  as input parameter, the buffer size of user,
                         as output parameter, the length of local device name.
 * @retval error code
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  获取本地设备名称。
 * @par    获取本地设备名称。
 * @attention 无
 * @param   [out]   name 本地设备名称。
 * @param   [inout] len  作为入参时为用户分配的内存大小，作为出参时为本地设备名称长度。
 * @retval 执行结果错误码
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_get_local_name(uint8_t *name, uint8_t *len);

/**
 * @if Eng
 * @brief Use this funtion to set advertising data.
 * @par   Use this funtion to set advertising data
 * @attention NULL
 * @param  [in] adv_id advertising ID
 * @param  [in] data   a pointer to the advertising data
 * @retval error code, the data set result will be returned at { @ref gap_ble_set_adv_data_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置广播数据。
 * @par    设置广播数据。
 * @attention 无
 * @param  [in] adv_id 广播ID
 * @param  [in] data   广播数据
 * @retval 执行结果错误码，数据设置状态将在 { @ref gap_ble_set_adv_data_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_adv_data(uint8_t adv_id, const gap_ble_config_adv_data_t *data);

/**
 * @if Eng
 * @brief Use this funtion to set advertising parameter.
 * @par   Use this funtion to set advertising parameter.
 * @attention NULL
 * @param  [in] adv_id advertising ID.
 * @param  [in] param  a pointer to the advertising parameter.
 * @retval error code, the parameter set result will be returned at { @ref gap_ble_set_adv_param_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置广播参数。
 * @par    设置广播参数。
 * @attention 无
 * @param  [in] adv_id 广播ID
 * @param  [in] param  广播参数
 * @retval 执行结果错误码，参数设置状态将在 { @ref gap_ble_set_adv_param_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_adv_param(uint8_t adv_id, const gap_ble_adv_params_t *param);

/**
 * @if Eng
 * @brief Use this funtion to start advertising.
 * @par   Use this funtion to start advertising
 * @attention NULL
 * @param  [in] adv_id advertising ID
 * @retval error code, the advertising status will be returned in { @ref gap_ble_start_adv_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  开始发送广播。
 * @par    开始发送广播。
 * @attention 无
 * @param  [in] adv_id 广播ID
 * @retval 执行结果错误码，广播状态将在 { @ref gap_ble_start_adv_callback } 中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_start_adv(uint8_t adv_id);

/**
 * @if Eng
 * @brief Use this funtion to stop advertising.
 * @par   Use this funtion to stop advertising
 * @attention NULL
 * @param  [in] adv_id advertising ID
 * @retval error code, the advertising status will be returned in { @ref gap_ble_stop_adv_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  停止发送广播。
 * @par    停止发送广播。
 * @attention 无
 * @param  [in] adv_id 广播ID
 * @retval 执行结果错误码，广播状态将在 { @ref gap_ble_stop_adv_callback } 中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_stop_adv(uint8_t adv_id);

/**
 * @if Eng
 * @brief Use this funtion to set scan parameter.
 * @par   Use this funtion to set scan parameter.
 * @attention NULL
 * @param  [in] param scan parameter.
 * @retval error code, the parameter set status will be returned in { @ref gap_ble_set_scan_param_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置扫描参数。
 * @par    设置扫描参数。
 * @attention 无
 * @param  [in] param 扫描参数。
 * @retval 执行结果错误码，参数设置状态将在 { @ref gap_ble_set_scan_param_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_scan_parameters(const gap_ble_scan_params_t *param);

/**
 * @if Eng
 * @brief Use this funtion to start scan.
 * @par   Use this funtion to start scan.
 * @attention NULL
 * @param  NULL.
 * @retval error code, the scan result will be returned at { @ref gap_ble_scan_result_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  开始扫描。
 * @par    开始扫描。
 * @attention 无
 * @param  无。
 * @retval 执行结果错误码，扫描结果将在 { @ref gap_ble_scan_result_callback }。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_start_scan(void);

/**
 * @if Eng
 * @brief Use this funtion to stop scan.
 * @par   Use this funtion to stop scan.
 * @attention NULL
 * @param  NULL.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  停止扫描。
 * @par    停止扫描。
 * @attention 无
 * @param  无。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_stop_scan(void);

/**
 * @if Eng
 * @brief Use this funtion to set the BLE PHY parameters.
 * @par   Use this funtion to set the BLE PHY parameters.
 * @attention NULL
 * @param  [in] param BLE PHY parameters.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置BLE PHY参数。
 * @par    设置BLE PHY参数。
 * @attention 无
 * @param  [in] param BLE PHY参数。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_phy(gap_le_set_phy_t *param);

/**
 * @if Eng
 * @brief Use this funtion to set the BLE packet transmission parameters.
 * @par   Use this funtion to set the BLE packet transmission parameters.
 * @attention NULL
 * @param  [in] param BLE packet transmission parameters.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置BLE发包参数。
 * @par    设置BLE发包参数。
 * @attention 无
 * @param  [in] param BLE发包参数。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_data_length(gap_le_set_data_length_t *param);

/**
 * @if Eng
 * @brief Use this funtion to start pairing.
 * @par   Use this funtion to start pairing.
 * @attention NULL
 * @param   [out]   addr   the list of paired devices address.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  启动配对。
 * @par    启动配对。
 * @attention 无
 * @param   [out]   addr   配对设备地址链表。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_pair_remote_device(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to get the number of paired devices.
 * @par   Use this funtion to get the number of paired devices.
 * @attention NULL
 * @param   [out] number number of paired devices.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  获取配对设备数量。
 * @par    获取配对设备数量。
 * @attention 无
 * @param   [out] number 配对设备数量。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_get_paired_devices_num(uint16_t *number);

/**
 * @if Eng
 * @brief Use this funtion to get the number of paired devices.
 * @par   Use this funtion to get the number of paired devices.
 * @attention NULL
 * @param   [out]   addr   the list of paired devices address.
 * @param   [inout] number as input parameter, the buffer size of user,
                           as output parameter, the number of paired devices.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  获取配对设备数量。
 * @par    获取配对设备数量。
 * @attention 无
 * @param   [out]   addr   配对设备地址链表。
 * @param   [inout] number 作为入参时为用户分配的内存大小，作为出参时为配对设备数量。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_get_paired_devices(bd_addr_t *addr, uint16_t *number);

/**
 * @if Eng
 * @brief Use this funtion to get the pairing state of a specific device.
 * @par   Use this funtion to get the pairing state of a specific device.
 * @attention NULL
 * @param  [in] addr address of the device.
 * @param  [out] status, status of the device.
 * @retval pair state
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  获取设备的配对状态。
 * @par    获取设备的配对状态。
 * @attention 无
 * @param  [in] addr 待查询的设备地址。
 * @param  [out] status 配对设备状态.
 * @retval 配对状态。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_get_pair_state(const bd_addr_t *addr, gap_ble_pair_state_t *status);

/**
 * @if Eng
 * @brief Use this funtion to remove pair of remote device.
 * @par   Use this funtion to remove pair of remote device.
 * @attention NULL
 * @param  [in] addr address of the remote device.
 * @retval error code
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  与指定设备取消配对。
 * @par    与指定设备取消配对。
 * @attention 无
 * @param  [in] addr 对端设备地址。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_remove_pair(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to remove all paired devices from the paired list.
 * @par   Use this funtion to remove all paired devices from the paired list.
 * @attention NULL
 * @param  NULL
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  删除所有BLE配对设备。
 * @par    删除所有BLE配对设备。
 * @attention 无
 * @param  无
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_remove_all_pairs(void);

/**
 * @if Eng
 * @brief Use this funtion to update ble connect param.
 * @par
 * @attention NULL
 * @param  [in] params user set connect param in it.
 * @retval #true Execute successfully
 * @retval #false Execution failed
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  ble连接参数更新
 * @par
 * @attention 无
 * @param  [in] param 待更新连接参数。
 * @retval #true  执行成功。
 * @retval #false  执行失败。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_connect_param_update(gap_conn_param_update_t *params);

/**
 * @if Eng
 * @brief Use this funtion to connect to a remote device.
 * @par   Use this funtion to establish an ACL connection with a remote device.
 * @attention NULL
 * @param  [in] addr address of the remote device.
 * @retval error code, the connect status will be returned at { @ref gap_ble_connect_state_changed_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  与设备建立连接。
 * @par    与设备建立ACL连接。
 * @attention 无
 * @param  [in] addr 待连接的设备地址。
 * @retval 执行结果错误码，连接状态将在{ @ref gap_ble_connect_state_changed_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_connect_remote_device(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to disconnect from a remote device.
 * @par   Use this funtion to disconnect all profiles from a remote device.
 * @attention NULL
 * @param  [in] addr address of the remote device.
 * @retval error code, the connect status will be returned at { @ref gap_ble_connect_state_changed_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  断开设备连接。
 * @par    断开设备连接，包括所有profile连接。
 * @attention 无
 * @param  [in] addr 待断开的设备地址。
 * @retval 执行结果错误码，连接状态将在{ @ref gap_ble_connect_state_changed_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_disconnect_remote_device(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to set security parameter.
 * @par   Use this funtion to set security parameter.
 * @attention NULL
 * @param  [in] param  a pointer to the security parameter.
 * @retval NULL。
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置安全参数。
 * @par    设置安全参数。
 * @attention 无
 * @param  [in] param  安全参数
 * @retval 无回调函数。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_set_sec_param(gap_ble_sec_params_t *params);

/**
 * @if Eng
 * @brief Use this funtion to read rssi of remote device.
 * @par   Use this funtion to read rssi of remote device by connect id.
 * @attention NULL
 * @param  [in] connect id.
 * @retval error code, the rssi result will be returned at { @ref gap_ble_read_rssi_callback }.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  读对端rssi值
 * @par    通过连接ID读对端rssi
 * @attention 无
 * @param  [in] 连接ID。
 * @retval 执行结果错误码，rssi 结果将在{ @ref gap_ble_read_rssi_callback }中返回。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_read_remote_device_rssi(uint16_t conn_id);

/**
 * @if Eng
 * @brief Use this funtion to register callback function of gap
 * @par   Use this funtion to register callback function of gap
 * @attention NULL
 * @param  [in] func gap_ble_callbacks_t *, a pointer to the callback functions.
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  注册ble gap回调函数。
 * @par    注册ble gap回调函数。
 * @attention 无
 * @param  [in] func 指向回调函数接口定义的指针
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_ble_register_callbacks(gap_ble_callbacks_t *func);

/**
 * @if Eng
 * @brief Use this funtion to initializes bth ota channel.
 * @par   Use this funtion to initializes bth ota channel.
 * @attention NULL
 * @param  NULL
 * @retval error code.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  初始化bth ota通道。
 * @par    初始化bth ota通道。
 * @attention 无
 * @param  NULL
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t bth_ota_init(void);

/**
 * @if Eng
 * @brief Use this funtion to config customize information.
 * @par   Use this funtion to config customize information.
 * @attention NULL
 * @param  [in] ble_pwr ble max power.
 * @param  [in] sle_pwr sle max power.
 * @retval error code.
 * @par Dependency:
 * @li  nv_common.h
 * @else
 * @brief  配置定制化信息
 * @par    配置定制化信息
 * @attention 无
 * @param  [in] ble_pwr ble 最大功率.
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bt_data_config.h
 * @endif
 */
errcode_t ble_customize_max_pwr(int8_t ble_pwr, int8_t sle_pwr);
#ifdef __cplusplus
}
#endif
#endif
/**
 * @}
 */
