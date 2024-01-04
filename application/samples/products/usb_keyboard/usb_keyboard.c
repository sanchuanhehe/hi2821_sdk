/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Mouse Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-07, Create file. \n
 */
#include "cmsis_os2.h"
#include "common_def.h"
#include "app_init.h"
#include "keyscan_usb/usb_init_keyboard_app.h"
#include "keyscan_usb/keyscan_init.h"

#define USB_KEYSCAN_TASK_STACK_SIZE 0x2000
#define USB_KEYSCAN_TASK_PRIO (osPriority_t)(17)
#define USB_KEYSCAN_TASK_DELAY_MS 2000

static void *usb_keyscan_task(const char *arg)
{
    unused(arg);

    int index = usb_init_keyscan_app(DEV_HID);
    if (index < 0) {
        return NULL;
    }

    keyscan_init(index);

    while (1) {
        osDelay(USB_KEYSCAN_TASK_DELAY_MS);
    }

    return NULL;
}

static void usb_keyscan_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "USBMouseTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = USB_KEYSCAN_TASK_STACK_SIZE;
    attr.priority = USB_KEYSCAN_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)usb_keyscan_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the usb_keyscan_entry. */
app_run(usb_keyscan_entry);