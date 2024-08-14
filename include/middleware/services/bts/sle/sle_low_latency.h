/**
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 *
 * Description: SLE low latency.
 */

/**
 * @defgroup SLE low latency API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_LOW_LATENCY_H
#define SLE_LOW_LATENCY_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  low latency support rate
 * @else
 * @brief  低时延调度速率。
 * @endif
 */
typedef enum {
    SLE_LOW_LATENCY_125HZ = 0,  /*!< @if Eng 125HZ.
                                     @else 125HZ 调度。 @endif */
    SLE_LOW_LATENCY_500HZ,      /*!< @if Eng 500HZ.
                                     @else 500HZ 调度。 @endif */
    SLE_LOW_LATENCY_1K,         /*!< @if Eng 1000HZ.
                                     @else 1000Hz 调度。 @endif */
    SLE_LOW_LATENCY_2K,         /*!< @if Eng 2000HZ.
                                     @else 2000HZ 调度。 @endif */
    SLE_LOW_LATENCY_3K,         /*!< @if Eng 3000HZ.
                                     @else 3000HZ 调度。 @endif */
    SLE_LOW_LATENCY_4K,         /*!< @if Eng 4000HZ.
                                     @else 4000Hz 调度。 @endif */
    SLE_LOW_LATENCY_5K,         /*!< @if Eng 5000HZ.
                                     @else 5000Hz 调度。 @endif */
    SLE_LOW_LATENCY_6K,         /*!< @if Eng 6000HZ.
                                     @else 6000Hz 调度。 @endif */
    SLE_LOW_LATENCY_7K,         /*!< @if Eng 7000HZ.
                                     @else 7000Hz 调度。 @endif */
    SLE_LOW_LATENCY_8K,         /*!< @if Eng 8000HZ.
                                     @else 8000Hz 调度。 @endif */
    SLE_LOW_LATENCY_MAX,
} sle_low_latency_rate_t;

/**
 * @if Eng
 * @brief  low latency enable.
 * @else
 * @brief  低时延使能。
 * @endif
 */
typedef enum {
    SLE_LOW_LATENCY_DISABLE = 0, /*!< @if Eng close low latency
                                      @else 关闭低时延调度 @endif */
    SLE_LOW_LATENCY_ENABLE       /*!< @if Eng open low latency
                                      @else 打开低时延 @endif */
} sle_low_latency_status_t;

/**
 * @if Eng
 * @brief  low latency enable.
 * @else
 * @brief  低时延使能。
 * @endif
 */
typedef enum {
    SLE_LOW_LATENCY_VALUE_GET_SUCCESS = 0, /*!< @if Eng get mouse value success
                                                @else 获取鼠标数据成功 @endif */
    SLE_LOW_LATENCY_VALUE_GET_FAIL        /*!< @if Eng get mouse value fail，No data is sent after failure
                                               @else 获取鼠标数据失败, 失败后不会发送数据 @endif */
} sle_low_latency_value_set_status_t;

/**
 * @brief  星闪层低时延配置接口
 */
typedef struct {
    uint16_t conn_id; /*!< @if Eng connection ID.
                           @else 连接ID @endif */
    uint8_t  enable;  /*!< @if Eng low latency enable, { @ref @ref sle_low_latency_rate_t }
                           @else 低时延使能状态, { @ref @ref sle_low_latency_rate_t } @endif */
    uint8_t  rate;    /*!< @if Eng low latency rate, { @ref @ref sle_low_latency_rate_t }.
                           @else 低时延调度速率, { @ref @ref sle_low_latency_rate_t }  @endif */
} sle_set_acb_low_latency_t;

/**
 * @if Eng
 * @brief The callback interface for sending data in mouse mode.
 * @param [out] button_mask button value.
 * @param [out] x Mouse x-coordinate.
 * @param [out] y Mouse y-coordinate.
 * @param [out] wheel Mouse wheel
 * @retval error code, { @ref @ref sle_low_latency_value_set_status_t }
 * @else
 * @brief Mouse模式数据发送回调接口定义。
 * @param [out] button_mask 按键值.
 * @param [out] x 鼠标X坐标.
 * @param [out] y 鼠标Y坐标.
 * @param [out] wheel Mouse 滚轮
 * @retval error code，{ @ref @ref sle_low_latency_value_set_status_t } .
 * @endif
 */
typedef errcode_t (*low_latency_key_value_set_callback)(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel);

/**
 * @if Eng
 * @brief Definition of the low-latency mouse callback function interface.
 * @else
 * @brief sle 低时延鼠标回调函数接口定义。
 * @endif
 */
typedef struct {
    low_latency_key_value_set_callback set_value_cb;  /*!< @if Eng Discovery structure callback.
                                                           @else 发现服务回调函数。 @endif */
} sle_low_latency_mouse_callbacks_t;

/**
 * @if Eng
 * @brief  Mouse mode initialization.
 * @par Description: SLE low-latency mouse enable.
 * @retval error code.
 * @else
 * @brief  Mouse模式初始化。
 * @par Description: SLE 低时延鼠标使能。
 * @retval error code.
 * @endif
 */
errcode_t sle_low_latency_mouse_enable(void);

/**
 * @if Eng
 * @brief  Definition of Data Obtaining and Invoking in Mouse Mode.
 * @par Description: Low-Latency Mouse Callback Method Registration.
 * @param [in] func Callback function。
 * @retval error code.
 * @else
 * @brief  Mouse模式数据获取调定义。
 * @par Description: 低时延鼠标回调方法注册。
 * @param [in] func 回调函数。
 * @retval error code.
 * @endif
 */
errcode_t sle_low_latency_mouse_register_callbacks(sle_low_latency_mouse_callbacks_t *mouse_cbk);

/**
 * @if Eng
 * @brief  Definition of Data Obtaining and Invoking in Mouse Mode.
 * @par Description: Low-Latency Mouse Callback Method Registration.
 * @param [in] func Callback function。
 * @retval error code.
 * @else
 * @brief  Mouse模式数据获取调定义。
 * @par Description: 低时延鼠标回调方法注册。
 * @param [in] func 回调函数。
 * @retval error code.
 * @endif
 */
typedef void(*low_latency_report_callback)(uint8_t *data, uint8_t len);

/**
 * @if Eng
 * @brief Struct of ssap client callback function.
 * @else
 * @brief ssap client回调函数接口定义。
 * @endif
 */
typedef struct {
    low_latency_report_callback report_cb;  /*!< @if Eng Discovery structure callback.
                                                 @else 发现服务回调函数。 @endif */
} sle_low_latency_dongle_callbacks_t;

/**
 * @if Eng
 * @brief  Mouse mode initialization.
 * @par Description: SLE low-latency dongle enable.
 * @attention NULL
 * @param [in] conn_id 公开ID。
 * @retval error code.
 * @else
 * @brief  Mouse模式初始化。
 * @par Description: SLE 低时延Dongle使能。
 * @attention NULL
 * @retval error code.
 * @endif
 */
errcode_t sle_low_latency_dongle_enable(void);

/**
 * @if Eng
 * @brief  Low-Latency Enabling and Scheduling Parameter Configurationn.
 * @par Description: Low-Latency Enabling and Scheduling Parameter Configuration.
 * @param [in] param  Scheduling parameters.
 * @retval error code.
 * @else
 * @brief  低时延使能和调度参数配置。
 * @par Description: 低时延使能和调度参数配置。
 * @param [in] param 调度参数.
 * @retval error code.
 * @endif
 */
errcode_t sle_low_latency_set(uint16_t conn_id, uint8_t enable, uint8_t  rate);

/**
 * @if Eng
 * @brief  Definition of Data Obtaining and Invoking in Mouse Mode.
 * @par Description: Low-Latency Mouse Callback Method Registration.
 * @param [in] func Callback function。
 * @retval error code.
 * @else
 * @brief  Dongle模式数据获取回调定义。
 * @par Description: 低时延Dongle回调方法注册。
 * @param [in] func 回调函数。
 * @retval error code.
 * @endif
 */
errcode_t sle_low_latency_dongle_register_callbacks(sle_low_latency_dongle_callbacks_t *dongle_cbk);

#ifdef __cplusplus
}
#endif
#endif /* SLE_LOW_LATENCY_H */
/**
 * @}
 */
