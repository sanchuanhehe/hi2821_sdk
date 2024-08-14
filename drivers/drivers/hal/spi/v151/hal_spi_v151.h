/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 HAL spi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_V151_H
#define HAL_SPI_V151_H

#include "hal_spi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_v100 SPI V100
 * @ingroup  drivers_hal_spi
 * @{
 */

/**
 * @if Eng
 * @brief  Get the interface of spi_v151_funcs.
 * @return The interface of spi_v151_funcs.
 * @else
 * @brief  获取spi_v151_funcs接口实例。
 * @return spi_v151_funcs接口实例。
 * @endif
 */
hal_spi_funcs_t *hal_spi_v151_funcs_get(void);

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
/**
 * @if Eng
 * @brief  Handler of the SPI interrupt request.
 * @param  [in]  bus The spi bus. see @ref spi_bus_t
 * @else
 * @brief  SPI中断处理函数
 * @param  [in]  bus 串口号， 参考 @ref spi_bus_t
 * @endif
 */
void hal_spi_v151_irq_handler(spi_bus_t bus);
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif