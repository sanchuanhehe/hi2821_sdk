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
#include "test_suite_log.h"
#include "mouse_sensor_port.h"

typedef enum {
    PAW3220DB,
    PAW3320DB,
    PMW3816DM,
    PWM3395DM,
    PWM3399DM,
    MOUSE_SENSOR_MAX_NUM,
} mouse_sensor_t;

typedef enum {
    MOUSE_FREQ_500 = 500,
    MOUSE_FREQ_1K  = 1000,
    MOUSE_FREQ_2K  = 2000,
    MOUSE_FREQ_4K  = 4000,
    MOUSE_FREQ_8K  = 8000,
    MOUSE_FREQ_MAX
} mouse_freq_type_t;

#undef PRINT
#define print(fmt, arg...) test_suite_log_stringf(fmt, ##arg)

typedef struct {
    void              (*get_xy)(int16_t *x, int16_t *y);    /*!< Get mouse data. */
    mouse_freq_type_t (*init)(void);                        /*!< Init mouse. */
} high_mouse_oprator_t;

#define trans_to_16_bit(num, bit) \
    ((((num) & (1 << ((bit) - 1))) != 0) ? ((num) | (0xFFFF - (1 << (bit)) + 1)) : (num))

high_mouse_oprator_t mouse_get_paw3220_operator(void);
high_mouse_oprator_t mouse_get_paw3399_operator(void);
high_mouse_oprator_t mouse_get_paw3395_operator(void);
high_mouse_oprator_t mouse_get_paw3320_operator(void);
high_mouse_oprator_t mouse_get_paw3816_operator(void);
high_mouse_oprator_t mouse_get_mx8650_operator(void);
void mouse_burst_read(uint8_t reg_addr, uint8_t *buf, uint8_t lenth);
void mouse_write_reg(uint8_t reg_addr, uint8_t val);
uint8_t mouse_read_reg(uint8_t reg_addr);
high_mouse_oprator_t get_high_mouse_operator(mouse_sensor_t mouse);

#endif