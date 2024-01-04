/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#include "interrupt.h"
#include "stdbool.h"
#include "gpio.h"
#include "pinctrl.h"
#include "mouse_spi.h"
#include "high_speed_mouse.h"
#include "chip_core_irq.h"

#define PIN_MOTION                  S_MGPIO2
#define SPI_RECV_DATA_LEN           1
#define SPI_SEND_DATA_LEN           2
#define BURST_MOTION_READ           0x16
#define READ_LENGTH                 12

static int16_t g_record_3816_x;
static int16_t g_record_3816_y;
const spi_mouse_cfg_t g_pmw3816dm_cfg[] = {
    { WRITE, {{ 0x3A, 0x5A }} },
    { READ, {{ 0x00, 0x00 }} },
    { READ, {{ 0x02, 0x00 }} },
    { READ, {{ 0x03, 0x00 }} },
    { READ, {{ 0x04, 0x00 }} },
    { READ, {{ 0x05, 0x00 }} },
    { READ, {{ 0x06, 0x00 }} },

    // Power up init.
    { READ, {{ 0x7F, 0x00 }} },
    { WRITE, {{ 0x7F, 0x00 }} },
    { WRITE, {{ 0x55, 0x01 }} },
    { WRITE, {{ 0x50, 0x07 }} },
    { WRITE, {{ 0x7F, 0x0E }} },
    { WRITE, {{ 0x43, 0x10 }} },
    { READ, {{ 0x47, 0x00 }} },
    { READ, {{ 0x67, 0x00 }} },
    { WRITE, {{ 0x48, 0x02 }} },
    { WRITE, {{ 0x7F, 0x00 }} },
    { WRITE, {{ 0x51, 0x7F }} },
    { WRITE, {{ 0x50, 0x00 }} },
    { WRITE, {{ 0x55, 0x00 }} },
    { WRITE, {{ 0x7F, 0x00 }} },
    { WRITE, {{ 0x54, 0x42 }} },
    { WRITE, {{ 0x44, 0x08 }} },
    { WRITE, {{ 0x45, 0x50 }} },
    { WRITE, {{ 0x55, 0x80 }} },
    { WRITE, {{ 0x7F, 0x05 }} },
    { WRITE, {{ 0x4D, 0x00 }} },
    { WRITE, {{ 0x5B, 0x98 }} },
    { WRITE, {{ 0x6D, 0x32 }} },
    { WRITE, {{ 0x41, 0xB3 }} },
    { WRITE, {{ 0x43, 0xD2 }} },
    { WRITE, {{ 0x6E, 0xc3 }} },
    { WRITE, {{ 0x47, 0x00 }} },
    { WRITE, {{ 0x5D, 0xBF }} },
    { WRITE, {{ 0x70, 0x64 }} },
    { WRITE, {{ 0x71, 0x64 }} },
    { WRITE, {{ 0x7B, 0x08 }} },
    { WRITE, {{ 0x7F, 0x06 }} },
    { WRITE, {{ 0x44, 0x1B }} },
    { WRITE, {{ 0x40, 0xBF }} },
    { WRITE, {{ 0x4E, 0x3F }} },
    { WRITE, {{ 0x7F, 0x07 }} },
    { WRITE, {{ 0x41, 0x11 }} },
    { WRITE, {{ 0x43, 0x14 }} },
    { WRITE, {{ 0x4B, 0x0E }} },
    { WRITE, {{ 0x45, 0x0F }} },
    { WRITE, {{ 0x44, 0x42 }} },
    { WRITE, {{ 0x4C, 0x80 }} },
    { WRITE, {{ 0x7F, 0x09 }} },
    { WRITE, {{ 0x5E, 0x0A }} },
    { WRITE, {{ 0x5F, 0x40 }} },
    { WRITE, {{ 0x48, 0x80 }} },
    { WRITE, {{ 0x49, 0x80 }} },
    { WRITE, {{ 0x57, 0x77 }} },
    { WRITE, {{ 0x62, 0x08 }} },
    { WRITE, {{ 0x63, 0x50 }} },
    { WRITE, {{ 0x7F, 0x0A }} },
    { WRITE, {{ 0x45, 0xA0 }} },
    { WRITE, {{ 0x54, 0x24 }} },
    { WRITE, {{ 0x50, 0x01 }} },
    { WRITE, {{ 0x7F, 0x14 }} },
    { WRITE, {{ 0x66, 0x58 }} },
    { WRITE, {{ 0x63, 0x78 }} },
    { WRITE, {{ 0x7F, 0x00 }} },
    { WRITE, {{ 0x5B, 0x20 }} },
};

static void pmw3816dm_mov_func(pin_t pin)
{
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    /*
    * BIT(7) set 1 means mouse in motion and motion data is able to be read
    * Sensor combines 16-bit x and y data into four uint8_ts for transmission
    * recv_motion_data[3] is the higher eight bits of the x data
    * recv_motion_data[5] is the higher eight bits of the y data
    */
    bool motion_flag = (recv_motion_data[0]) & (BIT(7));
    if (motion_flag) {
        int16_t x_data = (recv_motion_data[2] | (recv_motion_data[3] << 8));
        int16_t y_data = (recv_motion_data[4] | (recv_motion_data[5] << 8));
        g_record_3816_x += x_data;
        g_record_3816_y += y_data;
    }
    uapi_gpio_clear_interrupt(pin);
    int_clear_pending_irq(GPIO_0_IRQN);
}

static mouse_freq_type_t paw3816_mouse_init(void)
{
    mouse_spi_open();
    mouse_opration(g_pmw3816dm_cfg, sizeof(g_pmw3816dm_cfg) / sizeof(spi_mouse_cfg_t));
    uapi_pin_set_mode(PIN_MOTION, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(PIN_MOTION, CORE);
    uapi_gpio_set_dir(PIN_MOTION, GPIO_DIRECTION_INPUT);
    if (uapi_gpio_register_isr_func(PIN_MOTION, GPIO_INTERRUPT_LOW,
        (gpio_callback_t)pmw3816dm_mov_func) != ERRCODE_SUCC) {
        return MOUSE_FREQ_MAX;
    }
    return MOUSE_FREQ_1K;
}

static void paw3816_get_xy(int16_t *x, int16_t *y)
{
    *x = g_record_3816_x;
    *y = g_record_3816_y;
    g_record_3816_x = 0;
    g_record_3816_y = 0;
}

high_mouse_oprator_t g_paw3816_operator = {
    .get_xy = paw3816_get_xy,
    .init = paw3816_mouse_init,
};

high_mouse_oprator_t mouse_get_paw3816_operator(void)
{
    return g_paw3816_operator;
}
