/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides hal keyscan \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef HAL_KEYSCAN_H
#define HAL_KEYSCAN_H

#include <stdint.h>
#include "errcode.h"
#include "keyscan_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_keyscan_api Keyscan
 * @ingroup  drivers_hal_keyscan
 * @{
 */

/**
 * @if Eng
 * @brief  Whether report status is release, press or error.
 * @else
 * @brief  上报的状态是释放、按下还是还是。
 * @endif
 */
typedef enum {
    KEY_RELEASE,                    /*!< @if Eng KEYSCAN release.
                                         @else   KEYSCAN按键状态为释放 @endif */
    KEY_PRESS,                      /*!< @if Eng KEYSCAN press.
                                         @else   KEYSCAN按键状态为按下 @endif */
    KEY_ERROR                       /*!< @if Eng KEYSCAN ERROR.
                                         @else   KEYSCAN按键超出键盘范围 @endif */
} key_value_report_status_t;

/**
 * @if Eng
 * @brief  Definition of keyscan callback type.
 * @param  [in]  argc Number of parameters.
 * @param  [in]  argv All input parameters.
 * @retval 0            Success.
 * @retval Other        Failure.
 * @else
 * @brief  Key_scan回调类型的定义。
 * @param  [in]  argc 参数的数目。
 * @param  [in]  argv 所有输入的参数。
 * @retval 0            成功。
 * @retval Other        失败。
 * @endif
 */
typedef int (*keyscan_report_callback_t)(int argc, uint8_t argv[]);

/**
 * @if Eng
 * @brief  Definition of the contorl ID of hal keyscan.
 * @else
 * @brief  KEYSCAN控制ID定义。
 * @endif
 */
typedef enum hal_keyscan_ctrl_id {
    KEYSCAN_CTRL_ENABLE = 0,            /*!< @if Eng Key_scan enable.
                                              @else   Key_scan使能 @endif */
    KEYSCAN_CTRL_DISABLE,               /*!< @if Eng Key_scan disable.
                                              @else   Key_scan去使能 @endif */
#if defined (CONFIG_KEYSCAN_SUPPORT_LPM)
    KEYSCAN_CTRL_SUSPEND,               /*!< @if Eng Key_scan suspend.
                                              @else   Key_scan通道挂起 @endif */
    KEYSCAN_CTRL_RESUME,                /*!< @if Eng Key_scan resume.
                                              @else   Key_scan通道恢复 @endif */
#endif
    KEYSCAN_CTRL_MAX,
} hal_keyscan_ctrl_id_t;

/**
 * @if Eng
 * @brief  Callback of keyscanfor hal keyscan.
 * @param  [in]  key_value Number of key value.
 * @else
 * @brief  HAL层keyscan回调函数。
 * @param  [in]  key_value 待处理的键值。
 * @endif
 */
typedef void (*hal_keyscan_callback_t)(uint16_t key_value);

/**
 * @if Eng
 * @brief  Initialize device for hal keyscan.
 * @param  [in]  time Scan time of the keyscan.
 * @param  [in]  mode Scan mode of the keyscan.
 * @param  [in]  event_type Intterrupt type of the keyscan.
 * @param  [in]  callback Callback of the hal keyscan.
 * @else
 * @brief  HAL层keyscan的初始化接口。
 * @param  [in]  time keyscan配置扫描时长。
 * @param  [in]  mode keyscan配置扫描模式。
 * @param  [in]  event_type keyscan配置中断类型。
 * @param  [in]  callback 回调函数。
 * @endif
 */
typedef void (*hal_keyscan_init_t)(keyscan_pulse_time_t time, keyscan_mode_t mode, keyscan_int_t event_type, \
                                   hal_keyscan_callback_t callback);

/**
 * @if Eng
 * @brief  Deinitialize device for hal keyscan.
 * @else
 * @brief  HAL层keyscan的去初始化接口。
 * @endif
 */
typedef void (*hal_keyscan_deinit_t)(void);

/**
 * @if Eng
 * @brief  Control interface for hal keyscan.
 * @param  [in]  id ID of the keyscan control.
 * @param  [in]  param param of the keyscan control.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层keyscan控制接口。
 * @param  [in]  id keyscan控制请求ID。
 * @param  [in]  param keyscan控制请求参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_keyscan_ctrl_t)(hal_keyscan_ctrl_id_t id, uintptr_t param);

/**
 * @if Eng
 * @brief  Interface between keyscan driver and keyscan hal.
 * @else
 * @brief  Driver层keyscan和HAL层keyscan的接口。
 * @endif
 */
typedef struct {
    hal_keyscan_init_t             init;                   /*!< @if Eng Init device interface.
                                                                 @else   HAL层keyscan的初始化接口 @endif */
    hal_keyscan_deinit_t           deinit;                 /*!< @if Eng Deinit device interface.
                                                                 @else   HAL层keyscan去初始化接口 @endif */
    hal_keyscan_ctrl_t             ctrl;                   /*!< @if Eng Control device interface.
                                                                 @else   HAL层keyscan控制接口 @endif */
} hal_keyscan_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_keyscan_funcs_t into the g_hal_keyscans_funcs.
 * @param  [in]  funcs Interface between keyscan driver and keyscan hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_keyscan_funcs_t 到 g_hal_keyscans_funcs
 * @param  [in]  funcs Driver层keyscan和HAL层keyscan的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_keyscan_register_funcs(hal_keyscan_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_keyscan_funcs_t from the g_hal_keyscans_funcs.
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从g_hal_keyscans_funcs注销 @ref hal_keyscan_funcs_t 。
 * @return ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_keyscan_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between keyscan driver and keyscan hal, see @ref hal_keyscan_funcs_t.
 * @return Interface between keyscan driver and keyscan hal, see @ref hal_keyscan_funcs_t.
 * @else
 * @brief  获取Driver层keyscan和HAL层keyscan的接口实例，参考 @ref hal_keyscan_funcs_t 。
 * @return Driver层keyscan和HAL层keyscan的接口实例，参考 @ref hal_keyscan_funcs_t 。
 * @endif
 */
hal_keyscan_funcs_t *hal_keyscan_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
