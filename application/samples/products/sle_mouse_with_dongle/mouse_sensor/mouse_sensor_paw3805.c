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

extern ssap_mouse_key_t g_mouse_notify_data;

static uint8_t paw3805_read_reg(uint8_t reg_addr)
{
    uapi_gpio_set_val(SPI_PIN_CS, 0);
    uint8_t ret = mouse_spi_read_reg(reg_addr);
    uapi_gpio_set_val(SPI_PIN_CS, 1);
    return ret;
}

static void sle_send_msg(void)
{
    uint8_t conn_state = SLE_ACB_STATE_NONE;
    uint32_t pair_status = ERRCODE_SLE_FAIL;
    bool ssap_able = false;
    get_g_sle_mouse_server_conn_state(&conn_state);
    get_g_sle_mouse_pair_state(&pair_status);
    if (conn_state != SLE_ACB_STATE_CONNECTED || pair_status != ERRCODE_SLE_SUCCESS) {
        return;
    }
    get_g_read_ssap_support(&ssap_able);
    if (ssap_able == true) {
        sle_hid_mouse_server_send_input_report(&g_mouse_notify_data);
        osal_msleep(SLE_MOUSE_TASK_DELAY_20_MS);
    }
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
    *x = ((x_low | (x_hi << XY_DATA_SHIFT_LEN)));
    *y = ((y_low | (y_hi << XY_DATA_SHIFT_LEN)));
}

static void paw3805ek_mov_func(pin_t pin)
{
    int16_t x, y;
    paw3805_get_xy(&x, &y);
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    uapi_gpio_clear_interrupt(pin);
    osal_irq_clear(GPIO_0_IRQN);
    sle_send_msg();
}

static mouse_freq_t paw_3805_mouse_init(void)
{
    mouse_sensor_spi_open(0, 1, 1, 1);
    uapi_pin_set_mode(SPI_PIN_CS, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(SPI_PIN_CS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_PIN_CS, 1);
    osal_printk("pid1: 0x%x\r\n", paw3805_read_reg(0)); // 0x31
    osal_printk("pid2: 0x%x\r\n", paw3805_read_reg(1)); // 0x61
    if (uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MONTION, GPIO_INTERRUPT_LOW,
        (gpio_callback_t)paw3805ek_mov_func) != ERRCODE_SUCC) {
        return MOUSE_FREQ_MAX;
    }
    return MOUSE_FREQ_1K;
}

mouse_sensor_oprator_t g_sle_paw3805_operator = {
    .get_xy = paw3805_get_xy,
    .init = paw_3805_mouse_init,
};

mouse_sensor_oprator_t sle_mouse_get_paw3805_operator(void)
{
    return g_sle_paw3805_operator;
}