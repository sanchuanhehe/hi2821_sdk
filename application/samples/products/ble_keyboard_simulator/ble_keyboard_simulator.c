/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE KEYBOARD SIMULATOR Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-10, Create file. \n
 */

#include "cmsis_os2.h"
#include "common_def.h"
#include "osal_debug.h"
#include "app_init.h"
#include "bts_le_gap.h"
#include "ble_keyboard_server/ble_keyboard_server_adv.h"
#include "ble_keyboard_server/ble_keyboard_server.h"
#include "ble_keyboard_server/ble_hid_keyboard_server.h"
#include "securec.h"
#include "uart.h"
#include "tcxo.h"

#define BLE_KEYBOARD_SIMULATOR_TASK_STACK_SIZE            0x600
#define BLE_KEYBOARD_SIMULATOR_TASK_PRIO                  (osPriority_t)(17)
#define BLE_KEYBOARD_SIMULATOR_TASK_DURATION_MS           10
#define BLE_KEYBOARD_SIMULATOR_DELAY_S                    2000
#define BLE_KEYBOARD_MAX_KEY_LENTH                        6
#define BLE_KEYBOARD_REPORT_LENTH                         7

#define BLE_KEYBOARD_KEY_ZERO                             (0x27)
#define BLE_KEYBOARD_KEY_SPACE                            (0x2c)
#define BLE_KEYBOARD_KEY_ENTER                            (0x28)
#define BLE_KEYBOARD_KEY_ONE                              (0x1E)
#define BLE_KEYBOARD_KEY_A                                (0x04)

typedef struct usb_hid_keyboard_report {
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t key[BLE_KEYBOARD_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

static usb_hid_keyboard_report_t g_send_key_msg;

static void test_ble_keyboard_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    char *buff = (char *)buffer;
    uint8_t normal_key_num = BLE_KEYBOARD_MAX_KEY_LENTH - 1;
    char tmp_key = 0;
    if (memset_s(&g_send_key_msg, sizeof(g_send_key_msg), 0, sizeof(g_send_key_msg)) != EOK) {
        return;
    }

    for (uint32_t i = 0; i < length; i++) {
        tmp_key = buff[i];
        switch (tmp_key) {
            case '0':
                g_send_key_msg.key[normal_key_num]  = BLE_KEYBOARD_KEY_ZERO;
                normal_key_num--;
                break;
            case ' ':
                g_send_key_msg.key[normal_key_num]  = BLE_KEYBOARD_KEY_SPACE;
                normal_key_num--;
                break;
            case '\n':
                g_send_key_msg.key[normal_key_num]  = BLE_KEYBOARD_KEY_ENTER;
                normal_key_num--;
                break;
            default:
                if (tmp_key >= '1' && tmp_key <= '9') {
                    g_send_key_msg.key[normal_key_num] = BLE_KEYBOARD_KEY_ONE + tmp_key - '1';
                } else if (tmp_key >= 'a' && tmp_key <= 'z') {
                    g_send_key_msg.key[normal_key_num] = BLE_KEYBOARD_KEY_A + tmp_key - 'a';
                }
            normal_key_num--;
        }
    }
    uapi_tcxo_delay_ms(BLE_KEYBOARD_SIMULATOR_DELAY_S);
    ble_hid_keyboard_server_send_input_report_by_uuid((uint8_t *)(uintptr_t)&g_send_key_msg, BLE_KEYBOARD_REPORT_LENTH);
    if (memset_s(&g_send_key_msg, sizeof(g_send_key_msg), 0, sizeof(g_send_key_msg)) != EOK) {
        return;
    }
    ble_hid_keyboard_server_send_input_report_by_uuid((uint8_t *)(uintptr_t)&g_send_key_msg, BLE_KEYBOARD_REPORT_LENTH);
}

static void *ble_keyboard_simulator_task(const char *arg)
{
    unused(arg);
    errcode_t errcode = uapi_uart_register_rx_callback(UART_BUS_1, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                       1, test_ble_keyboard_uart_read_int_handler);
    if (errcode != ERRCODE_SUCC) {
        return NULL;
    }
    ble_keyboard_server_init();
    while (get_g_connection_state() == GAP_BLE_STATE_DISCONNECTED) {
        osDelay(BLE_KEYBOARD_SIMULATOR_TASK_DURATION_MS);
    }

    return NULL;
}

static void ble_keyboard_simulator_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "BLEKeyboardsimulatorTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BLE_KEYBOARD_SIMULATOR_TASK_STACK_SIZE;
    attr.priority = BLE_KEYBOARD_SIMULATOR_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)ble_keyboard_simulator_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the ble_keyboard_simulator_entry. */
app_run(ble_keyboard_simulator_entry);