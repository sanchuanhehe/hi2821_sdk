/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test usb multiple source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-09, Create file. \n
 */
#include "watchdog_porting.h"
#include "watchdog.h"
#include "cmsis_os2.h"
#include "tcxo.h"
#include "test_suite_log.h"
#include "securec.h"
#include "gadget/f_hid.h"
#include "std_def.h"
#include "test_usb_private.h"

#define USB_HID_SPECIAL_KEY_MIN   (0xE0)
#define USB_HID_SPECIAL_KEY_MAX   (0xE7)
#define USB_HID_KEY_ZERO          (0x27)
#define USB_HID_KEY_ONE           (0x1E)
#define USB_HID_KEY_SPACE         (0x2c)
#define USB_HID_KEY_ENTER         (0x28)
#define USB_HID_KEY_A             (0x04)
#define USB_HID_MOUSE_INIT_DELAY_MS      (2000UL)
#define USB_HID_MOUSE_INIT_DELAY_MS      (2000UL)
#define USB_HID_MULTIPLE_INIT_DELAY_MS      (3000ULL)

#define MULTIPLE_DEFAULT_REPORT_DESC_LENTH1 64
#define MULTIPLE_DEFAULT_REPORT_DESC_LENTH2 63
#define USB_HID_SEND_KEY_DELAY_MS           10

#define USB_MULTIPLE_DRAW_QUADRATE_ANGLE    4
#define USB_MULTIPLE_DRAW_QUADRATE_TIMES    100
#define USB_MULTIPLE_DRAW_QUADRATE_DELAY_MS 20

#define input(size)             (0x80 | (size))
#define output(size)            (0x90 | (size))
#define feature(size)           (0xb0 | (size))
#define collection(size)        (0xa0 | (size))
#define end_collection(size)    (0xc0 | (size))

/* Global items */
#define usage_page(size)        (0x04 | (size))
#define logical_minimum(size)   (0x14 | (size))
#define logical_maximum(size)   (0x24 | (size))
#define physical_minimum(size)  (0x34 | (size))
#define physical_maximum(size)  (0x44 | (size))
#define uint_exponent(size)     (0x54 | (size))
#define uint(size)              (0x64 | (size))
#define report_size(size)       (0x74 | (size))
#define report_id(size)         (0x84 | (size))
#define report_count(size)      (0x94 | (size))
#define push(size)              (0xa4 | (size))
#define pop(size)               (0xb4 | (size))

/* Local items */
#define usage(size)                 (0x08 | (size))
#define usage_minimum(size)         (0x18 | (size))
#define usage_maximum(size)         (0x28 | (size))
#define designator_index(size)      (0x38 | (size))
#define designator_minimum(size)    (0x48 | (size))
#define designator_maximum(size)    (0x58 | (size))
#define string_index(size)          (0x78 | (size))
#define string_minimum(size)        (0x88 | (size))
#define string_maximum(size)        (0x98 | (size))
#define delimiter(size)             (0xa8 | (size))

#define USB_MUL_KEYBOARD_REPORTER_LEN           9
#define USB_MUL_MOUSE_REPORTER_LEN              5

static uint8_t g_report_desc_multiple[ ] = {
    usage_page(1),      0x01,
    usage(1),           0x06,
    collection(1),      0x01,
    report_id(1),       0x01,

    usage_page(1),      0x07,
    usage_minimum(1),   0xE0,
    usage_maximum(1),   0xE7,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    report_size(1),     0x01,
    report_count(1),    0x08,
    input(1),           0x02,

    report_count(1),    0x01,
    report_size(1),     0x08,
    input(1),           0x01,

    report_count(1),    0x05,
    report_size(1),     0x01,
    usage_page(1),      0x08,
    usage_minimum(1),   0x01,
    usage_maximum(1),   0x05,
    output(1),          0x02,
    report_count(1),    0x01,
    report_size(1),     0x03,
    output(1),          0x01,

    report_count(1),    0x06,
    report_size(1),     0x08,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x65,
    usage_page(1),      0x07,
    usage_minimum(1),   0x00,
    usage_maximum(1),   0x65,
    input(1),           0x00,

    end_collection(0),

    usage_page(1),      0x01,
    usage(1),           0x02,
    collection(1),      0x01,
    report_id(1),       0x02,

    usage(1),           0x01,
    collection(1),      0x00,

    report_count(1),    0x03,
    report_size(1),     0x01,
    usage_page(1),      0x09,
    usage_minimum(1),   0x1,
    usage_maximum(1),   0x3,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x05,
    input(1),           0x01,
    report_count(1),    0x03,
    report_size(1),     0x08,
    usage_page(1),      0x01,
    usage(1),           0x30,
    usage(1),           0x31,
    usage(1),           0x38,
    logical_minimum(1), 0x81,
    logical_maximum(1), 0x7f,
    input(1),           0x06,
    end_collection(0),
    end_collection(0)
};

typedef struct usb_hid_mouse_report {
    union  {
        struct {
            uint8_t left_key      : 1;
            uint8_t right_key     : 1;
            uint8_t mid_key       : 1;
        } b;
        uint8_t d8;
    };
    int8_t x;                 /*!< A negative value indicates that the mouse moves left. */
    int8_t y;                 /*!< A negative value indicates that the mouse moves up. */
    int8_t wheel;              /*!< A negative value indicates that the wheel roll forward. */
} usb_hid_m_mouse_report_t;
#define USB_HID_M_KEYBOARD_MAX_KEY_LENTH 6

typedef struct usb_hid_keyboard_report {
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reversed;                            /*!< Reversed, Must be zero */
    uint8_t key[USB_HID_M_KEYBOARD_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

static usb_hid_m_mouse_report_t g_send_sim_m_mouse_msg[] = {
    { {{0, 0, 0}}, -1, 0, 0 },
    { {{0, 0, 0}}, 0, -1, 0 },
    { {{0, 0, 0}}, 1, 0, 0 },
    { {{0, 0, 0}}, 0, 1, 0 },
};

static void usb_multiple_keyscan_send_data(usb_hid_keyboard_report_t *rpt)
{
    uint8_t str_data[USB_MUL_KEYBOARD_REPORTER_LEN] = { 0 };
    str_data[0] = 0x1;

    if (memcpy_s((str_data + 1), USB_MUL_KEYBOARD_REPORTER_LEN - 1, rpt, sizeof(usb_hid_keyboard_report_t)) != EOK) {
        return;
    }

    int32_t ret = fhid_send_data(0, (char *)str_data, USB_MUL_KEYBOARD_REPORTER_LEN);
    if (ret == -1) {
        test_suite_log_stringf("send fail ret:%d\n", ret);
        return;
    }
}

static void usb_multiple_mouse_send_data(usb_hid_m_mouse_report_t *rpt)
{
    uint8_t str_data[USB_MUL_MOUSE_REPORTER_LEN] = { 0 };
    str_data[0] = 0x2;

    if (memcpy_s((str_data + 1), USB_MUL_MOUSE_REPORTER_LEN - 1, rpt, sizeof(usb_hid_m_mouse_report_t)) != EOK) {
        return;
    }

    int32_t ret = fhid_send_data(0, (char *)str_data, USB_MUL_MOUSE_REPORTER_LEN);
    if (ret == -1) {
        test_suite_log_stringf("send fail ret:%d\n");
        return;
    }
}

static char g_send_sim_key_msg[] = "0123456789 abcdefghijklmnopqrstuvwxyz\n";
static void test_send_key_sim_msg(void)
{
    int len = strlen(g_send_sim_key_msg);
    usb_hid_keyboard_report_t key_msg = { 0 };
    for (int i = 0; i < len; i++) {
        switch (g_send_sim_key_msg[i]) {
            case '0':
                key_msg.key[0] = USB_HID_KEY_ZERO;
                break;
            case ' ':
                key_msg.key[0] = USB_HID_KEY_SPACE;
                break;
            case '\n':
                key_msg.key[0] = USB_HID_KEY_ENTER;
                break;
            default:
                if (g_send_sim_key_msg[i] >= '1' && g_send_sim_key_msg[i] <= '9') {
                    key_msg.key[0] = USB_HID_KEY_ONE + g_send_sim_key_msg[i] - '1';
                } else if (g_send_sim_key_msg[i] >= 'a' && g_send_sim_key_msg[i] <= 'z') {
                    key_msg.key[0] = USB_HID_KEY_A + g_send_sim_key_msg[i] - 'a';
                }
        }
        usb_multiple_keyscan_send_data(&key_msg);
        uapi_tcxo_delay_ms(USB_HID_SEND_KEY_DELAY_MS);
    }
    key_msg.key[0] = 0;
    usb_multiple_keyscan_send_data(&key_msg);
}

static void test_usb_mouse_draw_quadrate(void)
{
    for (int i = 0; i < USB_MULTIPLE_DRAW_QUADRATE_ANGLE; i++) {
        for (int j = 0; j < USB_MULTIPLE_DRAW_QUADRATE_TIMES; j++) {
            usb_multiple_mouse_send_data(&g_send_sim_m_mouse_msg[i]);
            uapi_tcxo_delay_ms(USB_MULTIPLE_DRAW_QUADRATE_DELAY_MS);
        }
    }
}

static void test_usb_mutiple(void)
{
    test_usb_mouse_draw_quadrate();
    test_send_key_sim_msg();
    test_usb_mouse_draw_quadrate();
}


int tesetsuit_usb_mutiple_simulate(int argc, char *argv[])
{
    uint32_t times;
    if (argc == 0) {
        times = 1;
    } else {
        times = (uint32_t)strtol(argv[0], NULL, 0);
    }

    hid_add_report_descriptor(g_report_desc_multiple, sizeof(g_report_desc_multiple), 2); /* 2:mouse */
    osDelay(USB_HID_MULTIPLE_INIT_DELAY_MS);
    for (uint32_t i = 0; i < times; i++) {
        osDelay(USB_HID_MOUSE_INIT_DELAY_MS);
        test_usb_mutiple();
        uapi_watchdog_kick();
    }

    return 0;
}

int tesetsuit_usb_mutiple_dongle(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    uapi_watchdog_disable();
    watchdog_turnoff_clk();

    return 0;
}