/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides test suite errors \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-9-19, Create file. \n
 */

#ifndef LIB_CMD_TEST_SUITE_ERRORS_H
#define LIB_CMD_TEST_SUITE_ERRORS_H

typedef enum {
    TEST_SUITE_OK = 0,
    TEST_SUITE_UNKNOWN_FUNCTION = -1,
    TEST_SUITE_ERROR_TIMED_OUT = -2,
    TEST_SUITE_ERROR_BAD_PARAMS = -3,
    TEST_SUITE_ERROR_PIN_ALLOCATION = -4,
    TEST_SUITE_ERROR_PIN_DEALLOCATION = -5,
    TEST_SUITE_ERROR_UNEXPECTED_INTERRUPT = -6,
    TEST_SUITE_ALLOC_FAILED = -7,
    TEST_SUITE_CONFIG_FAILED = -8,
    TEST_SUITE_TEST_FAILED = -254,
} test_suite_error_e;

#endif
