/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide USB keyboard testsuite header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-12, Create file. \n
 */
#ifndef TEST_USB_KEYBOARD_H
#define TEST_USB_KEYBOARD_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup testcase_usb_keyboard USB KEYBOARD
 * @ingroup  testcase
 * @{
 */
int tesetsuit_usb_keyboard(int argc, char *argv[]);

int tesetsuit_usb_keyboard_simulator(int argc, char *argv[]);

int tesetsuit_usb_keyboard_input(int argc, char *argv[]);

int tesetsuit_usb_keyboard_get_times(int argc, char *argv[]);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif