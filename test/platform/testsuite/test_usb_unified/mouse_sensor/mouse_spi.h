/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor spi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#ifndef MOUSE_SPI_H
#define MOUSE_SPI_H

#include "stdint.h"

typedef void (*mouse_sensor_func)(void);

#define HAL_SPI_ENABLE 0x01
#define HAL_SPI_TRANS_MODE_MAX    0x03
#define HAL_SPI_TRANS_MODE_SHIFT  0x08
#define HAL_SPI_TRANS_MODE_TXRX   0x00
#define HAL_SPI_TRANS_MODE_TX     0x01
#define HAL_SPI_TRANS_MODE_RX     0x02
#define HAL_SPI_TRANS_MODE_EEPROM 0x03
#define HAL_SPI_RECEIVED_DATA_REG_MAX 0xFFFF
#define HAL_SPI_CE_LIN_TOGGLE_ENABLE (BIT(24))
#define HAL_SPI_TX_FIFO_NOT_FULL_FLAG  (BIT(1))
#define HAL_SPI_RX_FIFO_NOT_EMPTY_FLAG (BIT(3))

typedef enum {
    READ,
    WRITE,
    DELAY,
    MAX_OPRATION
} mouse_opration_type_t;

typedef union {
    struct {
        uint8_t addr;
        uint8_t value;
    };
    uint16_t delay;
} mouse_opration_val_t;

typedef struct {
    mouse_opration_type_t dict : 8; // 0 means read, 1 means write, 2 means delay, 3 means run func
    mouse_opration_val_t opt;
} spi_mouse_cfg_t;

#define MOUSE_SPI SPI_BUS_2
void mouse_spi_open(void);
void mouse_opration(const spi_mouse_cfg_t *cfg, int16_t lenth);

#endif