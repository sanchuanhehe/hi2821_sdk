/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor paw3805 source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-15, Create file. \n
 */

#include "gpio.h"
#include "hal_spi.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor_port.h"
#include "mouse_sensor.h"
#include "spi.h"
#include "tcxo.h"

#define XY_DATA_SHIFT_LEN               8
#define MOTION_BIT                      0x80
#define MOTION_REG                      0x2
#define X_LOW_REG                       0x3
#define Y_LOW_REG                       0x4
#define X_HI_REG                        0x11
#define Y_HI_REG                        0x12
static uint8_t paw3805_read_reg(uint8_t reg_addr)
{
    uapi_gpio_set_val(SPI_PIN_CS, 0);
    uint8_t ret = mouse_spi_read_reg(reg_addr);
    uapi_gpio_set_val(SPI_PIN_CS, 1);
    return ret;
}

static mouse_freq_t paw_3805_mouse_init(void)
{
    mouse_sensor_spi_open(0, 1, 1, 1);
    uapi_pin_set_mode(SPI_PIN_CS, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(SPI_PIN_CS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_PIN_CS, 1);
    osal_printk("pid1: 0x%x\r\n", paw3805_read_reg(0)); // 0x31
    osal_printk("pid2: 0x%x\r\n", paw3805_read_reg(1)); // 0x61
    return MOUSE_FREQ_1K;
}

void paw3805_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    uint8_t motion = paw3805_read_reg(MOTION_REG);
    if ((motion & MOTION_BIT) == 0) {
        *x = 0;
        *y = 0;
        return;
    }
    uint8_t x_low = paw3805_read_reg(X_LOW_REG);
    uint8_t y_low = paw3805_read_reg(Y_LOW_REG);
    uint8_t x_hi = paw3805_read_reg(X_HI_REG);
    uint8_t y_hi = paw3805_read_reg(Y_HI_REG);
    *x = -((x_low | (x_hi << XY_DATA_SHIFT_LEN)));
    *y = ((y_low | (y_hi << XY_DATA_SHIFT_LEN)));
}

mouse_sensor_oprator_t g_ble_paw3805_operator = {
    .get_xy = paw3805_get_xy,
    .init = paw_3805_mouse_init,
};

mouse_sensor_oprator_t ble_mouse_get_operator(void)
{
    return g_ble_paw3805_operator;
}