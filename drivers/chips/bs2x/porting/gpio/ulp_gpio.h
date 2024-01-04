/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: ulp_gpio file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-20, Create file. \n
 */

#ifndef ULP_GPIO_H
#define ULP_GPIO_H

#include "hal_gpio.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_ulp_gpio info.
 * @ingroup  drivers_ulp_gpio
 * @{
 */

typedef void (*ulp_gpio_irq_cb_t)(uint8_t ulp_gpio);

typedef enum {
    ULP_GPIO_INTERRUPT_LOW,
    ULP_GPIO_INTERRUPT_HIGH,
    ULP_GPIO_INTERRUPT_FALLING_EDGE,
    ULP_GPIO_INTERRUPT_RISING_EDGE,
    ULP_GPIO_INTERRUPT_DEDGE
} ulp_gpio_interrupt_t;

typedef struct pin_pad_slp_hold_cfg {
    pin_t pin;
    bool output_enable;
    bool output_high;
} pin_pad_slp_hold_cfg_t;

typedef struct ulp_gpio_int_wkup_cfg {
    uint8_t ulp_gpio;
    /**
    * 0-31：gpio独立选择
    * 32：swd_clk管脚
    * 33：swd_io管脚
    * 34：gpio或mask0
    * 35：gpio与mask0
    * 36：gpio或mask1
    * 37：gpio或mask1
    */
    uint8_t wk_mux;
    bool int_enable;
    ulp_gpio_interrupt_t trigger;
    ulp_gpio_irq_cb_t irq_cb;
} ulp_gpio_int_wkup_cfg_t;

void ulp_gpio_init(void);

void ulp_gpio_deinit(void);

void ulp_gpio_set_dir(uint8_t ulp_gpio, gpio_direction_t dir);

void ulp_gpio_set_val(uint8_t ulp_gpio, gpio_level_t level);

void ulp_gpio_enable_interrupt(uint8_t ulp_gpio);

void ulp_gpio_disable_interrupt(uint8_t ulp_gpio);

void ulp_gpio_clear_interrupt(uint8_t ulp_gpio);

void ulp_gpio_register_irq_func(uint8_t ulp_gpio, ulp_gpio_interrupt_t trigger, ulp_gpio_irq_cb_t cb);

void ulp_gpio_unregister_irq_func(uint8_t ulp_gpio);

void ulp_gpio_wk_src_sel(uint8_t ulp_gpio, uint8_t wk_mux);

void pin_pad_out_config_during_sleep(pin_t pin, bool oe, bool out_high);

void ulp_gpio_int_wkup_config(ulp_gpio_int_wkup_cfg_t *cfg, uint8_t array_num);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif