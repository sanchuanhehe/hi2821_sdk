/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test usb keyboard source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-09, Create file. \n
 */
#include <stdint.h>
#include "keyscan.h"
#include "keyscan_porting.h"
#include "test_suite_errors.h"
#include "cmsis_os2.h"
#include "securec.h"
#include "tcxo.h"
#include "test_suite_log.h"
#include "std_def.h"
#include "gadget/f_hid.h"
#include "test_usb.h"
#include "test_usb_timer.h"
#include "test_usb_keyboard.h"

#define USB_HID_SPECIAL_KEY_MIN   (0xE0)
#define USB_HID_SPECIAL_KEY_MAX   (0xE7)
#define USB_HID_KEY_ZERO          (0x27)
#define USB_HID_KEY_ONE           (0x1E)
#define USB_HID_KEY_SPACE         (0x2c)
#define USB_HID_KEY_ENTER         (0x28)
#define USB_HID_KEY_A             (0x04)
#define USB_HID_KEYBOARD_SIM_SEND_DELAY_MS  (800UL)
#define USB_HID_KEYBOARD_INIT_DELAY_MS      (500UL)
#define USB_CHAR_TO_NUM_16_BASE_VALUE       10
#define USB_STR_TO_NUM16_MUX_NUM            16
#define APP_AT_MAX_KEY_NUM                  6
#define KEYBOARD_SIM_TIME                   1

#define USB_KEYBOARD_REPORTER_LEN           9

#define USB_HID_KEYBOARD_MAX_KEY_LENTH      6

/**
 * @brief  USB HID Keyboard report massage.
 */
typedef struct usb_hid_keyboard_report {
    uint8_t kind;
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reversed;                            /*!< Reversed, Must be zero */
    uint8_t key[USB_HID_KEYBOARD_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

static char g_send_sim_key_msg[] = "0123456789 abcdefghijklmnopqrstuvwxyz\n";
static uint32_t g_keyboard_sim_send_times = 0;
static uint8_t g_key_val[APP_AT_MAX_KEY_NUM] = { 0 };
#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
uint8_t g_key_map_usb[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
    { 0x29, 0x2B, 0x14, 0x35, 0x04, 0x1E, 0x1D, 0x00 },
    { 0x3D, 0x3C, 0x08, 0x3B, 0x07, 0x20, 0x06, 0x00 },
    { 0x00, 0x39, 0x1A, 0x3A, 0x16, 0x1F, 0x1B, 0x00 },
    { 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xE4, 0x00 },
    { 0x0A, 0x17, 0x15, 0x22, 0x09, 0x21, 0x19, 0x05 },
    { 0x0B, 0x1C, 0x18, 0x23, 0x0D, 0x24, 0x10, 0x11 },
    { 0x3F, 0x30, 0x0C, 0x2E, 0x0E, 0x25, 0x36, 0x00 },
    { 0x00, 0x00, 0x12, 0x40, 0x0F, 0x26, 0x37, 0x00 },
    { 0x34, 0x2F, 0x13, 0x2D, 0x33, 0x27, 0x00, 0x38 },
    { 0x3E, 0x2A, 0x00, 0x41, 0x31, 0x42, 0x28, 0x2C },
    { 0x00, 0x00, 0xE3, 0x00, 0x00, 0x43, 0x00, 0x51 },
    { 0xE2, 0x00, 0x00, 0x00, 0x00, 0x45, 0xE5, 0xE6 },
    { 0x00, 0x53, 0x00, 0x00, 0xE1, 0x44, 0x00, 0x4F },
    { 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x50 },
    { 0x5F, 0x5C, 0x61, 0x5E, 0x59, 0x62, 0x55, 0x5B },
    { 0x54, 0x60, 0x56, 0x57, 0x5D, 0x5A, 0x58, 0x63 },
};
#else
uint8_t g_key_map_usb[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
    { 0x29, 0x2B },
    { 0x3D, 0x3C },
    { 0x00, 0x39 },
};
#endif
int tesetsuit_usb_get_hid_index2(void);
static void usb_keyscan_send_data(usb_hid_keyboard_report_t *rpt)
{
    rpt->kind = 0x1;

    int32_t ret = fhid_send_data(tesetsuit_usb_get_hid_index2(), (char *)rpt, USB_KEYBOARD_REPORTER_LEN);
    if (ret == -1) {
        test_suite_log_stringf("send data falied! ret:%d\n", ret);
        return;
    }
}

static usb_hid_keyboard_report_t g_send_key_msg;

static int test_usb_keyscan_callback(int argc, uint8_t argv[])
{
    uint8_t normal_key_num = 0;
    uint8_t tmp_key = 0;

    if (memset_s(&g_send_key_msg, sizeof(g_send_key_msg), 0, sizeof(g_send_key_msg)) != EOK) {
        return 0;
    }

    for (int i = 0; i < argc; i++) {
        tmp_key = argv[i];
        if (tmp_key >= USB_HID_SPECIAL_KEY_MIN && tmp_key <= USB_HID_SPECIAL_KEY_MAX) {
            g_send_key_msg.special_key |= (1 << (tmp_key - USB_HID_SPECIAL_KEY_MIN));
        } else {
            g_send_key_msg.key[normal_key_num] = tmp_key;
            normal_key_num++;
        }
    }
    usb_keyscan_send_data(&g_send_key_msg);
    return 1;
}

int tesetsuit_usb_keyboard(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xb) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
        osDelay(USB_HID_KEYBOARD_INIT_DELAY_MS);
    }

    uapi_set_keyscan_value_map((uint8_t **)g_key_map_usb, KEYSCAN_MAX_ROW, KEYSCAN_MAX_COL);
#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
    keyscan_porting_type_sel(FULL_KEYS_TYPE);
#else
    keyscan_porting_type_sel(SIX_KEYS_TYPE);
#endif
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback(test_usb_keyscan_callback);
    uapi_keyscan_enable();
    return 0;
}

static void test_send_key_sim_msg(void)
{
    int len = strlen(g_send_sim_key_msg);
    g_keyboard_sim_send_times++;
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
        usb_keyscan_send_data(&key_msg);
        uapi_tcxo_delay_ms(1ULL);
    }
    key_msg.key[0] = 0;
    usb_keyscan_send_data(&key_msg);
}

static void test_send_single_key_sim_msg(void)
{
    g_keyboard_sim_send_times++;
    usb_hid_keyboard_report_t key_msg = { 0 };
    int normal_key_num = 0;

    for (int i = 0; i < APP_AT_MAX_KEY_NUM; i++) {
        if (g_key_val[i] >= USB_HID_SPECIAL_KEY_MIN && g_key_val[i] <= USB_HID_SPECIAL_KEY_MAX) {
            key_msg.special_key |= (1 << (g_key_val[i] - USB_HID_SPECIAL_KEY_MIN));
        } else {
            key_msg.key[normal_key_num] = g_key_val[i];
            normal_key_num++;
        }
    }
    usb_keyscan_send_data(&key_msg);
    uapi_tcxo_delay_ms(10ULL);
    for (int8_t i = USB_HID_KEYBOARD_MAX_KEY_LENTH - 1; i >= 0; i--) {
        if (key_msg.key[i] != 0) {
            key_msg.key[i] = 0;
            usb_keyscan_send_data(&key_msg);
            uapi_tcxo_delay_ms(20ULL);
        }
    }
    key_msg.special_key = 0;
    usb_keyscan_send_data(&key_msg);
}

int tesetsuit_usb_keyboard_simulator(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xb) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
        osDelay(USB_HID_KEYBOARD_INIT_DELAY_MS);
    }

    uint32_t times;
    if (argc == 0) {
        times = 1;
    } else {
        times = (uint32_t)strtol(argv[0], NULL, 0);
    }
    test_suite_log_stringf("times:%d\r\n", times);
    for (uint32_t i = 0; i < times; i++) {
        osDelay(USB_HID_KEYBOARD_INIT_DELAY_MS);
        test_send_key_sim_msg();
    }

    return 0;
}

int tesetsuit_usb_keyboard_input(int argc, char *argv[])
{
    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xb) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
    }

    if (argc == 0 || argc > APP_AT_MAX_KEY_NUM) {
        test_suite_log_stringf("input para error!");
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }

    if (memset_s(&g_key_val, sizeof(g_key_val), 0, sizeof(g_key_val)) != EOK) {
        return TEST_SUITE_TEST_FAILED;
    }
    for (int i = 0; i < argc; i++) {
        g_key_val[i] = (uint8_t)strtol(argv[i], NULL, 0);
    }

    osDelay(USB_HID_KEYBOARD_INIT_DELAY_MS);
    test_send_single_key_sim_msg();
    return TEST_SUITE_OK;
}

int tesetsuit_usb_keyboard_get_times(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    test_suite_log_stringf("usb keyboard send data times:%d\r\n", g_keyboard_sim_send_times);
    return 0;
}
