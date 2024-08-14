/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide USB Testsuite header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-31, Create file. \n
 */
#ifndef TEST_USB_H
#define TEST_USB_H

#include "implementation/usb_init.h"
#include "test_usb_hid.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup testcase_usb USB
 * @ingroup  testcase
 * @{
 */

/**
 * @if Eng
 * @brief  Add the testcase of USB.
 * @else
 * @brief  添加USB测试用例
 * @endif
 */
void add_usb_test_case(void);

int test_usb_init_internal(device_type dtype, uint16_t pid);
int tesetsuit_usb_get_hid_index(void);
bool tesetsuit_usb_get_hid_is_inited(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif