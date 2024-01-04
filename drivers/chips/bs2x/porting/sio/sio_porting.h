/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: I2S port for project
 * Author: @CompanyNameTag
 * Create: 2023-03-10
 */

#ifndef SIO_PORTING_H
#define SIO_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "chip_io.h"
#include "product.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_sio SIO
 * @ingroup  drivers_port
 * @{
 */
#define I2S_S_CLK                       32000
#define I2S_CR                          0x608
#define I2S_CR_DIV_NUM_BIT              5
#define I2S_CR_DIV_NUM_LEN              4
#define M_CLKEN0                        0x40
#define I2S_CLKEN_BIT                   2
#define I2S_DUTY_CYCLE                  2
#define PCM_DIV_RATIO_NUM               1
#define CFG_I2S_BCLK_DIV_EN             0
#define CFG_I2S_BCLK_LOAD_DIV_EN        1
#define I2S_READ_WAIT_TIMES_MAX         8
#define I2S_PARAM                       2
#define I2S_MCLK_DIV                    2
#define FREQ_OF_NEED                    32
#define SIXTEEN_WIDTH                   16
#define EIGHTTEEN_WIDTH                 18
#define TWENTY_WIDTH                    20
#define TWENTY_FOUR_WIDTH               24
#define THIRTY_TWO_WIDTH                32
#define NUM_OF_PIN_REGS                 5
#define CONFIG_I2S_BUS_MAX_NUM          1
#define CONFIG_I2S_SUPPORT_LOOPBACK     1
#define CONFIG_DATA_LEN_MAX             128

/**
 * @brief  Definition of the contorl ID of hal sio.
 */

typedef void (*i2s_irq_func_t)(void);

/**
 * @brief  Get the base address of a specified sio.
 * @param  [in]  bus PCM device to use.
 * @return The base address of specified sio.
 */
uintptr_t sio_porting_base_addr_get(sio_bus_t bus);

/**
 * @brief  Register hal funcs objects into hal_sio module.
 * @param  [in]  bus PCM device to use.
 */
void sio_porting_register_hal_funcs(sio_bus_t bus);

/**
 * @brief  Unregister hal funcs objects from hal_sio module.
 * @param  [in]  bus PCM device to use.
 */
void sio_porting_unregister_hal_funcs(sio_bus_t bus);

/**
 * @brief  Register the interrupt of sio.
 * @param  [in]  bus PCM device to use.
 */
void sio_porting_register_irq(sio_bus_t bus);

/**
 * @brief  Unregister the interrupt of sio.
 * @param  [in]  bus PCM device to use.
 */
void sio_porting_unregister_irq(sio_bus_t bus);

/**
 * @brief  The interrupt handler of sio0.
 */
void irq_sio0_handler(void);

/**
 * @brief  SIO clock.
 * @param  [in]  enable Enable or not.
 */
void sio_porting_clock_enable(bool enable);

/**
 * @brief  Get the BCLK Div number.
 * @param  [in]  data_width I2S data width.
 * @param  [in]  ch I2S transmission Channels Number.
 * @return The BCLK Div number.
 */
uint32_t sio_porting_get_bclk_div_num(uint8_t data_width, uint32_t ch);

/**
 * @brief  Config the pin mode.
 */
void sio_porting_i2s_pinmux(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif