/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test keyscan source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16, Create file. \n
 */
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "test_suite.h"
#include "test_suite_log.h"
#include "test_suite_errors.h"
#include "errcode.h"
#include "keyscan.h"
#include "keyscan_porting.h"
#include "test_keyscan.h"

#define INDEX_OF_EVENT                      2
#define INDEX_OF_KEYS_TYPE                  3
#define PARAMS_NEEDED_TO_INPUT              4
#define KEYSCAN_REPORT_MAX_NUMS             6
#define CONVERT_DEC_TO_HEX                  16
#define MAX_NUM_OF_DEC                      10
#define LENGTH_OF_KEY_VALUE_STR             5

#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
uint8_t g_key_map_test[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
    { 0x29, 0x2B, 0x14, 0x35, 0x04, 0x1E, 0x1D, 0x00 },
    { 0x3D, 0x3C, 0x08, 0x3B, 0x07, 0x20, 0x06, 0x00 },
    { 0x00, 0x39, 0x1A, 0x3A, 0x16, 0x1F, 0x1B, 0x00 },
    { 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xE4, 0x00 },
    { 0x0A, 0x17, 0x15, 0x22, 0x09, 0x21, 0x19, 0x05 },
    { 0x0B, 0x1C, 0x18, 0x23, 0x0D, 0x24, 0x10, 0x11 },
    { 0x3F, 0x30, 0x0C, 0x2E, 0x0E, 0x25, 0x36, 0x00 },
    { 0x00, 0x00, 0x12, 0x40, 0x0F, 0x26, 0x37, 0x00 },
    { 0x34, 0x2F, 0x13, 0x2D, 0x33, 0x27, 0x00, 0x38 },
    { 0x3E, 0x2A, 0x00, 0x41, 0x31, 0x42, 0x28, 0x2C },
    { 0x00, 0x00, 0xE3, 0x00, 0x00, 0x43, 0x00, 0x51 },
    { 0xE2, 0x00, 0x00, 0x00, 0x00, 0x45, 0xE5, 0xE6 },
    { 0x00, 0x53, 0x00, 0x00, 0xE1, 0x44, 0x00, 0x4F },
    { 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x50 },
    { 0x5F, 0x5C, 0x61, 0x5E, 0x59, 0x62, 0x55, 0x5B },
    { 0x54, 0x60, 0x56, 0x57, 0x5D, 0x5A, 0x58, 0x63 },
};
#else
uint8_t g_key_map_test[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL]  = {
    { 0x29, 0x2B },
    { 0x3D, 0x3C },
    { 0x00, 0x39 },
};
#endif

static int keyscan_ble_report_callback(int key_nums, uint8_t key_values[])
{
    char *key_value_str[KEYSCAN_REPORT_MAX_NUMS];
    for (uint8_t i = 0; i < key_nums; i++) {
        key_value_str[i] = (char *)osal_vmalloc(LENGTH_OF_KEY_VALUE_STR);
        key_value_str[i][0] = '0';
        key_value_str[i][1] = 'x';
        uint32_t tran = key_values[i] / CONVERT_DEC_TO_HEX;
        if (tran < MAX_NUM_OF_DEC) {
            key_value_str[i][TEST_PARAM_ARGC_2] = '0' + tran;
        } else {
            key_value_str[i][TEST_PARAM_ARGC_2] = ('A' + tran - MAX_NUM_OF_DEC);
        }
        tran = key_values[i] % CONVERT_DEC_TO_HEX;
        if (tran < MAX_NUM_OF_DEC) {
            key_value_str[i][TEST_PARAM_ARGC_3] = '0' + tran;
        } else {
            key_value_str[i][TEST_PARAM_ARGC_3] = ('A' + tran - MAX_NUM_OF_DEC);
        }
        key_value_str[i][TEST_PARAM_ARGC_4] = '\0';
    }

#ifndef CFBB_TEST
    uapi_ble_hid_keyboard_input_str(key_nums, (char **)key_value_str);
#endif
    for (int i = 0; i < key_nums; i++) {
        test_suite_log_stringf("KEY val: %s\r\n", key_value_str[i]);
    }
    for (int i = 0; i < key_nums; i++) {
        osal_vfree(key_value_str[i]);
    }
    return 0;
}

static int test_keyscan_config_pins(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    keyscan_porting_config_pins();
    return TEST_SUITE_OK;
}

static int test_keyscan_init(int argc, char *argv[])
{
    /* User should input pulse/mode/event. */
    if (unlikely(argc != PARAMS_NEEDED_TO_INPUT)) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    if (uapi_set_keyscan_value_map((uint8_t **)g_key_map_test, KEYSCAN_MAX_ROW, KEYSCAN_MAX_COL) != ERRCODE_SUCC) {
        return TEST_SUITE_TEST_FAILED;
    }
    keyscan_pulse_time_t pulse = 0;
    keyscan_mode_t mode = 0;
    keyscan_int_t scan_int = 0;
    keyscan_keys_type_t keys_type = 0;

    pulse = (keyscan_pulse_time_t)strtol(argv[0], NULL, 0);
    mode = (keyscan_mode_t)strtol(argv[1], NULL, 0);
    scan_int = (keyscan_int_t)strtol(argv[INDEX_OF_EVENT], NULL, 0);
    keys_type = (keyscan_keys_type_t)strtol(argv[INDEX_OF_KEYS_TYPE], NULL, 0);
    keyscan_porting_type_sel(keys_type);
    uapi_keyscan_init(pulse, mode, scan_int);
    return TEST_SUITE_OK;
}

static int test_keyscan_funcs(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    errcode_t ret = ERRCODE_FAIL;
    ret = uapi_keyscan_register_callback(keyscan_ble_report_callback);
    if (ret != ERRCODE_SUCC) {
        return TEST_SUITE_TEST_FAILED;
    }
    uapi_keyscan_enable();
    return TEST_SUITE_OK;
}

static int test_keyscan_deinit(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    uapi_keyscan_deinit();
    return TEST_SUITE_OK;
}

void add_keyscan_test_case(void)
{
    uapi_test_suite_add_function("test_keyscan_config_pins",
        "It changes the uart h0 for full keys test, and should be run before \
        the test_keyscan_init", test_keyscan_config_pins);
    uapi_test_suite_add_function("test_keyscan_init",
        "Keyscan initialize. Configure duration of the pulse, scan mode, interrupt \
        source type and keyboard type. Params: <pulse_time>, <mode>, <event>, <keyboard_type>", test_keyscan_init);
    uapi_test_suite_add_function("test_keyscan_funcs", "Run key scan", test_keyscan_funcs);
    uapi_test_suite_add_function("test_keyscan_deinit", "Deinit key scan", test_keyscan_deinit);
}
