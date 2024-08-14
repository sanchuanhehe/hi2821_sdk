/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 gpio register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27， Create file. \n
 */
#ifndef HAL_GPIO_V100_REGS_OP_H
#define HAL_GPIO_V100_REGS_OP_H

#include <stdint.h>
#include "common_def.h"
#include "hal_gpio_v100_regs_def.h"
#include "gpio_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_gpio_v100_regs_op GPIO V100 Regs Operation
 * @ingroup  drivers_hal_gpio
 * @{
 */

/** sets the bit y in x to the value z */
#define hal_gpio_set_bit(x, y, z) (((uint32_t)(x) & ~bit(y)) | (((uint32_t)(z) & 1U) << (uint32_t)(y)))

/** Toggle the bit y in x */
#define hal_gpio_toggle_bit(x, y) (((x) & ~bit(y)) | ((~(x)) & bit(y)))

/** Read bit y in x */
#define hal_gpio_read_bit(x, y) (((uint32_t)(x) >> (uint32_t)(y)) & 1U)

/**
 * @brief  The registers list about interrupt.
 */
typedef enum gpio_int_reg {
    GPIO_INTMASK,                       /*!< Interrupt mask set/clear register. */
    GPIO_RAW_INTSTATUS,                 /*!< Raw interrupt status register. */
    GPIO_INTSTATUS,                     /*!< Masked interrupt status register. */
    GPIO_PORTA_EOI                      /*!< Interrupt clear register. */
} gpio_int_reg_t;

extern uintptr_t g_gpios_regs[GPIO_CHANNEL_MAX_NUM];
#define gpios_regs(ch) ((gpio_info_stru_t *)g_gpios_regs[ch])

/**
 * @brief  Get the value of @ref gpio_swporta_dr_data_t.port_a_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @return The value of @ref gpio_swporta_dr_data_t.port_a_data.
 */
static inline uint32_t hal_gpio_gpio_swporta_dr_get_data(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_swporta_dr_data_t gpio_swporta_dr;
    gpio_swporta_dr.d32 = gpios_regs(channel)->gpio_swporta_dr;
    return hal_gpio_read_bit(gpio_swporta_dr.b.port_a_data, channel_pin);
}

/**
 * @brief  Set the value of @ref gpio_swporta_dr_data_t.port_a_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swporta_dr_data_t.port_a_data
 */
static inline void hal_gpio_gpio_swporta_dr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swporta_dr_data_t gpio_swporta_dr;
    gpio_swporta_dr.d32 = gpios_regs(channel)->gpio_swporta_dr;
    gpio_swporta_dr.b.port_a_data = val;
    gpios_regs(channel)->gpio_swporta_dr = gpio_swporta_dr.d32;
}

/**
 * @brief  Set the value of @ref gpio_swporta_dr_data_t.port_a_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_swporta_dr_data_t.port_a_data
 */
static inline void hal_gpio_gpio_swporta_dr_set_bit(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_swporta_dr_data_t gpio_swporta_dr;
    gpio_swporta_dr.d32 = gpios_regs(channel)->gpio_swporta_dr;
    gpio_swporta_dr.b.port_a_data = hal_gpio_set_bit(gpio_swporta_dr.b.port_a_data, channel_pin, val);
    gpios_regs(channel)->gpio_swporta_dr = gpio_swporta_dr.d32;
}

/**
 * @brief  Set the value of @ref gpio_swporta_dr_data_t.port_a_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit  gpio.
 */
static inline void hal_gpio_gpio_swporta_dr_toggle_bit(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_swporta_dr_data_t gpio_swporta_dr;
    gpio_swporta_dr.d32 = gpios_regs(channel)->gpio_swporta_dr;
    gpio_swporta_dr.b.port_a_data = hal_gpio_toggle_bit(gpio_swporta_dr.b.port_a_data, channel_pin);
    gpios_regs(channel)->gpio_swporta_dr = gpio_swporta_dr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swporta_ddr_data_t.port_a_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @return The value of @ref gpio_swporta_ddr_data_t.port_a_data_direction.
 */
static inline uint32_t hal_gpio_gpio_swporta_ddr_get_data(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_swporta_ddr_data_t gpio_swporta_ddr;
    gpio_swporta_ddr.d32 = gpios_regs(channel)->gpio_swporta_ddr;
    return hal_gpio_read_bit(gpio_swporta_ddr.b.port_a_data_direction, channel_pin);
}

/**
 * @brief  Set the value of @ref gpio_swporta_ddr_data_t.port_a_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swporta_ddr_data_t.port_a_data_direction.
 */
static inline void hal_gpio_gpio_swporta_ddr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swporta_ddr_data_t gpio_swporta_ddr;
    gpio_swporta_ddr.d32 = gpios_regs(channel)->gpio_swporta_ddr;
    gpio_swporta_ddr.b.port_a_data_direction = val;
    gpios_regs(channel)->gpio_swporta_ddr = gpio_swporta_ddr.d32;
}

/**
 * @brief  Set the value of @ref gpio_swporta_ddr_data_t.port_a_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_swporta_ddr_data_t.port_a_data_direction.
 */
static inline void hal_gpio_gpio_swporta_ddr_set_bitdata(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_swporta_ddr_data_t gpio_swporta_ddr;
    gpio_swporta_ddr.d32 = gpios_regs(channel)->gpio_swporta_ddr;
    gpio_swporta_ddr.b.port_a_data_direction =
        hal_gpio_set_bit(gpio_swporta_ddr.b.port_a_data_direction, channel_pin, val);
    gpios_regs(channel)->gpio_swporta_ddr = gpio_swporta_ddr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swportb_dr_data_t.port_b_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_swportb_dr_data_t.port_b_data.
 */
static inline uint32_t hal_gpio_gpio_swportb_dr_get_data(gpio_channel_t channel)
{
    gpio_swportb_dr_data_t gpio_swportb_dr;
    gpio_swportb_dr.d32 = gpios_regs(channel)->gpio_swportb_dr;
    return gpio_swportb_dr.b.port_b_data;
}

/**
 * @brief  Set the value of @ref gpio_swportb_dr_data_t.port_b_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swportb_dr_data_t.port_b_data.
 */
static inline void hal_gpio_gpio_swportb_dr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swportb_dr_data_t gpio_swportb_dr;
    gpio_swportb_dr.d32 = gpios_regs(channel)->gpio_swportb_dr;
    gpio_swportb_dr.b.port_b_data = val;
    gpios_regs(channel)->gpio_swportb_dr = gpio_swportb_dr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swportb_ddr_data_t.port_b_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_swportb_ddr_data_t.port_b_data_direction.
 */
static inline uint32_t hal_gpio_gpio_swportb_ddr_get_data(gpio_channel_t channel)
{
    gpio_swportb_ddr_data_t gpio_swportb_ddr;
    gpio_swportb_ddr.d32 = gpios_regs(channel)->gpio_swportb_ddr;
    return gpio_swportb_ddr.b.port_b_data_direction;
}

/**
 * @brief  Set the value of @ref gpio_swportb_ddr_data_t.port_b_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swportb_ddr_data_t.port_b_data_direction.
 */
static inline void hal_gpio_gpio_swportb_ddr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swportb_ddr_data_t gpio_swportb_ddr;
    gpio_swportb_ddr.d32 = gpios_regs(channel)->gpio_swportb_ddr;
    gpio_swportb_ddr.b.port_b_data_direction = val;
    gpios_regs(channel)->gpio_swportb_ddr = gpio_swportb_ddr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swportc_dr_data_t.port_c_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_swportc_dr_data_t.port_c_data.
 */
static inline uint32_t hal_gpio_gpio_swportc_dr_get_data(gpio_channel_t channel)
{
    gpio_swportc_dr_data_t gpio_swportc_dr;
    gpio_swportc_dr.d32 = gpios_regs(channel)->gpio_swportc_dr;
    return gpio_swportc_dr.b.port_c_data;
}

/**
 * @brief  Set the value of @ref gpio_swportc_dr_data_t.port_c_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swportc_dr_data_t.port_c_data.
 */
static inline void hal_gpio_gpio_swportc_dr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swportc_dr_data_t gpio_swportc_dr;
    gpio_swportc_dr.d32 = gpios_regs(channel)->gpio_swportc_dr;
    gpio_swportc_dr.b.port_c_data = val;
    gpios_regs(channel)->gpio_swportc_dr = gpio_swportc_dr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swportc_ddr_data_t.port_c_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_swportc_ddr_data_t.port_c_data_direction.
 */
static inline uint32_t hal_gpio_gpio_swportc_ddr_get_data(gpio_channel_t channel)
{
    gpio_swportc_ddr_data_t gpio_swportc_ddr;
    gpio_swportc_ddr.d32 = gpios_regs(channel)->gpio_swportc_ddr;
    return gpio_swportc_ddr.b.port_c_data_direction;
}

/**
 * @brief  Set the value of @ref gpio_swportc_ddr_data_t.port_c_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swportc_ddr_data_t.port_c_data_direction.
 */
static inline void hal_gpio_gpio_swportc_ddr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swportc_ddr_data_t gpio_swportc_ddr;
    gpio_swportc_ddr.d32 = gpios_regs(channel)->gpio_swportc_ddr;
    gpio_swportc_ddr.b.port_c_data_direction = val;
    gpios_regs(channel)->gpio_swportc_ddr = gpio_swportc_ddr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swportd_dr_data_t.port_d_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_swportd_dr_data_t.port_d_data.
 */
static inline uint32_t hal_gpio_gpio_swportd_dr_get_data(gpio_channel_t channel)
{
    gpio_swportd_dr_data_t gpio_swportd_dr;
    gpio_swportd_dr.d32 = gpios_regs(channel)->gpio_swportd_dr;
    return gpio_swportd_dr.b.port_d_data;
}

/**
 * @brief  Set the value of @ref gpio_swportd_dr_data_t.port_d_data.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swportd_dr_data_t.port_d_data.
 */
static inline void hal_gpio_gpio_swportd_dr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swportd_dr_data_t gpio_swportd_dr;
    gpio_swportd_dr.d32 = gpios_regs(channel)->gpio_swportd_dr;
    gpio_swportd_dr.b.port_d_data = val;
    gpios_regs(channel)->gpio_swportd_dr = gpio_swportd_dr.d32;
}

/**
 * @brief  Get the value of @ref gpio_swportd_ddr_data_t.port_d_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_swportd_ddr_data_t.port_d_data_direction.
 */
static inline uint32_t hal_gpio_gpio_swportd_ddr_get_data(gpio_channel_t channel)
{
    gpio_swportd_ddr_data_t gpio_swportd_ddr;
    gpio_swportd_ddr.d32 = gpios_regs(channel)->gpio_swportd_ddr;
    return gpio_swportd_ddr.b.port_d_data_direction;
}

/**
 * @brief  Set the value of @ref gpio_swportd_ddr_data_t.port_d_data_direction.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_swportd_ddr_data_t.port_d_data_direction.
 */
static inline void hal_gpio_gpio_swportd_ddr_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_swportd_ddr_data_t gpio_swportd_ddr;
    gpio_swportd_ddr.d32 = gpios_regs(channel)->gpio_swportd_ddr;
    gpio_swportd_ddr.b.port_d_data_direction = val;
    gpios_regs(channel)->gpio_swportd_ddr = gpio_swportd_ddr.d32;
}

/**
 * @brief  Get the value of @ref gpio_inten_data_t.interrupt_enable.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_inten_data_t.interrupt_enable.
 */
static inline uint32_t hal_gpio_gpio_inten_get_data(gpio_channel_t channel)
{
    gpio_inten_data_t gpio_inten;
    gpio_inten.d32 = gpios_regs(channel)->gpio_inten;
    return gpio_inten.b.interrupt_enable;
}

/**
 * @brief  Set the value of @ref gpio_inten_data_t.interrupt_enable.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_inten_data_t.interrupt_enable.
 */
static inline void hal_gpio_gpio_inten_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_inten_data_t gpio_inten;
    gpio_inten.d32 = gpios_regs(channel)->gpio_inten;
    gpio_inten.b.interrupt_enable = val;
    gpios_regs(channel)->gpio_inten = gpio_inten.d32;
}

/**
 * @brief  Set the value of @ref gpio_inten_data_t.interrupt_enable.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_inten_data_t.interrupt_enable.
 */
static inline void hal_gpio_gpio_inten_set_bit(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_inten_data_t gpio_inten;
    gpio_inten.d32 = gpios_regs(channel)->gpio_inten;
    gpio_inten.b.interrupt_enable = hal_gpio_set_bit(gpio_inten.b.interrupt_enable, channel_pin, val);
    gpios_regs(channel)->gpio_inten = gpio_inten.d32;
}

/**
 * @brief  Get the value of @ref gpio_intmask_data_t.interrupt_mask.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_intmask_data_t.interrupt_mask.
 */
static inline uint32_t hal_gpio_gpio_intmask_get_data(gpio_channel_t channel)
{
    gpio_intmask_data_t gpio_intmask;
    gpio_intmask.d32 = gpios_regs(channel)->gpio_intmask;
    return gpio_intmask.b.interrupt_mask;
}

/**
 * @brief  Set the value of @ref gpio_intmask_data_t.interrupt_mask.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_intmask_data_t.interrupt_mask.
 */
static inline void hal_gpio_gpio_intmask_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_intmask_data_t gpio_intmask;
    gpio_intmask.d32 = gpios_regs(channel)->gpio_intmask;
    gpio_intmask.b.interrupt_mask = val;
    gpios_regs(channel)->gpio_intmask = gpio_intmask.d32;
}

/**
 * @brief  Get the value of @ref gpio_inttype_level_data_t.interrupt_level.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @return The value of @ref gpio_inttype_level_data_t.interrupt_level.
 */
static inline uint32_t hal_gpio_gpio_inttype_level_get_data(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_inttype_level_data_t gpio_inttype_level;
    gpio_inttype_level.d32 = gpios_regs(channel)->gpio_inttype_level;
    return hal_gpio_read_bit(gpio_inttype_level.b.interrupt_level, channel_pin);
}

/**
 * @brief  Set the value of @ref gpio_inttype_level_data_t.interrupt_level.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_inttype_level_data_t.interrupt_level.
 */
static inline void hal_gpio_gpio_inttype_level_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_inttype_level_data_t gpio_inttype_level;
    gpio_inttype_level.d32 = gpios_regs(channel)->gpio_inttype_level;
    gpio_inttype_level.b.interrupt_level = val;
    gpios_regs(channel)->gpio_inttype_level = gpio_inttype_level.d32;
}

/**
 * @brief  Set the value of @ref gpio_inttype_level_data_t.interrupt_level.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_inttype_level_data_t.interrupt_level.
 */
static inline void hal_gpio_gpio_inttype_level_set_bit(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_inttype_level_data_t gpio_inttype_level;
    gpio_inttype_level.d32 = gpios_regs(channel)->gpio_inttype_level;
    gpio_inttype_level.b.interrupt_level = hal_gpio_set_bit(gpio_inttype_level.b.interrupt_level, channel_pin, val);
    gpios_regs(channel)->gpio_inttype_level = gpio_inttype_level.d32;
}

/**
 * @brief  Get the value of @ref gpio_int_polarity_data_t.interrupt_polarity.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_int_polarity_data_t.interrupt_polarity.
 */
static inline uint32_t hal_gpio_gpio_int_polarity_get_data(gpio_channel_t channel)
{
    gpio_int_polarity_data_t gpio_int_polarity;
    gpio_int_polarity.d32 = gpios_regs(channel)->gpio_int_polarity;
    return gpio_int_polarity.b.interrupt_polarity;
}

/**
 * @brief  Set the value of @ref gpio_int_polarity_data_t.interrupt_polarity.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_int_polarity_data_t.interrupt_polarity.
 */
static inline void hal_gpio_gpio_int_polarity_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_int_polarity_data_t gpio_int_polarity;
    gpio_int_polarity.d32 = gpios_regs(channel)->gpio_int_polarity;
    gpio_int_polarity.b.interrupt_polarity = val;
    gpios_regs(channel)->gpio_int_polarity = gpio_int_polarity.d32;
}

/**
 * @brief  Set the value of @ref gpio_int_polarity_data_t.interrupt_polarity.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_int_polarity_data_t.interrupt_polarity.
 */
static inline void hal_gpio_gpio_int_polarity_set_bit(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_int_polarity_data_t gpio_int_polarity;
    gpio_int_polarity.d32 = gpios_regs(channel)->gpio_int_polarity;
    gpio_int_polarity.b.interrupt_polarity = hal_gpio_set_bit(gpio_int_polarity.b.interrupt_polarity, channel_pin, val);
    gpios_regs(channel)->gpio_int_polarity = gpio_int_polarity.d32;
}

/**
 * @brief  Get the value of @ref gpio_intstatus_data_t.interrupt_status.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @return The value of @ref gpio_intstatus_data_t.interrupt_status.
 */
static inline uint32_t hal_gpio_gpio_intstatus_get_data(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_intstatus_data_t gpio_intstatus;
    gpio_intstatus.d32 = gpios_regs(channel)->gpio_intstatus;
    return hal_gpio_read_bit(gpio_intstatus.b.interrupt_status, channel_pin);
}

/**
 * @brief  Get the value of @ref gpio_raw_intstatus_data_t.raw_interrupt_status.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_raw_intstatus_data_t.raw_interrupt_status.
 */
static inline uint32_t hal_gpio_gpio_raw_intstatus_get_data(gpio_channel_t channel)
{
    gpio_raw_intstatus_data_t gpio_raw_intstatus;
    gpio_raw_intstatus.d32 = gpios_regs(channel)->gpio_raw_intstatus;
    return gpio_raw_intstatus.b.raw_interrupt_status;
}

/**
 * @brief  Get the value of @ref gpio_debounce_data_t.debounce_enable.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_debounce_data_t.debounce_enable.
 */
static inline uint32_t hal_gpio_gpio_debounce_get_data(gpio_channel_t channel)
{
    gpio_debounce_data_t gpio_debounce;
    gpio_debounce.d32 = gpios_regs(channel)->gpio_debounce;
    return gpio_debounce.b.debounce_enable;
}

/**
 * @brief  Set the value of @ref gpio_debounce_data_t.debounce_enable.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_debounce_data_t.debounce_enable.
 */
static inline void hal_gpio_gpio_debounce_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_debounce_data_t gpio_debounce;
    gpio_debounce.d32 = gpios_regs(channel)->gpio_debounce;
    gpio_debounce.b.debounce_enable = val;
    gpios_regs(channel)->gpio_debounce = gpio_debounce.d32;
}

/**
 * @brief  Set the value of @ref gpio_debounce_data_t.debounce_enable.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_debounce_data_t.debounce_enable.
 */
static inline void hal_gpio_gpio_debounce_set_bit_data(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_debounce_data_t gpio_debounce;
    gpio_debounce.d32 = gpios_regs(channel)->gpio_debounce;
    gpio_debounce.b.debounce_enable = hal_gpio_set_bit(gpio_debounce.b.debounce_enable, channel_pin, val);
    gpios_regs(channel)->gpio_debounce = gpio_debounce.d32;
}

/**
 * @brief  Set the value of @ref gpio_porta_eoi_data_t.clear_interrupt_w.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_porta_eoi_data_t.clear_interrupt_w.
 */
static inline void hal_gpio_gpio_porta_eoi_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_porta_eoi_data_t gpio_porta_eoi;
    gpio_porta_eoi.d32 = gpios_regs(channel)->gpio_porta_eoi;
    gpio_porta_eoi.b.clear_interrupt_w = val;
    gpios_regs(channel)->gpio_porta_eoi = gpio_porta_eoi.d32;
}

/**
 * @brief  Set the value of @ref gpio_porta_eoi_data_t.clear_interrupt_w.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_porta_eoi_data_t.clear_interrupt_w.
 */
static inline void hal_gpio_gpio_porta_eoi_set_bit(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_porta_eoi_data_t gpio_porta_eoi;
    gpio_porta_eoi.d32 = gpios_regs(channel)->gpio_porta_eoi;
    gpio_porta_eoi.b.clear_interrupt_w = hal_gpio_set_bit(gpio_porta_eoi.b.clear_interrupt_w, channel_pin, val);
    gpios_regs(channel)->gpio_porta_eoi = gpio_porta_eoi.d32;
}

/**
 * @brief  Get the value of @ref gpio_ext_porta_data_t.external_port_a.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_ext_porta_data_t.external_port_a.
 */
static inline uint32_t hal_gpio_gpio_ext_porta_get_data(gpio_channel_t channel)
{
    gpio_ext_porta_data_t gpio_ext_porta;
    gpio_ext_porta.d32 = gpios_regs(channel)->gpio_ext_porta;
    return gpio_ext_porta.b.external_port_a;
}

/**
 * @brief  Get the value of @ref gpio_ext_porta_data_t.external_port_a.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @return The value of @ref gpio_ext_porta_data_t.external_port_a.
 */
static inline uint32_t hal_gpio_gpio_ext_porta_get_bit_data(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_ext_porta_data_t gpio_ext_porta;
    gpio_ext_porta.d32 = gpios_regs(channel)->gpio_ext_porta;
    return hal_gpio_read_bit(gpio_ext_porta.b.external_port_a, channel_pin);
}

/**
 * @brief  Get the value of @ref gpio_ext_portb_data_t.external_port_b.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_ext_portb_data_t.external_port_b.
 */
static inline uint32_t hal_gpio_gpio_ext_portb_get_data(gpio_channel_t channel)
{
    gpio_ext_portb_data_t gpio_ext_portb;
    gpio_ext_portb.d32 = gpios_regs(channel)->gpio_ext_portb;
    return gpio_ext_portb.b.external_port_b;
}

/**
 * @brief  Get the value of @ref gpio_ext_portc_data_t.external_port_c.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_ext_portc_data_t.external_port_c.
 */
static inline uint32_t hal_gpio_gpio_ext_portc_get_data(gpio_channel_t channel)
{
    gpio_ext_portc_data_t gpio_ext_portc;
    gpio_ext_portc.d32 = gpios_regs(channel)->gpio_ext_portc;
    return gpio_ext_portc.b.external_port_c;
}

/**
 * @brief  Get the value of @ref gpio_ext_portd_data_t.external_port_d.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_ext_portd_data_t.external_port_d.
 */
static inline uint32_t hal_gpio_gpio_ext_portd_get_data(gpio_channel_t channel)
{
    gpio_ext_portd_data_t gpio_ext_portd;
    gpio_ext_portd.d32 = gpios_regs(channel)->gpio_ext_portd;
    return gpio_ext_portd.b.external_port_d;
}

/**
 * @brief  Get the value of @ref gpio_ls_sync_data_t.synchronization_level.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_ls_sync_data_t.synchronization_level.
 */
static inline uint32_t hal_gpio_gpio_ls_sync_get_data(gpio_channel_t channel)
{
    gpio_ls_sync_data_t gpio_ls_sync;
    gpio_ls_sync.d32 = gpios_regs(channel)->gpio_ls_sync;
    return gpio_ls_sync.b.synchronization_level;
}

/**
 * @brief  Set the value of @ref gpio_ls_sync_data_t.synchronization_level.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  val The value of @ref gpio_ls_sync_data_t.synchronization_level.
 */
static inline void hal_gpio_gpio_ls_sync_set_data(gpio_channel_t channel, uint32_t val)
{
    gpio_ls_sync_data_t gpio_ls_sync;
    gpio_ls_sync.d32 = gpios_regs(channel)->gpio_ls_sync;
    gpio_ls_sync.b.synchronization_level = val;
    gpios_regs(channel)->gpio_ls_sync = gpio_ls_sync.d32;
}

/**
 * @brief  Get the value of @ref gpio_id_code_data_t.gpio_id_code.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_id_code_data_t.gpio_id_code.
 */
static inline uint32_t hal_gpio_gpio_id_code_get_data(gpio_channel_t channel)
{
    gpio_id_code_data_t gpio_id_code;
    gpio_id_code.d32 = gpios_regs(channel)->gpio_id_code;
    return gpio_id_code.b.gpio_id_code;
}

/**
 * @brief  Get the value of @ref gpio_int_bothedge_data_t.gpio_int_bothedge.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @return The value of @ref gpio_int_bothedge_data_t.gpio_int_bothedge.
 */
static inline uint32_t hal_gpio_gpio_int_bothedge_get_data(gpio_channel_t channel, uint32_t channel_pin)
{
    gpio_int_bothedge_data_t gpio_int_bothedge;
    gpio_int_bothedge.d32 = gpios_regs(channel)->gpio_int_bothedge;
    return hal_gpio_read_bit(gpio_int_bothedge.b.gpio_int_bothedge, channel_pin);
}

/**
 * @brief  Set the value of @ref gpio_int_bothedge_data_t.gpio_int_bothedge.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @param  [in]  channel_pin The bit of gpio.
 * @param  [in]  val The value of @ref gpio_int_bothedge_data_t.gpio_int_bothedge
 */
static inline void hal_gpio_gpio_int_bothedge_set_bit(gpio_channel_t channel, uint32_t channel_pin, uint32_t val)
{
    gpio_int_bothedge_data_t gpio_int_bothedge;
    gpio_int_bothedge.d32 = gpios_regs(channel)->gpio_int_bothedge;
    gpio_int_bothedge.b.gpio_int_bothedge = hal_gpio_set_bit(gpio_int_bothedge.b.gpio_int_bothedge, channel_pin, val);
    gpios_regs(channel)->gpio_int_bothedge = gpio_int_bothedge.d32;
}

/**
 * @brief  Get the value of @ref gpio_ver_id_code_data_t.gpio_component_version.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_ver_id_code_data_t.gpio_component_version.
 */
static inline uint32_t hal_gpio_gpio_ver_id_code_get_data(gpio_channel_t channel)
{
    gpio_ver_id_code_data_t gpio_ver_id_code;
    gpio_ver_id_code.d32 = gpios_regs(channel)->gpio_ver_id_code;
    return gpio_ver_id_code.b.gpio_component_version;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.apb_data_width.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.apb_data_width.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_apb_data_width(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.apb_data_width;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.num_ports.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.num_ports.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_num_ports(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.num_ports;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.porta_single_ctl.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.porta_single_ctl.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_porta_single_ctl(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.porta_single_ctl;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.portb_single_ctl.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.portb_single_ctl.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_portb_single_ctl(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.portb_single_ctl;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.portc_single_ctl.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.portc_single_ctl.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_portc_single_ctl(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.portc_single_ctl;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.portd_single_ctl.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.portd_single_ctl.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_portd_single_ctl(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.portd_single_ctl;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.hw_porta.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.hw_porta.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_hw_porta(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.hw_porta;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.hw_portb.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.hw_portb.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_hw_portb(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.hw_portb;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.hw_portc.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.hw_portc.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_hw_portc(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.hw_portc;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.hw_portd.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.hw_portd.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_hw_portd(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.hw_portd;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.porta_intr.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.porta_intr.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_porta_intr(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.porta_intr;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.debounce.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.debounce.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_debounce(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.debounce;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.add_encoded_params.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.add_encoded_params.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_add_encoded_params(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.add_encoded_params;
}

/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.gpio_id.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.gpio_id.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_gpio_id(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.gpio_id;
}


/**
 * @brief  Get the value of @ref gpio_config_reg1_data_t.encoded_id_width.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg1_data_t.encoded_id_width.
 */
static inline uint32_t hal_gpio_gpio_config_reg1_get_encoded_id_width(gpio_channel_t channel)
{
    gpio_config_reg1_data_t gpio_config_reg1;
    gpio_config_reg1.d32 = gpios_regs(channel)->gpio_config_reg1;
    return gpio_config_reg1.b.encoded_id_width;
}

/**
 * @brief  Get the value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_a.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_a.
 */
static inline uint32_t hal_gpio_gpio_config_reg2_get_encoded_id_pwidth_a(gpio_channel_t channel)
{
    gpio_config_reg2_data_t gpio_config_reg2;
    gpio_config_reg2.d32 = gpios_regs(channel)->gpio_config_reg2;
    return gpio_config_reg2.b.encoded_id_pwidth_a;
}

/**
 * @brief  Get the value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_b.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_b.
 */
static inline uint32_t hal_gpio_gpio_config_reg2_get_encoded_id_pwidth_b(gpio_channel_t channel)
{
    gpio_config_reg2_data_t gpio_config_reg2;
    gpio_config_reg2.d32 = gpios_regs(channel)->gpio_config_reg2;
    return gpio_config_reg2.b.encoded_id_pwidth_b;
}

/**
 * @brief  Get the value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_c.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_c.
 */
static inline uint32_t hal_gpio_gpio_config_reg2_get_encoded_id_pwidth_c(gpio_channel_t channel)
{
    gpio_config_reg2_data_t gpio_config_reg2;
    gpio_config_reg2.d32 = gpios_regs(channel)->gpio_config_reg2;
    return gpio_config_reg2.b.encoded_id_pwidth_c;
}

/**
 * @brief  Get the value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_d.
 * @param  [in]  channel The index of gpio. @ref gpio_channel_t
 * @return The value of @ref gpio_config_reg2_data_t.encoded_id_pwidth_d.
 */
static inline uint32_t hal_gpio_gpio_config_reg2_get_encoded_id_pwidth_d(gpio_channel_t channel)
{
    gpio_config_reg2_data_t gpio_config_reg2;
    gpio_config_reg2.d32 = gpios_regs(channel)->gpio_config_reg2;
    return gpio_config_reg2.b.encoded_id_pwidth_d;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif