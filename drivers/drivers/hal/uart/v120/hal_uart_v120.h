/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V120 HAL uart \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-07, Create file. \n
 */
#ifndef HAL_UART_V120_H
#define HAL_UART_V120_H

#include "hal_uart.h"
#include "hal_uart_v120_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v120 UART V120
 * @ingroup  drivers_hal_uart
 * @{
 */

/**
 * @if Eng
 * @brief  Get the instance of v120.
 * @return The instance of v120.
 * @else
 * @brief  获取v120实例。
 * @return v120实例。
 * @endif
 */
hal_uart_funcs_t *hal_uart_v120_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the uart interrupt request.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @else
 * @brief  UART中断处理函数
 * @param  [in]  bus 串口号， 参考 @ref uart_bus_t
 * @endif
 */
void hal_uart_irq_handler(uart_bus_t bus);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif