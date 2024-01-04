/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides test suite ipc functions \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-19, Create file. \n
 */

#ifndef TEST_SUITE_IPC_H
#define TEST_SUITE_IPC_H

#include "test_suite_channel.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_ipc IPC
 * @ingroup  test_common_testsuite
 * @{
 */

test_suite_channel_funcs_t *test_suite_ipc_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif