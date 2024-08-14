/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides V150 HAL GPIO \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

#ifndef HAL_GPIO_V150_H
#define HAL_GPIO_V150_H

#include "hal_gpio.h"
#include "hal_gpio_v150_regs_op.h"
#include "hal_gpio_v150_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v150 GPIO V150
 * @ingroup  drivers_hal_gpio
 * @{
 */

/**
 * @if Eng
 * @brief  GPIO LEVEL.
 * @else
 * @brief  GPIO高低电平。
 * @endif
 */
typedef enum hal_gpio_level {
    HAL_GPIO_LEVEL_LOW,
    HAL_GPIO_LEVEL_HIGH,
} hal_gpio_level_t;

/**
 * @if Eng
 * @brief  GPIO directions.
 * @else
 * @brief  GPIO输入输出方向。
 * @endif
 */
typedef enum hal_gpio_direction {
    HAL_GPIO_DIRECTION_OUTPUT,
    HAL_GPIO_DIRECTION_INPUT
} hal_gpio_direction_t;

/**
 * @if Eng
 * @brief  GPIO ctrl mode.
 * @else
 * @brief  GPIO控制模式。
 * @endif
 */
typedef enum hal_gpio_ctrl_mode {
    HAL_GPIO_CTRL_MODE_SW,
    HAL_GPIO_CTRL_MODE_HW
} hal_gpio_ctrl_mode_t;

/**
 * @if Eng
 * @brief  GPIO interrupt enable.
 * @else
 * @brief  GPIO中断使能。
 * @endif
 */
typedef enum hal_gpio_intr_enable {
    HAL_GPIO_INTR_DISABLE,
    HAL_GPIO_INTR_ENABLE
} hal_gpio_intr_enable_t;

/**
 * @if Eng
 * @brief  GPIO interrupt mask.
 * @else
 * @brief  GPIO中断屏蔽。
 * @endif
 */
typedef enum hal_gpio_intr_mask {
    HAL_GPIO_INTR_UNMASK,
    HAL_GPIO_INTR_MASK
} hal_gpio_intr_mask_t;

/**
 * @if Eng
 * @brief  GPIO debounce enable/disable.
 * @else
 * @brief  GPIO去毛刺使能或去使能。
 * @endif
 */
typedef enum hal_gpio_debounce {
    HAL_GPIO_DEBOUNCE_DISABLED,     /*!< @if Eng GPIO debounce disabled.
                                         @else   GPIO去毛刺不使能。 @endif */
    HAL_GPIO_DEBOUNCE_ENABLED,      /*!< @if Eng GPIO debounce enabled.
                                         @else   GPIO去毛刺使能。 @endif */
} hal_gpio_debounce_t;

/**
 * @if Eng
 * @brief  GPIO debounce enable/disable.
 * @else
 * @brief  GPIO中断清除。
 * @endif
 */
typedef enum hal_gpio_intr_clr {
    HAL_GPIO_INTR_NOT_CLEAR,        /*!< @if Eng GPIO interrupt not clear.
                                         @else   GPIO中断不清除。 @endif */
    HAL_GPIO_INTR_CLEAR,            /*!< @if Eng GPIO interrupt clear.
                                         @else   GPIO中断清除。 @endif */
} hal_gpio_intr_clr_t;

/**
 * @if Eng
 * @brief  Get the interface instance of GPIO V150.
 * @return The interface instance of @ref hal_gpio_funcs_t.
 * @else
 * @brief  获取GPIO的接口实例。
 * @return @ref hal_gpio_funcs_t 的接口实例。
 * @endif
 */
hal_gpio_funcs_t *hal_gpio_v150_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the GPIO interrupt request.
 * @else
 * @brief  GPIO中断处理函数。
 * @endif
 */
void hal_gpio_v150_irq_handler(void);

/**
 * @if Eng
 * @brief  Get the context of current module, see @ref gpio_context_t.
 * @return The context of GPIO module, see @ref gpio_context_t.
 * @else
 * @brief  获取当前模块的上下文描述，参考 @ref gpio_context_t 。
 * @return 当前模块的上下文描述，参考 @ref gpio_context_t 。
 * @endif
 */
gpio_context_t *gpio_v150_context_get(void);

/**
 * @if Eng
 * @brief  Get the HAL context of current module, see @ref hal_gpio_context_t.
 * @return The HAL context of GPIO module, see @ref hal_gpio_context_t.
 * @else
 * @brief  获取当前HAL层模块的上下文描述，参考 @ref hal_gpio_context_t 。
 * @return 当前HAL层模块的上下文描述，参考 @ref hal_gpio_context_t 。
 * @endif
 */
hal_gpio_context_t *hal_gpio_v150_context_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
