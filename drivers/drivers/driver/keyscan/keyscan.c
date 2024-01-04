/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides keyscan driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-15, Create file. \n
 */
#include <stdbool.h>
#include "securec.h"
#include "soc_osal.h"
#include "securec.h"
#include "errcode.h"
#include "common_def.h"
#include "hal_keyscan.h"
#include "keyscan_porting.h"
#include "irmalloc.h"
#include "keyscan.h"

#define KEYSCAN_MAX_REPORT_CHANNEL_NUM                      3
#define KEY_VALUE_STRING_LEN                                5
#define CONVERT_DEC_TO_HEX                                  16
#define LIMIT_OF_NUMBERS                                    10  /* In hexadecimal, numbers are before 10. */
#define KEYSCAN_KEY_VALUE_FIFO_LEN                          0x09
#define KEYSCAN_COL_BITS                                    0x7
#define KEYSCAN_ROW_BITS                                    3

static keyscan_report_callback_t g_hal_keyscan_report_list[KEYSCAN_MAX_REPORT_CHANNEL_NUM];
static uint8_t g_pressed_key_num = 0;
static bool g_keyscan_inited = false;
static bool g_powered = false;
static hal_keyscan_funcs_t *g_hal_funcs = NULL;
static uint8_t g_key_value_num[CONFIG_KEYSCAN_REPORT_MAX_NUMS] = { 0 };
static uint8_t g_keyscan_value_map[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = { 0 };
static void keyscan_event_callback(uint16_t key_value);

void uapi_keyscan_init(keyscan_pulse_time_t time, keyscan_mode_t mode, keyscan_int_t event_type)
{
    if (unlikely(g_keyscan_inited == true)) {
        return;
    }
    keyscan_port_register_hal_funcs();
    g_hal_funcs = hal_keyscan_get_funcs();
    g_hal_funcs->init(time, mode, event_type, keyscan_event_callback);
    g_keyscan_inited = true;
}

void uapi_keyscan_deinit(void)
{
    if (unlikely(g_keyscan_inited == false)) {
        return;
    }
    g_hal_funcs->deinit();
    keyscan_port_unregister_hal_funcs();
    g_keyscan_inited = false;
}

errcode_t uapi_keyscan_enable(void)
{
    if (unlikely(g_keyscan_inited == false)) {
        return ERRCODE_KEYSCAN_NOT_INIT;
    }
    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = g_hal_funcs->ctrl(KEYSCAN_CTRL_ENABLE, 0);
    osal_irq_restore(irq_sts);
    if (unlikely(ret == ERRCODE_KEYSCAN_POWER_ON)) {
        g_powered = true;
    }
    return ret;
}

errcode_t uapi_keyscan_disable(void)
{
    if (unlikely(g_powered == false)) {
        return ERRCODE_KEYSCAN_NOT_POWER_ON;
    }
    errcode_t ret = g_hal_funcs->ctrl(KEYSCAN_CTRL_DISABLE, 0);
    if (unlikely(ret == ERRCODE_SUCC)) {
        g_powered = false;
    }
    return ret;
}

errcode_t uapi_keyscan_register_callback(keyscan_report_callback_t callback)
{
    if (callback == NULL) {
        return ERRCODE_FAIL;
    }
    for (int32_t i = 0; i < KEYSCAN_MAX_REPORT_CHANNEL_NUM; i++) {
        if (g_hal_keyscan_report_list[i] == callback) {
            return ERRCODE_SUCC;
        }
    }
    for (int32_t i = 0; i < KEYSCAN_MAX_REPORT_CHANNEL_NUM; i++) {
        if (g_hal_keyscan_report_list[i] == NULL) {
            g_hal_keyscan_report_list[i] = callback;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t uapi_keyscan_unregister_callback(keyscan_report_callback_t callback)
{
    if (callback == NULL) {
        return ERRCODE_FAIL;
    }

    for (int32_t i = 0; i < KEYSCAN_MAX_REPORT_CHANNEL_NUM; i++) {
        if (unlikely(g_hal_keyscan_report_list[i] == callback)) {
            g_hal_keyscan_report_list[i] = NULL;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

static void keyscan_info_report_invoke_callbacks(int key_nums, uint8_t key_values[])
{
    for (int32_t i = 0; i < KEYSCAN_MAX_REPORT_CHANNEL_NUM; i++) {
        if (g_hal_keyscan_report_list[i] != NULL) {
            g_hal_keyscan_report_list[i](key_nums, key_values);
        }
    }
}

static uint16_t keyscan_get_row(uint16_t value)
{
    uint8_t low_bits = value & 0xFF;
    return (uint16_t)(low_bits >> KEYSCAN_ROW_BITS);
}

static uint8_t keyscan_get_col(uint16_t value)
{
    uint8_t low_bits = value & 0xFF;
    return (low_bits & KEYSCAN_COL_BITS);
}

static key_value_report_status_t keyscan_get_value_and_status(uint16_t key_value, uint16_t *row, uint8_t *clo)
{
    *row = keyscan_get_row(key_value);
    *clo = keyscan_get_col(key_value);
    if ((*row > CONFIG_KEYSCAN_ENABLE_ALL_ROW) || (*clo > CONFIG_KEYSCAN_ENABLE_ALL_CLO)) {
        *row = 0;
        *clo = 0;
        return KEY_ERROR;
    }
    return (key_value >> (KEYSCAN_KEY_VALUE_FIFO_LEN - 1));
}

static void keyscan_event_callback(uint16_t key_value)
{
    uint16_t row;
    uint8_t col;
    key_value_report_status_t status = keyscan_get_value_and_status(key_value, &row, &col);
    uint8_t temp = g_keyscan_value_map[row][col];
    if (temp == 0) {
        return;
    }

    if (status == KEY_ERROR) {
        return;
    }
    if (status == KEY_PRESS) {
        bool is_key_pressed = false;
        for (uint8_t i = 0; i <= g_pressed_key_num; i++) {
            if (g_key_value_num[i] == temp) {
                is_key_pressed = true;
            }
        }

        if ((!is_key_pressed) && (g_pressed_key_num < CONFIG_KEYSCAN_REPORT_MAX_NUMS)) {
            g_key_value_num[g_pressed_key_num] = temp;
            g_pressed_key_num++;
        }
        keyscan_info_report_invoke_callbacks(g_pressed_key_num, g_key_value_num);
    } else if (status == KEY_RELEASE) {
        for (uint8_t i = 0; i < g_pressed_key_num; i++) {
            if (g_key_value_num[i] == temp) {
                g_key_value_num[i] = g_key_value_num[g_pressed_key_num - 1];
                g_key_value_num[g_pressed_key_num - 1] = 0;
                g_pressed_key_num--;
                break;
            }
        }

        if (g_pressed_key_num == 0) {
            keyscan_info_report_invoke_callbacks(0, 0);
        } else {
            keyscan_info_report_invoke_callbacks(g_pressed_key_num, g_key_value_num);
        }
    }
}

errcode_t uapi_set_keyscan_value_map(uint8_t **map_addr, uint16_t row, uint8_t col)
{
    if (map_addr == NULL) {
        return ERRCODE_KEYSCAN_NULL;
    }
    if (row != KEYSCAN_MAX_ROW || col != KEYSCAN_MAX_COL) {
        return ERRCODE_KEYSCAN_MAP_WRONG_SIZE;
    }
    if (memcpy_s(g_keyscan_value_map, sizeof(g_keyscan_value_map), map_addr, sizeof(g_keyscan_value_map)) != 0) {
        return ERRCODE_KEYSCAN_SAFE_FUNC_FAIL;
    }
    return ERRCODE_SUCC;
}

#if defined (CONFIG_KEYSCAN_SUPPORT_LPM)
errcode_t uapi_keyscan_suspend(uintptr_t arg)
{
    unused(arg);
    return g_hal_funcs->ctrl(KEYSCAN_CTRL_SUSPEND, 0);
}

errcode_t uapi_keyscan_resume(uintptr_t arg)
{
    unused(arg);
    return g_hal_funcs->ctrl(KEYSCAN_CTRL_RESUME, 0);
}
#endif