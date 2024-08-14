/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: RCU Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
 */
#include "securec.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "app_init.h"
#include "watchdog.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "osal_msgqueue.h"
#include "keyscan.h"
#include "keyscan_porting.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_rcu_server/sle_rcu_server.h"
#include "sle_rcu_server/sle_rcu_server_adv.h"
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
#include "ble_rcu_server/ble_rcu_server.h"
#include "ble_rcu_server/ble_rcu_server_adv.h"
#include "ble_rcu_server/ble_hid_rcu_server.h"
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
#include "bts_le_gap.h"
#include "../ir/ir_nec.h"

#define USB_HID_MAX_KEY_LENTH              6
#define KEYSCAN_REPORT_MAX_KEY_NUMS        6
#define CONVERT_DEC_TO_HEX                 16
#define MAX_NUM_OF_DEC                     10
#define LENGTH_OF_KEY_VALUE_STR            5
#define SLE_RCU_PARAM_ARGC_2               2
#define SLE_RCU_PARAM_ARGC_3               3
#define SLE_RCU_PARAM_ARGC_4               4

#define SLE_RCU_STATE_DISCONNECT           0
#define SLE_RCU_STATE_CONNECTED            1

#define RCU_TASK_STACK_SIZE                0xc00
#define RCU_TASK_PRIO                      (osPriority_t)(13)
#define RCU_TASK_DURATION_MS               2000
#define SLE_RCU_WAIT_SSAPS_READY           500
#define SLE_RCU_SERVER_DELAY_COUNT         3
#define SLE_ADV_HANDLE_DEFAULT             1
#define SLE_RCU_SERVER_MSG_QUEUE_MAX_SIZE  32
#define SLE_RCU_SERVER_LOG                 "[sle rcu server]"
#define USB_RCU_TASK_DELAY_MS              10

#define RCU_KEY_A                          0x4
#define RCU_KEY_B                          0x5
#define RCU_KEY_C                          0x6
#define RCU_KEY_D                          0x7
#define RCU_KEY_E                          0x8
#define RCU_KEY_F                          0x9
#define RCU_KEY_G                          0xA
#define RCU_KEY_H                          0xB
#define RCU_KEY_I                          0xC
#define RCU_KEY_J                          0xD

#define RCU_KEY_APPLIC                     0x65
#define RCU_KEY_ENTER                      0x28
#define RCU_KEY_PAGEUP                     0x4B
#define RCU_KEY_PAGEDN                     0x4E
#define RCU_KEY_RIGHT                      0x4F
#define RCU_KEY_LEFT                       0x50
#define RCU_KEY_DOWN                       0x51
#define RCU_KEY_UP                         0x52

#define IR_NEC_USER_CODE                   0x00
#define IR_NEC_KEY_UP                      0xCA
#define IR_NEC_KEY_DOWN                    0xD2
#define IR_NEC_KEY_RIGHT                   0xC1
#define IR_NEC_KEY_LEFT                    0x99
#define IR_NEC_KEY_SELECT                  0xCE
#define IR_NEC_KEY_BACK                    0x90
#define IR_NEC_KEY_MENU                    0x9D
#define IR_NEC_KEY_POWER                   0x9C
#define IR_NEC_KEY_HOME                    0xCB
#define IR_NEC_KEY_VOLUMEUP                0x80
#define IR_NEC_KEY_VOLUMEDOWN              0x81
#define IR_NEC_KEY_MUTE                    0xDD

static bool g_switch_mouse_and_keyboard = true;
static bool g_check_consumer_send = false;
static bool g_check_mouse_send = false;
static bool g_check_keyboard_send = false;

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER) && !defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static bool g_switch_sle_and_ble = false;
#else
static bool g_switch_sle_and_ble = true;
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

static uint32_t g_keyboard_send_count = 0;
static uint16_t g_conn_id = 0;

#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
static uint8_t g_key_map[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
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
static uint8_t g_key_map[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL]  = {
    { 0xD, 0xC },
    { 0x4F, 0x50 },
    { 0x51, 0x52 },
};
#endif /* CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE */

typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
        uint8_t reserved   : 5;
    } b;
    uint8_t d8;
} mouse_key_t;

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
typedef struct usb_hid_sle_mouse_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_sle_mouse_report_t;

typedef struct usb_hid_sle_keyboard_report {
    uint8_t kind;
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_sle_keyboard_report_t;

typedef struct usb_hid_sle_consumer_report {
    uint8_t kind;
    uint8_t comsumer_key0;
    uint8_t comsumer_key1;
} usb_hid_sle_consumer_report_t;
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
typedef struct usb_hid_ble_mouse_report {
    mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_ble_mouse_report_t;

typedef struct usb_hid_ble_keyboard_report {
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_ble_keyboard_report_t;

typedef struct usb_hid_ble_consumer_report {
    uint8_t comsumer_key0;
    uint8_t comsumer_key1;
} usb_hid_ble_consumer_report_t;
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static usb_hid_sle_keyboard_report_t g_hid_sle_keyboard_report;
static usb_hid_sle_mouse_report_t g_hid_sle_mouse_report;
static usb_hid_sle_consumer_report_t g_hid_sle_consumer_report;
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static usb_hid_ble_keyboard_report_t g_hid_ble_keyboard_report;
static usb_hid_ble_mouse_report_t g_hid_ble_mouse_report;
static usb_hid_ble_consumer_report_t g_hid_ble_consumer_report;
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    if (read_cb_para == NULL) {
        osal_printk("%s ssaps_server_read_request_cbk fail, read_cb_para is null!\r\n", SLE_RCU_SERVER_LOG);
        return;
    }
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
                SLE_RCU_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
}

static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(write_cb_para);
    unused(status);

    if (write_cb_para == NULL) {
        osal_printk("%s ssaps_server_write_request_cbk fail, write_cb_para is null!\r\n", SLE_RCU_SERVER_LOG);
        return;
    }
    osal_printk("%s ssaps write request callback cbk handle:%x, status:%x\r\n",
                SLE_RCU_SERVER_LOG, write_cb_para->handle, status);
}

static void sle_rcu_keyboard_send_report(uint8_t key_value, bool send_flag)
{
    if (key_value != 0) {
        g_hid_sle_keyboard_report.key[g_keyboard_send_count++] = key_value;
    }
    if (send_flag) {
        g_hid_sle_keyboard_report.kind = 0x1;
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_keyboard_report,
                                             sizeof(usb_hid_sle_keyboard_report_t), g_conn_id);
        if (memset_s(&g_hid_sle_keyboard_report, sizeof(g_hid_sle_keyboard_report), 0,
                     sizeof(g_hid_sle_keyboard_report)) != EOK) {
            g_keyboard_send_count = 0;
            return;
        }
        g_keyboard_send_count = 0;
    }
}

static void sle_rcu_mouse_send_report(uint8_t key_value, bool send_flag)
{
    switch (key_value) {
        case RCU_KEY_RIGHT:
            g_hid_sle_mouse_report.x = 0x10;
            break;
        case RCU_KEY_LEFT:
            g_hid_sle_mouse_report.x = 0xF0;
            break;
        case RCU_KEY_DOWN:
            g_hid_sle_mouse_report.y = 0x10;
            break;
        case RCU_KEY_UP:
            g_hid_sle_mouse_report.y = 0xF0;
            break;
        default:
            break;
    }
    if (send_flag) {
        g_hid_sle_mouse_report.kind = 0x4;
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_mouse_report,
                                             sizeof(usb_hid_sle_mouse_report_t), g_conn_id);
        if (memset_s(&g_hid_sle_mouse_report, sizeof(g_hid_sle_mouse_report), 0,
                     sizeof(g_hid_sle_mouse_report)) != EOK) {
            return;
        }
    }
}

static void sle_rcu_consumer_send_report(uint8_t key_value)
{
    if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                 sizeof(g_hid_sle_consumer_report)) != EOK) {
        return;
    }
    g_hid_sle_consumer_report.kind = 0x3;
    switch (key_value) {
        case RCU_KEY_A:
            g_hid_sle_consumer_report.comsumer_key0 = 0xE2;
            g_hid_sle_consumer_report.comsumer_key1 = 0x00;
            break;
        case RCU_KEY_B:
            g_hid_sle_consumer_report.comsumer_key0 = 0x23;
            g_hid_sle_consumer_report.comsumer_key1 = 0x02;
            break;
        case RCU_KEY_C:
            g_hid_sle_consumer_report.comsumer_key0 = 0x24;
            g_hid_sle_consumer_report.comsumer_key1 = 0x02;
            break;
        case RCU_KEY_D:
            g_hid_sle_consumer_report.comsumer_key0 = 0x21;
            g_hid_sle_consumer_report.comsumer_key1 = 0x02;
            break;
        case RCU_KEY_E:
            g_hid_sle_consumer_report.comsumer_key0 = 0xE9;
            g_hid_sle_consumer_report.comsumer_key1 = 0x00;
            break;
        case RCU_KEY_F:
            g_hid_sle_consumer_report.comsumer_key0 = 0xEA;
            g_hid_sle_consumer_report.comsumer_key1 = 0x00;
            break;
        default:
            break;
    }
    sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_consumer_report,
                                         sizeof(usb_hid_sle_consumer_report_t), g_conn_id);
}

static void sle_rcu_system_power_down_send_report(void)
{
    if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                 sizeof(g_hid_sle_consumer_report)) != EOK) {
        return;
    }
    g_hid_sle_consumer_report.kind = 0x2;
    g_hid_sle_consumer_report.comsumer_key0 = 0x01;
    g_hid_sle_consumer_report.comsumer_key1 = 0x00;
    sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_consumer_report,
                                         sizeof(usb_hid_sle_consumer_report_t), g_conn_id);
}

static void sle_rcu_send_end(void)
{
    if (g_check_consumer_send) {
        if (memset_s(&g_hid_sle_consumer_report, sizeof(g_hid_sle_consumer_report), 0,
                     sizeof(g_hid_sle_consumer_report)) != EOK) {
            return;
        }
        g_hid_sle_consumer_report.kind = 0x3;
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_consumer_report,
                                             sizeof(usb_hid_sle_consumer_report_t), g_conn_id);
        g_check_consumer_send = false;
    }
    if (g_check_mouse_send) {
        if (memset_s(&g_hid_sle_mouse_report, sizeof(g_hid_sle_mouse_report), 0,
                     sizeof(g_hid_sle_mouse_report)) != EOK) {
            return;
        }
        g_hid_sle_mouse_report.kind = 0x4;
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_mouse_report,
                                             sizeof(usb_hid_sle_mouse_report_t), g_conn_id);
        g_check_mouse_send = false;
    }
    if (g_check_keyboard_send) {
        if (memset_s(&g_hid_sle_keyboard_report, sizeof(g_hid_sle_keyboard_report), 0,
                     sizeof(g_hid_sle_keyboard_report)) != EOK) {
            return;
        }
        g_hid_sle_keyboard_report.kind = 0x1;
        sle_rcu_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_sle_keyboard_report,
                                             sizeof(usb_hid_sle_keyboard_report_t), g_conn_id);
        g_check_keyboard_send = false;
    }
}
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
static void ble_rcu_keyboard_send_report(uint8_t key_value, bool send_flag)
{
    if (key_value != 0) {
        g_hid_ble_keyboard_report.key[g_keyboard_send_count++] = key_value;
    }
    if (send_flag) {
        ble_hid_rcu_server_send_keyboard_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_keyboard_report,
                                                              sizeof(usb_hid_ble_keyboard_report_t), g_conn_id);
        if (memset_s(&g_hid_ble_keyboard_report, sizeof(g_hid_ble_keyboard_report), 0,
                     sizeof(g_hid_ble_keyboard_report)) != EOK) {
            g_keyboard_send_count = 0;
            return;
        }
        g_keyboard_send_count = 0;
    }
}

static void ble_rcu_mouse_send_report(uint8_t key_value, bool send_flag)
{
    switch (key_value) {
        case RCU_KEY_RIGHT:
            g_hid_ble_mouse_report.x = 0x10;
            break;
        case RCU_KEY_LEFT:
            g_hid_ble_mouse_report.x = 0xF0;
            break;
        case RCU_KEY_DOWN:
            g_hid_ble_mouse_report.y = 0x10;
            break;
        case RCU_KEY_UP:
            g_hid_ble_mouse_report.y = 0xF0;
            break;
        default:
            break;
    }
    if (send_flag) {
        ble_hid_rcu_server_send_mouse_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_mouse_report,
                                                           sizeof(usb_hid_ble_mouse_report_t), g_conn_id);
        if (memset_s(&g_hid_ble_mouse_report, sizeof(g_hid_ble_mouse_report), 0,
                     sizeof(g_hid_ble_mouse_report)) != EOK) {
            return;
        }
    }
}

static void ble_rcu_consumer_send_report(uint8_t key_value)
{
    if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                 sizeof(g_hid_ble_consumer_report)) != EOK) {
        return;
    }
    switch (key_value) {
        case RCU_KEY_A:
            g_hid_ble_consumer_report.comsumer_key0 = 0xE2;
            g_hid_ble_consumer_report.comsumer_key1 = 0x00;
            break;
        case RCU_KEY_B:
            g_hid_ble_consumer_report.comsumer_key0 = 0x23;
            g_hid_ble_consumer_report.comsumer_key1 = 0x02;
            break;
        case RCU_KEY_C:
            g_hid_ble_consumer_report.comsumer_key0 = 0x24;
            g_hid_ble_consumer_report.comsumer_key1 = 0x02;
            break;
        case RCU_KEY_D:
            g_hid_ble_consumer_report.comsumer_key0 = 0x21;
            g_hid_ble_consumer_report.comsumer_key1 = 0x02;
            break;
        case RCU_KEY_E:
            g_hid_ble_consumer_report.comsumer_key0 = 0xE9;
            g_hid_ble_consumer_report.comsumer_key1 = 0x00;
            break;
        case RCU_KEY_F:
            g_hid_ble_consumer_report.comsumer_key0 = 0xEA;
            g_hid_ble_consumer_report.comsumer_key1 = 0x00;
            break;
        default:
            break;
    }
    ble_hid_rcu_server_send_consumer_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_consumer_report,
                                                          sizeof(usb_hid_ble_consumer_report_t), g_conn_id);
}

static void ble_rcu_system_power_down_send_report(void)
{
    if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                 sizeof(g_hid_ble_consumer_report)) != EOK) {
        return;
    }
    g_hid_ble_consumer_report.comsumer_key0 = 0x01;
    g_hid_ble_consumer_report.comsumer_key1 = 0x00;
    ble_hid_rcu_server_send_power_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_consumer_report,
                                                       sizeof(usb_hid_ble_consumer_report_t), g_conn_id);
}

static void ble_rcu_send_end(void)
{
    if (g_check_consumer_send) {
        if (memset_s(&g_hid_ble_consumer_report, sizeof(g_hid_ble_consumer_report), 0,
                     sizeof(g_hid_ble_consumer_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_consumer_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_consumer_report,
                                                              sizeof(usb_hid_ble_consumer_report_t), g_conn_id);
        g_check_consumer_send = false;
    }
    if (g_check_mouse_send) {
        if (memset_s(&g_hid_ble_mouse_report, sizeof(g_hid_ble_mouse_report), 0,
                     sizeof(g_hid_ble_mouse_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_mouse_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_mouse_report,
                                                           sizeof(usb_hid_ble_mouse_report_t), g_conn_id);
        g_check_mouse_send = false;
    }
    if (g_check_keyboard_send) {
        if (memset_s(&g_hid_ble_keyboard_report, sizeof(g_hid_ble_keyboard_report), 0,
                     sizeof(g_hid_ble_keyboard_report)) != EOK) {
            return;
        }
        ble_hid_rcu_server_send_keyboard_input_report_by_uuid((uint8_t *)(uintptr_t)&g_hid_ble_keyboard_report,
                                                              sizeof(usb_hid_ble_keyboard_report_t), g_conn_id);
        g_check_keyboard_send = false;
    }
}
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

static void rcu_keyboard_send_report(uint8_t key_value)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_keyboard_send_report(key_value, false);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_keyboard_send_report(key_value, false);
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_mouse_and_keyboard_send_report(uint8_t key_value)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        if (g_switch_mouse_and_keyboard) {
            sle_rcu_mouse_send_report(key_value, false);
            g_check_mouse_send = true;
        } else {
            sle_rcu_keyboard_send_report(key_value, false);
            g_check_keyboard_send = true;
        }
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        if (g_switch_mouse_and_keyboard) {
            ble_rcu_mouse_send_report(key_value, false);
            g_check_mouse_send = true;
        } else {
            ble_rcu_keyboard_send_report(key_value, false);
            g_check_keyboard_send = true;
        }
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_system_power_down_send_report(void)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_system_power_down_send_report();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_system_power_down_send_report();
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_mouse_and_keyboard_send_start(void)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        if (g_check_mouse_send) {
            sle_rcu_mouse_send_report(0, true);
        }
        if (g_check_keyboard_send) {
            sle_rcu_keyboard_send_report(0, true);
        }
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        if (g_check_mouse_send) {
            ble_rcu_mouse_send_report(0, true);
        }
        if (g_check_keyboard_send) {
            ble_rcu_keyboard_send_report(0, true);
        }
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_send_end(void)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_send_end();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_send_end();
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

static void rcu_consumer_send_report(uint8_t key_value)
{
    if (g_switch_sle_and_ble) {
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
        sle_rcu_consumer_send_report(key_value);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    } else {
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
        ble_rcu_consumer_send_report(key_value);
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */
    }
}

#if defined(CONFIG_SAMPLE_SUPPORT_IR)
static void sle_ir_function_switch(uint8_t key)
{
    switch (key) {
        case RCU_KEY_A:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_MUTE);
            break;
        case RCU_KEY_B:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_HOME);
            break;
        case RCU_KEY_C:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_BACK);
            break;
        case RCU_KEY_D:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_VOLUMEDOWN);
            break;
        case RCU_KEY_E:
            ir_transmit_nec(IR_NEC_USER_CODE, IR_NEC_KEY_VOLUMEUP);
            break;
        default:
            break;
    }
}
#endif

static int rcu_keyscan_callback(int key_nums, uint8_t key_values[])
{
    rcu_send_end();
    for (uint8_t i = 0; i < key_nums; i++) {
        switch (key_values[i]) {
            case RCU_KEY_A:
            case RCU_KEY_B:
            case RCU_KEY_C:
            case RCU_KEY_D:
            case RCU_KEY_E:
            case RCU_KEY_F:
#if defined(CONFIG_SAMPLE_SUPPORT_IR)
                sle_ir_function_switch(key_values[i]);
#endif
                rcu_consumer_send_report(key_values[i]);
                g_check_consumer_send = true;
                break;
            case RCU_KEY_H:
                rcu_system_power_down_send_report();
                break;
            case RCU_KEY_APPLIC:
            case RCU_KEY_ENTER:
            case RCU_KEY_PAGEUP:
            case RCU_KEY_PAGEDN:
                rcu_keyboard_send_report(key_values[i]);
                g_check_keyboard_send = true;
                break;
            case RCU_KEY_RIGHT:
            case RCU_KEY_LEFT:
            case RCU_KEY_DOWN:
            case RCU_KEY_UP:
                rcu_mouse_and_keyboard_send_report(key_values[i]);
                break;
            case RCU_KEY_G:
                g_switch_mouse_and_keyboard = !g_switch_mouse_and_keyboard;
                break;
            case RCU_KEY_I:
                g_switch_sle_and_ble = !g_switch_sle_and_ble;
                break;
            case RCU_KEY_J:
                g_conn_id = !g_conn_id;
                break;
            default:
                break;
        }
    }
    rcu_mouse_and_keyboard_send_start();
    return 1;
}

static void *rcu_task(const char *arg)
{
    unused(arg);

    osal_printk("enter rcu_task!\r\n");

    /* pin config, full key need run it */
#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
    keyscan_porting_config_pins();
#endif
    /* keyscan init */
    uapi_set_keyscan_value_map((uint8_t **)g_key_map, KEYSCAN_MAX_ROW, KEYSCAN_MAX_COL);
#if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
    keyscan_porting_type_sel(FULL_KEYS_TYPE);
#else
    keyscan_porting_type_sel(SIX_KEYS_TYPE);
#endif
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    osal_msleep(RCU_TASK_DURATION_MS);
    uapi_keyscan_register_callback(rcu_keyscan_callback);
    uapi_keyscan_enable();

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
    ble_rcu_server_init();
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
    /* sle server init */
    sle_rcu_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);

    while (get_g_ssaps_ready() == false) {
        osal_msleep(RCU_TASK_DURATION_MS);
    }
    while (get_g_conn_update() == 0) {
        osal_msleep(RCU_TASK_DURATION_MS);
    }

    while (1) {
        uapi_watchdog_kick();
        if (get_g_sle_conn_num()) {
            osal_msleep(RCU_TASK_DURATION_MS);
        }
    }
    uapi_keyscan_deinit();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER */
    return NULL;
}

static void rcu_entry(void)
{
    osThreadAttr_t attr = { 0 };

    attr.name = "RcuTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = RCU_TASK_STACK_SIZE;
    attr.priority = RCU_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)rcu_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the rcu_entry. */
app_run(rcu_entry);