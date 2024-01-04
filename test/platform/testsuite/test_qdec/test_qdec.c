/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test qdec source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-27, Create file. \n
 */
#include "osal_debug.h"
#include "osal_addr.h"
#include "securec.h"
#include "common_def.h"
#include "test_suite.h"
#include "test_suite_errors.h"
#include "test_suite_log.h"
#include "qdec.h"
#include "qdec_porting.h"
#include "test_qdec.h"

static qdec_config_t g_qdec_config = QDEC_DEFAULT_CONFIG;
static int32_t g_qdec_count = 0;

static int32_t qdec_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_qdec_count += argc;
    test_suite_log_stringf("curren count is: %d. \r\n", g_qdec_count);
    return ERRCODE_SUCC;
}

static int32_t test_qdec_open(int argc, char *argv[])
{
    uint8_t acc_per_roll = 1;
    if (argc != 0) {
        acc_per_roll = (uint8_t)strtol(argv[0], NULL, 0);
    }
    qdec_port_pinmux_init(QDEC_A, QDEC_B);
    uapi_qdec_init(&g_qdec_config);
    qdec_port_set_acc_per_roll(acc_per_roll);
    uapi_qdec_register_callback(qdec_report_callback);
    uapi_qdec_enable();
    return TEST_SUITE_OK;
}

static int32_t test_qdec_disable(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    uapi_qdec_disable();
    return TEST_SUITE_OK;
}

static int32_t test_qdec_deinit(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    uapi_qdec_deinit();
    return TEST_SUITE_OK;
}

void add_qdec_test_case(void)
{
    uapi_test_suite_add_function("test_cfbb_qdec_open", "qdec read acc", test_qdec_open);
    uapi_test_suite_add_function("test_cfbb_qdec_disable_check", "qdec disable", test_qdec_disable);
    uapi_test_suite_add_function("test_cfbb_qdec_deinit_check", "qdec deinit", test_qdec_deinit);
}