/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide USB mouse testsuite header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-31, Create file. \n
 */
#ifndef TEST_USB_MOUSE_H
#define TEST_USB_MOUSE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup testcase_usb_mouse USB Mouse
 * @ingroup  testcase
 * @{
 */

/**
 * @if Eng
 * @brief  Set the Mouse Reporter Description.
 * @else
 * @brief  设置鼠标报告描述符。
 * @endif
 */
int tesetsuit_usb_remote_wakeup(int argc, char *argv[]);

int tesetsuit_usb_mouse_simulator(int argc, char *argv[]);

int tesetsuit_usb_mouse_input(int argc, char *argv[]);

int tesetsuit_usb_mouse_get_times(int argc, char *argv[]);

int tesetsuit_usb_dongle_mouse(int argc, char *argv[]);

void usb_ble_high_mouse_report(uint8_t *data, uint8_t lenth);

void high_speed_mouse_report(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif