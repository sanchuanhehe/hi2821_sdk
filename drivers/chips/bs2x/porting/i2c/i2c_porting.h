/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides i2c port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15， Create file. \n
 */
#ifndef I2C_PORTING_H
#define I2C_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CONFIG_I2C_SUPPORT_MASTER 1
#define CONFIG_I2C_SUPPORT_SLAVE 1
#define CONFIG_I2C_WAIT_CONDITION_TIMEOUT 0xFFFFFFFF
#define CONFIG_I2S_HAS_DMA
#define CONFIG_I2S_TX_BUFFER_DEPTH 8
#define CONFIG_I2S_RX_BUFFER_DEPTH 8

#define I2C_BUS_MAX_NUM I2C_BUS_MAX_NUMBER

/**
 * @brief  Base address list for all of the IPs.
 */
extern uintptr_t g_i2c_base_addrs[I2C_BUS_MAX_NUM];

/**
 * @brief  Register hal funcs objects into hal_i2c module.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 */
void i2c_port_register_hal_funcs(i2c_bus_t bus);

/**
 * @brief  Unregister hal funcs objects from hal_i2c module.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 */
void i2c_port_unregister_hal_funcs(i2c_bus_t bus);

/**
 * @brief  Get the bus clock of specified i2c.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @return The bus clock of specified I2C.
 */
uint32_t i2c_port_get_clock_value(i2c_bus_t bus);

/**
 * @brief  Register the interrupt of I2C.
 */
void i2c_port_register_irq(i2c_bus_t bus);

/**
 * @brief  Unregister the interrupt of I2C.
 */
void i2c_port_unregister_irq(i2c_bus_t bus);

/**
 * @brief  I2C lock.
 * @param [in]  bus The bus index of I2C.
 * @return The irq lock number of I2C.
 */
uint32_t i2c_porting_lock(i2c_bus_t bus);

/**
 * @brief  I2C unlock.
 * @param [in]  bus The bus index of I2C.
 * @param [in]  irq_sts The irq lock number of I2C.
 */
void i2c_porting_unlock(i2c_bus_t bus, uint32_t irq_sts);

/**
 * @brief  I2C clock enable or disable.
 * @param  [in]  bus The bus index of I2C.
 * @param  [in]  on Enable or disable.
 */
void i2c_port_clock_enable(i2c_bus_t bus, bool on);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
