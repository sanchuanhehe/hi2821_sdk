/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides test suite commands processor \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-19, Create file. \n
 */

#ifndef TEST_SUITE_COMMANDS_PROCESSOR_H
#define TEST_SUITE_COMMANDS_PROCESSOR_H

#include <stdint.h>
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup test_common_testsuite_cmd_processor Commands Processor
 * @ingroup  test_common_testsuite
 * @{
 */

/**
 * @if Eng
 * @brief  Prase test suite commands.
 * @param  [in]  commands user input commands over char process.
 * @else
 * @brief  命令解析。
 * @param  [in]  commands 经过单字节处理之后的输入命令集。
 * @endif
 */
void test_suite_commands_prase(char *commands);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
