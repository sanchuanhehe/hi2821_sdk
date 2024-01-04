/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Air Mouse with dongle Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-16, Create file. \n
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
#include "dongle/air_mouse_usb/usb_init_app.h"
#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
#include "mouse/sle_air_mouse_server/sle_air_mouse_server_adv.h"
#include "mouse/sle_air_mouse_server/sle_air_mouse_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE)
#include "mouse/sle_air_mouse_client/sle_air_mouse_client.h"
#include "dongle/air_mouse_usb/usb_init_app.h"
#endif
#include "glp_client_main.h"

#define USB_MOUSE_TASK_STACK_SIZE 0xa00
#define USB_MOUSE_TASK_PRIO (osPriority_t)(17)

#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
static void *sle_air_mouse_task(const char *arg)
{
    unused(arg);

    sle_air_mouse_server_init();

    GlpClientTask(NULL);

    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE)
static void *sle_air_mouse_dongle_task(const char *arg)
{
    unused(arg);
    int usb_hid_index = -1;
    sle_air_mouse_client_init();
    usb_hid_index = usb_init_app(DEV_HID);
    osal_printk("usb_hid_init %s\n", arg);
    if (usb_hid_index < 0) {
        return NULL;
    }

    GlpClientTask(NULL);

    return NULL;
}
#endif


static void sle_air_mouse_with_dongle_entry(void)
{
    osal_task *task_cb = NULL;
    osal_kthread_lock();

#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE)
    task_cb = osal_kthread_create((osal_kthread_handler)sle_air_mouse_dongle_task, NULL,
                                  "SLEAirMouseTask", USB_MOUSE_TASK_STACK_SIZE);
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
    task_cb = osal_kthread_create((osal_kthread_handler)sle_air_mouse_task, NULL,
                                  "SLEAirMouseTask", USB_MOUSE_TASK_STACK_SIZE);
#endif
    if (task_cb != NULL) {
        osal_kthread_set_priority(task_cb, USB_MOUSE_TASK_PRIO);
        osal_vfree(task_cb);
    }
    osal_kthread_unlock();
}

/* Run the sle_air_mouse_with_dongle_entry. */
app_run(sle_air_mouse_with_dongle_entry);