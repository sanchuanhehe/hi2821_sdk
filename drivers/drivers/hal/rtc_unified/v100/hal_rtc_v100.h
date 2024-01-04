/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V100 HAL rtc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-03, Create file. \n
 */
#ifndef HAL_RTC_V100_H
#define HAL_RTC_V100_H

#include "hal_rtc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_rtc_v100 RTC V100
 * @ingroup  drivers_hal_rtc
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the RTC mode.
 * @else
 * @brief  RTC模式定义。
 * @endif
 */
typedef enum control_reg_mode {
    /** @if Eng  RTC mode: free run mode.
     *  @else    定时器控制模式：自由运行模式。
     *  @endif */
    RTC_MODE_FREE_RUN,
    /** @if Eng  RTC mode: user define mode.
     *  @else    定时器控制模式：用户自定义模式。
     *  @endif */
    RTC_MODE_USER_DEF
} hal_rtc_v100_ctrl_reg_mode_t;

/**
 * @if Eng
 * @brief  Get the interface instance of RTC v100.
 * @return The interface instance of @ref hal_rtc_funcs_t.
 * @else
 * @brief  获取RTC的接口实例。
 * @return @ref hal_rtc_funcs_t 的接口实例。
 * @endif
 */
hal_rtc_funcs_t *hal_rtc_v100_get_funcs(void);

/**
 * @if Eng
 * @brief  Handler of the RTC interrupt request.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  RTC中断处理函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_v100_irq_handler(rtc_index_t index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif