/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Initialize Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-07, Create file. \n
 */
#include "securec.h"
#include "osal_debug.h"
#include "gadget/f_hid.h"
#include "keyscan.h"
#include "keyscan_porting.h"
#include "keyscan_init.h"

#define USB_HID_SPECIAL_KEY_MIN             (0xE0)
#define USB_HID_SPECIAL_KEY_MAX             (0xE7)
#define USB_KEYBOARD_REPORTER_LEN           9
#define USB_HID_KEYBOARD_MAX_KEY_LENTH      6

typedef struct usb_hid_keyboard_report {
    uint8_t kind;
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reversed;                            /*!< Reversed, Must be zero */
    uint8_t key[USB_HID_KEYBOARD_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

static usb_hid_keyboard_report_t g_send_keyscan_msg;
static int g_usb_keyscan_hid_index = -1;

#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
uint8_t g_keyscan_map_usb[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
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
uint8_t g_keyscan_map_usb[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
    { 0x29, 0x2B },
    { 0x3D, 0x3C },
    { 0x00, 0x39 },
};
#endif

static void usb_keyscan_send_data(usb_hid_keyboard_report_t *rpt)
{
    rpt->kind = 0x1;

    int32_t ret = fhid_send_data(g_usb_keyscan_hid_index, (char *)rpt, USB_KEYBOARD_REPORTER_LEN);
    if (ret == -1) {
        osal_printk("send data falied! ret:%d\n", ret);
        return;
    }
}

static int usb_keyscan_callback(int argc, uint8_t argv[])
{
    uint8_t normal_key_num = 0;
    uint8_t tmp_key = 0;

    if (memset_s(&g_send_keyscan_msg, sizeof(g_send_keyscan_msg), 0, sizeof(g_send_keyscan_msg)) != EOK) {
        return 0;
    }

    for (int i = 0; i < argc; i++) {
        tmp_key = argv[i];
        if (tmp_key >= USB_HID_SPECIAL_KEY_MIN && tmp_key <= USB_HID_SPECIAL_KEY_MAX) {
            g_send_keyscan_msg.special_key |= (1 << (tmp_key - USB_HID_SPECIAL_KEY_MIN));
        } else {
            g_send_keyscan_msg.key[normal_key_num] = tmp_key;
            normal_key_num++;
        }
    }
    usb_keyscan_send_data(&g_send_keyscan_msg);
    return 1;
}

void keyscan_init(int hid_index)
{
    g_usb_keyscan_hid_index = hid_index;
    uapi_set_keyscan_value_map((uint8_t **)g_keyscan_map_usb, KEYSCAN_MAX_ROW, KEYSCAN_MAX_COL);
#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
    keyscan_porting_type_sel(FULL_KEYS_TYPE);
#else
    keyscan_porting_type_sel(SIX_KEYS_TYPE);
#endif
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback(usb_keyscan_callback);
    uapi_keyscan_enable();
}
