/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#include "stdbool.h"
#include "non_os.h"
#include "gpio.h"
#include "pinctrl.h"
#include "mouse_spi.h"
#include "high_speed_mouse.h"

#define PIN_MOTION                      S_MGPIO21
#define SPI_RECV_DATA_LEN           1
#define SPI_SEND_DATA_LEN           2
#define PAW3320_DATA_BIT_LEN        12

static int16_t g_record_3320_x;
static int16_t g_record_3320_y;

const spi_mouse_cfg_t g_paw3320db_cfg[] = {
    { WRITE, {{ 0xCB, 0x04 }} },
    { WRITE, {{ 0x89, 0x5A }} },
    { WRITE, {{ 0xDC, 0xD4 }} },
    { WRITE, {{ 0x99, 0x14 }} },
    { WRITE, {{ 0x8D, 0x1A }} },
    { WRITE, {{ 0x8E, 0x1C }} },
    { WRITE, {{ 0xFF, 0x01 }} },
    { WRITE, {{ 0xC2, 0x4F }} },
    { WRITE, {{ 0xC3, 0x93 }} },
    { WRITE, {{ 0xC4, 0x98 }} },
    { WRITE, {{ 0xC5, 0xF2 }} },
    { WRITE, {{ 0xC7, 0x4F }} },
    { WRITE, {{ 0xC8, 0x93 }} },
    { WRITE, {{ 0xC9, 0x48 }} },
    { WRITE, {{ 0xCA, 0xF3 }} },
    { WRITE, {{ 0xE4, 0x66 }} },
    { WRITE, {{ 0xF9, 0x08 }} },
    { WRITE, {{ 0xFF, 0x00 }} },
    { WRITE, {{ 0x89, 0x00 }} },
    { READ, {{ 0x00, 0x00 }} },
};

static void paw3320_mov_func(pin_t pin)
{
    UNUSED(pin);
    non_os_enter_critical();
    uint8_t x = mouse_read_reg(0x03);
    uint8_t y = mouse_read_reg(0x04);
    uint8_t xy = mouse_read_reg(0x12);

    /* Sensor combines 12-bit x and y data into three uint8_ts for transmission
    * The higher four bits of recv_delta_xy are the higher four bits of the 12-bit x data
    * The lower four bits of recv_delta_xy are the higher four bits of the 12-bit y data
    */
    int16_t temp_x =  (x | ((xy & 0xf0) << 4));
    int16_t temp_y =  (y | ((xy & 0x0f) << 8));
    g_record_3320_x = trans_to_16_bit((uint16_t)temp_x, PAW3320_DATA_BIT_LEN);
    g_record_3320_y = trans_to_16_bit((uint16_t)temp_y, PAW3320_DATA_BIT_LEN);

    non_os_exit_critical();
    uapi_gpio_clear_interrupt(pin);
    int_clear_pending_irq(GPIO_0_IRQN);
}

static bool mouse_motion_pin_init(void)
{
    uapi_pin_set_mode(PIN_MOTION, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(PIN_MOTION, CORE);
    uapi_gpio_set_dir(PIN_MOTION, GPIO_DIRECTION_INPUT);
    if (!uapi_gpio_register_isr_func(PIN_MOTION, GPIO_INTERRUPT_LOW, (gpio_callback_t)paw3320_mov_func)) {
        return false;
    }
    return true;
}

static mouse_freq_type_t paw_3320_mouse_init(void)
{
    mouse_spi_open();
    mouse_opration(g_paw3320db_cfg, sizeof(g_paw3320db_cfg) / sizeof(spi_mouse_cfg_t));
    if (mouse_motion_pin_init()) {
        return MOUSE_FREQ_1K;
    } else {
        return MOUSE_FREQ_MAX;
    }
}

static void paw3320_get_xy(int16_t *x, int16_t *y)
{
    *x = g_record_3320_x;
    *y = g_record_3320_y;
    g_record_3320_x = 0;
    g_record_3320_y = 0;
}

high_mouse_oprator_t g_paw3320_operator = {
    .get_xy = paw3320_get_xy,
    .init = paw_3320_mouse_init,
};

high_mouse_oprator_t mouse_get_paw3320_operator(void)
{
    return g_paw3320_operator;
}