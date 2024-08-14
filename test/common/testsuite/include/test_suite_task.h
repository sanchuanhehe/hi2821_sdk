/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides test suite task \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-14, Create file. \n
 */

#ifndef TEST_SUITE_TASK_H
#define TEST_SUITE_TASK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_task Task
 * @ingroup  test_common_testsuite
 * @{
 */

/**
 * @if Eng
 * @brief  Write data to message queue.
 * @param  [in]  buffer_addr data address.
 * @param  [in]  buffer_size data size.
 * @else
 * @brief  将数据填给消息队列。
 * @param  [in]  buffer_addr 数据地址。
 * @param  [in]  buffer_size 数据大小。
 * @endif
 */
int32_t test_suite_write_msgqueue(const uint8_t *buffer_addr, uint16_t buffer_size);

/**
 * @if Eng
 * @brief  Create test suite task and message queue.
 * @else
 * @brief  创建测试套件任务及其消息队列。
 * @endif
 */
void test_suite_task_init(void);

/**
 * @if Eng
 * @brief  Destory test suite task and message queue.
 * @else
 * @brief  删除测试套件任务及其消息队列。
 * @endif
 */
void test_suite_task_destroy(void);

/**
 * @if Eng
 * @brief  Set test suite task status.
 * @param  [in]  status test suite task status, false:running; true:exit.
 * @else
 * @brief  设置测试套件运行状态。
 * @param  [in]  status 测试套件运行状态，false:运行中; true:退出。
 * @endif
 */
void test_suite_task_set_exit(bool status);

/**
 * @if Eng
 * @brief  Get test suite task status.
 * @return test suite task status, false:running; true:exit.
 * @else
 * @brief  设置测试套件运行状态。
 * @return 测试套件运行状态, false:运行中; true:退出。
 * @endif
 */
bool test_suite_task_is_exit(void);

/**
 * @if Eng
 * @brief  This function should be called in the main loop, if nonos.
 * @else
 * @brief  无OS的条件下，应该在主循环中调用该函数。
 * @endif
 */
void test_suite_nonos_loop(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
