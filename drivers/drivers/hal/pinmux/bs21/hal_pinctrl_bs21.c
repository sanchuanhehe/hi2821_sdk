/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides hal pinctrl \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-29, Create file. \n
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "pinctrl_porting.h"
#include "hal_pinctrl_bs21.h"

#define HAL_PIN_PINMUX_ADDR                    0x57000450
#define HAL_PIN_DS_ADDR                        0x5702c780
#define HAL_PIN_PULL_ADDR                      0x5702c780

// ULP_AON_CTL: S_MGPI24
#define HAL_CFG_GPIO_0_24_MODE      (HAL_PIN_ULP_AON_CTL_ADDR + 0x210)
// MCU_PINMUX_CTL: S_MGPI25~S_AGPIO6
#define HAL_CFG_GPIO_0_25_MODE      (HAL_PIN_PINMUX_MCU_PINMUX_CTL_ADDR + 0x60)
// ULP_AON_CTL: S_AGPIO10~S_AGPIO11
#define HAL_CFG_GPIO_1_10_MODE      (HAL_PIN_ULP_AON_CTL_ADDR + 0x200)
// MCU_PINMUX_CTL: S_AGPIO12~S_AGPIO15
#define HAL_CFG_GPIO_1_12_MODE      (HAL_PIN_PINMUX_MCU_PINMUX_CTL_ADDR + 0x98)

// MCU_PINMUX_CTL: S_MGPI0~S_MGPIO23
#define HAL_S_MGPIO0_ADDR           (HAL_PIN_PINMUX_MCU_PINMUX_CTL_ADDR + 0xB0)
// ULP_AON_CTL: S_MGPI24
#define HAL_S_MGPIO24_ADDR          (HAL_PIN_ULP_AON_CTL_ADDR + 0x214)
// MCU_PINMUX_CTL: S_MGPI25~S_AGPIO6
#define HAL_S_MGPIO25_ADDR          (HAL_PIN_PINMUX_MCU_PINMUX_CTL_ADDR + 0x120)
// AON_PINMUX_CTL S_AGPIO7~S_AGPIO9
#define HAL_S_AGPIO7_ADDR           (HAL_PIN_AON_PINMUX_CTL_ADDR + 0x20)
// ULP_AON_CTL: S_AGPIO10~S_AGPIO11
#define HAL_S_AGPIO10_ADDR          (HAL_PIN_ULP_AON_CTL_ADDR + 0x208)
// MCU_PINMUX_CTL: S_AGPIO12~S_AGPIO15
#define HAL_S_AGPIO12_ADDR          (HAL_PIN_PINMUX_MCU_PINMUX_CTL_ADDR + 0x158)

#define HAL_PIN_BS21_MODE_CONTROL_CONFIG_BITS 7U
#define HAL_PIN_BS21_DS_CFG_BITS              2U

#define HAL_PIN_BS21_PULL_CFG_BITS            2U
#define HAL_PIN_BS21_PULL_START_BITS          4
#define HAL_PIN_BS21_IE_CFG_BITS              1U
#define HAL_PIN_BS21_INPUT_ENABLE_BIT         6
/**
 * @brief  PIN config dirver strength addr map.
 */
typedef struct {
    pin_t begin;             //!< The first pin of each group.
    pin_t end;               //!< The last pin of each group.
    uint32_t addr;           //!< Address of the group.
    uint32_t reg_num;        //!< Configure the number of registers of the pin.
    uint32_t reg_bit;        //!< The first bit to config.
} hal_pin_ds_map_t;

/**
 * @brief  PIN config mode addr map.
 */
typedef struct {
    pin_t begin;                //!< The first pin of each group.
    pin_t end;                  //!< The last pin of each group.
    uint32_t reg_addr;          //!< Address of the group.
    uint32_t per_num;           //!< Number of pins that can be configured by one register.
} hal_pin_mode_map_t;

/**
 * @brief  PIN config pull addr map.
 */
typedef struct {
    pin_t begin;            //!< The first pin of each group.
    pin_t end;              //!< The last pin of each group.
    uint32_t addr;          //!< Address of group.
    uint32_t config_bit;    //!< The first bit to config.
} hal_pin_pad_ctl_map_t;

static hal_pin_mode_map_t g_pin_mode_map[] = {
    { S_MGPIO0,  S_MGPIO31, HAL_PIN_PINMUX_ADDR, 1 }, //!< PIN_0 ~ PIN_23   address:0x52008000  reg_num:1
};

static hal_pin_pad_ctl_map_t g_pin_pull_map[] = {
    { S_MGPIO0,  S_MGPIO31, HAL_PIN_PULL_ADDR, HAL_PIN_BS21_PULL_START_BITS }
};

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
static hal_pin_pad_ctl_map_t g_pin_ie_map[] = {
    { S_MGPIO0,  S_MGPIO31, HAL_PIN_PULL_ADDR, HAL_PIN_BS21_INPUT_ENABLE_BIT }
};
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

static hal_pin_ds_map_t g_pin_ds_map[] = {
    { S_MGPIO0,  S_MGPIO31, HAL_PIN_DS_ADDR, 1, 0 }
};

static void hal_pin_get_mode_addr(hal_pin_mode_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_mode_map);
    *pin_map = g_pin_mode_map;
}

static void hal_pin_get_pull_addr(hal_pin_pad_ctl_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_pull_map);
    *pin_map = g_pin_pull_map;
}

static void hal_pin_get_ds_addr(hal_pin_ds_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_ds_map);
    *pin_map = g_pin_ds_map;
}

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
static void hal_pin_get_ie_addr(hal_pin_pad_ctl_map_t **pin_map, uint32_t *map_size)
{
    *map_size = (uint32_t)array_size(g_pin_ie_map);
    *pin_map = g_pin_ie_map;
}
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

static hal_pin_mode_map_t *hal_pin_bs21_get_aon_mode_addr(pin_t pin)
{
    hal_pin_mode_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_mode_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_bs21_set_aon_mode(pin_t pin, pin_mode_t mode)
{
    uint32_t reg_addr;
    hal_pin_mode_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_aon_mode_addr(pin);
    if ((pin_map == NULL) || (pin_map->per_num == 0)) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->reg_addr + ((uint32_t)(pin - pin_map->begin) / pin_map->per_num) *
               (sizeof(uint32_t));
    reg16_setbits(reg_addr, 0, HAL_PIN_BS21_MODE_CONTROL_CONFIG_BITS, mode);
    return ERRCODE_SUCC;
}

static pin_mode_t hal_pin_bs21_get_aon_mode(pin_t pin)
{
    uint32_t reg_addr;
    hal_pin_mode_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_aon_mode_addr(pin);
    if ((pin_map == NULL) || (pin_map->per_num == 0)) {
        return PIN_MODE_MAX;
    }
    reg_addr = (uintptr_t)pin_map->reg_addr + ((uint32_t)(pin - pin_map->begin) / pin_map->per_num) *
               (sizeof(uint32_t));
    return (pin_mode_t)reg16_getbits(reg_addr, 0, HAL_PIN_BS21_MODE_CONTROL_CONFIG_BITS);
}

static hal_pin_ds_map_t *hal_pin_bs21_get_aon_ds_addr(pin_t pin)
{
    hal_pin_ds_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_ds_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_bs21_set_aon_ds(pin_t pin, pin_drive_strength_t ds)
{
    uint32_t config_pos = 0;
    uint32_t reg_addr;
    hal_pin_ds_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_aon_ds_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)(pin_map->begin)) * (sizeof(uint32_t));
    config_pos = pin_map->reg_bit;
    reg16_setbits(reg_addr, config_pos, HAL_PIN_BS21_DS_CFG_BITS, ds);
    return ERRCODE_SUCC;
}

static pin_drive_strength_t hal_pin_bs21_get_aon_ds(pin_t pin)
{
    uint32_t config_pos = 0;
    uint32_t reg_addr;
    hal_pin_ds_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_aon_ds_addr(pin);
    if (pin_map == NULL) {
        return PIN_DS_MAX;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)(pin_map->begin)) * (sizeof(uint32_t));
    config_pos = pin_map->reg_bit;

    return (pin_drive_strength_t)reg16_getbits(reg_addr, config_pos, HAL_PIN_BS21_DS_CFG_BITS);
}

static hal_pin_pad_ctl_map_t *hal_pin_bs21_get_pull_addr(pin_t pin)
{
    hal_pin_pad_ctl_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_pull_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_bs21_set_aon_pull(pin_t pin, pin_pull_t pull_type)
{
    uint32_t reg_addr;
    uint32_t config_pos;
    hal_pin_pad_ctl_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_pull_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)pin_map->begin) *
            (uint32_t)(sizeof(uint32_t));
    config_pos = pin_map->config_bit;
    reg16_setbits(reg_addr, config_pos, HAL_PIN_BS21_PULL_CFG_BITS, pull_type);
    return ERRCODE_SUCC;
}

static pin_pull_t hal_pin_bs21_get_aon_pull(pin_t pin)
{
    uint32_t reg_addr;
    uint32_t config_pos;
    hal_pin_pad_ctl_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_pull_addr(pin);
    if (pin_map == NULL) {
        return PIN_PULL_MAX;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)pin_map->begin) *
            (uint32_t)(sizeof(uint32_t));
    config_pos = pin_map->config_bit;
    return (pin_pull_t)reg16_getbits(reg_addr, config_pos, HAL_PIN_BS21_PULL_CFG_BITS);
}

static errcode_t hal_pin_bs21_set_mode(pin_t pin, pin_mode_t mode)
{
    return hal_pin_bs21_set_aon_mode(pin, mode);
}

static pin_mode_t hal_pin_bs21_get_mode(pin_t pin)
{
    return hal_pin_bs21_get_aon_mode(pin);
}

static errcode_t hal_pin_bs21_set_ds(pin_t pin, pin_drive_strength_t ds)
{
    return hal_pin_bs21_set_aon_ds(pin, ds);
}

static pin_drive_strength_t hal_pin_bs21_get_ds(pin_t pin)
{
    return hal_pin_bs21_get_aon_ds(pin);
}

static errcode_t hal_pin_bs21_set_pull(pin_t pin, pin_pull_t pull_type)
{
    return hal_pin_bs21_set_aon_pull(pin, pull_type);
}

static pin_pull_t hal_pin_bs21_get_pull(pin_t pin)
{
    return hal_pin_bs21_get_aon_pull(pin);
}

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
static hal_pin_pad_ctl_map_t *hal_pin_bs21_get_ie_addr(pin_t pin)
{
    hal_pin_pad_ctl_map_t *pin_map = NULL;
    uint32_t map_size = 0;

    hal_pin_get_ie_addr(&pin_map, &map_size);
    for (uint32_t i = 0; i < map_size; i++) {
        if ((pin >= pin_map[i].begin) && (pin <= pin_map[i].end)) {
            return (pin_map + i);
        }
    }
    return NULL;
}

static errcode_t hal_pin_bs21_set_ie(pin_t pin, pin_input_enable_t ie_type)
{
    uint32_t reg_addr;
    uint32_t config_pos;
    hal_pin_pad_ctl_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_ie_addr(pin);
    if (pin_map == NULL) {
        return ERRCODE_FAIL;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)pin_map->begin) *
            (uint32_t)(sizeof(uint32_t));
    config_pos = pin_map->config_bit;
    reg16_setbits(reg_addr, config_pos, HAL_PIN_BS21_IE_CFG_BITS, ie_type);
    return ERRCODE_SUCC;
}

static pin_input_enable_t hal_pin_bs21_get_ie(pin_t pin)
{
    uint32_t reg_addr;
    uint32_t config_pos;
    hal_pin_pad_ctl_map_t *pin_map = NULL;

    pin_map = hal_pin_bs21_get_ie_addr(pin);
    if (pin_map == NULL) {
        return PIN_IE_MAX;
    }
    reg_addr = (uintptr_t)pin_map->addr + ((uint32_t)pin - (uint32_t)pin_map->begin) *
            (uint32_t)(sizeof(uint32_t));
    config_pos = pin_map->config_bit;
    return (pin_input_enable_t)reg16_getbits(reg_addr, config_pos, HAL_PIN_BS21_IE_CFG_BITS);
}
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

static hal_pin_funcs_t g_hal_pin_bs21_funcs = {
    .set_mode = hal_pin_bs21_set_mode,
    .get_mode = hal_pin_bs21_get_mode,
    .set_ds = hal_pin_bs21_set_ds,
    .get_ds = hal_pin_bs21_get_ds,
    .set_pull = hal_pin_bs21_set_pull,
    .get_pull = hal_pin_bs21_get_pull,
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    .set_ie = hal_pin_bs21_set_ie,
    .get_ie = hal_pin_bs21_get_ie,
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
};

hal_pin_funcs_t *hal_pin_bs21_funcs_get(void)
{
    return &g_hal_pin_bs21_funcs;
}
