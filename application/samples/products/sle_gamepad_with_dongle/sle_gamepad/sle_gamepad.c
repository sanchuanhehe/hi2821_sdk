/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
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
#include "adc.h"
#include "watchdog.h"
#include "adc_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_gamepad_button/sle_gamepad_button.h"
#include "sle_gamepad_joystick/sle_gamepad_joystick.h"
#include "sle_gamepad_server.h"
#include "sle_gamepad_server_adv.h"

#define USB_HID_GAMEPAD_MAX_KEY_LENTH           7
#define KEYSCAN_REPORT_MAX_KEY_NUMS             6
#define CONVERT_DEC_TO_HEX                      16
#define MAX_NUM_OF_DEC                          10
#define LENGTH_OF_KEY_VALUE_STR                 5
#define SLE_GAMEPAD_PARAM_ARGC_2                2
#define SLE_GAMEPAD_PARAM_ARGC_3                3
#define SLE_GAMEPAD_PARAM_ARGC_4                4
#define SLE_GAMEPAD_PARAM_ARGC_5                5
#define SLE_GAMEPAD_PARAM_ARGC_6                6

#define SLE_ADV_HANDLE_DEFAULT                  1
#define SLE_GAMEPAD_KEY_BIT_VALUE               1
#define SLE_HID_GAMEPAD_REPORT_KIND             0X03
#define SLE_GAMEPAD_BUTTON_KEY_UP               0x29
#define SLE_GAMEPAD_BUTTON_KEY_DOWN             0x2B
#define SLE_GAMEPAD_BUTTON_KEY_LEFT             0x3D
#define SLE_GAMEPAD_BUTTON_KEY_RIGHT            0x3C
#define SLE_GAMEPAD_SERVER_DELAY_COUNT          3
#define SLE_GAMEPAD_TASK_DURATION_MS            2000
#define SLE_GAMEPAD_STATE_DISCONNECT            0
#define SLE_GAMEPAD_STATE_CONNECTED             1
#define SLE_GAMEPAD_SERVER_LOG                  "[sle gamepad server]"
#define SLE_GAMEPAD_TASK_STACK_SIZE             0x1000
#define SLE_GAMEPAD_TASK_PRIO                   (osPriority_t)(17)

#define USB_BIT_8                               8
#define USB_HAT_SWITCH                          0xF
#define USB_GAMEPAD_TASK_DELAY_MS               10
#define SINGLE_END_AINN3                        5
#define SINGLE_END_AINN4                        7
#define GAFE_SAMPLE_VALUE_SIGN_BIT              16
#define GADC_CORRECTION_VALUE                   7
#define ADC_REFERENCE_VOLTAGE_MV                2600
#define ADC_REF_VOL_DIFFERENCE_MULT             2
#define ADC_TICK2VOL_REF_VOLTAGE_MV             (ADC_REFERENCE_VOLTAGE_MV * ADC_REF_VOL_DIFFERENCE_MULT)

#define SLE_RET_SUCC                            1
#define SLE_RET_FAIL                            0

typedef struct {
    uint8_t x_axis;                 // 左摇杆X
    uint8_t y_axis;                 // 左摇杆Y
    uint8_t z_axis;                 // 右摇杆X
    uint8_t z_axisrotation;         // 右摇杆Y；Z轴旋转
} joystick_status_t;

typedef union gamepad_key {
    uint16_t d16;
    struct {
        uint16_t y_key      : 1;    // 手柄y键
        uint16_t b_key      : 1;    // 手柄b键
        uint16_t a_key      : 1;    // 手柄a键
        uint16_t x_key      : 1;    // 手柄x键
        uint16_t l_key      : 1;
        uint16_t r_key      : 1;
        uint16_t zl_key     : 1;
        uint16_t zr_key     : 1;
        uint16_t n_key      : 1;    // negative
        uint16_t p_key      : 1;    // postive
        uint16_t l_joystick : 1;    // 手柄左摇杆
        uint16_t r_joystick : 1;    // 手柄右摇杆
        uint16_t home_key   : 1;
        uint16_t photo_key  : 1;
        uint16_t reserved   : 2;
    } b;
} usb_hid_gamepad_key_t;

typedef struct usb_hid_gamepad_report {
    uint8_t kind;
    uint8_t data[USB_HID_GAMEPAD_MAX_KEY_LENTH];
} usb_hid_gamepad_report_t;

static joystick_status_t g_sle_joystick_status = { 0 };
static usb_hid_gamepad_key_t g_sle_gamepad_key = { 0 };
static usb_hid_gamepad_report_t g_sle_hid_gamepad_report;

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    if (read_cb_para == NULL) {
        osal_printk("%s ssaps_server_read_request_cbk fail, read_cb_para is null!\r\n", SLE_GAMEPAD_SERVER_LOG);
        return;
    }
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
                SLE_GAMEPAD_SERVER_LOG, server_id, conn_id, read_cb_para->handle, status);
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
        osal_printk("%s ssaps_server_write_request_cbk fail, write_cb_para is null!\r\n", SLE_GAMEPAD_SERVER_LOG);
        return;
    }
    osal_printk("%s ssaps write request callback cbk handle:%x, status:%x\r\n",
                SLE_GAMEPAD_SERVER_LOG, write_cb_para->handle, status);
    if ((write_cb_para->length > 0) && write_cb_para->value) {
        // todo,recv data from client,send to pc by uart or send to keyboard
        osal_printk("%s recv data from client, len =[%d], data= ", SLE_GAMEPAD_SERVER_LOG, write_cb_para->length);
        for (i = 0; i < write_cb_para->length; i++) {
            osal_printk("0x%02x ", write_cb_para->value[i]);
        }
        osal_printk("\r\n");
        key_values = write_cb_para->value;
        key_nums = write_cb_para->length;
        for (i = 0; i < write_cb_para->length; i++) {
            key_value_str[i] = (char *)osal_vmalloc(LENGTH_OF_KEY_VALUE_STR);
            key_value_str[i][0] = '0';
            key_value_str[i][1] = 'x';
            uint32_t tran = key_values[i] / CONVERT_DEC_TO_HEX;
            if (tran < MAX_NUM_OF_DEC) {
                key_value_str[i][SLE_GAMEPAD_PARAM_ARGC_2] = '0' + tran;
            } else {
                key_value_str[i][SLE_GAMEPAD_PARAM_ARGC_2] = ('A' + tran - MAX_NUM_OF_DEC);
            }
            tran = key_values[i] % CONVERT_DEC_TO_HEX;
            if (tran < MAX_NUM_OF_DEC) {
                key_value_str[i][SLE_GAMEPAD_PARAM_ARGC_3] = '0' + tran;
            } else {
                key_value_str[i][SLE_GAMEPAD_PARAM_ARGC_3] = ('A' + tran - MAX_NUM_OF_DEC);
            }
            key_value_str[i][SLE_GAMEPAD_PARAM_ARGC_4] = '\0';
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

static int sle_gamepad_keyscan_callback(int key_nums, uint8_t key_values[])
{
    if (memset_s(&g_sle_gamepad_key, sizeof(g_sle_gamepad_key), 0, sizeof(g_sle_gamepad_key)) != EOK) {
        return SLE_RET_FAIL;
    }

    for (uint8_t i = 0; i < key_nums; i++) {
        switch (key_values[i]) {
            case SLE_GAMEPAD_BUTTON_KEY_UP:
                g_sle_gamepad_key.b.y_key = SLE_GAMEPAD_KEY_BIT_VALUE;
                break;
            case SLE_GAMEPAD_BUTTON_KEY_DOWN:
                g_sle_gamepad_key.b.b_key = SLE_GAMEPAD_KEY_BIT_VALUE;
                break;
            case SLE_GAMEPAD_BUTTON_KEY_LEFT:
                g_sle_gamepad_key.b.a_key = SLE_GAMEPAD_KEY_BIT_VALUE;
                break;
            case SLE_GAMEPAD_BUTTON_KEY_RIGHT:
                g_sle_gamepad_key.b.x_key = SLE_GAMEPAD_KEY_BIT_VALUE;
                break;
            default:
                break;
        }
    }
    return SLE_RET_SUCC;
}

static int sle_send_state(uint16_t button, int16_t hatswitch)
{
    if (sle_gamepad_client_is_connected() == 0) {
        return SLE_RET_SUCC;
    }
    if (memset_s(&g_sle_joystick_status, sizeof(g_sle_joystick_status), 0, sizeof(g_sle_joystick_status)) != EOK) {
        return SLE_RET_FAIL;
    }
    if (memset_s(&g_sle_hid_gamepad_report, sizeof(g_sle_hid_gamepad_report), 0,
        sizeof(g_sle_hid_gamepad_report)) != EOK) {
        return SLE_RET_FAIL;
    }

    g_sle_gamepad_key.b.l_joystick = SLE_GAMEPAD_KEY_BIT_VALUE;
    uapi_adc_open_channel(SINGLE_END_AINN3);
    g_sle_joystick_status.x_axis = (uint8_t)(((uapi_adc_auto_sample(AFE_GADC_MODE) * ADC_TICK2VOL_REF_VOLTAGE_MV) >>
                                            GAFE_SAMPLE_VALUE_SIGN_BIT) / GADC_CORRECTION_VALUE);
    uapi_adc_open_channel(SINGLE_END_AINN4);
    g_sle_joystick_status.y_axis = (uint8_t)(((uapi_adc_auto_sample(AFE_GADC_MODE) * ADC_TICK2VOL_REF_VOLTAGE_MV) >>
                                            GAFE_SAMPLE_VALUE_SIGN_BIT) / GADC_CORRECTION_VALUE);
    uint16_t button_tmp = button;
    int16_t hatswitch_tmp = hatswitch;
    g_sle_hid_gamepad_report.data[0] = button_tmp & 0xFF;
    button_tmp >>= USB_BIT_8;
    g_sle_hid_gamepad_report.data[1] = button_tmp & 0xFF;

    if (hatswitch_tmp < 0) {
        hatswitch_tmp = USB_BIT_8;
    }
    g_sle_hid_gamepad_report.data[SLE_GAMEPAD_PARAM_ARGC_2] = hatswitch_tmp & USB_HAT_SWITCH;
    g_sle_hid_gamepad_report.data[SLE_GAMEPAD_PARAM_ARGC_3] = g_sle_joystick_status.x_axis;
    g_sle_hid_gamepad_report.data[SLE_GAMEPAD_PARAM_ARGC_4] = g_sle_joystick_status.y_axis;
    g_sle_hid_gamepad_report.data[SLE_GAMEPAD_PARAM_ARGC_5] = g_sle_joystick_status.z_axis;
    g_sle_hid_gamepad_report.data[SLE_GAMEPAD_PARAM_ARGC_6] = g_sle_joystick_status.z_axisrotation;

    g_sle_hid_gamepad_report.kind = SLE_HID_GAMEPAD_REPORT_KIND;
    if (sle_gamepad_server_send_report_by_handle((uint8_t *)(uintptr_t)&g_sle_hid_gamepad_report,
                                                 sizeof(usb_hid_gamepad_report_t)) != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s send report fail!\r\n", SLE_GAMEPAD_SERVER_LOG);
        return SLE_RET_FAIL;
    }
    osal_printk("%s sle_send_state end!\r\n", SLE_GAMEPAD_SERVER_LOG);
    osDelay(USB_GAMEPAD_TASK_DELAY_MS);
    return SLE_RET_SUCC;
}

static void *sle_gamepad_task(const char *arg)
{
    unused(arg);
    errcode_t ret;
    uint8_t conn_state = SLE_GAMEPAD_STATE_DISCONNECT;

    osal_printk("%s enter sle_gamepad_task!\r\n", SLE_GAMEPAD_SERVER_LOG);
    // 1.delay 6s for sle start.
    osDelay(SLE_GAMEPAD_TASK_DURATION_MS * SLE_GAMEPAD_SERVER_DELAY_COUNT);
    // 2.gamepad button init, current use six keys type.
    sle_gamepad_button_init(sle_gamepad_keyscan_callback);
    // 3.gamepad joysticks init.
    sle_gamepad_joysticks_init();
    // 4.sle server init.
    sle_gamepad_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);

    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_connect_state_changed_cbk,sle_start_announce fail :%02x\r\n",
                    SLE_GAMEPAD_SERVER_LOG, ret);
    }

    while (1) {
        while (conn_state == SLE_GAMEPAD_STATE_DISCONNECT) {
            osDelay(USB_GAMEPAD_TASK_DELAY_MS);
            get_g_sle_gamepad_server_conn_state(&conn_state);
        }
        while (conn_state == SLE_GAMEPAD_STATE_CONNECTED) {
            sle_send_state(g_sle_gamepad_key.d16, 0);
            osDelay(USB_GAMEPAD_TASK_DELAY_MS);
            get_g_sle_gamepad_server_conn_state(&conn_state);
        }
    }
    uapi_keyscan_deinit();
    uapi_adc_deinit();
    return NULL;
}

static void sle_gamepad_entry(void)
{
    osThreadAttr_t attr = { 0 };

    attr.name = "SLEGamepadTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_GAMEPAD_TASK_STACK_SIZE;
    attr.priority = SLE_GAMEPAD_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)sle_gamepad_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the sle_gamepad_entry. */
app_run(sle_gamepad_entry);