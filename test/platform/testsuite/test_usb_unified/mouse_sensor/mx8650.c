/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */


#include "stdbool.h"
#include "pinctrl.h"
#include "high_speed_mouse.h"
#include "tcxo.h"
#include "gpio.h"
#include "arch_barrier.h"

#define PIN_CLK         S_MGPIO26
#define PIN_DT          S_MGPIO27
#define PIN_MT          S_MGPIO28

#define USB_MOUSE_MOVED_EVENT       (0x84)

#define REG_ADDR_LEN    1
#define DATA_LEN        1
#define R_W_DATA_BITS   8
#define DELAY_US200     200
#define MOUSE_RE_SYNC_DELAY_MS     3

#define MOUSE_DELAY_LOOP_TIME     40
#define MOUSE_OPRATE_DELAY_US     6
#define MOUSE_READ_DELAY_US       2

static void test_mouse_synchronize(void)
{
    uapi_gpio_set_val(PIN_CLK, 1);
    uapi_tcxo_delay_ms(MOUSE_RE_SYNC_DELAY_MS);
    uapi_gpio_set_val(PIN_CLK, 0);
}

static void test_mouse_init(void)
{
    uapi_pin_set_mode(PIN_CLK, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_DT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(PIN_CLK, CORES_APPS_CORE);
    gpio_select_core(PIN_DT, CORES_APPS_CORE);
    uapi_gpio_set_dir(PIN_CLK, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(PIN_DT, GPIO_DIRECTION_OUTPUT);
    test_mouse_synchronize();
}

static void mouse_delay(uint8_t us)
{
    for (int8_t i = 0; i < us; i++) {
        for (int8_t j = 0; j < MOUSE_DELAY_LOOP_TIME; j++) {
            nop();
        }
    }
}

static uint8_t mouse_read(void)
{
    uint8_t ret = 0;

    uapi_gpio_set_dir(PIN_CLK, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(PIN_DT, GPIO_DIRECTION_INPUT);

    for (uint8_t i = 0; i < R_W_DATA_BITS; i++) {
        ret = ret << 1U;
        reg32_clrbit(0x57010000, PIN_CLK);
        mouse_delay(MOUSE_READ_DELAY_US);
        reg32_setbit(0x57010000, PIN_CLK);
        nop();
        if (uapi_gpio_get_val(PIN_DT)) {
            ret |= 0x01;
        } else {
            ret &= 0xFE;
        }
        mouse_delay(MOUSE_READ_DELAY_US);
    }

    return ret;
}

static void mouse_write(uint8_t addr)
{
    uint8_t address = addr;
    uapi_gpio_set_dir(PIN_CLK, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(PIN_DT, GPIO_DIRECTION_OUTPUT);
    reg32_clrbit(0x57010000, PIN_CLK);
    for (uint8_t i = 0; i < R_W_DATA_BITS; i++) {
        reg32_clrbit(0x57010000, PIN_CLK);
        if (address & 0x80) {
            reg32_setbit(0x57010000, PIN_DT);
        } else {
            reg32_clrbit(0x57010000, PIN_DT);
        }
        mouse_delay(1);
        reg32_setbit(0x57010000, PIN_CLK);
        address = address << 1U;
        mouse_delay(1);
    }
    uapi_gpio_set_dir(PIN_DT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(PIN_CLK, GPIO_DIRECTION_INPUT);
}

static int8_t test_mouse_read_x(void)
{
    const uint8_t x_addr = 0x03;
    int8_t x_data;
    mouse_write(x_addr);
    mouse_delay(MOUSE_OPRATE_DELAY_US);
    x_data = (int8_t)mouse_read();
    return x_data;
}

static int8_t test_mouse_read_y(void)
{
    const uint8_t y_addr = 0x04;
    int8_t y_data;
    mouse_write(y_addr);
    mouse_delay(MOUSE_OPRATE_DELAY_US);
    y_data = (int8_t)mouse_read();
    return y_data;
}

static uint8_t test_mouse_read_motion(void)
{
    const uint8_t mot_sts_reg_addr = 0x02;
    uint8_t m_data;
    mouse_write(mot_sts_reg_addr);
    mouse_delay(MOUSE_OPRATE_DELAY_US);
    m_data = mouse_read();
    return m_data;
}

static mouse_freq_type_t mx8650_mouse_init(void)
{
    print("mx3816 init\r\n");
    test_mouse_init();
    return MOUSE_FREQ_500;
}

static void mx8650_get_xy(int16_t *x, int16_t *y)
{
    if ((test_mouse_read_motion() & USB_MOUSE_MOVED_EVENT) != USB_MOUSE_MOVED_EVENT) {
        *x = 0;
        *y = 0;
        return;
    }
    *x = -test_mouse_read_x();
    *y = test_mouse_read_y();
}

high_mouse_oprator_t g_mx8650_operator = {
    .get_xy = mx8650_get_xy,
    .init = mx8650_mouse_init,
};

high_mouse_oprator_t mouse_get_mx8650_operator(void)
{
    print("return 3395 oprator\r\n");
    return g_mx8650_operator;
}