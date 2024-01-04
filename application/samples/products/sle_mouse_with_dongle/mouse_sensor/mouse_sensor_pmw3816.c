/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor pmw3816 source \n
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
#define BURST_MOTION_READ               0x16
#define READ_LENGTH                     12
#define X_LOW_BIT                       0x2
#define X_HI_BIT                        0x3
#define Y_LOW_BIT                       0x4
#define Y_HI_BIT                        0x5

extern ssap_mouse_key_t g_mouse_notify_data;

static uint8_t pmw3816_read_reg(uint8_t reg_addr)
{
    uapi_gpio_set_val(SPI_PIN_CS, 0);
    uint8_t ret = mouse_spi_read_reg(reg_addr);
    uapi_gpio_set_val(SPI_PIN_CS, 1);
    return ret;
}

const spi_mouse_cfg_t g_sle_pmw3816dm_cfg[] = {
    { WRITE, 0x3A, 0x5A, NULL },
    { READ, 0x00, 0x00, NULL },
    { READ, 0x02, 0x00, NULL },
    { READ, 0x03, 0x00, NULL },
    { READ, 0x04, 0x00, NULL },
    { READ, 0x05, 0x00, NULL },
    { READ, 0x06, 0x00, NULL },

    // Power up init.
    { READ, 0x7F, 0x00, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x55, 0x01, NULL },
    { WRITE, 0x50, 0x07, NULL },
    { WRITE, 0x7F, 0x0E, NULL },
    { WRITE, 0x43, 0x10, NULL },
    { READ, 0x47, 0x00, NULL },
    { READ, 0x67, 0x00, NULL },
    { WRITE, 0x48, 0x02, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x51, 0x7F, NULL },
    { WRITE, 0x50, 0x00, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x54, 0x42, NULL },
    { WRITE, 0x44, 0x08, NULL },
    { WRITE, 0x45, 0x50, NULL },
    { WRITE, 0x55, 0x80, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x4D, 0x00, NULL },
    { WRITE, 0x5B, 0x98, NULL },
    { WRITE, 0x6D, 0x32, NULL },
    { WRITE, 0x41, 0xB3, NULL },
    { WRITE, 0x43, 0xD2, NULL },
    { WRITE, 0x6E, 0xc3, NULL },
    { WRITE, 0x47, 0x00, NULL },
    { WRITE, 0x5D, 0xBF, NULL },
    { WRITE, 0x70, 0x64, NULL },
    { WRITE, 0x71, 0x64, NULL },
    { WRITE, 0x7B, 0x08, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x44, 0x1B, NULL },
    { WRITE, 0x40, 0xBF, NULL },
    { WRITE, 0x4E, 0x3F, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x41, 0x11, NULL },
    { WRITE, 0x43, 0x14, NULL },
    { WRITE, 0x4B, 0x0E, NULL },
    { WRITE, 0x45, 0x0F, NULL },
    { WRITE, 0x44, 0x42, NULL },
    { WRITE, 0x4C, 0x80, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x5E, 0x0A, NULL },
    { WRITE, 0x5F, 0x40, NULL },
    { WRITE, 0x48, 0x80, NULL },
    { WRITE, 0x49, 0x80, NULL },
    { WRITE, 0x57, 0x77, NULL },
    { WRITE, 0x62, 0x08, NULL },
    { WRITE, 0x63, 0x50, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x45, 0xA0, NULL },
    { WRITE, 0x54, 0x24, NULL },
    { WRITE, 0x50, 0x01, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x66, 0x58, NULL },
    { WRITE, 0x63, 0x78, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x5B, 0x20, NULL },
};

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

static void pmw3816dm_mov_func(pin_t pin)
{
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_spi_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    /*
    * BIT(7) set 1 means mouse in motion and motion data is able to be read
    * Sensor combines 16-bit x and y data into four uint8_ts for transmission
    * recv_motion_data[3] is the higher eight bits of the x data
    * recv_motion_data[5] is the higher eight bits of the y data
    */
    bool motion_flag = (recv_motion_data[0]) & (BIT(7));
    if (motion_flag) {
        g_mouse_notify_data.x = (recv_motion_data[X_LOW_BIT] | (recv_motion_data[X_HI_BIT] << XY_DATA_SHIFT_LEN));
        g_mouse_notify_data.y = (recv_motion_data[Y_LOW_BIT] | (recv_motion_data[Y_HI_BIT] << XY_DATA_SHIFT_LEN));
        sle_send_msg();
    }
    uapi_gpio_clear_interrupt(pin);
}

static mouse_freq_t pmw3816_mouse_init(void)
{
    mouse_sensor_spi_open(0, 1, 1, 1);
    mouse_sensor_spi_opration(g_sle_pmw3816dm_cfg, sizeof(g_sle_pmw3816dm_cfg)/ sizeof(spi_mouse_cfg_t));
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_SPI_CS, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_SPI_CS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_SPI_CS, 1);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MONTION, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_MOUSE_PIN_MONTION, CORE);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_MONTION, PIN_PULL_UP);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_MONTION, PIN_IE_1);
#endif
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MONTION, GPIO_DIRECTION_INPUT);
    osal_printk("pid1: 0x%x\r\n", pmw3816_read_reg(0));    // Expected value 0x49
    osal_printk("pid2: 0x%x\r\n", pmw3816_read_reg(0x5f)); // Expected value 0xb6
    if (uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MONTION, GPIO_INTERRUPT_LOW,
        (gpio_callback_t)pmw3816dm_mov_func) != ERRCODE_SUCC) {
        return MOUSE_FREQ_MAX;
    }
    return MOUSE_FREQ_1K;
}

void pmw3816_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    uint8_t motion = pmw3816_read_reg(MOTION_REG);
    if ((motion & MOTION_BIT) == 0) {
        *x = 0;
        *y = 0;
        return;
    }
    uint8_t x_low = pmw3816_read_reg(X_LOW_REG);
    uint8_t y_low = pmw3816_read_reg(Y_LOW_REG);
    uint8_t x_hi = pmw3816_read_reg(X_HI_REG);
    uint8_t y_hi = pmw3816_read_reg(Y_HI_REG);
    *x = ((x_low | (x_hi << XY_DATA_SHIFT_LEN)));
    *y = ((y_low | (y_hi << XY_DATA_SHIFT_LEN)));
}

mouse_sensor_oprator_t g_sle_pmw3816_operator = {
    .get_xy = pmw3816_get_xy,
    .init = pmw3816_mouse_init,
};

mouse_sensor_oprator_t sle_mouse_get_pmw3816_operator(void)
{
    return g_sle_pmw3816_operator;
}