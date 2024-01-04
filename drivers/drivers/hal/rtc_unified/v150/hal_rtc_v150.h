/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V150 HAL rtc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-12-08, Create file. \n
 */
#ifndef HAL_RTC_V150_H
#define HAL_RTC_V150_H

#include <stdint.h>
#include "hal_rtc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_rtc_v150 RTC V150
 * @ingroup  drivers_hal_rtc
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the rtc mode.
 * @else
 * @brief  Timer模式定义。
 * @endif
 */
typedef enum control_reg_mode_v150 {
    /** @if Eng  Timer mode: one shot mode(Same as RTC_V150_MODE_ONE_SHOT_1).
     *  @else    定时器控制模式：单触发模式（等同于RTC_V150_MODE_ONE_SHOT_1）。
     *  @endif */
    RTC_V150_MODE_ONE_SHOT,
    /** @if Eng  Timer mode: periodic mode.
     *  @else    定时器控制模式：周期触发模式。
     *  @endif */
    RTC_V150_MODE_PERIODIC,
    /** @if Eng  Timer mode: one shot mode(Same as RTC_V150_MODE_ONE_SHOT).
     *  @else    定时器控制模式：单触发模式（等同于RTC_V150_MODE_ONE_SHOT）。
     *  @endif */
    RTC_V150_MODE_ONE_SHOT_1,
    /** @if Eng  Timer mode: free run mode.
     *  @else    定时器控制模式：自由运行模式。
     *  @endif */
    RTC_V150_MODE_FREERUN
} control_reg_mode_v150_t;

/**
 * @if Eng
 * @brief  Get the interface instance of rtc V150.
 * @return The interface instance of @ref hal_rtc_funcs_t.
 * @else
 * @brief  获取Timer的接口实例。
 * @return @ref hal_rtc_funcs_t 的接口实例。
 * @endif
 */
hal_rtc_funcs_t *hal_rtc_v150_get_funcs(void);

/**
 * @if Eng
 * @brief  Handler of the rtc interrupt request.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  rtc中断处理函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v150_irq_handler(rtc_index_t index);

/**
 * @if Eng
 * @brief  clear rtc interrupt.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  清除rtc模块内部的中断。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v150_interrupt_clear(rtc_index_t index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif