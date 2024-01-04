/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides v150 hal sio \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-15, Create file. \n
 */
#ifndef HAL_SIO_V150_H
#define HAL_SIO_V150_H

#include "hal_sio_v150_regs_op.h"
#include "hal_sio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sio_v150 SIO V150
 * @ingroup  drivers_hal_sio
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface between sio driver and sio hal.
 * @else
 * @brief  获取Driver层SIO和HAL层SIO的接口
 * @endif
 */
hal_sio_funcs_t *hal_sio_v150_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the sio interrupt request.
 * @param  [in]  bus The sio bus. see @ref sio_bus_t.
 * @else
 * @brief  SIO中断处理函数
 * @param  [in]  bus 串口号， 参考 @ref sio_bus_t.
 * @endif
 */
void hal_sio_v150_irq_handler(sio_bus_t bus);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif