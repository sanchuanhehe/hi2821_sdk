/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 HAL timer \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-08, Create file. \n
 */
#ifndef HAL_TIMER_V100_H
#define HAL_TIMER_V100_H

#include "hal_drv_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_timer_v100 TIMER V100
 * @ingroup  drivers_hal_timer
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the timer mode.
 * @else
 * @brief  Timer模式定义。
 * @endif
 */
typedef enum control_reg_mode {
    /** @if Eng  Timer mode: free run mode.
     *  @else    定时器控制模式：自由运行模式。
     *  @endif */
    TIMER_MODE_FREERUN,
    /** @if Eng  Timer mode: user define mode.
     *  @else    定时器控制模式：用户自定义模式。
     *  @endif */
    TIMER_MODE_USERDEF
} control_reg_mode_t;

/**
 * @if Eng
 * @brief  Get the interface instance of timer v100.
 * @return The interface instance of @ref hal_timer_funcs_t.
 * @else
 * @brief  获取Timer的接口实例。
 * @return @ref hal_timer_funcs_t 的接口实例。
 * @endif
 */
hal_timer_funcs_t *hal_timer_v100_get_funcs(void);

/**
 * @if Eng
 * @brief  Handler of the timer interrupt request.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  Timer中断处理函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_v100_irq_handler(timer_index_t index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
