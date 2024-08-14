/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: ztw523 touch driver.
 * Author: @CompanyNameTag
 * Create: 2021-05-29
 */

#include "ztw523_touch1.h"
#include "time.h"
#include "soc_osal.h"
#include "osal_timer.h"
#include "osal_mutex.h"
#include "securec.h"
#include "ztw523_ctrl.h"
#include "tcxo.h"
#include "cmsis_os2.h"
#include "common_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

ztw523_drv_data g_ztw523_drv = {
    .chip_status = ZTW523_NOT_READY,
    .ops_mux = {0},
    .int_mask = ZTW523_INT_MASK,
    .work_status = ZTW523_WORK_INVALID,
    .fw_upgrade_flag = TD_FALSE,
    .gesture_wakeup = TD_FALSE,
    .host_dev_init_flag = TD_FALSE,
    .chip_info = ZTW523_DEVICE_INFO,
    .p_void = NULL,
};

static void ztw523_set_irq_mode(ztw523_irq_status irq_status, uint16_t irq_mask)
{
    unused(irq_mask);
    ext_errno ret;
    uint16_t reg_write = 0;

    tp_print("TOUCH:set irq mode %d", irq_status);

    switch (irq_status) {
        case ZTW523_IRQ_DISABLE:
            /* Write Int Register */
            reg_write = ZTW523_INT_NONE;
            ret = tp_i2c_reg_write(ZTW523_INT_ENABLE_FLAG, reg_write);
            if (ret != EXT_ERR_SUCCESS) {
                tp_err("TOUCH:set irq mode disable 0x%x", ret);
                return;
            }
            break;

        case ZTW523_IRQ_ENABLE:
            /* Write Int Register */
            reg_write = ZTW523_INT_MASK;
            ret = tp_i2c_reg_write(ZTW523_INT_ENABLE_FLAG, reg_write);
            if (ret != EXT_ERR_SUCCESS) {
                tp_err("TOUCH:set irq mode enable 0x%x", ret);
                return;
            }
            break;

        default:
            tp_err("TOUCH:not support irq mode 0x%x", irq_status);
            break;
    }

    return;
}

static ext_errno ztw523_parse_gesture_input(void)
{
    ext_errno ret;
    uint16_t gesture_val;
    uint16_t reg_read;
    uint16_t reg_write;

    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_GESTURE_WAKEUP_REG, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:parse gesture input read fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    gesture_val = reg_read;
    tp_print("TOUCH:parse gesture input ok 0x%x", reg_read);

    reg_write = 0;
    ret = tp_i2c_reg_write(ZTW523_GESTURE_WAKEUP_REG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:parse gesture input write fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    switch (gesture_val) {
        case ZTW523_GESTURE_SIGNAL_CLICK:
            g_ztw523_drv.touch_msg.tp_event = MC_TP_SHORT_CLICK;
            break;
        case ZTW523_GESTURE_DOUBLE_CLICK:
            g_ztw523_drv.touch_msg.tp_event = MC_TP_DOUBLE_CLICK;
            break;
        case ZTW523_GESTURE_SLIDE_UP:
            g_ztw523_drv.touch_msg.tp_event = MC_TP_SLIDE_UP;
            break;
        case ZTW523_GESTURE_SLIDE_DOWN:
            g_ztw523_drv.touch_msg.tp_event = MC_TP_SLIDE_DOWN;
            break;
        default:
            tp_err("TOUCH:gesture %d not Support", gesture_val);
            return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_parse_touch_input(void)
{
    static uint8_t touch_input[ZTW523_TOUCHINFO_LEN] = { 0 };
    uint16_t touch_status;
    uint16_t x_axis;
    uint16_t y_axis;
    uint8_t sub_status;

    ext_errno ret = tp_i2c_data_read(ZTW523_POINT_STATUS_REG, touch_input, ZTW523_TOUCHINFO_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:touch input read error! ret = 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    touch_status = ((uint16_t)touch_input[ZTW523_STATUSH_OFFSET] << ZTW523_REGDATA_SHIFT) |
                   (touch_input[ZTW523_STATUSL_OFFSET]);
    if ((touch_status & ZTW523_PALM_MASK) != 0) {
        g_ztw523_drv.touch_msg.tp_event = MC_TP_COVER;
        return EXT_ERR_SUCCESS;
    }

    if (touch_status != 0) {
        sub_status = touch_input[ZTW523_SUBSTATUS_OFFSET] & ZTW523_TOUCHINFO_MASK;
        x_axis = ((uint16_t)touch_input[ZTW523_XH_OFFSET] << ZTW523_REGDATA_SHIFT) | (touch_input[ZTW523_XL_OFFSET]);
        y_axis = ((uint16_t)touch_input[ZTW523_YH_OFFSET] << ZTW523_REGDATA_SHIFT) | (touch_input[ZTW523_YL_OFFSET]);

        switch (sub_status) {
            case ZTW523_TOUCH_DOWN:
                g_ztw523_drv.touch_msg.tp_event = MC_TP_PRESS;
                break;
            /* Need To Modify For UI */
            case ZTW523_TOUCH_EXIST:
            case ZTW523_TOUCH_MOVE:
            case ZTW523_TOUCH_NONE:
                g_ztw523_drv.touch_msg.tp_event = MC_TP_MOVE;
                break;
            case ZTW523_TOUCH_UP:
                g_ztw523_drv.touch_msg.tp_event = MC_TP_RELEASE;
                break;

            default:
                tp_err("TOUCH:parse input wrong status|substatus 0x%08x",
                    ((uint32_t)touch_status << ZTW523_SHIFT_16_BIT) | (uint32_t)sub_status);
                return EXT_ERR_FAILURE;
        }
        g_ztw523_drv.touch_msg.tp_event_info.x_axis[0] = x_axis;
        g_ztw523_drv.touch_msg.tp_event_info.y_axis[0] = y_axis;
        g_ztw523_drv.touch_msg.tv_usec = (uint32_t)uapi_tcxo_get_ms();
    } else {
        g_ztw523_drv.touch_msg.tp_event = MC_TP_INVALID;
        tp_print("TOUCH:parse wrong touchstatus %d", touch_status);
        return EXT_ERR_SUCCESS;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_parse_input(void)
{
    ext_errno ret = EXT_ERR_FAILURE;
    uint16_t reg_write;

    if (g_ztw523_drv.gesture_wakeup == TD_TRUE) {
        ret = ztw523_parse_gesture_input();
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("TOUCH:parse gesture input fail 0x%x", ret);
            return EXT_ERR_FAILURE;
        }
    } else {
        ret = ztw523_parse_touch_input();
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("TOUCH:parse touch input fail 0x%x", ret);
            return EXT_ERR_FAILURE;
        }
    }
    /* clear int */
    reg_write = ZTW523_CLEAR_INT_STATUS_CMD;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:parse input clear int fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno ztw523_irq_callback(uint8_t *data_buf, uint8_t data_len)
{
    unused(data_len);
    ext_errno ret;
    int32_t mem_ret;
    uint16_t reg_write;

    if (g_ztw523_drv.work_status == ZTW523_WORK_GETCMCP) {
        tp_warning("in getcmcp status: %d", ZTW523_WORK_GETCMCP);
        return EXT_ERR_SUCCESS;
    }

    g_ztw523_drv.touch_msg.tp_event = MC_TP_INVALID;

    if ((g_ztw523_drv.work_status == ZTW523_WORK_NORMAL) || (g_ztw523_drv.work_status == ZTW523_WORK_STANDBY)) {
        g_ztw523_drv.irq_count++;
        ret = ztw523_parse_input();
        if (ret != EXT_ERR_SUCCESS) {
            ztw523_init();
            reg_write = ZTW523_CLEAR_INT_STATUS_CMD;
            (void)tp_i2c_cmd_write(reg_write);
            tp_err("touch parse input fail, ret = 0x%x", ret);
            return EXT_ERR_FAILURE;
        }
    }

    mem_ret = memcpy_s(data_buf, sizeof(input_event_info), &(g_ztw523_drv.touch_msg), sizeof(input_event_info));
    if (mem_ret != EOK) {
        tp_err("TOUCH:mem cpy error, ret = 0x%x", mem_ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_power_sequence(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint16_t reg_read;
    reg_write = ZTW523_ENABLE;

    ret = tp_i2c_reg_write(ZTW523_REG_CMD_ENABLE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_VENDOR_CMD_ENABLE;
    }
    osDelay(ZTW523_GENERAL_DELAY_1);
    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_REG_CHIP_ID, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_READ_CHIPID;
    }
    osDelay(ZTW523_GENERAL_DELAY_1);

    reg_write = ZTW523_REG_INTN_CLEAR;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_INTN_CLEAR;
    }
    osDelay(ZTW523_GENERAL_DELAY_1);

    reg_write = ZTW523_ENABLE;
    ret = tp_i2c_reg_write(ZTW523_REG_NVM, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_NVM_INIT;
    }
    osDelay(ZTW523_GENERAL_DELAY_5);

    reg_write = ZTW523_ENABLE;
    ret = tp_i2c_reg_write(ZTW523_REG_PROGRAM_START, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_PROGRAM_START;
    }

    /* Delay 150ms, Refer Demo Code */
    osDelay(ZTW523_FIRMWAREON_DELAY);

    /* config report rate to 60Hz */
    reg_write = ZTW523_REPORT_RATE;
    ret = tp_i2c_reg_write(ZTW523_REPORT_REG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_REPORT_RATE;
    }

    /* sw reset */
    reg_write = ZTW523_SWRESET_CMD;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_SW_RESET;
    }
    /* Delay 10ms for sw reset */
    osDelay(ZTW523_GENERAL_DELAY_10);

    return EXT_ERR_SUCCESS;
}

static void ztw523_clear_interrupt(uint8_t retry_times, uint16_t delay)
{
    uint16_t reg_write = ZTW523_CLEAR_INT_STATUS_CMD;
    ext_errno ret;
    uint8_t times;
    for (times = 1; times <= retry_times; times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            return;
        }
        osDelay(delay);
    }

    if (times > retry_times) {
        tp_warning("TOUCH:Clear Int Cmd1! ret = 0x%x", ret);
    }
}

static ext_errno ztw523_reg_config(void)
{
    ext_errno ret;
    uint16_t reg_write;

    /* Initial Touch Mode */
    reg_write = ZTW523_POINT_MODE;
    ret = tp_i2c_reg_write(ZTW523_INITIAL_TOUCH_MODE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_INIT_TOUCHMODE;
    }

    ret = tp_i2c_reg_write(ZTW523_TOUCH_MODE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_TOUCHMODE;
    }

    /* Set Finger Num */
    reg_write = ZTW523_MAX_SUPPORTED_FINGER_NUM;
    ret = tp_i2c_reg_write(ZTW523_SUPPORTED_FINGER_NUM, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_FINGER_NUM;
    }

    /* Set X Resolution */
    reg_write = ZTW523_RES_MAX_X;
    ret = tp_i2c_reg_write(ZTW523_X_RESOLUTION, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_XRES;
    }

    /* Set Y Resolution */
    reg_write = ZTW523_RES_MAX_Y;
    ret = tp_i2c_reg_write(ZTW523_Y_RESOLUTION, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_YRES;
    }

    /* Set Lock Point Range */
    ret = tp_i2c_reg_write(ZTW523_HOLD_POINT_TRESHHOLD, ZTW523_POINT_TRESHHOLD);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_YRES;
    }

    /* Write Calib Cmd */
    reg_write = ZTW523_CALIBRATE_CMD;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_CALIBCMD;
    }

    /* Write Int Register */
    reg_write = ZTW523_INT_MASK;
    ret = tp_i2c_reg_write(ZTW523_INT_ENABLE_FLAG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_INTMASK;
    }
#ifdef ZTW523_GESTURE_WAKEUP
    /* Write Gesture Register */
    reg_write = ZTW523_GESTURE_WAKEUP_INIT;
    ret = tp_i2c_reg_write(ZTW523_GESTURE_WAKEUP_REG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_WRITE_GESWAKEUP;
    }
#endif

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_configuration(void)
{
    ext_errno ret;
    uint8_t retry_times = 0;
    uint16_t reg_read;
    uint16_t reg_write;

    /* Software Reset */
    reg_write = ZTW523_SWRESET_CMD;
    for (retry_times = 0; retry_times < ZTW523_RETRY_TIMES; retry_times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        osDelay(ZTW523_RESETHIGH_DELAY);
    }

    if (retry_times >= ZTW523_RETRY_TIMES) {
        tp_err("TOUCH:configuration sw reset fail! ret = 0x%x", ret);
        return EXT_ERR_TP_ZTW523_CONFIG_RESET;
    }

    /* Read Firmware Version */
    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_FIRMWARE_VERSION, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_READ_FWVERSION;
    }
    g_ztw523_drv.firmware_version = reg_read;

    /* Read Minor Firmware Version */
    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_MINOR_FW_VERSION, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_READ_FWMINORVERSION;
    }
    g_ztw523_drv.minor_firmware_version = reg_read;

    /* Read Hw ID */
    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_HW_ID, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("configuration error (read hw id) 0x%x", ret);
        return EXT_ERR_TP_ZTW523_GET_HWID_READ_ID;
    }
    g_ztw523_drv.chip_hw_id = reg_read;

    /* Read Register Version */
    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_DATA_VERSION_REG, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_CONFIG_READ_DATAREG;
    }
    g_ztw523_drv.reg_version = reg_read;

    ret = ztw523_reg_config();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    /* Write Clear Int Cmd */
    ztw523_clear_interrupt(ZTW523_RETRY_TIMES, ZTW523_GENERAL_DELAY_1);

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_get_panel_info(void)
{
    ext_errno ret;
    uint16_t reg_read = 0;
    /* Read X Channel Num */
    ret = tp_i2c_data_read(ZTW523_TOTAL_NUMBER_OF_X, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:GetPanelInfo error read x channel 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    g_ztw523_drv.panel_info.xchannel_num = reg_read;
    /* Read Y Channel Num */
    reg_read = 0;
    ret = tp_i2c_data_read(ZTW523_TOTAL_NUMBER_OF_Y, (uint8_t *)&reg_read, ZTW523_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:GetPanelInfo error read y channel 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    g_ztw523_drv.panel_info.ychannel_num = reg_read;
    g_ztw523_drv.panel_info.total_dnd_num =
        g_ztw523_drv.panel_info.xchannel_num * g_ztw523_drv.panel_info.ychannel_num;
    g_ztw523_drv.panel_info.total_self_dnd_num =
        g_ztw523_drv.panel_info.xchannel_num + g_ztw523_drv.panel_info.ychannel_num;

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_standby(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint8_t retry_times;

    if ((g_ztw523_drv.work_status == ZTW523_WORK_STANDBY) || (g_ztw523_drv.work_status == ZTW523_WORK_SLEEP)) {
        tp_print("alread standby or sleep");
        return EXT_ERR_SUCCESS;
    }
    /* Disabel Irq */
    ztw523_set_irq_mode(ZTW523_IRQ_DISABLE, ZTW523_INT_NONE);

    reg_write = ZTW523_POWER_CTL_BEGIN;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        ztw523_set_irq_mode(ZTW523_IRQ_ENABLE, ZTW523_INT_MASK);
        tp_err("standby failed 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    /* delay 10 ms */
    osDelay(ZTW523_GENERAL_DELAY_10);
    /* Send Clear Int Cmd */
    reg_write = ZTW523_CLEAR_INT_STATUS_CMD;
    for (retry_times = 0; retry_times < ZTW523_INIT_RETRY_TIMERS; retry_times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        /* delay 10 ms */
        osDelay(ZTW523_GENERAL_DELAY_10);
    }

    /* Send Idle Cmd */
    reg_write = ZTW523_IDLE_CMD;
    for (retry_times = 0; retry_times < ZTW523_INIT_RETRY_TIMERS; retry_times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        /* delay 10 ms */
        osDelay(ZTW523_GENERAL_DELAY_10);
    }

    reg_write = ZTW523_POWER_CTL_END;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        ztw523_set_irq_mode(ZTW523_IRQ_ENABLE, ZTW523_INT_MASK);
        tp_err("standby failed 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    /* delay 10 ms */
    osDelay(ZTW523_GENERAL_DELAY_10);
    ztw523_set_irq_mode(ZTW523_IRQ_ENABLE, ZTW523_INT_MASK);
    g_ztw523_drv.work_status = ZTW523_WORK_STANDBY;
    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_suspend(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint8_t retry_times = 0;

    if (g_ztw523_drv.work_status == ZTW523_WORK_SLEEP) {
        tp_print("TOUCH:alread suspend");
        return EXT_ERR_SUCCESS;
    }
    /* Disabel Irq */
    ztw523_set_irq_mode(ZTW523_IRQ_DISABLE, ZTW523_INT_NONE);

    reg_write = ZTW523_POWER_CTL_BEGIN;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:suspend begin 0x%x", ret);
        goto suspend_exit;
    }

    osDelay(ZTW523_GENERAL_DELAY_10);
    /* Send Clear Int Cmd */
    reg_write = ZTW523_CLEAR_INT_STATUS_CMD;
    for (retry_times = 0; retry_times < ZTW523_INIT_RETRY_TIMERS; retry_times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        osDelay(ZTW523_GENERAL_DELAY_10);
    }

    /* Send Sleep Cmd */
    reg_write = ZTW523_SLEEP_CMD;
    for (retry_times = 0; retry_times < ZTW523_INIT_RETRY_TIMERS; retry_times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        osDelay(ZTW523_GENERAL_DELAY_10);
    }

    reg_write = ZTW523_POWER_CTL_END;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:suspend end 0x%x", ret);
        goto suspend_exit;
    }

    osDelay(ZTW523_GENERAL_DELAY_10);

    g_ztw523_drv.work_status = ZTW523_WORK_SLEEP;

    return EXT_ERR_SUCCESS;

suspend_exit:
    /* Enable Irq */
    ztw523_set_irq_mode(ZTW523_IRQ_ENABLE, ZTW523_INT_MASK);

    tp_err("TOUCH:suspend failed 0x%x", ret);

    return EXT_ERR_FAILURE;
}

static ext_errno ztw523_resume_post(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint16_t cmd_write;

    reg_write = ZTW523_ENABLE;
    ret = tp_i2c_reg_write(ZTW523_REG_CMD_ENABLE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("resume post reg cmd failed 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    /* delay for ms */
    osDelay(ZTW523_GENERAL_DELAY_1);

    cmd_write = ZTW523_REG_INTN_CLEAR;
    ret = tp_i2c_cmd_write(cmd_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_TP_ZTW523_POWERUP_INTN_CLEAR;
    }
    osDelay(ZTW523_GENERAL_DELAY_1);

    reg_write = ZTW523_ENABLE;
    ret = tp_i2c_reg_write(ZTW523_REG_NVM, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("resume post reg nvm failed 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    osDelay(ZTW523_GENERAL_DELAY_1);

    reg_write = ZTW523_ENABLE;
    ret = tp_i2c_reg_write(ZTW523_REG_PROGRAM_START, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("resume post reg program start failed 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    osDelay(ZTW523_GENERAL_DELAY_50); /* delay 50 ms */

    return EXT_ERR_SUCCESS;
}

ext_errno ztw523_resume(void)
{
    ext_errno ret;
    uint8_t retry_times = 0;

    g_ztw523_drv.work_status = ZTW523_WORK_RESUME;

    ztw523_resume_reset();

    for (; retry_times < ZTW523_INIT_RETRY_TIMERS; retry_times++) {
        ret = ztw523_resume_post();
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("resume: post err %d", ret);
            /* retry delay 200ms */
            osDelay(ZTW523_GENERAL_DELAY_200);
        } else {
            break;
        }
    }

    if (retry_times == ZTW523_INIT_RETRY_TIMERS) {
        tp_err("resume: overtimes %d", retry_times);
        goto resume_exit;
    }

    ret = ztw523_configuration();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:resume config fail 0x%x", ret);
        return EXT_ERR_TP_ZTW523_INIT_CONFIG;
    }

    g_ztw523_drv.work_status = ZTW523_WORK_NORMAL;
#ifdef ZTW523_GESTURE_WAKEUP
    g_ztw523_drv.gesture_wakeup = TD_FALSE;
#endif
    /* Enable Irq */
    ztw523_set_irq_mode(ZTW523_IRQ_ENABLE, ZTW523_INT_MASK);

    return EXT_ERR_SUCCESS;

resume_exit:

    /* Enable Irq */
    ztw523_set_irq_mode(ZTW523_IRQ_DISABLE, ZTW523_INT_MASK);

    tp_err("resume failed 0x%x", ret);

    return EXT_ERR_FAILURE;
}

ext_errno ztw523_get_power_mode(uint32_t *power_mode)
{
    if (power_mode == NULL) {
        return EXT_ERR_FAILURE;
    }

    *power_mode = (uint32_t)(g_ztw523_drv.work_status);

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_set_standby_mode(void)
{
    ext_errno ret;

    ret = ztw523_standby();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:ztw523 set standby 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
#ifdef ZTW523_GESTURE_WAKEUP
    g_ztw523_drv.gesture_wakeup = TD_TRUE;
#endif
    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_set_sleep_mode(void)
{
    ext_errno ret;

    ret = ztw523_suspend();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:ztw523 set suspend fail 0x%x", ret);
    }
#ifdef ZTW523_GESTURE_WAKEUP
    g_ztw523_drv.gesture_wakeup = TD_FALSE;
#endif
    g_ztw523_drv.work_status = ZTW523_WORK_SLEEP;
    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_set_normal_mode(void)
{
    ext_errno ret;
    uint16_t reg_write;

    if (g_ztw523_drv.work_status == ZTW523_WORK_NORMAL) {
        return EXT_ERR_SUCCESS;
    }

    reg_write = ZTW523_SDRAM_RESET;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    /* delay 50ms */
    osDelay(ZTW523_GENERAL_DELAY_50);
    ret = ztw523_power_sequence();
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    ztw523_set_irq_mode(ZTW523_IRQ_ENABLE, ZTW523_INT_MASK);
    g_ztw523_drv.work_status = ZTW523_WORK_NORMAL;
#ifdef ZTW523_GESTURE_WAKEUP
    g_ztw523_drv.gesture_wakeup = TD_FALSE;
#endif
    return EXT_ERR_SUCCESS;
}

ext_errno ztw523_set_power_mode(tp_work_mode power_mode)
{
    ext_errno ret = EXT_ERR_SUCCESS;

    switch (power_mode) {
        case MC_TP_SLEEP_WORK_MODE:
        case MC_TP_SUSPEND_SCAN:
            ret = ztw523_set_sleep_mode();
            break;
        case MC_TP_NORMAL_WORK_MODE:
        case MC_TP_RESUME_SCAN:
            ret = ztw523_set_normal_mode();
            break;
        case MC_TP_STANDBY_WORK_MODE:
            ret = ztw523_set_standby_mode();
            break;
        default:
            tp_err("TOUCH:ztw523 not support powermode 0x%x", power_mode);
            ret = EXT_ERR_FAILURE;
            break;
    }

    return ret;
}

static ext_errno ztw523_config(void)
{
    ext_errno ret;

    /* Step 1 hardware reset */
    ret = ztw523_reset();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:Init reset fail 0x%x", ret);
        return EXT_ERR_TP_ZTW523_INIT_RESET;
    }

    /* Step 2 power up */
    ret = ztw523_power_sequence();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:init powerup fail 0x%x", ret);
        return EXT_ERR_TP_ZTW523_INIT_POWERUP;
    }

    /* Step 3 configuration */
    ret = ztw523_configuration();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:init config fail 0x%x", ret);
        return EXT_ERR_TP_ZTW523_INIT_CONFIG;
    }

    /* Step 3 Panel Info */
    ret = ztw523_get_panel_info();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:init get panelinfo fail 0x%x", ret);
        return EXT_ERR_TP_ZTW523_INIT_GETINFO;
    }

    /* Record Status For Init */
    g_ztw523_drv.chip_status = ZTW523_READY;
    g_ztw523_drv.work_status = ZTW523_WORK_NORMAL;

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_resource_deinit(void)
{
    g_ztw523_drv.work_status = ZTW523_WORK_INVALID;
    g_ztw523_drv.fw_upgrade_flag = TD_FALSE;
    osal_mutex_destroy(&g_ztw523_drv.ops_mux);

    return EXT_ERR_SUCCESS;
}

ext_errno ztw523_init(void)
{
    ext_errno ret = EXT_ERR_FAILURE;
    ext_errno err_ret;

    /* create tp operation mutex */
    if (g_ztw523_drv.ops_mux.mutex == NULL) {
        ret = osal_mutex_init(&(g_ztw523_drv.ops_mux));
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("TOUCH:ztw523 init create mux lock fail 0x%x", ret);
            return EXT_ERR_FAILURE;
        }
    }

    /* get tp operater mutex */
    ret = osal_mutex_lock_timeout(&g_ztw523_drv.ops_mux, ZTW523_OPERATER_MUTEX_WAIT_TIME);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:ztw523 init wait mux lock fail 0x%x", ret);
        goto init_fail;
    }

    /* tp hardware reset and init */
    ret = ztw523_config();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:ztw523 init fail 0x%x", ret);
        goto init_fail;
    }

    /* release tp operater mutex */
    osal_mutex_unlock(&g_ztw523_drv.ops_mux);

    tp_print("TOUCH:ztw523 init succ!");
    return EXT_ERR_SUCCESS;

init_fail:
    /* release tp operater mutex */
    osal_mutex_unlock(&g_ztw523_drv.ops_mux);
    err_ret =  ztw523_resource_deinit();
    if (err_ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:delete mux lock err! ret = 0x%x", err_ret);
    }
    return ret;
}

ext_errno ztw523_deinit(void)
{
    ext_errno ret = ztw523_resource_deinit();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH: ztw523 deinit fail! ret = 0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
