/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide USB Testsuite header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-31, Create file. \n
 */
#ifndef TEST_USB_HID_H
#define TEST_USB_HID_H

#include "implementation/usb_init.h"

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

typedef enum {
    USB_HID_MOUSE = 0,
    USB_HID_KEYBOARD = 1,
} usb_hid_type_t;


/**
 * @if Eng
 * @brief  Add the report description of USB HID.
 * @return the HID index.
 * @else
 * @brief  添加USB的HID报告描述符。
 * @return HID报告索引。
 * @endif
 */
int tesetsuit_usb_add_report_desc_hid(usb_hid_type_t type);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif