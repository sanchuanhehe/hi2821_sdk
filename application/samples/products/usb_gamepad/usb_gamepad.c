/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Gamepad Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */
#include "cmsis_os2.h"
#include "common_def.h"
#include "pinctrl.h"
#include "app_init.h"
#include "osal_debug.h"
#include "gamepad_usb/usb_init_app.h"
#include "gamepad_button/gamepad_button.h"
#include "gamepad_joystick/gamepad_joystick.h"

#define USB_GAMEPAD_TASK_STACK_SIZE 0x8000
#define USB_GAMEPAD_TASK_PRIO (osPriority_t)(17)
#define USB_GAMEPAD_TASK_DELAY_MS 10
#define USB_KEYBOARD_REPORTER_LEN           8
#define SINGLE_END_AINN3                  5
#define SINGLE_END_AINN4                  7
#define ADC_REFERENCE_VOLTAGE_MV        2600
#define ADC_REF_VOL_DIFFERENCE_MULT     2
#define ADC_TICK2VOL_REF_VOLTAGE_MV     (ADC_REFERENCE_VOLTAGE_MV * ADC_REF_VOL_DIFFERENCE_MULT)
#define GAFE_SAMPLE_VALUE_SIGN_BIT   16
#define GADC_CORRECTION_VALUE        7
#define USB_BIT_8                    8
#define USB_HAT_SWITCH               0xF
#define PARAM_2                      2
#define PARAM_3                      3
#define PARAM_4                      4
#define PARAM_5                      5
#define PARAM_6                      6
#define USB_HID_KEYBOARD_MAX_KEY_LENTH      6
#define JOYSTICK_STATE_SIZE      7

typedef struct {
    uint8_t x_axis;             // 左摇杆X
    uint8_t y_axis;             // 左摇杆Y
    uint8_t z_axis;             // 右摇杆X
    uint8_t z_axisrotation;     // 右摇杆Y；Z轴旋转
} joystick_status_t;

typedef union gamepad_key {
    uint16_t d16;
    struct {
        uint16_t y_key      : 1;
        uint16_t b_key      : 1;
        uint16_t a_key      : 1;
        uint16_t x_key      : 1;
        uint16_t l_key      : 1;
        uint16_t r_key      : 1;
        uint16_t zl_key     : 1;
        uint16_t zr_key     : 1;
        uint16_t n_key      : 1;   // negative
        uint16_t p_key      : 1;   // postive
        uint16_t l_joystick : 1;
        uint16_t r_joystick : 1;
        uint16_t home_key   : 1;
        uint16_t photo_key  : 1;
        uint16_t reserved   : 2;
    } b;
} usb_hid_gamepad_key_t;

typedef struct usb_hid_gamepad_report {
    uint8_t kind;
    uint8_t data[JOYSTICK_STATE_SIZE];
} usb_hid_gamepad_report_t;

static joystick_status_t g_joystick_status = { 0 };
static usb_hid_gamepad_key_t g_gamepad_key = { 0 };
static int g_usb_gamepad_hid_index = -1;
static usb_hid_gamepad_report_t g_send_gamepad_msg;

static void send_state(uint16_t button, int16_t hatswitch)
{
    uint16_t button_tmp = button;
    int16_t tmp_hatswitch = hatswitch;
    g_send_gamepad_msg.data[0] = button_tmp & 0xFF;
    button_tmp >>= USB_BIT_8;
    g_send_gamepad_msg.data[1] = button_tmp & 0xFF;

    if (tmp_hatswitch < 0) {
        tmp_hatswitch = USB_BIT_8;
    }
    g_send_gamepad_msg.data[PARAM_2] = tmp_hatswitch & USB_HAT_SWITCH;
    g_send_gamepad_msg.data[PARAM_3] = g_joystick_status.x_axis;
    g_send_gamepad_msg.data[PARAM_4] = g_joystick_status.y_axis;
    g_send_gamepad_msg.data[PARAM_5] = g_joystick_status.z_axis;
    g_send_gamepad_msg.data[PARAM_6] = g_joystick_status.z_axisrotation;

    fhid_send_data(g_usb_gamepad_hid_index, (char *)&g_send_gamepad_msg, USB_KEYBOARD_REPORTER_LEN);
}

static int usb_keyscan_callback(int key_nums, uint8_t key_values[])
{
    g_gamepad_key.b.y_key = 0;
    g_gamepad_key.b.b_key = 0;
    g_gamepad_key.b.a_key = 0;
    g_gamepad_key.b.x_key = 0;

    for (int i = 0; i < key_nums; i++) {
        switch (key_values[i]) {
            case 0x29:
                g_gamepad_key.b.y_key = 1;
                break;
            case 0x2B:
                g_gamepad_key.b.b_key = 1;
                break;
            case 0x3D:
                g_gamepad_key.b.a_key = 1;
                break;
            case 0x3C:
                g_gamepad_key.b.x_key = 1;
                break;
            default:
                break;
        }
    }

    return 1;
}

static void *usb_gamepad_task(const char *arg)
{
    unused(arg);
    g_usb_gamepad_hid_index = usb_init_app(DEV_HID);
    if (g_usb_gamepad_hid_index < 0) {
        return NULL;
    }

    g_send_gamepad_msg.kind = 0x03;
    gamepad_button_init(usb_keyscan_callback);
    gamepad_joysticks_init();

    while (1) {
        g_gamepad_key.b.l_joystick = 1;
        uapi_adc_open_channel(SINGLE_END_AINN3);
        g_joystick_status.x_axis = (uint8_t)(((uapi_adc_auto_sample(AFE_GADC_MODE) * ADC_TICK2VOL_REF_VOLTAGE_MV) >>
                                             GAFE_SAMPLE_VALUE_SIGN_BIT) / GADC_CORRECTION_VALUE);
        uapi_adc_open_channel(SINGLE_END_AINN4);
        g_joystick_status.y_axis = (uint8_t)(((uapi_adc_auto_sample(AFE_GADC_MODE) * ADC_TICK2VOL_REF_VOLTAGE_MV) >>
                                             GAFE_SAMPLE_VALUE_SIGN_BIT) / GADC_CORRECTION_VALUE);
        send_state(g_gamepad_key.d16, 0);
        osDelay(USB_GAMEPAD_TASK_DELAY_MS);
    }
    g_gamepad_key.b.l_joystick = 0;
    return NULL;
}

static void usb_gamepad_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "USBGAMEPADTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = USB_GAMEPAD_TASK_STACK_SIZE;
    attr.priority = USB_GAMEPAD_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)usb_gamepad_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the usb_gamepad_entry. */
app_run(usb_gamepad_entry);