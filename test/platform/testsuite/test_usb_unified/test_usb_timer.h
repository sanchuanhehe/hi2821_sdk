/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide Timers for USB. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-31, Create file. \n
 */
#ifndef TEST_USB_TIMER_H
#define TEST_USB_TIMER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef void (*timer_callback_t)(void);
void test_usb_timer1_start(uint32_t ms, timer_callback_t cb, uint32_t times);
void test_usb_timer1_star_us(uint32_t us, timer_callback_t cb, uint32_t times);
void test_usb_timer1_stop(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif