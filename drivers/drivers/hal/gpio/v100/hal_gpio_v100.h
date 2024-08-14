/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 HAL gpio \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */
#ifndef HAL_GPIO_V100_H
#define HAL_GPIO_V100_H

#include "hal_gpio.h"
#include "hal_gpio_v100_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v100 GPIO V100
 * @ingroup  drivers_hal_gpio
 * @{
 */

/**
 * @if Eng
 * @brief  Get the interface instance of GPIO V100.
 * @return The interface instance of @ref hal_gpio_funcs_t.
 * @else
 * @brief  获取GPIO的接口实例。
 * @return @ref hal_gpio_funcs_t 的接口实例。
 * @endif
 */
hal_gpio_funcs_t *hal_gpio_v100_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the gpio interrupt request.
 * @else
 * @brief  GPIO中断处理函数
 * @endif
 */
void hal_gpio_irq_handler(void);

/**
 * @if Eng
 * @brief  Get the context of current module, see @ref gpio_context_t.
 * @return The context of gpio module, see @ref gpio_context_t.
 * @else
 * @brief  获取当前模块的上下文描述，参考 @ref gpio_context_t.
 * @return gpio 模块的上下文描述，参考 @ref gpio_context_t.
 * @endif
 */
gpio_context_t *gpio_context_get(void);

/**
 * @if Eng
 * @brief  Get the context of current module, see @ref hal_gpio_context_t.
 * @return The context of gpio module, see @ref hal_gpio_context_t.
 * @else
 * @brief  获取当前模块的上下文描述，参考 @ref hal_gpio_context_t.
 * @return 当前模块的上下文描述，参考 @ref hal_gpio_context_t.
 * @endif
 */
hal_gpio_context_t *hal_gpio_context_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif