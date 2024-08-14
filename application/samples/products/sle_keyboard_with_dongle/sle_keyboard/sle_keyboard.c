/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE KEYBOARD Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-28, Create file. \n
 */
#include "securec.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "app_init.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "osal_msgqueue.h"
#include "keyscan.h"
#include "keyscan_porting.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_keyboard_server.h"
#include "sle_keyboard_server_adv.h"

#define USB_HID_KEYBOARD_MAX_KEY_LENTH          6
#define USB_KEYBOARD_REPORTER_LEN               9
#define KEYSCAN_REPORT_MAX_KEY_NUMS             6
#define CONVERT_DEC_TO_HEX                      16
#define MAX_NUM_OF_DEC                          10
#define LENGTH_OF_KEY_VALUE_STR                 5
#define USB_HID_SPECIAL_KEY_MIN                 0xE0
#define USB_HID_SPECIAL_KEY_MAX                 0xE7
#define SLE_KEYBOARD_PARAM_ARGC_2               2
#define SLE_KEYBOARD_PARAM_ARGC_3               3
#define SLE_KEYBOARD_PARAM_ARGC_4               4

#define SLE_KEYBOARD_TASK_STACK_SIZE            0x1000
#define SLE_KEYBOARD_TASK_PRIO                  (osPriority_t)(17)
#define SLE_KEYBOARD_TASK_DURATION_MS           2000
#define SLE_KEYBOARD_SERVER_DELAY_COUNT         3
#define SLE_ADV_HANDLE_DEFAULT                  1
#define SLE_KEYBOARD_SERVER_MSG_QUEUE_LEN       5
#define SLE_KEYBOARD_SERVER_MSG_QUEUE_MAX_SIZE  32
#define SLE_KEYBOARD_SERVER_QUEUE_DELAY         0xFFFFFFFF
#define SLE_KEYBOARD_SERVER_LOG                 "[sle keyboard server]"

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
    { 0x04, 0x05 },
    { 0x06, 0x07 },
    { 0x08, 0x09 },
};
#endif /* CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE */
typedef struct usb_hid_keyboard_report {
    uint8_t kind;
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reversed;                            /*!< Reversed, Must be zero */
    uint8_t key[USB_HID_KEYBOARD_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

static usb_hid_keyboard_report_t g_hid_keyboard_report;
static unsigned long g_sle_keyboard_server_msgqueue_id;

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    if (read_cb_para == NULL) {
        osal_printk("%s ssaps_server_read_request_cbk fail, read_cb_para is null!\r\n", SLE_KEYBOARD_SERVER_LOG);
        return;
    }
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
                SLE_KEYBOARD_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
}
static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    char *key_value_str[KEYSCAN_REPORT_MAX_KEY_NUMS];
    uint8_t *key_values = NULL;
    uint16_t key_nums, i;

    if (write_cb_para == NULL) {
        osal_printk("%s ssaps_server_write_request_cbk fail, write_cb_para is null!\r\n", SLE_KEYBOARD_SERVER_LOG);
        return;
    }
    osal_printk("%s ssaps write request callback cbk handle:%x, status:%x\r\n",
                SLE_KEYBOARD_SERVER_LOG, write_cb_para->handle, status);
    if ((write_cb_para->length > 0) && write_cb_para->value) {
        // todo,recv data from client,send to pc by uart or send to keyboard
        osal_printk("%s recv data from client, len =[%d], data= ", SLE_KEYBOARD_SERVER_LOG, write_cb_para->length);
        for (i = 0; i < write_cb_para->length; i++) {
            osal_printk("0x%02x ", write_cb_para->value[i]);
        }
        osal_printk("\r\n");
        key_values = write_cb_para->value;
        key_nums = write_cb_para->length;
        for (i = 0; i < write_cb_para->length; i++) {
            key_value_str[i] = (char *)osal_vmalloc(LENGTH_OF_KEY_VALUE_STR);
            if (key_value_str[i] == NULL) {
                osal_printk("[ERROR]send input report new fail\r\n");
                return;
            }
            key_value_str[i][0] = '0';
            key_value_str[i][1] = 'x';
            uint32_t tran = key_values[i] / CONVERT_DEC_TO_HEX;
            if (tran < MAX_NUM_OF_DEC) {
                key_value_str[i][SLE_KEYBOARD_PARAM_ARGC_2] = '0' + tran;
            } else {
                key_value_str[i][SLE_KEYBOARD_PARAM_ARGC_2] = ('A' + tran - MAX_NUM_OF_DEC);
            }
            tran = key_values[i] % CONVERT_DEC_TO_HEX;
            if (tran < MAX_NUM_OF_DEC) {
                key_value_str[i][SLE_KEYBOARD_PARAM_ARGC_3] = '0' + tran;
            } else {
                key_value_str[i][SLE_KEYBOARD_PARAM_ARGC_3] = ('A' + tran - MAX_NUM_OF_DEC);
            }
            key_value_str[i][SLE_KEYBOARD_PARAM_ARGC_4] = '\0';
        }
        // 发送到键盘
        if (key_nums > 0) {
            uapi_ble_hid_keyboard_input_str(key_nums, (char **)key_value_str);
        }
        for (i = 0; i < key_nums; i++) {
            osal_vfree(key_value_str[i]);
        }
    }
}

static void sle_keyboard_server_create_msgqueue(void)
{
    if (osal_msg_queue_create("sle_keyboard_server_msgqueue", SLE_KEYBOARD_SERVER_MSG_QUEUE_LEN, \
        (unsigned long *)&g_sle_keyboard_server_msgqueue_id, 0, SLE_KEYBOARD_SERVER_MSG_QUEUE_MAX_SIZE) \
        != EOK) {
        osal_printk("^%s sle_keyboard_server_create_msgqueue message queue create failed!\n", SLE_KEYBOARD_SERVER_LOG);
    }
}

static void sle_keyboard_server_delete_msgqueue(void)
{
    osal_msg_queue_delete(g_sle_keyboard_server_msgqueue_id);
}

static void sle_keyboard_server_write_msgqueue(uint8_t *buffer_addr, uint32_t buffer_size)
{
    if (buffer_addr == NULL) {
        osal_printk("%s sle_keyboard_server_write_msgqueue fail, buffer_addr is null!\r\n", SLE_KEYBOARD_SERVER_LOG);
        return;
    }
    osal_msg_queue_write_copy(g_sle_keyboard_server_msgqueue_id, (void *)buffer_addr, \
                              (uint32_t)buffer_size, 0);
}

static int32_t sle_keyboard_server_receive_msgqueue(uint8_t *buffer_addr, uint32_t *buffer_size)
{
    if (buffer_addr == NULL) {
        osal_printk("%s sle_keyboard_server_receive_msgqueue fail, buffer_addr is null!\r\n", SLE_KEYBOARD_SERVER_LOG);
        return -1;
    }
    return osal_msg_queue_read_copy(g_sle_keyboard_server_msgqueue_id, (void *)buffer_addr, \
                                    buffer_size, SLE_KEYBOARD_SERVER_QUEUE_DELAY);
}

static void sle_keyboard_server_rx_buf_init(uint8_t *buffer_addr, uint32_t buffer_size)
{
    if (buffer_addr == NULL) {
        osal_printk("%s sle_keyboard_server_rx_buf_init fail, buffer_addr is null!\r\n", SLE_KEYBOARD_SERVER_LOG);
        return;
    }
    if (memset_s(buffer_addr, buffer_size, 0, buffer_size) != EOK) {
        osal_printk("%s sle_keyboard_server_rx_buf_init memset_s fail!\r\n", SLE_KEYBOARD_SERVER_LOG);
    }
}

static int sle_keyboard_keyscan_callback(int key_nums, uint8_t key_values[])
{
    uint8_t normal_key_num = 0;

    if (sle_keyboard_client_is_connected() == 0) {
        return 1;
    }
    if (memset_s(&g_hid_keyboard_report, sizeof(g_hid_keyboard_report), 0, sizeof(g_hid_keyboard_report)) != EOK) {
        return 0;
    }
    for (uint8_t i = 0; i < key_nums; i++) {
        uint8_t tmp_key = key_values[i];
        if (tmp_key >= USB_HID_SPECIAL_KEY_MIN && tmp_key <= USB_HID_SPECIAL_KEY_MAX) {
            g_hid_keyboard_report.special_key |= (1 << (tmp_key - USB_HID_SPECIAL_KEY_MIN));
        } else {
            g_hid_keyboard_report.key[normal_key_num] = tmp_key;
            normal_key_num++;
        }
    }
    g_hid_keyboard_report.kind = 0x01;
    sle_keyboard_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_hid_keyboard_report,
                                              sizeof(usb_hid_keyboard_report_t));
    return 1;
}

static void *sle_keyboard_task(const char *arg)
{
    unused(arg);
    errcode_t ret;
    uint8_t rx_buf[SLE_KEYBOARD_SERVER_MSG_QUEUE_MAX_SIZE] = { 0 };
    uint32_t rx_length = SLE_KEYBOARD_SERVER_MSG_QUEUE_MAX_SIZE;
    uint8_t sle_connect_state[] = "sle_dis_connect";

    osal_printk("enter sle_keyboard_task!\r\n");
    // 1.delay for sle start
    osDelay(SLE_KEYBOARD_TASK_DURATION_MS * SLE_KEYBOARD_SERVER_DELAY_COUNT);
    // 2.pin config, full key need run it
    #if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
    keyscan_porting_config_pins();
    #endif
    // 3.keyscan init
    uapi_set_keyscan_value_map((uint8_t **)g_key_map, KEYSCAN_MAX_ROW, KEYSCAN_MAX_COL);
    #if defined(CONFIG_KEYSCAN_USE_FULL_KEYS_TYPE)
    keyscan_porting_type_sel(FULL_KEYS_TYPE);
    #else
    keyscan_porting_type_sel(SIX_KEYS_TYPE);
    #endif
    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    osDelay(SLE_KEYBOARD_TASK_DURATION_MS);
    uapi_keyscan_register_callback(sle_keyboard_keyscan_callback);
    uapi_keyscan_enable();
    // 4.sle server init.
    sle_keyboard_server_create_msgqueue();
    sle_keyboard_server_register_msg(sle_keyboard_server_write_msgqueue);
    sle_keyboard_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);

    while (1) {
        sle_keyboard_server_rx_buf_init(rx_buf, rx_length);
        sle_keyboard_server_receive_msgqueue(rx_buf, &rx_length);
        if (strncmp((const char *)rx_buf, (const char *)sle_connect_state, sizeof(sle_connect_state)) == 0) {
            ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
            if (ret != ERRCODE_SLE_SUCCESS) {
                osal_printk("%s sle_connect_state_changed_cbk,sle_start_announce fail :%02x\r\n",
                            SLE_KEYBOARD_SERVER_LOG, ret);
            }
        }
        rx_length = SLE_KEYBOARD_SERVER_MSG_QUEUE_MAX_SIZE;
    }
    sle_keyboard_server_delete_msgqueue();
    uapi_keyscan_deinit();
    return NULL;
}

static void sle_keyboard_entry(void)
{
    osThreadAttr_t attr = { 0 };

    attr.name = "SLEKeyboardTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_KEYBOARD_TASK_STACK_SIZE;
    attr.priority = SLE_KEYBOARD_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)sle_keyboard_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the sle_keyboard_entry. */
app_run(sle_keyboard_entry);