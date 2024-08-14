/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test usb source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-09, Create file. \n
 */
#include "chip_io.h"
#include "cmsis_os2.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "securec.h"
#include "test_suite_errors.h"
#include "test_suite_log.h"
#include "test_suite.h"
#include "test_usb_mouse.h"
#include "test_usb_keyboard.h"
#include "test_usb_private.h"
#include "test_usb_hid.h"
#include "test_usb_dfu.h"
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
#include "test_usb_serial.h"
#endif
#include "test_usb.h"

#define TEST_USB_MANUFACTURER  { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define TEST_USB_MANUFACTURER_LEN   20
#define TEST_USB_PRODUCT  { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define TEST_USB_PRODUCT_LEN        22
#define TEST_USB_SERIAL   { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define TEST_USB_SERIAL_LEN         16
#define USB_INIT_DELAY_MS      (3000UL)
#define WORD_LENGTH                 4
#define SWITCH_TO_DFU_FLAG          0x1e
#define SWITCH_TO_ACM_FLAG          0x1b
#define CUSTUMER_PAGE_REPORT_ID     0x8
#define CUSTOM_RW_PAGE_REPORT_ID    0x9
#define RECV_MAX_LENGTH             64
#define USB_RECV_STACK_SIZE         0x400
#define USB_RECV_FAIL_DELAY         50
#define USB_COMMAND_LENTH           13
#define USB_DEINIT_DELAY            50

uint8_t g_test_write[RECV_MAX_LENGTH];
typedef struct {
    uint32_t start_flag;
    uint16_t packet_size;
    uint8_t frame_type;
    uint8_t frame_type_reserve;
    uint16_t flag;
    uint16_t check_sum;
} seboot_switch_dfu_t;

typedef enum {
    USB_RECV_HID,
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
    USB_RECV_SERIAL,
#endif
    USB_RECV_SUSPEND,
} usb_recv_state_t;


static bool g_test_usb_inited = false;
static int g_hid_index = 0;
static int g_hid_index2 = 0;
static usb_recv_state_t g_usb_recv_state = USB_RECV_HID;
static osal_task *g_usb_recv_task = NULL;

void usb_change_recv_state(usb_recv_state_t state)
{
    g_usb_recv_state = state;
}

static uint8_t usb_hid_recv_data(void)
{
    static uint32_t recv_count = 0;
    uint8_t recv_data[RECV_MAX_LENGTH];
    for (;;) {
        int32_t ret = fhid_recv_data(g_hid_index2, (char*)recv_data, RECV_MAX_LENGTH);
        if (ret <= 0) {
            printf("recv error\n");
            osDelay(USB_RECV_FAIL_DELAY);
            continue;
        }

        if (ret == USB_COMMAND_LENTH && recv_data[0] == CUSTUMER_PAGE_REPORT_ID) {
            seboot_switch_dfu_t command;
            if (memcpy_s(&command, sizeof(seboot_switch_dfu_t), &recv_data[1], USB_COMMAND_LENTH - 1) != EOK) {
                continue;
            }
            if (command.frame_type == SWITCH_TO_DFU_FLAG) {
                osal_printk("start dfu\n");
                usb_deinit();
                osal_msleep(USB_DEINIT_DELAY);
                usb_dfu_init();
                usb_dfu_wait_ugrade_done_and_reset();
                break;
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
            } else if (command.frame_type == SWITCH_TO_ACM_FLAG) {
                osal_printk("start serial\n");
                usb_deinit();
                osal_msleep(USB_DEINIT_DELAY);
                usb_serial_init();
                usb_change_recv_state(USB_RECV_SERIAL);
                break;
#endif
            }
        }
        if (ret == RECV_MAX_LENGTH && recv_data[0] == CUSTOM_RW_PAGE_REPORT_ID) {
            for (uint8_t i = 0; i < RECV_MAX_LENGTH; i++) {
                osal_printk("%x ", recv_data[i]);
            }
            osal_printk("\r\n");
            recv_count++;
            osal_printk("recv_count:%d\r\n", recv_count);
        }
    }
    return 0;
}

static int usb_recv_task(void *para)
{
    UNUSED(para);
    
    for (;;) {
        switch (g_usb_recv_state) {
            case USB_RECV_HID:
                usb_hid_recv_data();
                break;
#ifdef CONFIG_DRIVERS_USB_SERIAL_GADGET
            case USB_RECV_SERIAL:
                if (usb_serial_recv_data() < 0) {
                    usb_change_recv_state(USB_RECV_SUSPEND);
                }
                break;
#endif
            case USB_RECV_SUSPEND:
                osal_kthread_suspend(g_usb_recv_task);
                break;
        }
    }
    osal_printk("usb recv task over\n");
    return 0;
}

int test_usb_init_internal(device_type dtype, uint16_t pid)
{
    if (g_test_usb_inited) {
        return TEST_SUITE_OK;
    }

    const char manufacturer[TEST_USB_MANUFACTURER_LEN] = TEST_USB_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = TEST_USB_MANUFACTURER_LEN
    };

    const char product[TEST_USB_PRODUCT_LEN] = TEST_USB_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = TEST_USB_PRODUCT_LEN
    };

    const char serial[TEST_USB_SERIAL_LEN] = TEST_USB_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = TEST_USB_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };
    dev_id.product_id = pid;

    if (dtype == DEV_HID) {
        g_hid_index = tesetsuit_usb_add_report_desc_hid(USB_HID_MOUSE);
        g_hid_index2 = tesetsuit_usb_add_report_desc_hid(USB_HID_KEYBOARD);
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        return TEST_SUITE_TEST_FAILED;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        test_suite_log_stringf("usb_init fail\n");
        return TEST_SUITE_TEST_FAILED;
    }
    g_usb_recv_task = osal_kthread_create(usb_recv_task, NULL, "lcd_task", USB_RECV_STACK_SIZE);

    g_test_usb_inited = true;

    return TEST_SUITE_OK;
}

int test_usb_deinit(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    usb_deinit();
    g_test_usb_inited = false;
    return 0;
}

int test_usb_write(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    fhid_send_data(g_hid_index2, (char *)&g_test_write, RECV_MAX_LENGTH);
    return 0;
}

int tesetsuit_usb_get_hid_index(void)
{
    return g_hid_index;
}

int tesetsuit_usb_get_hid_index2(void)
{
    return g_hid_index2;
}

bool tesetsuit_usb_get_hid_is_inited(void)
{
    return g_test_usb_inited;
}

void add_usb_test_case(void)
{
    for (int i = 0; i < USB_HID_MOUSE; i++) {
        g_test_write[i] = i;
    }
    g_test_write[0] = CUSTOM_RW_PAGE_REPORT_ID;
    uapi_test_suite_add_function("usb_deinit", "Deinit USB", test_usb_deinit);
    uapi_test_suite_add_function("mouse_init", "", mouse_init_usb);
    uapi_test_suite_add_function("mouse_init_bt", "", mouse_init_bt);
    uapi_test_suite_add_function("mouse_write", "USB Mouse demo.", test_usb_write);
    uapi_test_suite_add_function("usb_dongle_mouse", "dongle mouse.", tesetsuit_usb_dongle_mouse);
    uapi_test_suite_add_function("usb_mouse_remote_wakeup", "Mouse.", tesetsuit_usb_remote_wakeup);
    uapi_test_suite_add_function("usb_mutiple_simulate", "usb mutiple.", tesetsuit_usb_mutiple_simulate);
    uapi_test_suite_add_function("usb_mouse_input", "Mouse input.", tesetsuit_usb_mouse_input);
    uapi_test_suite_add_function("usb_mouse_simulate", "Mouse simulator.", tesetsuit_usb_mouse_simulator);
#ifdef CONFIG_DRIVER_SUPPORT_KEYSCAN
    uapi_test_suite_add_function("usb_keyboard", "Keyboard.", tesetsuit_usb_keyboard);
    uapi_test_suite_add_function("usb_keyboard_input", "Keyboard input.", tesetsuit_usb_keyboard_input);
    uapi_test_suite_add_function("usb_keyboard_simulate", "Keyboard simulator.", tesetsuit_usb_keyboard_simulator);
#endif
    uapi_test_suite_add_function("usb_mouse_simulate_get_times", "Get USB Mouse simulator send times.",
        tesetsuit_usb_mouse_get_times);
    uapi_test_suite_add_function("usb_keyboard_simulate_get_times", "USB Keyboard simulator get the send times..",
        tesetsuit_usb_keyboard_get_times);
}
