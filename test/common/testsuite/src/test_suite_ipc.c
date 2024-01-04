/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides test suite ipc functions \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-14, Create file. \n
 */

#include "test_suite_channel.h"
#include "test_suite_ipc.h"

/**
 * @brief  Gets test suite ipc functions
 * @return test suite ipc functions address
 * @else
 * @brief  获取测试套件ipc功能函数
 * @return 测试套件ipc功能函数接口地址
 * @endif
 */
test_suite_channel_funcs_t *test_suite_ipc_funcs_get(void)
{
    static test_suite_channel_funcs_t test_suite_ipc_funcs = { 0 };
    return &test_suite_ipc_funcs;
}