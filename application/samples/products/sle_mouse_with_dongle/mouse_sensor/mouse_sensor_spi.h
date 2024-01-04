/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor spi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */

#ifndef MOUSE_SENSOR_SPI_H
#define MOUSE_SENSOR_SPI_H

#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef void (*mouse_spi_sensor_func_t)(void);

#define HAL_SPI_ENABLE                   0x01
#define HAL_SPI_TRANS_MODE_MAX           0x03
#define HAL_SPI_TRANS_MODE_SHIFT         0x08
#define HAL_SPI_TRANS_MODE_TXRX          0x00
#define HAL_SPI_TRANS_MODE_TX            0x01
#define HAL_SPI_TRANS_MODE_RX            0x02
#define HAL_SPI_TRANS_MODE_EEPROM        0x03
#define HAL_SPI_RECEIVED_DATA_REG_MAX    0xFFFF
#define HAL_SPI_CE_LIN_TOGGLE_ENABLE     (BIT(24))
#define HAL_SPI_TX_FIFO_NOT_FULL_FLAG    (BIT(1))
#define HAL_SPI_RX_FIFO_NOT_EMPTY_FLAG   (BIT(3))
#define MOUSE_SPI SPI_BUS_2

typedef enum mouse_opration {
    READ,
    WRITE,
    DELAY,
    RUN_FUNC,
    MAX_OPRATION
} mouse_opration_t;

typedef struct spi_mouse_cfg {
    mouse_opration_t opration;
    uint16_t addr;
    int16_t value;
    mouse_spi_sensor_func_t func;
} spi_mouse_cfg_t;

void mouse_sensor_spi_open(uint8_t frame_format, uint8_t clk_polarity, uint8_t clk_phase, uint8_t mhz);
void mouse_sensor_spi_opration(const spi_mouse_cfg_t *cfg, int16_t lenth);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif