/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test pinctrl source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-16， Create file. \n
 */
#include "test_pinctrl.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "test_suite.h"
#include "test_suite_errors.h"
#include "debug_print.h"

#define TEST_PINCTRL_GET_PARAM_NUM  1
#define TEST_PINCTRL_SET_PARAM_NUM  2

#ifndef PRINT
#define PRINT osal_printk
#endif

static int test_pinctrl_get_mode(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_GET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_mode_t mode = uapi_pin_get_mode(pin);
    if (mode == PIN_MODE_MAX) {
        PRINT("Invalid pin id-%d.\r\n", pin);
        return TEST_SUITE_TEST_FAILED;
    }

    PRINT("The mode of pin<%d> is %d.\r\n", pin, mode);
    return TEST_SUITE_OK;
}

static int test_pinctrl_set_mode(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_SET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_mode_t mode = (pin_mode_t)strtol(argv[1], NULL, 0);
    errcode_t ret = uapi_pin_set_mode(pin, mode);
    if (ret != ERRCODE_SUCC) {
        PRINT("Error code is 0x%x.\r\n", ret);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The mode of pin<%d> has been set.\r\n", pin);
    return TEST_SUITE_OK;
}

static int test_pinctrl_get_ds(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_GET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_drive_strength_t ds = uapi_pin_get_ds(pin);
    if (ds == PIN_DS_MAX) {
        PRINT("Invalid pin id-%d.\r\n", pin);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The driver-strength of pin<%d> is %d.\r\n", pin, ds);
    return TEST_SUITE_OK;
}

static int test_pinctrl_set_ds(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_SET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_drive_strength_t ds = (pin_drive_strength_t)strtol(argv[1], NULL, 0);
    errcode_t ret = uapi_pin_set_ds(pin, ds);
    if (ret != ERRCODE_SUCC) {
        PRINT("Error code is 0x%x.\r\n", ret);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The driver-strength of pin<%d> has been set.\r\n", pin);
    return TEST_SUITE_OK;
}

static int test_pinctrl_get_pull(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_GET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_pull_t pull = uapi_pin_get_pull(pin);
    if (pull == PIN_PULL_MAX) {
        PRINT("Invalid pin id-%d.\r\n", pin);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The pull-status of pin<%d> is %d.\r\n", pin, pull);
    return TEST_SUITE_OK;
}

static int test_pinctrl_set_pull(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_SET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_pull_t pull = (pin_pull_t)strtol(argv[1], NULL, 0);
    errcode_t ret = uapi_pin_set_pull(pin, pull);
    if (ret != ERRCODE_SUCC) {
        PRINT("Error code is 0x%x.\r\n", ret);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The pull-status of pin<%d> has been set.\r\n", pin);
    return TEST_SUITE_OK;
}

#if defined(CONFIG_PINCTRL_SUPPORT_IE)
static int test_pinctrl_get_ie(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_GET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_input_enable_t ie = uapi_pin_get_ie(pin);
    if (ie == PIN_IE_MAX) {
        PRINT("Invalid pin id-%d.\r\n", pin);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The input-enable of pin<%d> is %d.\r\n", pin, ie);
    return TEST_SUITE_OK;
}

static int test_pinctrl_set_ie(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_SET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_input_enable_t ie = (pin_input_enable_t)strtol(argv[1], NULL, 0);
    errcode_t ret = uapi_pin_set_ie(pin, ie);
    if (ret != ERRCODE_SUCC) {
        PRINT("Error code is 0x%x.\r\n", ret);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The input-enable of pin<%d> has been set.\r\n", pin);
    return TEST_SUITE_OK;
}
#endif /* CONFIG_PINCTRL_SUPPORT_IE */

#if defined(CONFIG_PINCTRL_SUPPORT_ST)
static int test_pinctrl_get_st(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_GET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_schmitt_trigger_t st = uapi_pin_get_st(pin);
    if (st == PIN_ST_MAX) {
        PRINT("Invalid pin id-%d.\r\n", pin);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The schmitt-trigger of pin<%d> is %d.\r\n", pin, st);
    return TEST_SUITE_OK;
}

static int test_pinctrl_set_st(int argc, char *argv[])
{
    if (argc < TEST_PINCTRL_SET_PARAM_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    pin_t pin = (pin_t)strtol(argv[0], NULL, 0);
    pin_schmitt_trigger_t st = (pin_schmitt_trigger_t)strtol(argv[1], NULL, 0);
    errcode_t ret = uapi_pin_set_st(pin, st);
    if (ret != ERRCODE_SUCC) {
        PRINT("Error code is 0x%x.\r\n", ret);
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("The schmitt-trigger of pin<%d> has been set.\r\n", pin);
    return TEST_SUITE_OK;
}
#endif /* CONFIG_PINCTRL_SUPPORT_ST */

void add_pinctrl_test_case(void)
{
    uapi_pin_init();
    uapi_test_suite_add_function("pin_get_mode", "Pin mode getting test. Params: <pin>", test_pinctrl_get_mode);
    uapi_test_suite_add_function("pin_set_mode", "Pin mode setting test. Params: <pin>, <mode>", test_pinctrl_set_mode);
    uapi_test_suite_add_function("pin_get_ds", "Pin driver-strength getting test. Params: <pin>", test_pinctrl_get_ds);
    uapi_test_suite_add_function(
        "pin_set_ds", "Pin driver-strength setting test. Params: <pin>, <ds>", test_pinctrl_set_ds);
    uapi_test_suite_add_function("pin_get_pull", "Pin pull-status getting test. Params: <pin>", test_pinctrl_get_pull);
    uapi_test_suite_add_function("pin_set_pull",
                                 "Pin pull-status setting test. Params: <pin>, <0: pull-none, 1: pull-down, "
                                 "2: pull-up>", test_pinctrl_set_pull);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_test_suite_add_function("pin_get_ie", "Pin input-enable getting test. Params: <pin>", test_pinctrl_get_ie);
    uapi_test_suite_add_function(
        "pin_set_ie", "Pin input-enable setting test. Params: <pin>, <ie>", test_pinctrl_set_ie);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
#if defined(CONFIG_PINCTRL_SUPPORT_ST)
    uapi_test_suite_add_function("pin_get_st", "Pin schmitt-trigger getting test. Params: <pin>", test_pinctrl_get_st);
    uapi_test_suite_add_function(
        "pin_set_st", "Pin schmitt-trigger setting test. Params: <pin>, <st>", test_pinctrl_set_st);
#endif /* CONFIG_PINCTRL_SUPPORT_ST */
}