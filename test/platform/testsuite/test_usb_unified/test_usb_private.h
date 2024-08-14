/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test usb demo header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-09, Create file. \n
 */
#ifndef TEST_USB_PRIVATE
#define TEST_USB_PRIVATE

#include "stdint.h"
#include "test_suite_log.h"

int tesetsuit_usb_remote_wakeup(int argc, char *argv[]);
typedef void (*timer_cb)(void);
void test_usb_timer1_start(uint32_t ms, timer_cb cb, uint32_t times);
void test_usb_timer1_star_us(uint32_t us, timer_cb cb, uint32_t times);
void test_usb_timer1_stop(void);
int tesetsuit_usb_dongle_mouse(int argc, char *argv[]);
int tesetsuit_usb_mouse_get_times(int argc, char *argv[]);
int tesetsuit_usb_mouse_simulator(int argc, char *argv[]);
int tesetsuit_usb_mouse(int argc, char *argv[]);
int test_usb_stop_simulate(int argc, char *argv[]);
int tesetsuit_usb_keyboard_simulator(int argc, char *argv[]);
int tesetsuit_usb_mutiple_simulate(int argc, char *argv[]);
int tesetsuit_usb_mutiple_dongle(int argc, char *argv[]);
int mouse_init_usb(int argc, char *argv[]);
int mouse_init_bt(int argc, char *argv[]);
int mouse_init(int argc, char *argv[]);
int enter_mouse_capture(int argc, char *argv[]);
int mouse_capture(int argc, char *argv[]);
int exit_mouse_capture(int argc, char *argv[]);
int burst_motion_read(int argc, char *argv[]);
int tesetsuit_usb_remote_wakeup(int argc, char *argv[]);
int tesetsuit_usb_mouse_input(int argc, char *argv[]);

#endif