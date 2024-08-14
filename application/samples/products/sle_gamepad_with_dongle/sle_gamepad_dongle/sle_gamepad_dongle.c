/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD Dongle Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */
#include "securec.h"
#include "chip_io.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "app_init.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "implementation/usb_init.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_gamepad_client.h"
#include "sle_gamepad_hid.h"

#define SLE_GAMEPAD_DONGLE_TASK_STACK_SIZE   0x1000
#define SLE_GAMEPAD_DONGLE_TASK_PRIO         (osPriority_t)(17)
#define SLE_GAMEPAD_DONGLE_TASK_DELAY_MS     2000
#define USB_HID_GAMEPAD_INIT_DELAY_MS        (500UL)
#define USB_GAMEPAD_REPORTER_LEN             8
#define SLE_GAMEPAD_USB_MANUFACTURER         { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define SLE_GAMEPAD_USB_MANUFACTURER_LEN     20
#define SLE_GAMEPAD_USB_PRODUCT       { 'G', 0, 'A', 0, 'M', 0, 'E', 0, 'P', 0, 'A', 0, 'D', 0, '6', 0, '6', 0, '6', 0 }
#define SLE_GAMEPAD_USB_PRODUCT_LEN          22
#define SLE_GAMEPAD_USB_SERIAL               { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define SLE_GAMEPAD_USB_SERIAL_LEN           16
#define RECV_MAX_LENGTH                      13
#define USB_RECV_STACK_SIZE                  0x400
#define SLE_GAMEPAD_DONGLE_LOG               "[sle gamepad dongle]"

static bool g_sle_gamepad_dongle_inited = false;
static uint32_t g_sle_gamepad_dongle_hid_index = 0;

static void sle_gamepad_dongle_send_data(usb_hid_gamepad_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    rpt->kind = 0x03;
    int32_t ret = fhid_send_data(g_sle_gamepad_dongle_hid_index, (char *)rpt, USB_GAMEPAD_REPORTER_LEN);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_GAMEPAD_DONGLE_LOG, ret);
        return;
    }
}

static void sle_gamepad_send_to_server_handler(const uint8_t *buffer, uint16_t length)
{
    ssapc_write_param_t g_sle_gamepad_send_param = get_sle_gamepad_send_param();
    uint16_t g_sle_gamepad_conn_id = get_sle_gamepad_conn_id();
    g_sle_gamepad_send_param.data_len = length;
    g_sle_gamepad_send_param.data = (uint8_t *)buffer;
    ssapc_write_req(0, g_sle_gamepad_conn_id, &g_sle_gamepad_send_param);
    osal_printk("%s sle gamepad send data ,len: %d\r\n", SLE_GAMEPAD_DONGLE_LOG, length);
}

static void *sle_gamepad_dongle_usb_recv_task(const char *para)
{
    UNUSED(para);
    uint8_t recv_hid_data[RECV_MAX_LENGTH];

    osal_printk("%s enter sle_gamepad_dongle_usb_recv_task!\r\n", SLE_GAMEPAD_DONGLE_LOG);
    while (1) {
        int32_t ret = fhid_recv_data(g_sle_gamepad_dongle_hid_index, (char*)recv_hid_data, RECV_MAX_LENGTH);
        if (ret <= 0) {
            continue;
        }
        osal_printk("%s gamepad recv data from pc, len = [%d], data: \r\n", SLE_GAMEPAD_DONGLE_LOG, ret);
        for (int i = 0; i < ret; i++) {
            osal_printk("0x%02x ", recv_hid_data[i]);
        }
        osal_printk("\r\n");
        sle_gamepad_send_to_server_handler(recv_hid_data, ret);
    }
    osal_printk("%s usb recv task over\r\n", SLE_GAMEPAD_DONGLE_LOG);
    return NULL;
}

static uint8_t sle_gamepad_dongle_init_internal(device_type dtype)
{
    if (g_sle_gamepad_dongle_inited) {
        return SLE_GAMEPAD_DONGLE_OK;
    }

    const char manufacturer[SLE_GAMEPAD_USB_MANUFACTURER_LEN] = SLE_GAMEPAD_USB_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = SLE_GAMEPAD_USB_MANUFACTURER_LEN
    };

    const char product[SLE_GAMEPAD_USB_PRODUCT_LEN] = SLE_GAMEPAD_USB_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = SLE_GAMEPAD_USB_PRODUCT_LEN
    };

    const char serial[SLE_GAMEPAD_USB_SERIAL_LEN] = SLE_GAMEPAD_USB_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = SLE_GAMEPAD_USB_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };

    if (dtype == DEV_HID) {
        g_sle_gamepad_dongle_hid_index = sle_gamepad_dongle_set_report_desc_hid();
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        osal_printk("%s set device info fail!\r\n", SLE_GAMEPAD_DONGLE_LOG);
        return SLE_GAMEPAD_DONGLE_FAILED;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        osal_printk("%s usb_init failed!\r\n", SLE_GAMEPAD_DONGLE_LOG);
        return SLE_GAMEPAD_DONGLE_FAILED;
    }
    osal_kthread_create((void *)sle_gamepad_dongle_usb_recv_task, NULL, "sle_gamepad_dongle_recv",
                        USB_RECV_STACK_SIZE);
    g_sle_gamepad_dongle_inited = true;
    return SLE_GAMEPAD_DONGLE_OK;
}

static uint8_t sle_gamepad_dongle_init(void)
{
    if (!g_sle_gamepad_dongle_inited) {
        if (sle_gamepad_dongle_init_internal(DEV_HID) != SLE_GAMEPAD_DONGLE_OK) {
            return SLE_GAMEPAD_DONGLE_FAILED;
        }
        osDelay(USB_HID_GAMEPAD_INIT_DELAY_MS);
    }
    return SLE_GAMEPAD_DONGLE_OK;
}

static void sle_gamepad_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                        errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    usb_hid_gamepad_report_t *recv_usb_hid_gamepad = NULL;
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        osal_printk("%s sle_gamepad_notification_cb fail, recv data is null!\r\n", SLE_GAMEPAD_DONGLE_LOG);
    }
    osal_printk("%s sle gamepad recive notification\r\n", SLE_GAMEPAD_DONGLE_LOG);
    recv_usb_hid_gamepad = (usb_hid_gamepad_report_t *)data->data;
    osal_printk("%s recv_usb_hid_gamepad.kind = [%d]\r\n", SLE_GAMEPAD_DONGLE_LOG, recv_usb_hid_gamepad->kind);
    osal_printk("%s recv_usb_hid_gamepad.data = ", SLE_GAMEPAD_DONGLE_LOG);
    for (uint8_t i = 0; i < USB_HID_GAMEPAD_MAX_KEY_LENTH; i++) {
        osal_printk("0x%02x ", recv_usb_hid_gamepad->data[i]);
    }
    osal_printk("\r\n");
    sle_gamepad_dongle_send_data((usb_hid_gamepad_report_t *)data->data);
}

static void sle_gamepad_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
                                      errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    usb_hid_gamepad_report_t *recv_usb_hid_gamepad = NULL;
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        osal_printk("%s sle_gamepad_indication_cb fail, recv data is null!\r\n", SLE_GAMEPAD_DONGLE_LOG);
    }
    osal_printk("%s sle gamepad recive indication\r\n", SLE_GAMEPAD_DONGLE_LOG);
    recv_usb_hid_gamepad = (usb_hid_gamepad_report_t *)data->data;
    osal_printk("%s recv_usb_hid_gamepad.kind = [%d]\r\n", SLE_GAMEPAD_DONGLE_LOG, recv_usb_hid_gamepad->kind);
    osal_printk("%s recv_usb_hid_gamepad.data = ", SLE_GAMEPAD_DONGLE_LOG);
    for (uint8_t i = 0; i < USB_HID_GAMEPAD_MAX_KEY_LENTH; i++) {
        osal_printk("0x%02x ", recv_usb_hid_gamepad->data[i]);
    }
    osal_printk("\r\n");
    sle_gamepad_dongle_send_data((usb_hid_gamepad_report_t *)data->data);
}

static void *sle_gamepad_dongle_task(const char *arg)
{
    unused(arg);
    uint8_t ret;

    osal_printk("%s enter sle_gamepad_dongle_task\r\n", SLE_GAMEPAD_DONGLE_LOG);
    // 1. sle gamepad dongle init.
    ret = sle_gamepad_dongle_init();
    if (ret != SLE_GAMEPAD_DONGLE_OK) {
        osal_printk("%s sle_gamepad_dongle_init fail! ret = %d\r\n", SLE_GAMEPAD_DONGLE_LOG, ret);
    }
    // 2. sle gamepad client init.
    sle_gamepad_client_init(sle_gamepad_notification_cb, sle_gamepad_indication_cb);
    while (1) {
        osDelay(SLE_GAMEPAD_DONGLE_TASK_DELAY_MS);
    }
    return NULL;
}

static void sle_gamepad_dongle_entry(void)
{
    osThreadAttr_t attr = { 0 };

    attr.name = "SLEGamepadDongleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_GAMEPAD_DONGLE_TASK_STACK_SIZE;
    attr.priority = SLE_GAMEPAD_DONGLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)sle_gamepad_dongle_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the sle_gamepad_dongle_entry. */
app_run(sle_gamepad_dongle_entry);