/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Mouse with dongle Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "cmsis_os2.h"
#include "app_init.h"
#include "gadget/f_hid.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "common_def.h"
#include "securec.h"
#include "uart.h"
#include "los_memory.h"
#include "sle_errcode.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_low_latency.h"
#include "mouse_usb/usb_init_app.h"
#include "mouse_button/mouse_button.h"
#include "mouse_sensor/mouse_sensor.h"
#include "sle_low_latency_service.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE)
#include "sle_mouse_server/sle_mouse_server_adv.h"
#include "sle_mouse_server/sle_mouse_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
#include "sle_mouse_client/sle_mouse_client.h"
#include "mouse_usb/usb_init_app.h"
#endif

#define USB_MOUSE_TASK_STACK_SIZE 0xc00
#define USB_MOUSE_TASK_PRIO (osPriority_t)(17)
#define USB_MOUSE_TASK_DELAY_MS 2000
#define SLE_MOUSE_TASK_DELAY_20_MS 20

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
static void *sle_mouse_dongle_task(const char *arg)
{
#ifdef CONFIG_SAMPLE_SLE_DONGLE_8K
    uint8_t report_rate_idx = 5;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_4K)
    uint8_t report_rate_idx = 6;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_2K)
    uint8_t report_rate_idx = 7;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_1K)
    uint8_t report_rate_idx = 8;
#endif
    unused(arg);
    int usb_hid_index = -1;
    sle_mouse_client_init();
    usb_hid_index = usb_init_app(DEV_HID);
    osal_printk("usb_hid_init %s\n", arg);
    if (usb_hid_index < 0) {
        return NULL;
    }
    while (1) {
        while (get_g_sle_mouse_client_conn_state() == SLE_ACB_STATE_NONE ||
               get_g_sle_mouse_client_conn_state() == SLE_ACB_STATE_DISCONNECTED) {
            osal_msleep(USB_MOUSE_TASK_DELAY_MS);
        }
        sle_low_latency_dongle_init(usb_hid_index);
        sle_low_latency_set(get_g_sle_mouse_client_conn_id(), TRUE, report_rate_idx); /* report_rate_idx表示4k鼠标 */
        sle_low_latency_dongle_enable();
        while (get_g_sle_mouse_client_conn_state() == SLE_ACB_STATE_CONNECTED) {
            osal_msleep(USB_MOUSE_TASK_DELAY_MS);
        }
    }
    return NULL;
}
#endif

static void sle_mouse_with_dongle(void)
{
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
    osThreadAttr_t attr;

    attr.name = "SLEMouseTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = USB_MOUSE_TASK_STACK_SIZE;
    attr.priority = USB_MOUSE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)sle_mouse_dongle_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE)
    mouse_init(PWM3395DM);
    sle_low_latency_mouse_app_init();
    sle_low_latency_mouse_enable();
    sle_mouse_server_init();
#endif
}

/* Run the sle_mouse_with_dongle. */
app_run(sle_mouse_with_dongle);