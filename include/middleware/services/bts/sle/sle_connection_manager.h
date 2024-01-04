/**
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 *
 * Description: SLE Connection Manager module.
 */

/**
 * @defgroup sle_connection_manager connection manager API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_CONNECTION_MANAGER
#define SLE_CONNECTION_MANAGER

#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of sle pairing state.
 * @else
 * @brief 星闪配对状态。
 * @endif
 */
typedef enum {
    SLE_PAIR_NONE    = 0x01,    /*!< @if Eng Pair state of none
                                     @else   未配对状态 @endif */
    SLE_PAIR_PAIRING = 0x02,    /*!< @if Eng Pair state of pairing
                                     @else   正在配对 @endif */
    SLE_PAIR_PAIRED  = 0x03     /*!< @if Eng Pair state of paired
                                     @else   已完成配对 @endif */
} sle_pair_state_t;

/**
 * @if Eng
 * @brief Enum of sle pairing state.
 * @else
 * @brief 星闪断链原因。
 * @endif
 */
typedef enum {
    SLE_DISCONNECT_BY_REMOTE = 0x10,    /*!< @if Eng disconnect by remote
                                             @else   远端断链 @endif */
    SLE_DISCONNECT_BY_LOCAL  = 0x11,    /*!< @if Eng disconnect by local
                                             @else   本端断链 @endif */
} sle_disc_reason_t;

/**
 * @if Eng
 * @brief Enum of sle ACB connection state.
 * @else
 * @brief SLE ACB连接状态。
 * @endif
 */
typedef enum {
    SLE_ACB_STATE_NONE          = 0x00,   /*!< @if Eng SLE ACB connect state of none
                                               @else   SLE ACB 未连接状态 @endif */
    SLE_ACB_STATE_CONNECTED     = 0x01,   /*!< @if Eng SLE ACB connect state of connected
                                               @else   SLE ACB 已连接 @endif */
    SLE_ACB_STATE_DISCONNECTED  = 0x02,   /*!< @if Eng SLE ACB connect state of disconnected
                                               @else   SLE ACB 已断接 @endif */
} sle_acb_state_t;

/**
 * @if Eng
 * @brief Enum of sle crytography algorithm.
 * @else
 * @brief 星闪加密算法类型。
 * @endif
 */
typedef enum {
    SLE_CRYTO_ALGO_AC1     = 0x00,   /*!< @if Eng crytography algorithm ac1
                                          @else   AC1加密算法类型 @endif */
    SLE_CRYTO_ALGO_AC2     = 0x01,   /*!< @if Eng crytography algorithm ac2
                                          @else   AC2加密算法类型@endif */
    SLE_CRYTO_ALGO_EA1     = 0x02,   /*!< @if Eng crytography algorithm ea1
                                          @else   EA1加密算法类型 @endif */
    SLE_CRYTO_ALGO_EA2     = 0x03,   /*!< @if Eng crytography algorithm ea2
                                          @else   EA2加密算法类型 @endif */
} sle_crypto_algo_t;

/**
 * @if Eng
 * @brief Enum of sle key derivation algorithm
 * @else
 * @brief 星闪秘钥分发算法类型。
 * @endif
 */
typedef enum {
    SLE_KEY_DERIV_ALGO_HA1     = 0x00,   /*!< @if Eng key derivation algorithm ac1
                                              @else   HA1秘钥分发算法类型 @endif */
    SLE_KEY_DERIV_ALGO_HA2     = 0x01,   /*!< @if Eng key derivation algorithm ac2
                                              @else   HA2秘钥分发算法类型 @endif */
} sle_key_deriv_algo_t;

/**
 * @if Eng
 * @brief Enum of sle integrity check indicator
 * @else
 * @brief 星闪完整性校验指示类型。
 * @endif
 */
typedef enum {
    SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_ENABLE      = 0x00,   /*!< @if Eng Encryption and integrity check
                                                                           are enabled at the same time.
                                                                   @else   加密和完整性保护同时启动 @endif */
    SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_ENABLE     = 0x01,   /*!< @if Eng Do not enable encryption, but enable
                                                                           integrity check.
                                                                   @else   不启动加密，启动完整性保护 @endif */
    SLE_ENCRYPTION_ENABLE_INTEGRITY_CHK_DISABLE     = 0x02,   /*!< @if Eng Encryption is enabled, but integrity
                                                                           check is disabled.
                                                                   @else   启动加密，不启动完整性保护 @endif */
    SLE_ENCRYPTION_DISABLE_INTEGRITY_CHK_DISABLE    = 0x03,   /*!< @if Eng Encryption and integrity check
                                                                           are not enabled.
                                                                   @else   不启动加密，不启动完整性保护 @endif */
} sle_integr_chk_ind_t;

/**
 * @if Eng
 * @brief Enum of sle logical link update parameters.
 * @else
 * @brief 星闪逻辑链路更新参数请求
 * @endif
 */
typedef struct {
    uint16_t interval_min;        /*!< @if Eng minimum interval
                                       @else   链路调度最小间隔，单位slot @endif */
    uint16_t interval_max;        /*!< @if Eng maximum interval
                                       @else   链路调度最大间隔，单位slot @endif */
    uint16_t max_latency;         /*!< @if Eng maximum latency
                                       @else   延迟周期，单位slot @endif */
    uint16_t supervision_timeout; /*!< @if Eng timeout
                                       @else   超时时间，单位10ms @endif */
} sle_connection_param_update_req_t;

/**
 * @if Eng
 * @brief Enum of sle logical link update parameters.
 * @else
 * @brief 星闪逻辑链路更新参数
 * @endif
 */
typedef struct {
    uint16_t conn_id;             /*!< @if Eng connection ID
                                       @else   连接ID @endif */
    uint16_t interval_min;        /*!< @if Eng minimum interval
                                       @else   链路调度最小间隔，单位slot @endif */
    uint16_t interval_max;        /*!< @if Eng maximum interval
                                       @else   链路调度最大间隔，单位slot @endif */
    uint16_t max_latency;         /*!< @if Eng maximum latency
                                       @else   延迟周期，单位slot @endif */
    uint16_t supervision_timeout; /*!< @if Eng timeout
                                       @else   超时时间，单位10ms @endif */
} sle_connection_param_update_t;

/**
 * @if Eng
 * @brief Enum of sle logical link update event parameters.
 * @else
 * @brief 星闪逻辑链路更新事件参数
 * @endif
 */
typedef struct {
    uint16_t interval;              /*!< @if Eng interval
                                         @else   链路调度间隔，单位slot @endif */
    uint16_t latency;               /*!< @if Eng latency
                                         @else   延迟周期，单位slot @endif */
    uint16_t supervision;           /*!< @if Eng timeout
                                         @else   超时时间，单位10ms @endif */
} sle_connection_param_update_evt_t;

/**
 * @if Eng
 * @brief Enum of sle authentication result.
 * @else
 * @brief 星闪认证结果
 * @endif
 */
typedef struct {
    uint8_t link_key[SLE_LINK_KEY_LEN];      /*!< @if Eng link key
                                                  @else   链路密钥 @endif */
    uint8_t crypto_algo;                     /*!< @if Eng encryption algorithm type { @ref sle_crypto_algo_t }
                                                  @else   加密算法类型 { @ref sle_crypto_algo_t } @endif */
    uint8_t key_deriv_algo;                  /*!< @if Eng key distribution algorithm type { @ref sle_key_deriv_algo_t }
                                                  @else   秘钥分发算法类型 { @ref sle_key_deriv_algo_t } @endif */
    uint8_t integr_chk_ind;                  /*!< @if Eng integrity check indication { @ref sle_integr_chk_ind_t }
                                                  @else   完整性校验指示 { @ref sle_integr_chk_ind_t } @endif */
} sle_auth_info_evt_t;


/**
 * @if Eng
 * @brief Callback invoked when connect state changed.
 * @par Callback invoked when connect state changed.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id    connection ID.
 * @param [in] addr       address.
 * @param [in] conn_state connection state { @ref sle_acb_state_t }.
 * @param [in] pair_state pairing state { @ref sle_pair_state_t }.
 * @param [in] disc_reason the reason of disconnect { @ref sle_disc_reason_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  连接状态改变的回调函数。
 * @par    连接状态改变的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] conn_id    连接 ID。
 * @param [in] addr       地址。
 * @param [in] conn_state 连接状态 { @ref sle_acb_state_t }。
 * @param [in] pair_state 配对状态 { @ref sle_pair_state_t }。
 * @param [in] disc_reason 断链原因 { @ref sle_disc_reason_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*sle_connect_state_changed_callback)(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason);

/**
 * @if Eng
 * @brief Callback invoked when connect parameter updated.
 * @par Callback invoked when connect parameter updated.
 * @attention 1.This function is called in SLE service context, should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id    connection ID.
 * @param [in] addr       address.
 * @param [in] status     error code.
 * @param [in] param      connection param.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  连接参数更新的回调函数。
 * @par    连接参数更新的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] conn_id    连接 ID。
 * @param [in] addr       地址。
 * @param [in] status     执行结果错误码。
 * @param [in] param      连接参数。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*sle_connect_param_update_callback)(uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param);

/**
 * @if Eng
 * @brief Callback invoked before the request for updating the connect parameter is complete.
 * @par Callback invoked before the request for updating the connect parameter is complete.
 * @attention 1.This function is called in SLE service context, should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id    connection ID.
 * @param [in] status     error code.
 * @param [in] param      connection param.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  连接参数更新请求完成前的回调函数。
 * @par    连接参数更新请求完成前的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] conn_id    连接 ID。
 * @param [in] status     执行结果错误码。
 * @param [in] param      连接参数。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*sle_connect_param_update_req_callback)(uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_req_t *param);

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
typedef void (*sle_auth_complete_callback)(uint16_t conn_id, const sle_addr_t *addr, errcode_t status,
    const sle_auth_info_evt_t* evt);

/**
 * @if Eng
 * @brief Callback invoked when pairing complete.
 * @par Callback invoked when pairing complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id connection ID.
 * @param [in] addr    address.
 * @param [in] status  error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  配对完成的回调函数。
 * @par    配对完成的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] conn_id 连接 ID。
 * @param [in] addr    地址。
 * @param [in] status  执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*sle_pair_complete_callback)(uint16_t conn_id, const sle_addr_t *addr, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when rssi read complete.
 * @par Callback invoked when rssi read complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] conn_id connection ID.
 * @param [in] rssi    rssi.
 * @param [in] status  error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  读取rssi的回调函数。
 * @par    读取rssi的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] conn_id 连接 ID。
 * @param [in] rssi    rssi。
 * @param [in] status  执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*sle_read_rssi_callback)(uint16_t conn_id, int8_t rssi, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when set low latency complete.
 * @par Callback invoked when set low latency complete.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param [in] status result of set low latency.
 * @param [in] addr   remote device address.
 * @param [in] rate   mouse report rate { @ref sle_low_latency_rate_t }.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @else
 * @brief  设置low latency的回调函数。
 * @par    设置low latency的回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param [in] status 设置low latency结果。
 * @param [in] addr   对端设备地址。
 * @param [in] rate   鼠标回报率 { @ref sle_low_latency_rate_t }。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_connection_callbacks_t
 * @endif
 */
typedef void (*sle_low_latency_callback)(uint8_t status, sle_addr_t *addr, uint8_t rate);

/**
 * @if Eng
 * @brief Struct of SLE connection manager callback function.
 * @else
 * @brief SLE连接管理回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_connect_state_changed_callback connect_state_changed_cb;         /*!< @if Eng Connect state changed callback.
                                                                            @else   连接状态改变回调函数。 @endif */
    sle_connect_param_update_req_callback connect_param_update_req_cb;   /*!< @if Eng Connect param updated callback.
                                                                            @else   连接参数更新回调函数。 @endif */
    sle_connect_param_update_callback connect_param_update_cb;           /*!< @if Eng Connect param updated callback.
                                                                            @else   连接参数更新回调函数。 @endif */
    sle_auth_complete_callback auth_complete_cb;                         /*!< @if Eng Authentication complete callback.
                                                                            @else   认证完成回调函数。 @endif */
    sle_pair_complete_callback pair_complete_cb;                         /*!< @if Eng Pairing complete callback.
                                                                            @else   配对完成回调函数。 @endif */
    sle_read_rssi_callback read_rssi_cb;                                 /*!< @if Eng Read rssi callback.
                                                                            @else   读取rssi回调函数。 @endif */
    sle_low_latency_callback low_latency_cb;                             /*!< @if Eng Set low latency callback.
                                                                            @else   设置low latency回调函数。 @endif */
} sle_connection_callbacks_t;

/**
 * @brief  星闪phy参数
 */
typedef struct {
    uint8_t tx_format;          /*!< 发送无线帧类型，参考gle_radio_frame_type_t */
    uint8_t rx_format;          /*!< 接收无线帧类型，参考gle_radio_frame_type_t */
    uint8_t tx_phy;             /*!< 发送PHY，参考gle_tx_rx_phy_t */
    uint8_t rx_phy;             /*!< 接收PHY，参考gle_tx_rx_phy_t */
    uint8_t tx_pilot_density;   /*!< 发送导频密度指示，参考gle_tx_rx_pilot_density_t */
    uint8_t rx_pilot_density;   /*!< 接收导频密度指示，参考gle_tx_rx_pilot_density_t */
    uint8_t g_feedback;         /*!< 先发链路反馈类型指示，取值范围0-63。
                                     0：指示基于CBG的反馈
                                     1-25：指示不携带数据信息场景组播反馈信息的比特位置, 其中位置信息为指示信息的数值
                                     26：指示基于TB的反馈
                                     27-34：指示携带数据信息场景组播反馈信息的比特位置，其中位置信息为(指示信息的数值-26)
                                     35-63：预留 */
    uint8_t t_feedback;         /*!< 后发链路反馈类型指示，取值范围0-7
                                     0-5：指示半可靠组播反馈，并指示采用m序列的编号
                                     6：指示基于CBG的反馈
                                     7：指示基于TB的反馈 */
} sle_set_phy_t;

/**
 * @if Eng
 * @brief  Send connect request to remote device.
 * @par Description: Send connect request to remote device.
 * @param [in] addr address.
 * @retval error code, connection state change result will be returned at { @ref sle_connect_state_changed_callback }.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送连接请求。
 * @par Description: 发送连接请求。
 * @param [in] addr 地址。
 * @retval 执行结果错误码， 连接状态改变结果将在 { @ref sle_connect_state_changed_callback }中返回。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_connect_remote_device(const sle_addr_t *addr);

/**
 * @if Eng
 * @brief  Send disconnect request to remote device.
 * @par Description: Send disconnect request to remote device.
 * @param [in] addr address.
 * @retval error code, connection state change result will be returned at { @ref sle_connect_state_changed_callback }.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送断开连接请求。
 * @par Description: 发送断开连接请求。
 * @param [in] addr 地址。
 * @retval 执行结果错误码， 连接状态改变结果将在 { @ref sle_connect_state_changed_callback }中返回。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_disconnect_remote_device(const sle_addr_t *addr);

/**
 * @if Eng
 * @brief  Send connection parameter update request to remote device.
 * @par Description: Send connection parameter update request to remote device.
 * @param [in] params connection parameter.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送更新连接参数请求。
 * @par Description: 发送更新连接参数请求。
 * @param [in] params 连接参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_update_connect_param(sle_connection_param_update_t *params);

/**
 * @if Eng
 * @brief  Send pairing request to remote device.
 * @par Description: Send pairing request to remote device.
 * @param [in] addr address.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  发送配对请求。
 * @par Description: 发送配对请求。
 * @param [in] addr 地址。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_pair_remote_device(const sle_addr_t *addr);

/**
 * @if Eng
 * @brief  Remove pairing.
 * @par Description: Remove pairing.
 * @param [in] addr address.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  删除配对。
 * @par Description: 删除配对。
 * @param [in] addr 地址。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_remove_paired_remote_device(const sle_addr_t *addr);

/**
 * @if Eng
 * @brief  Remove all pairing.
 * @par Description: Remove all pairing.
 * @param NULL
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  删除所有配对。
 * @par Description: 删除所有配对。
 * @param NULL
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_remove_all_pairs(void);

/**
 * @if Eng
 * @brief  Get paired device number.
 * @par Description: Get paired device number.
 * @param [out] number device number.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取配对设备数量。
 * @par Description: 获取配对设备数量。
 * @param [out] number 设备数量。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_get_paired_devices_num(uint16_t *number);

/**
 * @if Eng
 * @brief  Get paired device.
 * @par Description: Get paired device.
 * @param [out]   addr   linked list of device address.
 * @param [inout] number device number.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取配对设备。
 * @par Description: 获取配对设备。
 * @param [out]   addr   设备地址链表。
 * @param [inout] number 设备数量。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_get_paired_devices(sle_addr_t *addr, uint16_t *number);

/**
 * @if Eng
 * @brief  Get pair state.
 * @par Description: Get pair state.
 * @param [in]  addr  device address.
 * @param [out] state pair state { @ref sle_pair_state_t }.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  获取配对状态。
 * @par Description: 获取配对状态。
 * @param [in]  addr  设备地址。
 * @param [out] state 配对状态 { @ref sle_pair_state_t }。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_get_pair_state(const sle_addr_t *addr, uint8_t *state);

/**
 * @if Eng
 * @brief  Read remote device rssi value.
 * @par Description: Read remote device rssi value.
 * @param [in]  conn_id connection ID.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  读取对端设备rssi值。
 * @par Description: 读取对端设备rssi值。
 * @param [in]  conn_id 连接 ID。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_read_remote_device_rssi(uint16_t conn_id);

/**
 * @if Eng
 * @brief  Set sle phy.
 * @par Description: Set sle phy.
 * @param [in]  param sle phy param.
 * @retval error code.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief 设置PHY参数
 * @par Description: 设置PHY参数。
 * @param [in]  param 星闪的PHY参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_set_phy_param(uint16_t conn_hdl, sle_set_phy_t *param);

/**
 * @if Eng
 * @brief  Register SLE connection manager callbacks.
 * @par Description: Register SLE connection manager callbacks.
 * @param [in] func Callback function.
 * @retval error code.
 * @else
 * @brief  注册SLE连接管理回调函数。
 * @par Description: 注册SLE连接管理回调函数。
 * @param [in] func 回调函数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t sle_connection_register_callbacks(sle_connection_callbacks_t *func);

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
 * @param  [in] sle_pwr ble 最大功率.
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li  bt_data_config.h
 * @endif
 */
errcode_t sle_customize_max_pwr(int8_t ble_pwr, int8_t sle_pwr);

#ifdef __cplusplus
}
#endif
#endif /* SLE_CONNECTION_MANAGER */
/**
 * @}
 */
