/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#ifndef HIGH_SPEED_MOUSE_H
#define HIGH_SPEED_MOUSE_H

#include "stdint.h"
#include "stdbool.h"
#include "mouse_sensor_port.h"

typedef enum mouse_sensor {
    PWM3395DM,
    MOUSE_SENSOR_MAX_NUM
} mouse_sensor_t;

typedef enum mouse_freq {
    MOUSE_FREQ_500 = 500,
    MOUSE_FREQ_1K  = 1000,
    MOUSE_FREQ_2K  = 2000,
    MOUSE_FREQ_4K  = 4000,
    MOUSE_FREQ_8K  = 8000,
    MOUSE_FREQ_MAX
} mouse_freq_t;

typedef struct mouse_sensor_oprator {
    mouse_freq_t (*init)(void);                /* Init mouse. */
    void (*get_xy)(int16_t *x, int16_t *y);    /* Get mouse data. */
} mouse_sensor_oprator_t;

#define trans_to_16_bit(num, bit) \
    ((((num) & (1 << ((bit) - 1))) != 0) ? ((num) | (0xFFFF - (1 << (bit)) + 1)) : (num))

mouse_sensor_oprator_t usb_mouse_get_paw3395_operator(void);

void mouse_spi_burst_read(uint8_t reg_addr, uint8_t *buf, uint8_t lenth);
void mouse_spi_write_reg(uint8_t reg_addr, uint8_t val);
uint8_t mouse_spi_read_reg(uint8_t reg_addr);

mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor);

#endif