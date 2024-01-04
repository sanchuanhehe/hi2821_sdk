/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Mouse Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-07, Create file. \n
 */
#include "cmsis_os2.h"
#include "app_init.h"
#include "gadget/f_hid.h"
#include "usb_porting.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_button/mouse_button.h"
#include "mouse_wheel/mouse_wheel.h"
#include "mouse_usb/usb_init_app.h"

#define USB_MOUSE_TASK_STACK_SIZE 0x2000
#define USB_MOUSE_TASK_PRIO (osPriority_t)(17)
#define USB_MOUSE_TASK_DELAY_MS 2000

typedef struct usb_hid_mouse_report {
    uint8_t kind;
    usb_hid_mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_mouse_report_t;

static int g_usb_mouse_hid_index = -1;
static uint8_t g_usb_mouse_polling_rate = 4;
static usb_hid_mouse_report_t g_send_mouse_msg;
static mouse_sensor_oprator_t g_usb_hid_hs_mouse_operator;

static void mouse_cb(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t usb_sof_cnt = 0;
    usb_sof_cnt = (usb_sof_cnt + 1) % g_usb_mouse_polling_rate;
    if (usb_sof_cnt != 0) {
        return;
    }
    static usb_hid_mouse_report_t mouse_message = { 0 };
    int16_t x = 0;
    int16_t y = 0;

    g_usb_hid_hs_mouse_operator.get_xy(&x, &y);
    mouse_message.x = x;
    mouse_message.y = y;
    mouse_message.wheel = g_send_mouse_msg.wheel;
    mouse_message.key.d8 = g_send_mouse_msg.key.d8;
    g_send_mouse_msg.wheel = 0;
    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_usb_mouse_hid_index;
}

static void *usb_mouse_task(const char *arg)
{
    UNUSED(arg);

    g_usb_mouse_hid_index = usb_init_app(DEV_HID);
    if (g_usb_mouse_hid_index < 0) {
        return NULL;
    }

    g_send_mouse_msg.kind = 0x2;

    g_usb_hid_hs_mouse_operator = get_mouse_sensor_operator(PWM3395DM);
    g_usb_hid_hs_mouse_operator.init();

    mouse_button_init(&(g_send_mouse_msg.key));
    mouse_wheel_init(&(g_send_mouse_msg.wheel));
    usb_register_callback(mouse_cb);
    while (1) {
        osDelay(USB_MOUSE_TASK_DELAY_MS);
    }

    return NULL;
}

static void usb_mouse_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "USBMouseTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = USB_MOUSE_TASK_STACK_SIZE;
    attr.priority = USB_MOUSE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)usb_mouse_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the usb_mouse_entry. */
app_run(usb_mouse_entry);