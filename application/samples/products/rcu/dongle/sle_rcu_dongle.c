/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE RCU Dongle Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
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
#include "sle_rcu_client.h"
#include "sle_rcu_hid.h"

#define SLE_RCU_DONGLE_TASK_STACK_SIZE      0x1000
#define SLE_RCU_DONGLE_TASK_PRIO            (osPriority_t)(13)
#define SLE_RCU_DONGLE_TASK_DELAY_MS        2000
#define USB_HID_RCU_INIT_DELAY_MS           (500UL)
#define USB_RCU_KEYBOARD_REPORTER_LEN       9
#define USB_RCU_MOUSE_REPORTER_LEN          5
#define USB_RCU_CONSUMER_REPORTER_LEN       3
#define SLE_KRYBOARD_USB_MANUFACTURER       { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define SLE_KRYBOARD_USB_MANUFACTURER_LEN   20
#define SLE_KRYBOARD_USB_PRODUCT    { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define SLE_KRYBOARD_USB_PRODUCT_LEN        22
#define SLE_KRYBOARD_USB_SERIAL             { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define SLE_KRYBOARD_USB_SERIAL_LEN         16
#define RECV_MAX_LENGTH                     13
#define USB_RECV_STACK_SIZE                 0x400
#define SLE_RCU_WAIT_SSAPS_READY            500
#define SLE_RCU_DONGLE_LOG                  "[sle rcu dongle]"

static bool g_sle_rcu_dongle_inited = false;
static uint32_t g_sle_rcu_dongle_hid_index = 0;
static uint8_t g_rcu_sle_buffer[] = {"hello sle micro server!"};

static void sle_rcu_keyboard_dongle_send_data(usb_hid_rcu_keyboard_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    rpt->kind = 0x1;
    int32_t ret = fhid_send_data(g_sle_rcu_dongle_hid_index, (char *)rpt, USB_RCU_KEYBOARD_REPORTER_LEN);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_RCU_DONGLE_LOG, ret);
        return;
    }
}

static void sle_rcu_mouse_dongle_send_data(usb_hid_rcu_mouse_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    rpt->kind = 0x4;
    int32_t ret = fhid_send_data(g_sle_rcu_dongle_hid_index, (char *)rpt, USB_RCU_MOUSE_REPORTER_LEN);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_RCU_DONGLE_LOG, ret);
        return;
    }
}

static void sle_rcu_consumer_dongle_send_data(usb_hid_rcu_consumer_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    int32_t ret = fhid_send_data(g_sle_rcu_dongle_hid_index, (char *)rpt, USB_RCU_CONSUMER_REPORTER_LEN);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_RCU_DONGLE_LOG, ret);
        return;
    }
}

static uint8_t sle_rcu_dongle_init_internal(device_type dtype)
{
    if (g_sle_rcu_dongle_inited) {
        return SLE_RCU_DONGLE_OK;
    }

    const char manufacturer[SLE_KRYBOARD_USB_MANUFACTURER_LEN] = SLE_KRYBOARD_USB_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = SLE_KRYBOARD_USB_MANUFACTURER_LEN
    };

    const char product[SLE_KRYBOARD_USB_PRODUCT_LEN] = SLE_KRYBOARD_USB_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = SLE_KRYBOARD_USB_PRODUCT_LEN
    };

    const char serial[SLE_KRYBOARD_USB_SERIAL_LEN] = SLE_KRYBOARD_USB_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = SLE_KRYBOARD_USB_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };

    if (dtype == DEV_HID) {
        g_sle_rcu_dongle_hid_index = sle_rcu_dongle_set_report_desc_hid();
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        osal_printk("%s set device info fail!\r\n", SLE_RCU_DONGLE_LOG);
        return SLE_RCU_DONGLE_FAILED;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        osal_printk("%s usb_init failed!\r\n", SLE_RCU_DONGLE_LOG);
        return SLE_RCU_DONGLE_FAILED;
    }
    g_sle_rcu_dongle_inited = true;
    return SLE_RCU_DONGLE_OK;
}

static uint8_t sle_rcu_dongle_init(void)
{
    if (!g_sle_rcu_dongle_inited) {
        if (sle_rcu_dongle_init_internal(DEV_HID) != SLE_RCU_DONGLE_OK) {
            return SLE_RCU_DONGLE_FAILED;
        }
        osal_msleep(USB_HID_RCU_INIT_DELAY_MS);
    }
    return SLE_RCU_DONGLE_OK;
}

static void sle_rcu_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        osal_printk("%s sle_rcu_notification_cb fail, recv data is null!\r\n", SLE_RCU_DONGLE_LOG);
    }
    if (data->data_len == USB_RCU_KEYBOARD_REPORTER_LEN) {
        usb_hid_rcu_keyboard_report_t *recv_usb_hid_rcu_keyboard = NULL;
        osal_printk("%s sle rcu recive notification\r\n", SLE_RCU_DONGLE_LOG);
        recv_usb_hid_rcu_keyboard = (usb_hid_rcu_keyboard_report_t *)data->data;
        osal_printk("%s recv_usb_hid_rcu_keyboard.kind = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_keyboard->kind);
        osal_printk("%s recv_usb_hid_rcu_keyboard.special_key = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_keyboard->special_key);
        osal_printk("%s recv_usb_hid_rcu_keyboard.reversed = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_keyboard->reserve);
        osal_printk("%s recv_usb_hid_rcu_keyboard.key = ", SLE_RCU_DONGLE_LOG);
        for (uint8_t i = 0; i < USB_HID_RCU_MAX_KEY_LENTH; i++) {
            osal_printk("0x%02x ", recv_usb_hid_rcu_keyboard->key[i]);
        }
        osal_printk("\r\n");
        sle_rcu_keyboard_dongle_send_data((usb_hid_rcu_keyboard_report_t *)data->data);
    } else if (data->data_len == USB_RCU_MOUSE_REPORTER_LEN) {
        usb_hid_rcu_mouse_report_t *recv_usb_hid_rcu_mouse = NULL;
        osal_printk("%s sle rcu recive notification\r\n", SLE_RCU_DONGLE_LOG);
        recv_usb_hid_rcu_mouse = (usb_hid_rcu_mouse_report_t *)data->data;
        osal_printk("%s recv_usb_hid_rcu_mouse.kind = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->kind);
        osal_printk("%s recv_usb_hid_rcu_mouse.key = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->key.d8);
        osal_printk("%s recv_usb_hid_rcu_mouse.x = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->x);
        osal_printk("%s recv_usb_hid_rcu_mouse.y = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->y);
        osal_printk("%s recv_usb_hid_rcu_mouse.wheel = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->wheel);
        sle_rcu_mouse_dongle_send_data((usb_hid_rcu_mouse_report_t *)data->data);
    } else if (data->data_len == USB_RCU_CONSUMER_REPORTER_LEN) {
        usb_hid_rcu_consumer_report_t *recv_usb_hid_rcu_consumer = NULL;
        osal_printk("%s sle rcu recive notification\r\n", SLE_RCU_DONGLE_LOG);
        recv_usb_hid_rcu_consumer = (usb_hid_rcu_consumer_report_t *)data->data;
        osal_printk("%s recv_usb_hid_rcu_consumer.kind = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_consumer->kind);
        osal_printk("%s recv_usb_hid_rcu_consumer.comsumer_key0 = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_consumer->comsumer_key0);
        osal_printk("%s recv_usb_hid_rcu_consumer.comsumer_key1 = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_consumer->comsumer_key1);
        sle_rcu_consumer_dongle_send_data((usb_hid_rcu_consumer_report_t *)data->data);
    }
}

static void sle_rcu_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        osal_printk("%s sle_rcu_notification_cb fail, recv data is null!\r\n", SLE_RCU_DONGLE_LOG);
    }
    if (data->data_len == USB_RCU_KEYBOARD_REPORTER_LEN) {
        usb_hid_rcu_keyboard_report_t *recv_usb_hid_rcu_keyboard = NULL;
        osal_printk("%s sle rcu recive notification\r\n", SLE_RCU_DONGLE_LOG);
        recv_usb_hid_rcu_keyboard = (usb_hid_rcu_keyboard_report_t *)data->data;
        osal_printk("%s recv_usb_hid_rcu_keyboard.kind = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_keyboard->kind);
        osal_printk("%s recv_usb_hid_rcu_keyboard.special_key = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_keyboard->special_key);
        osal_printk("%s recv_usb_hid_rcu_keyboard.reversed = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_keyboard->reserve);
        osal_printk("%s recv_usb_hid_rcu_keyboard.key = ", SLE_RCU_DONGLE_LOG);
        for (uint8_t i = 0; i < USB_HID_RCU_MAX_KEY_LENTH; i++) {
            osal_printk("0x%02x ", recv_usb_hid_rcu_keyboard->key[i]);
        }
        osal_printk("\r\n");
        sle_rcu_keyboard_dongle_send_data((usb_hid_rcu_keyboard_report_t *)data->data);
    } else if (data->data_len == USB_RCU_MOUSE_REPORTER_LEN) {
        usb_hid_rcu_mouse_report_t *recv_usb_hid_rcu_mouse = NULL;
        osal_printk("%s sle rcu recive notification\r\n", SLE_RCU_DONGLE_LOG);
        recv_usb_hid_rcu_mouse = (usb_hid_rcu_mouse_report_t *)data->data;
        osal_printk("%s recv_usb_hid_rcu_mouse.kind = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->kind);
        osal_printk("%s recv_usb_hid_rcu_mouse.key = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->key.d8);
        osal_printk("%s recv_usb_hid_rcu_mouse.x = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->x);
        osal_printk("%s recv_usb_hid_rcu_mouse.y = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->y);
        osal_printk("%s recv_usb_hid_rcu_mouse.wheel = [%d]\r\n", SLE_RCU_DONGLE_LOG, recv_usb_hid_rcu_mouse->wheel);
        sle_rcu_mouse_dongle_send_data((usb_hid_rcu_mouse_report_t *)data->data);
    } else if (data->data_len == USB_RCU_CONSUMER_REPORTER_LEN) {
        usb_hid_rcu_consumer_report_t *recv_usb_hid_rcu_consumer = NULL;
        osal_printk("%s sle rcu recive notification\r\n", SLE_RCU_DONGLE_LOG);
        recv_usb_hid_rcu_consumer = (usb_hid_rcu_consumer_report_t *)data->data;
        osal_printk("%s recv_usb_hid_rcu_consumer.kind = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_consumer->kind);
        osal_printk("%s recv_usb_hid_rcu_consumer.comsumer_key0 = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_consumer->comsumer_key0);
        osal_printk("%s recv_usb_hid_rcu_consumer.comsumer_key1 = [%d]\r\n", SLE_RCU_DONGLE_LOG,
                    recv_usb_hid_rcu_consumer->comsumer_key1);
        sle_rcu_consumer_dongle_send_data((usb_hid_rcu_consumer_report_t *)data->data);
    }
}

static void *sle_rcu_dongle_task(const char *arg)
{
    unused(arg);
    uint8_t ret;

    osal_printk("%s enter sle_rcu_dongle_task\r\n", SLE_RCU_DONGLE_LOG);
    /* sle rcu dongle init */
    ret = sle_rcu_dongle_init();
    if (ret != SLE_RCU_DONGLE_OK) {
        osal_printk("%s sle_rcu_dongle_init fail! ret = %d\r\n", SLE_RCU_DONGLE_LOG, ret);
    }
    /* sle rcu client init */
    sle_rcu_client_init(sle_rcu_notification_cb, sle_rcu_indication_cb);
    while (get_ssap_find_ready() == 0) {
        osal_msleep(SLE_RCU_WAIT_SSAPS_READY);
    }
    osal_printk("%s get_g_ssap_find_ready.\r\n", SLE_RCU_DONGLE_LOG);
    /* delay for param update complete */
    osal_msleep(SLE_RCU_DONGLE_TASK_DELAY_MS);
    ssapc_write_param_t *sle_micro_send_param = get_sle_rcu_send_param();
    sle_micro_send_param->data_len = (uint8_t)strlen((char *)g_rcu_sle_buffer);
    sle_micro_send_param->data = g_rcu_sle_buffer;
    while (1) {
        osal_msleep(SLE_RCU_DONGLE_TASK_DELAY_MS);
    }
    return NULL;
}

static void sle_rcu_dongle_entry(void)
{
    osThreadAttr_t attr = { 0 };

    attr.name = "SLERcuDongleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_RCU_DONGLE_TASK_STACK_SIZE;
    attr.priority = SLE_RCU_DONGLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)sle_rcu_dongle_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the sle_rcu_dongle_entry. */
app_run(sle_rcu_dongle_entry);