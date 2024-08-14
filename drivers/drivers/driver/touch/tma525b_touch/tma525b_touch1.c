/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: tma525b touch driver.
 * Author: @CompanyNameTag
 * Create: 2023-02-06
 */

#include "tma525b_touch1.h"
#include "securec.h"
#include "soc_osal.h"
#include "osal_timer.h"
#include "osal_mutex.h"
#include "tcxo.h"
#include "gpio.h"
#include "time.h"
#include "pinctrl.h"
#include "hal_gpio.h"
#include "tma525b_ctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint8_t g_tma525b_input[TMA525B_INPUT_SIZE] = {0};
semaphoreex g_intsem = { { "IntSem", 0, NULL, 0 }, 1, 0 };
/* Global Drv Data */
static tma525b_touch_drv_data g_tmadrv = {
    .debug = 0,
    .systemlock = NULL,
    .operlock = NULL,
    .intsem = NULL,
    .crctable = {
        0x0000,
        0x1021,
        0x2042,
        0x3063,
        0x4084,
        0x50a5,
        0x60c6,
        0x70e7,
        0x8108,
        0x9129,
        0xa14a,
        0xb16b,
        0xc18c,
        0xd1ad,
        0xe1ce,
        0xf1ef,
    },
    .pvoid = NULL,
    .osready = TD_FALSE,
    .selfcheckstatus = TD_FALSE,
    .tpmsgstatus = false,
};

tma525b_drv_data g_tma525b_drv = {
    .chip_status = TMA525B_NOT_READY,
    .ops_mux = {0},
    .int_mask = TMA525B_INT_MASK,
    .work_status = TMA525B_WORK_INVALID,
    .fw_upgrade_flag = TD_FALSE,
    .gesture_wakeup = TD_FALSE,
    .host_dev_init_flag = TD_FALSE,
    .chip_info = TMA525B_DEVICE_INFO,
    .p_void = NULL,
};

static void tma525b_set_irq_mode(tma525b_irq_status irq_status, uint16_t irq_mask)
{
    unused(irq_mask);
    ext_errno ret;
    uint16_t reg_write = 0;

    tp_print("TOUCH:set irq mode %d", irq_status);

    switch (irq_status) {
        case TMA525B_IRQ_DISABLE:
            /* Write Int Register */
            reg_write = TMA525B_INT_NONE;
            ret = tp_i2c_reg_write(TMA525B_INT_ENABLE_FLAG, reg_write);
            if (ret != EXT_ERR_SUCCESS) {
                tp_err("TOUCH:set irq mode disable 0x%x", ret);
                return;
            }
            break;

        case TMA525B_IRQ_ENABLE:
            /* Write Int Register */
            reg_write = TMA525B_INT_MASK;
            ret = tp_i2c_reg_write(TMA525B_INT_ENABLE_FLAG, reg_write);
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

static int32_t tma525b_read_input(void)
{
    int32_t ret;
    uint32_t readsize;
    uint8_t readbuf[TMA525B_INPUT_SIZE_LEN] = { 0 };
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    ret = tp_i2c_data_read(0, readbuf, TMA525B_INPUT_SIZE_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2_ReadInput read size fail 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    readsize = get_input_size(readbuf);
    coredata->inputsize = readsize;
    if ((readsize == 0) || (readsize == TMA525B_EMPTY_PIP1P7_BEFORE) || (readsize == TMA525B_EMPTY_PIP1P7_LATER)) {
        return EXT_ERR_SUCCESS;
    }

    if (readsize > TMA525B_INPUT_SIZE) {
        tp_err("TOUCH:ReadInput: max input size exceeded 0x%x\r\n", readsize);
        readsize = TMA525B_INPUT_SIZE;
        coredata->inputsize = readsize;
    }

    (void)memset_s(coredata->inputbuf, TMA525B_INPUT_SIZE, 0, TMA525B_INPUT_SIZE);
    ret = tp_i2c_data_read(0, coredata->inputbuf, readsize);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2_ReadInput read data fail 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_parse_touch_input(inputeventinfo *eventinfo)
{
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    uint32_t inputsize = coredata->inputsize;
    if ((inputsize == TMA525B_REPORT_SIZE) || (inputsize == TMA525B_REPORT_HEAD_SIZE)) {
        uint8_t coverinfo = coredata->inputbuf[TMA525B_RECORD_LO_OFFSET];
        uint8_t coverlo = (coverinfo & TMA525B_RECORD_LO_BIT_MASK) >> TMA525B_RECORD_LO_BIT_OFFSET;
        uint8_t reportnum = coverinfo & TMA525B_RECORD_FINGER_BIT_MASK;
        /* if palm and up report together, report up. next point will report cover only. */
        if ((coverlo != 0) && (reportnum == 0)) {
            eventinfo->tpevent = TP_COVER;
            return EXT_ERR_SUCCESS;
        }
        if (inputsize == TMA525B_REPORT_HEAD_SIZE) {
            eventinfo->tpevent = TP_INVALID;
            return EXT_ERR_SUCCESS;
        }
    } else if (inputsize == TMA525B_REPORT_SIZE_TWO_FINGER) {
        /* Last Input Report Only Has 7Byte Head Data Or Two Finger Support Default */
        tp_err("TOUCH:touch input: input header size 0x%x\r\n", inputsize);
        return EXT_ERR_SUCCESS;
    } else {
        tp_err("TOUCH:touch input: wrong input size 0x%x\r\n", inputsize);
        return EXT_ERR_FAILURE;
    }

    uint8_t noiseeffects = coredata->inputbuf[TMA525B_RECORD_COUNTER_NOISE_OFFSET];
    noiseeffects = noiseeffects & TMA525B_RECORD_NOISE_BIT_MASK;

    if (noiseeffects != 0) {
        tp_err("TOUCH:touch input: noise=0x%x\r\n", noiseeffects);
    }

    uint16_t xaxis = ((uint16_t)coredata->inputbuf[TMA525B_XH_OFFSET] << TMA525B_SHIFT_8_BIT) +
            coredata->inputbuf[TMA525B_XL_OFFSET];
    uint16_t yaxis = ((uint16_t)coredata->inputbuf[TMA525B_YH_OFFSET] << TMA525B_SHIFT_8_BIT) +
            coredata->inputbuf[TMA525B_YL_OFFSET];
    uint8_t touchevent = coredata->inputbuf[TMA525B_EVENT_OFFSET] & TMA525B_MASK_EVENT_BIT;

    switch (touchevent) {
        case TMA525B_TYPE_TOUCH_DOWN:
            eventinfo->tpevent = TP_PRESS;
            break;

        case TMA525B_TYPE_TOUCH_MOVE:
            eventinfo->tpevent = TP_MOVE;
            break;

        case TMA525B_TYPE_TOUCH_UP:
            eventinfo->tpevent = TP_RELEASE;
            break;

        default:
            tp_err("TOUCH:not support touch event 0x%x\r\n", touchevent);
            return EXT_ERR_FAILURE;
    }

    eventinfo->tpeventinfo.xaxis[0] = xaxis;
    eventinfo->tpeventinfo.yaxis[0] = yaxis;

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_parse_wakeup_input(inputeventinfo *eventinfo)
{
    uint8_t eventcode;
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    if (eventinfo == NULL) {
        return EXT_ERR_FAILURE;
    }

    eventcode = coredata->inputbuf[TMA525B_EVENT_CODE_OFFSET];
    switch (eventcode) {
        case TMA525B_GESTURE_SINGLE_CLICK:
            eventinfo->tpevent = TP_SHORT_CLICK;
            break;
        case TMA525B_GESTURE_DOUBLE_CLICK:
            eventinfo->tpevent = TP_DOUBLE_CLICK;
            break;
        case TMA525B_GESTURE_SLIDE_UP:
            eventinfo->tpevent = TP_SLIDE_UP;
            break;
        case TMA525B_GESTURE_SLIDE_DOWN:
            eventinfo->tpevent = TP_SLIDE_DOWN;
            break;
        default:
            tp_err("TOUCH:EasyWake: not support gesture 0x%x\r\n", eventcode);
            return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_release_wait_event(void)
{
    int32_t ret;

    if (g_tmadrv.osready == true) {
        ret = osSemaphoreRelease(g_tmadrv.intsem);
        if (ret != osOK) {
            tp_err("WaitEventTimeout: wait 0x%x\r\n", ret);
            return ERROR_RELEASE_EVENT_FAIL;
        }
        return EXT_ERR_SUCCESS;
    } else {
        g_tmadrv.intflag = TD_TRUE;
        return EXT_ERR_SUCCESS;
    }
}

static int32_t tma525b_wait_mutex(osMutexId_t mutexid, uint32_t timeout)
{
    int32_t osret;

    if (g_tmadrv.osready == true) {
        osret = osMutexAcquire(mutexid, timeout);
        if (osret != osOK) {
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_wait_event_timeout(void)
{
    int32_t ret;
    uint32_t intcount = 0;

    if (g_tmadrv.osready == true) {
        if (g_tmadrv.intsem == NULL) {
            tp_err("WaitEventTimeout: IntSem NULL\r\n");
            return EXT_ERR_FAILURE;
        }

        ret = osSemaphoreAcquire(g_tmadrv.intsem, g_tmadrv.timeoutms);
        if (ret != osOK) {
            tp_err("WaitEventTimeout: wait 0x%x\r\n", ret);
            return ERROR_WAIT_EVENT_TIMEOUT;
        }
        return EXT_ERR_SUCCESS;
    } else {
        g_tmadrv.intflag = TD_FALSE;
        while ((g_tmadrv.intflag == TD_FALSE) && (intcount < g_tmadrv.timeoutms)) {
            osDelay(1);
            intcount++;
        }
        if (intcount >= g_tmadrv.timeoutms) {
            return ERROR_WAIT_EVENT_TIMEOUT;
        }
        return EXT_ERR_SUCCESS;
    }
}

static int32_t tma525b_release_mutex(osMutexId_t mutexid)
{
    int32_t osret;

    if (g_tmadrv.osready == true) {
        osret = osMutexRelease(mutexid);
        if (osret != osOK) {
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_parse_command_input(void)
{
    errno_t memret;
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    memret = memset_s(coredata->responsebuf, TMA525B_INPUT_SIZE, 0, TMA525B_INPUT_SIZE);
    if (memret != EOK) {
        tp_err("TOUCH:TPIC2_ParseCommandInput:mem cpy error\r\n");
        return EXT_ERR_FAILURE;
    }
    memret = memcpy_s(coredata->responsebuf, TMA525B_INPUT_SIZE, coredata->inputbuf, coredata->inputsize);
    if (memret != EOK) {
        tp_err("TOUCH:TPIC2_ParseCommandInput:mem cpy error\r\n");
        return EXT_ERR_FAILURE;
    }
    coredata->responsesize = coredata->inputsize;
    (void)tma525b_wait_mutex(g_tmadrv.systemlock, TMA525B_SYSTEM_MUTEX_WAIT_TIME);
    g_tmadrv.hidcmdstate = 0;
    (void)tma525b_release_mutex(g_tmadrv.systemlock);

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_parse_input(inputeventinfo *eventinfo)
{
    int32_t ret;
    uint32_t inputsize;
    uint32_t reportid;
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    inputsize = coredata->inputsize;
    /* handle touch input or cmd response */
    if ((inputsize != 0) && (inputsize != TMA525B_EMPTY_PIP1P7_BEFORE) && (inputsize < TMA525B_EMPTY_PIP1P7_LATER)) {
        reportid = coredata->inputbuf[TMA525B_REPORTID_SHIFT];
        tp_print("TOUCH:parse input: report id:0x%x\r\n", reportid);

        /* ESD may lead to (cd->input_buf[5] & 0x80 == 1) */
        if ((reportid == HID_TOUCH_REPORT_ID) && ((coredata->inputbuf[TMA525B_ESD_SHIFT] & TMA525B_ESD_MASK) != 0)) {
            tp_err("TOUCH:parse input:ESD error detected, need to reset\r\n");
            return EXT_ERR_FAILURE;
        }

        /* Parse Touch Input */
        if (reportid == HID_TOUCH_REPORT_ID) {
            g_tmadrv.tpmsgstatus = false;
            ret = tma525b_parse_touch_input(eventinfo);
            return ret;
        }
        /* Check wake-up report */
        if (reportid == HID_WAKEUP_REPORT_ID) {
            g_tmadrv.tpmsgstatus = false;
            return tma525b_parse_wakeup_input(eventinfo);
        }

        /* Parse Cmd Input */
        if ((reportid != HID_TOUCH_REPORT_ID) && (reportid != HID_BTN_REPORT_ID) &&
            (reportid != HID_SENSOR_DATA_REPORT_ID) && (reportid != HID_TRACKING_HEATMAP_REPOR_ID) &&
            (g_tmadrv.hidcmdstate != 0)) {
            tma525b_parse_command_input();

            ret = tma525b_release_wait_event();
            if (ret != EXT_ERR_SUCCESS) {
                tp_err("TOUCH:parse_input:ReleaseWaitEvent err1 0x%x\r\n", ret);
                return ret;
            }
            return EXT_ERR_SUCCESS;
        } else {
            tp_err("TOUCH:parseinput:not support report id 0x%x\r\n", reportid);
            return EXT_ERR_FAILURE;
        }
    } else if ((inputsize == TMA525B_EMPTY_PIP1P7_BEFORE) || (inputsize == TMA525B_EMPTY_PIP1P7_LATER)) {
        return EXT_ERR_SUCCESS;
    } else {
        return EXT_ERR_FAILURE;
    }
}

ext_errno tma525b_irq_callback(uint8_t *data_buf, uint8_t data_len)
{
    unused(data_len);
    ext_errno ret;
    int32_t mem_ret;
    inputeventinfo eventinfo;

    ret = tma525b_read_input();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:MC_TPIC2_IrqCallback read input fail 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    ret = tma525b_parse_input(&eventinfo);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:MC_TPIC2_IrqCallback parse input err 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    g_tma525b_drv.touch_msg.tp_event_info.x_axis[0] = eventinfo.tpeventinfo.xaxis[0];
    g_tma525b_drv.touch_msg.tp_event_info.y_axis[0] = eventinfo.tpeventinfo.yaxis[0];
    g_tma525b_drv.touch_msg.tp_event = (tp_event_type)eventinfo.tpevent;
    mem_ret = memcpy_s(data_buf, sizeof(input_event_info), &(g_tma525b_drv.touch_msg), sizeof(input_event_info));
    if (mem_ret != EOK) {
        tp_err("TOUCH:mem cpy error, ret = 0x%x", mem_ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_power_sequence(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint16_t reg_read;
    reg_write = TMA525B_ENABLE;

    ret = tp_i2c_reg_write(TMA525B_REG_CMD_ENABLE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    osDelay(TMA525B_GENERAL_DELAY_1);
    reg_read = 0;
    ret = tp_i2c_data_read(TMA525B_REG_CHIP_ID, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    osDelay(TMA525B_GENERAL_DELAY_1);

    reg_write = TMA525B_REG_INTN_CLEAR;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    osDelay(TMA525B_GENERAL_DELAY_1);

    reg_write = TMA525B_ENABLE;
    ret = tp_i2c_reg_write(TMA525B_REG_NVM, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    osDelay(TMA525B_GENERAL_DELAY_5);

    reg_write = TMA525B_ENABLE;
    ret = tp_i2c_reg_write(TMA525B_REG_PROGRAM_START, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* Delay 150ms, Refer Demo Code */
    osDelay(TMA525B_FIRMWAREON_DELAY);

    /* config report rate to 60Hz */
    reg_write = TMA525B_REPORT_RATE;
    ret = tp_i2c_reg_write(TMA525B_REPORT_REG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* sw reset */
    reg_write = TMA525B_SWRESET_CMD;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    /* Delay 10ms for sw reset */
    osDelay(TMA525B_GENERAL_DELAY_10);

    return EXT_ERR_SUCCESS;
}

static void tma525b_clear_interrupt(uint8_t retry_times, uint16_t delay)
{
    uint16_t reg_write = TMA525B_CLEAR_INT_STATUS_CMD;
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

static ext_errno tma525b_reg_config(void)
{
    ext_errno ret;
    uint16_t reg_write;

    /* Initial Touch Mode */
    reg_write = TMA525B_POINT_MODE;
    ret = tp_i2c_reg_write(TMA525B_INITIAL_TOUCH_MODE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    ret = tp_i2c_reg_write(TMA525B_TOUCH_MODE, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* Set Finger Num */
    reg_write = TMA525B_MAX_SUPPORTED_FINGER_NUM;
    ret = tp_i2c_reg_write(TMA525B_SUPPORTED_FINGER_NUM, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* Set X Resolution */
    reg_write = TMA525B_RES_MAX_X;
    ret = tp_i2c_reg_write(TMA525B_X_RESOLUTION, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* Set Y Resolution */
    reg_write = TMA525B_RES_MAX_Y;
    ret = tp_i2c_reg_write(TMA525B_Y_RESOLUTION, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* Write Calib Cmd */
    reg_write = TMA525B_CALIBRATE_CMD;
    ret = tp_i2c_cmd_write(reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    /* Write Int Register */
    reg_write = TMA525B_INT_MASK;
    ret = tp_i2c_reg_write(TMA525B_INT_ENABLE_FLAG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
#ifdef TMA525B_GESTURE_WAKEUP
    /* Write Gesture Register */
    reg_write = TMA525B_GESTURE_WAKEUP_INIT;
    ret = tp_i2c_reg_write(TMA525B_GESTURE_WAKEUP_REG, reg_write);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
#endif

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_configuration(void)
{
    ext_errno ret;
    uint8_t retry_times = 0;
    uint16_t reg_read;
    uint16_t reg_write;

    /* Software Reset */
    reg_write = TMA525B_SWRESET_CMD;
    for (retry_times = 0; retry_times < TMA525B_RETRY_TIMES; retry_times++) {
        ret = tp_i2c_cmd_write(reg_write);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        osDelay(TMA525B_RESETHIGH_DELAY);
    }

    if (retry_times >= TMA525B_RETRY_TIMES) {
        tp_err("TOUCH:configuration sw reset fail! ret = 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    /* Read Firmware Version */
    reg_read = 0;
    ret = tp_i2c_data_read(TMA525B_FIRMWARE_VERSION, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    g_tma525b_drv.firmware_version = reg_read;

    /* Read Minor Firmware Version */
    reg_read = 0;
    ret = tp_i2c_data_read(TMA525B_MINOR_FW_VERSION, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    g_tma525b_drv.minor_firmware_version = reg_read;

    /* Read Hw ID */
    reg_read = 0;
    ret = tp_i2c_data_read(TMA525B_HW_ID, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("configuration error (read hw id) 0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    g_tma525b_drv.chip_hw_id = reg_read;

    /* Read Register Version */
    reg_read = 0;
    ret = tp_i2c_data_read(TMA525B_DATA_VERSION_REG, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    g_tma525b_drv.reg_version = reg_read;

    ret = tma525b_reg_config();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    /* Write Clear Int Cmd */
    tma525b_clear_interrupt(TMA525B_RETRY_TIMES, TMA525B_GENERAL_DELAY_1);

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_get_panel_info(void)
{
    ext_errno ret;
    uint16_t reg_read = 0;
    /* Read X Channel Num */
    ret = tp_i2c_data_read(TMA525B_TOTAL_NUMBER_OF_X, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:GetPanelInfo error read x channel 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    g_tma525b_drv.panel_info.xchannel_num = reg_read;
    /* Read Y Channel Num */
    reg_read = 0;
    ret = tp_i2c_data_read(TMA525B_TOTAL_NUMBER_OF_Y, (uint8_t *)&reg_read, TMA525B_DATA_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:GetPanelInfo error read y channel 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    g_tma525b_drv.panel_info.ychannel_num = reg_read;
    g_tma525b_drv.panel_info.total_dnd_num =
        g_tma525b_drv.panel_info.xchannel_num * g_tma525b_drv.panel_info.ychannel_num;
    g_tma525b_drv.panel_info.total_self_dnd_num =
        g_tma525b_drv.panel_info.xchannel_num + g_tma525b_drv.panel_info.ychannel_num;

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_config(void)
{
    ext_errno ret;

    /* Step 1 hardware reset */
    ret = tma525b_reset();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:Init reset fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    /* Step 2 power up */
    ret = tma525b_power_sequence();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:init powerup fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    /* Step 3 configuration */
    ret = tma525b_configuration();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:init config fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    /* Step 3 Panel Info */
    ret = tma525b_get_panel_info();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:init get panelinfo fail 0x%x", ret);
        return EXT_ERR_FAILURE;
    }

    /* Record Status For Init */
    g_tma525b_drv.chip_status = TMA525B_READY;
    g_tma525b_drv.work_status = TMA525B_WORK_NORMAL;

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_resource_deinit(void)
{
    g_tma525b_drv.work_status = TMA525B_WORK_INVALID;
    g_tma525b_drv.fw_upgrade_flag = TD_FALSE;
    osal_mutex_destroy(&g_tma525b_drv.ops_mux);

    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_deinit(void)
{
    ext_errno ret = tma525b_resource_deinit();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH: tma525b deinit fail! ret = 0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

#if CHIP_ASIC
static int32_t tma525b_touch_reset(tma525b_reset_type resettype)
{
    int32_t ret;
    uint8_t intnum;

    uapi_gpio_set_dir(TMA525B_RESET_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_HIGH);
    osDelay(TMA525B_RESET_HIGH_DELAY);
    switch (resettype) {
        case RESET_TO_BOOTLOADER:
            g_tmadrv.cmdstatus = TMA525B_CMD_RESET;
            intnum = TMA525B_RESET_BOOTLOADER_INT_NUM;
            break;
        case RESET_TO_APPLICATION:
            g_tmadrv.cmdstatus = TMA525B_CMD_RESET;
            intnum = TMA525B_RESET_APPLICATION_INT_NUM;
            break;
        case RESET_TO_DETECT:
            g_tmadrv.cmdstatus = TMA525B_CMD_DETECT;
            intnum = TMA525B_RESET_INIT_NUM;
            break;
    }
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_LOW);
    osDelay(TMA525B_RESET_LOW_DELAY);
    g_tmadrv.timeoutms = TMA525B_SEMAPHORE_WAIT_MAX_TIME_RESET;
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_HIGH);

    if (g_tmadrv.osready == true) {
        osDelay(TMA525B_RESETHIGH_DELAY);
    }

    for (uint8_t intcnt = 0; intcnt < intnum; intcnt++) {
        g_tmadrv.cmdstatus = TMA525B_CMD_RESET;
        g_tmadrv.hidresetcmdstate = 0;
        uint8_t dataread[TMA525B_INPUT_SIZE_LEN] = {0};

        ret = tp_i2c_data_read(0, dataread, TMA525B_INPUT_SIZE_LEN);
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("TOUCH:TPIC2Reset read err\r\n");
            return EXT_ERR_FAILURE;
        }
    }
    if (resettype == RESET_TO_DETECT) {
        osDelay(TMA525B_DETECT_DELAY);
    } else {
        osDelay(TMA525B_RESET_DELAY);
    }

    return EXT_ERR_SUCCESS;
}
#endif

static void tma525b_drv_data_init(void)
{
    g_tmadrv.hiddesc.outputregaddr = TMA525B_OUTPUT_REGISTER_ADDR;
    g_tmadrv.hiddesc.commandregaddr = TMA525B_COMMAND_REGISTER_ADDR;
    g_tmadrv.timeoutms = TMA525B_SEMAPHORE_WAIT_MAX_TIME_RESET;
    g_tmadrv.hidresetcmdstate = TMA525B_RESET_INIT_NUM;
    g_tmadrv.modetype = TMA525B_MODE_UNKNOWN;
    g_tmadrv.reservedvalue.reserved = 0;
}

static void tma525b_read_response(void)
{
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    if (tma525b_read_input() != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2_ReadResponse err\r\n");
    }
    if (memset_s(coredata->responsebuf, TMA525B_INPUT_SIZE, 0, TMA525B_INPUT_SIZE) != EOK) {
        tp_err("TOUCH:TPIC2_ReadResponse memset err\r\n");
    }
    if (memcpy_s(coredata->responsebuf, TMA525B_INPUT_SIZE, coredata->inputbuf, coredata->inputsize) != EOK) {
        tp_err("TOUCH:TPIC2_ReadResponse memcpy err\r\n");
    }
    coredata->responsesize = coredata->inputsize;
}


static int32_t tma525b_hid_get_descriptor(void)
{
    int32_t ret;
    uint8_t sendcmd[TMA525B_DATALEN_2] = { 0x01, 0x00 };

    g_tmadrv.cmdstatus = TMA525B_CMD_GET_HID_DESCRIPTOR;

    ret = tp_i2c_cmd_write(*(uint16_t *)sendcmd);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    osDelay(TMA525B_CMD_RESP_DELAY);
    tma525b_read_response();
    if (g_tmadrv.coredata.inputsize != sizeof(g_tmadrv.hiddesc)) {
        return EXT_ERR_FAILURE;
    }
    /* no need to handle return value here */
    (void)memcpy_s((uint8_t *)&(g_tmadrv.hiddesc), sizeof(g_tmadrv.hiddesc),
        g_tmadrv.coredata.responsebuf, g_tmadrv.coredata.responsesize);
    tp_print("TOUCH:TPIC2_Get_Mode: reportId 0x%x\r\n", g_tmadrv.hiddesc.reportid);

    switch (g_tmadrv.hiddesc.reportid) {
        case HID_BL_REPORT_ID:
            g_tmadrv.modetype = TMA525B_MODE_BOOTLOADER;
            break;
        case HID_APP_REPORT_ID:
            g_tmadrv.modetype = TMA525B_MODE_APPLICATION;
            break;
        default:
            g_tmadrv.modetype = TMA525B_MODE_UNKNOWN;
            return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static uint16_t tma525b_compute_crc(uint8_t *databuf, uint32_t buflen)
{
    uint16_t remainder = 0xFFFF;
    uint16_t xormask = 0x0000;
    uint32_t index;
    uint32_t bytevalue;
    uint32_t tableindex;
    uint32_t crcbitwidth = sizeof(uint16_t) * CRC_BIT_LEN; /* 16 */

    if (databuf == NULL) {
        return 0;
    }
    /* Divide the message by polynomial, via the table. */
    for (index = 0; index < buflen; index++) {
        bytevalue = databuf[index];
        tableindex = ((bytevalue >> CRC_SHIFT_4) & CRC_MASK) ^ (remainder >> (crcbitwidth - CRC_SHIFT_4));
        remainder = g_tmadrv.crctable[tableindex] ^ (remainder << CRC_SHIFT_4);
        tableindex = (bytevalue & CRC_MASK) ^ (remainder >> (crcbitwidth - CRC_SHIFT_4));
        remainder = g_tmadrv.crctable[tableindex] ^ (remainder << CRC_SHIFT_4);
    }

    /* Perform the final remainder CRC. */
    return remainder ^ xormask;
}

static int32_t tma525b_hid_send_output(tma525b_hidoutput *hidoutput)
{
    int32_t ret;
    uint8_t cmdtype;
    uint8_t *cmdbuf = NULL;
    uint32_t cmdlen = 0;
    uint8_t reportid = 0;
    uint32_t cmdoffset = 0;
    uint16_t cmdcrc = 0;
    errno_t memret;
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    if (hidoutput == NULL) {
        return EXT_ERR_FAILURE;
    }

    cmdtype = hidoutput->cmdtype;

    switch (cmdtype) {
        case HID_OUTPUT_CMD_APP:
            reportid = HID_APP_OUTPUT_REPORT_ID;
            /* 5 equals LEN(2) + REPORT_ID + RSVD + CMD + OUTPUT */
            cmdlen = TMA525B_APPLICATION_CMD_LEN;
            break;

        case HID_OUTPUT_CMD_BL:
            reportid = HID_BL_OUTPUT_REPORT_ID;
            /* 11 equals LEN(2) + REPORT_ID + RSVD + SOP + CMD + LEN(2) + OUTPUT + CRC(2) + EOP */
            cmdlen = TMA525B_BOOTLOADER_CMD_LEN; /* 5 + SOP + LEN(2) + CRC(2) + EOP */
            break;

        default:
            tp_err("TOUCH:SendOutput:err cmdtype=0x%x\r\n", hidoutput->cmdtype);
            return EXT_ERR_FAILURE;
    }

    cmdlen += hidoutput->datalen;

    /* 2: REG(2) */
    if ((cmdlen + TMA525B_OUTPUT_REG_ADDR_LEN) > TMA525B_OUTPUT_SIZE) {
        tp_err("TOUCH:TPIC2_HID_SendOutput: cmd too long 0x%x\r\n", cmdlen);
        return EXT_ERR_FAILURE;
    } else {
        cmdbuf = coredata->cmdbuf;
    }

    /* Set Output register */
    memret = memcpy_s(&cmdbuf[cmdoffset], TMA525B_OUTPUT_REG_ADDR_LEN,
                      (uint8_t *)&(g_tmadrv.hiddesc.outputregaddr), TMA525B_OUTPUT_REG_ADDR_LEN);
    if (memret != EOK) {
        tp_err("TOUCH:TPIC2_HID_SendOutput:mem cpy error\r\n");
        return EXT_ERR_FAILURE;
    }
    cmdoffset += TMA525B_OUTPUT_REG_ADDR_LEN;

    cmdbuf[cmdoffset++] = low_byte(cmdlen);
    cmdbuf[cmdoffset++] = high_byte(cmdlen);
    cmdbuf[cmdoffset++] = reportid;
    cmdbuf[cmdoffset++] = 0x0; /* reserved */

    if (cmdtype == HID_OUTPUT_CMD_BL) {
        cmdbuf[cmdoffset++] = HID_OUTPUT_BL_SOP;
    }

    cmdbuf[cmdoffset++] = hidoutput->cmdcode;

    /* Set Data Length for bootloader */
    if (cmdtype == HID_OUTPUT_CMD_BL) {
        cmdbuf[cmdoffset++] = low_byte(hidoutput->datalen);
        cmdbuf[cmdoffset++] = high_byte(hidoutput->datalen);
    }

    /* Set Data */
    if ((hidoutput->datalen != 0) && (hidoutput->databuf != 0)) {
        memret = memcpy_s(&cmdbuf[cmdoffset], hidoutput->datalen, hidoutput->databuf, hidoutput->datalen);
        if (memret != EOK) {
            tp_err("TOUCH:TPIC2_HID_SendOutput:mem cpy error\r\n");
            return EXT_ERR_FAILURE;
        }
        cmdoffset += hidoutput->datalen;
    }

    if (cmdtype  == HID_OUTPUT_CMD_BL) {
        /* 4: SOP + CMD + LEN(2) */
        cmdcrc = tma525b_compute_crc(&cmdbuf[OUT_OFFSET_SOP_6], hidoutput->datalen + 4);
        cmdbuf[cmdoffset++] = low_byte(cmdcrc);
        cmdbuf[cmdoffset++] = high_byte(cmdcrc);
        cmdbuf[cmdoffset++] = HID_OUTPUT_BL_EOP;
    }

    ret = tp_i2c_data_write(0, cmdbuf, cmdlen + TMA525B_OUTPUT_REG_ADDR_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2_HID_SendOutput: write cmd err 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static bool tma525b_judge_touch_report(void)
{
    tma525b_coredata *coredata = &(g_tmadrv.coredata);
    uint8_t reportid = coredata->responsebuf[TMA525B_REPORTID_SHIFT];
    /* touch and cmd packets size always >= 7 */
    if (coredata->inputsize < TMA525B_REPORT_HEAD_SIZE) {
        return false;
    }

    if ((reportid != HID_TOUCH_REPORT_ID) &&
        (reportid != HID_BTN_REPORT_ID) &&
        (reportid != HID_SENSOR_DATA_REPORT_ID) &&
        (reportid != HID_TRACKING_HEATMAP_REPOR_ID)) {
        return false;
    }

    return true;
}

static int32_t tma525b_parse_input(inputeventinfo *eventinfo);

static int32_t mc_tma525b_exception_touch_process(void)
{
    int32_t ret;
    inputeventinfo inputeventinfo;

    inputeventinfo.tpevent = TP_INVALID;

    ret = tma525b_parse_input(&inputeventinfo);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:MC_TPIC2_IrqCallback parse input err 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static bool tma525b_check_and_parse_touch(tma525b_cmd_status tmpcmdstatus)
{
    if (tma525b_judge_touch_report() == true) {
        g_tmadrv.cmdstatus = tmpcmdstatus;
        /* Pass Touch event to upper layer */
        if (g_tmadrv.cmdstatus != TMA525B_CMD_SET_WORK_MODE) {
            mc_tma525b_exception_touch_process();
        }
        return true;
    } else {
        return false;
    }
}

static int32_t tma525b_hid_output_validate_blresponse(const tma525b_hidoutput *hidoutput)
{
    uint32_t responsesize;
    uint16_t respcrc;
    uint8_t cmdstatus;
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    responsesize = coredata->responsesize;
    if ((hidoutput->resetexpect != 0) && (responsesize == 0)) {
        return EXT_ERR_SUCCESS;
    }

    if ((responsesize < HID_OUTPUT_BL_RESP_SIZE_MIN) || (responsesize > TMA525B_INPUT_SIZE)) {
        tp_err("TOUCH:ValidateBLResponse: wrong size:0x%x\r\n", responsesize);
        return EXT_ERR_FAILURE;
    }

    if (coredata->responsebuf[HID_OUTPUT_RESPONSE_REPORT_OFFSET] != HID_BL_RESPONSE_REPORT_ID) {
        tp_err("TOUCH:VBLResponse:output response, wrong report_id:0x%x\r\n",
            coredata->responsebuf[HID_OUTPUT_RESPONSE_REPORT_OFFSET]);
        return EXT_ERR_FAILURE;
    }

    /* 4 : SOP of response */
    if (coredata->responsebuf[RESP_OFFSET_SOP_4] != HID_OUTPUT_BL_SOP) {
        tp_err("TOUCH:VBLResponse: HID output response, wrong SOP:0x%x\r\n",
            coredata->responsebuf[RESP_OFFSET_SOP_4]);
        return EXT_ERR_FAILURE;
    }

    /* size -1 : EOP of response */
    if (coredata->responsebuf[responsesize - 1] != HID_OUTPUT_BL_EOP) {
        tp_err("TOUCH:VBLResponse: HID output response, wrong EOP:0x%x\r\n",
            coredata->responsebuf[responsesize - 1]);
        return EXT_ERR_FAILURE;
    }

    respcrc = tma525b_compute_crc(&coredata->responsebuf[RESP_OFFSET_SOP_4], responsesize - CRC_CALC_OFFSET);
    if (coredata->responsebuf[responsesize - CRC_LOWBYTE_OFFSET] != low_byte(respcrc) ||
        coredata->responsebuf[responsesize - CRC_HIGHBYTE_OFFSET] != high_byte(respcrc)) {
        tp_err("TOUCH:VBLResponse: HID output response, wrong CRC 0x0x%x\r\n", respcrc);
        return EXT_ERR_FAILURE;
    }

    cmdstatus = coredata->responsebuf[RESP_OFFSET_STATUS_5]; /* 5 : status of response */

    if (cmdstatus != CMD_STATUS_SUCCESS) {
        tp_err("TOUCH:VBLResponse: HID output response, ERROR:0x%x\r\n", cmdstatus);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_output_validate_appresponse(const tma525b_hidoutput *hidoutput)
{
    int32_t cmdcode;
    uint32_t responsesize;
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    responsesize = coredata->responsesize;
    if ((hidoutput->resetexpect != 0) && (responsesize == 0)) {
        return EXT_ERR_SUCCESS;
    }

    if (coredata->responsebuf[HID_OUTPUT_RESPONSE_REPORT_OFFSET] != HID_APP_RESPONSE_REPORT_ID) {
        tp_err("TOUCH:VAppResponse:output response, wrong report id:0x%x\r\n",
            coredata->responsebuf[HID_OUTPUT_RESPONSE_REPORT_OFFSET]);
        return EXT_ERR_FAILURE;
    }

    cmdcode = coredata->responsebuf[HID_OUTPUT_RESPONSE_CMD_OFFSET] & HID_OUTPUT_RESPONSE_CMD_MASK;

    if (cmdcode != hidoutput->cmdcode) {
        tp_err("TOUCH:VAppResponse:output response, wrong command code:0x%x\r\n",
            cmdcode);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_output_validate_response(tma525b_hidoutput *hidoutput)
{
    switch ((tma525b_output_type)hidoutput->cmdtype) {
        case HID_OUTPUT_CMD_BL:
            return tma525b_hid_output_validate_blresponse(hidoutput);
        case HID_OUTPUT_CMD_APP:
            return tma525b_hid_output_validate_appresponse(hidoutput);
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_send_output_and_wait(tma525b_hidoutput *hidoutput, tma525b_cmd_type cmdtype)
{
    unused(cmdtype);
    int32_t ret;
    int32_t retry = 0;
    tma525b_cmd_status tmpcmdstatus = g_tmadrv.cmdstatus;

    (void)tma525b_wait_mutex(g_tmadrv.systemlock, TMA525B_SYSTEM_MUTEX_WAIT_TIME);
    g_tmadrv.hidcmdstate = hidoutput->cmdcode + HID_OUTPUT_CMD_DIFF; /* +1: different from output cmd code */
    (void)tma525b_release_mutex(g_tmadrv.systemlock);

    if (hidoutput->timeoutms != 0) {
        g_tmadrv.timeoutms = hidoutput->timeoutms;
    } else {
        g_tmadrv.timeoutms = TMA525B_HID_OUTPUT_TIMEOUT;
    }

    ret = tma525b_hid_send_output(hidoutput);
    if (ret != EXT_ERR_SUCCESS) {
        ret = COMMAND_SEND_FAIL;
        tp_err("TOUCH:TPIC2_HID_SendOutputAndWait: send err 0x%x\r\n", ret);
        return ret;
    }

    if (g_tmadrv.tpmsgstatus == true) {
        tma525b_read_response();
        ret = tma525b_check_and_parse_touch(tmpcmdstatus);
        if (ret == false) {
            tp_err("TOUCH:TPIC2_CurWrongTp Message parse err 0x%x\r\n", ret);
        }
    }
    /* in order to deal touch parse */
    do {
        ret = tma525b_wait_event_timeout();
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("TOUCH:HID_SendOutputAndWait: HID output cmd timed out\r\n");
            return ret;
        }
        tma525b_read_response();
        /* read response cmd info sync */
        ret = tma525b_check_and_parse_touch(tmpcmdstatus);
        if (ret == false) {
            break;
        }
    /* retrytimes set 3 times */
    } while ((retry++) < 3);

    if (hidoutput->novalid == 0) {
        ret = tma525b_hid_output_validate_response(hidoutput);
        if (ret != EXT_ERR_SUCCESS) {
            tp_err("TOUCH:HID_SendOutputAndWait: validate err 0x%x\r\n", ret);
            ret = RESPONSE_VALIDATE_FAIL;
        }
    }

    (void)tma525b_wait_mutex(g_tmadrv.systemlock, TMA525B_SYSTEM_MUTEX_WAIT_TIME);
    g_tmadrv.hidcmdstate = 0;
    (void)tma525b_release_mutex(g_tmadrv.systemlock);

    return ret;
}

static int32_t tma525b_hid_bl_get_panelid(void)
{
    int32_t ret;
    /* read offset is 0xe600,readlen is 0x10,dufferSize is 3 */
    uint8_t buffer[3] = {0x00, 0xe6, 0x10};
    tma525b_hidoutput hidoutput = {
        hid_output_bl_command(HID_OUTPUT_BL_READ_IMAGE),
        .timeoutms = TMA525B_HID_OUTPUT_BL_READ_IMAGE_TIMEOUT,
        /* panelid datalen is 3 */
        .datalen = array_size(buffer),
    };
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    hidoutput.databuf = buffer;

    g_tmadrv.cmdstatus = TMA525B_CMD_FW_UPGRADE;
    ret = tma525b_hid_send_output_and_wait(&hidoutput, TMA525B_SYNC_CMD);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    g_tmadrv.sysinfo.panelid = coredata->responsebuf[TMA525B_BL_PANEL_ID_OFFSET];

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_output_get_sysinfo(void)
{
    int32_t ret;
    tma525b_hidoutput hidoutput = {
        hid_output_app_command(HID_OUTPUT_GET_SYSINFO),
        .timeoutms = TMA525B_HID_OUTPUT_GET_SYSINFO_TIMEOUT,
        .datalen = 0,
    };
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    g_tmadrv.cmdstatus = TMA525B_CMD_GET_SYSINFO;
    ret = tma525b_hid_send_output_and_wait(&hidoutput, TMA525B_SYNC_CMD);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }

    g_tmadrv.sysinfo.pipmajorversion = coredata->responsebuf[TMA525B_PIP_MAJOR_VER_OFFSET];
    g_tmadrv.sysinfo.pipminorversion = coredata->responsebuf[TMA525B_PIP_MINOR_VER_OFFSET];
    g_tmadrv.sysinfo.fwmajorversion = coredata->responsebuf[TMA525B_FW_MAJOR_VER_OFFSET];
    g_tmadrv.sysinfo.fwminorversion = coredata->responsebuf[TMA525B_FW_MINOR_VER_OFFSET];
    g_tmadrv.sysinfo.revctrlnumber = ((uint32_t)coredata->responsebuf[T525_REVCTRL_H1_OFFSET] << TMA525B_SHIFT_24_BIT) |
                                     ((uint32_t)coredata->responsebuf[T525_REVCTRL_H0_OFFSET] << TMA525B_SHIFT_16_BIT) |
                                     ((uint32_t)coredata->responsebuf[T525_REVCTRL_L1_OFFSET] << TMA525B_SHIFT_8_BIT) |
                                     coredata->responsebuf[T525_REVCTRL_L0_OFFSET];
    g_tmadrv.sysinfo.configversion =
        ((uint16_t)coredata->responsebuf[TMA525B_CFG_VER_H_OFFSET] << TMA525B_SHIFT_8_BIT) |
        (uint16_t)coredata->responsebuf[TMA525B_CFG_VER_L_OFFSET];
    g_tmadrv.sysinfo.blmajorversion = coredata->responsebuf[TMA525B_BL_MAJOR_VER_OFFSET];
    g_tmadrv.sysinfo.blminorversion = coredata->responsebuf[TMA525B_BL_MINOR_VER_OFFSET];
    g_tmadrv.sysinfo.siliconid = ((uint16_t)coredata->responsebuf[TMA525B_SILICON_H_OFFSET] << TMA525B_SHIFT_8_BIT) |
                                 (uint16_t)coredata->responsebuf[TMA525B_SILICON_L_OFFSET];
    g_tmadrv.sysinfo.revisionid = coredata->responsebuf[TMA525B_REVISION_ID_OFFSET];
    g_tmadrv.sysinfo.panelid = coredata->responsebuf[TMA525B_PANEL_ID_OFFSET];

    return EXT_ERR_SUCCESS;
}


ext_errno tma525b_get_power_mode(uint32_t *power_mode)
{
    if (power_mode == NULL) {
        return EXT_ERR_FAILURE;
    }

    *power_mode = (uint32_t)(g_tma525b_drv.work_status);

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_output_enter_easywake(void)
{
    int32_t ret;
    uint8_t writebuf[TMA525B_DATALEN_1] = { 0 };
    uint8_t status;
    tma525b_hidoutput hidoutput = {
        hid_output_app_command(HID_OUTPUT_ENTER_EASYWAKE_STATE),
        .databuf = writebuf,
        .datalen = TMA525B_DATALEN_1,
    };
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    g_tmadrv.cmdstatus = TMA525B_CMD_SET_WORK_MODE;
    ret = tma525b_hid_send_output_and_wait(&hidoutput, TMA525B_SYNC_CMD);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:EnterEasyWake: send cmd fail %d\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    status = coredata->responsebuf[RESP_OFFSET_STATUS_5];
    /* 1 success, easywake state is entered */
    if (status != TMA525B_POWERMODE_EASYWAKE_STATUS) {
        tp_err("TOUCH:EnterEasyWake: status %d\r\n", status);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_exec_cmd(tma525b_hidcmd *hidcmd)
{
    int32_t ret;
    errno_t memret;
    uint8_t cmdbuf[TMA525B_OUTPUT_SIZE] = { 0 };
    uint8_t cmdlength;
    uint8_t cmdoffset = 0;

    if (hidcmd == NULL) {
        return EXT_ERR_FAILURE;
    }

    cmdlength = TMA525B_CMD_REG_LEN +
                TMA525B_CMD_LEN +
                ((hidcmd->reportid >= TMA525B_HID_CMD_OUT_F) ? TMA525B_REPORTID_LEN : 0) +
                (hidcmd->hasdatareg ? TMA525B_DATA_REG_LEN : 0) +
                hidcmd->writelen;

    if (cmdlength > TMA525B_OUTPUT_SIZE) {
        tp_err("TOUCH:ExecCmd: cmd len too large %d\r\n", cmdlength);
        return EXT_ERR_FAILURE;
    }

    /* Set Command register */
    memret = memcpy_s(&cmdbuf[cmdoffset], sizeof(g_tmadrv.hiddesc.commandregaddr),
                      &(g_tmadrv.hiddesc.commandregaddr), sizeof(g_tmadrv.hiddesc.commandregaddr));
    if (memret != EOK) {
        tp_err("TPIC2_HID_ExecCmd memcpy_s err %d\r\n", memret);
    }
    cmdoffset += sizeof(g_tmadrv.hiddesc.commandregaddr);

    /* Set Command */
    set_cmd_report_type(cmdbuf[cmdoffset], hidcmd->reporttype);

    if (hidcmd->reportid >= TMA525B_HID_CMD_OUT_F) {
        set_cmd_report_id(cmdbuf[cmdoffset], TMA525B_HID_CMD_OUT_F);
    } else {
        set_cmd_report_id(cmdbuf[cmdoffset], hidcmd->reportid);
    }

    cmdoffset++;
    set_cmd_opcode(cmdbuf[cmdoffset], hidcmd->opcode);

    cmdoffset++;
    if (hidcmd->reportid >= TMA525B_HID_CMD_OUT_F) {
        cmdbuf[cmdoffset] = hidcmd->reportid;
        cmdoffset++;
    }

    /* Set Data register */
    if (hidcmd->hasdatareg) {
        memret = memcpy_s(&cmdbuf[cmdoffset], sizeof(g_tmadrv.hiddesc.dataregaddr),
                          &(g_tmadrv.hiddesc.dataregaddr), sizeof(g_tmadrv.hiddesc.dataregaddr));
        if (memret != EOK) {
            tp_err("TOUCH:TPIC2_HID_ExecCmd:mem cpy error\r\n");
            return EXT_ERR_FAILURE;
        }
        cmdoffset += sizeof(g_tmadrv.hiddesc.dataregaddr);
    }

    /* Set Data */
    if (hidcmd->writelen && hidcmd->writebuf) {
        memret = memcpy_s(&cmdbuf[cmdoffset], hidcmd->writelen, hidcmd->writebuf, hidcmd->writelen);
        if (memret != EOK) {
            tp_err("TOUCH:TPIC2_HID_ExecCmd:mem cpy error\r\n");
            return EXT_ERR_FAILURE;
        }
        cmdoffset += hidcmd->writelen;
    }

    ret = tp_i2c_data_write(0, cmdbuf, cmdlength);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:ExecCmd: cmd write fail %d %u\r\n", ret, cmdoffset);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_hid_exec_cmd_and_wait(tma525b_hidcmd *hidcmd)
{
    int32_t ret;
    uint8_t *cmdstate = NULL;
    int32_t retry = 0;
    tma525b_cmd_status tmpcmdstatus = g_tmadrv.cmdstatus;

    if (hidcmd->resetcmd) {
        cmdstate = &(g_tmadrv.hidresetcmdstate);
    } else {
        cmdstate = &(g_tmadrv.hidcmdstate);
    }

    (void)tma525b_wait_mutex(g_tmadrv.operlock, TMA525B_SYSTEM_MUTEX_WAIT_TIME);
    *cmdstate = HID_OUTPUT_CMD_DIFF;
    (void)tma525b_release_mutex(g_tmadrv.operlock);

    ret = tma525b_hid_exec_cmd(hidcmd);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:ExecCmdAndWait: HID exec cmd err\r\n");
        goto error;
    }

    g_tmadrv.timeoutms = (hidcmd->timeoutms == 0) ? TMA525B_HID_RESET_TIMEOUT : hidcmd->timeoutms;

    do {
        ret = tma525b_wait_event_timeout();
        if (ret != EXT_ERR_SUCCESS) {
            tp_print("TOUCH:ExecCmdAndWait:cmd timed out %d\r\n", hidcmd->timeoutms);
        }
        tma525b_read_response();

        ret = tma525b_check_and_parse_touch(tmpcmdstatus);
        if (ret == false) {
            break;
        }
    /* retrytimes set 3 times,deal with two touch packet case */
    } while ((retry++) < 3);

    return EXT_ERR_SUCCESS;

error:
    (void)tma525b_wait_mutex(g_tmadrv.operlock, TMA525B_SYSTEM_MUTEX_WAIT_TIME);
    *cmdstate = 0;
    (void)tma525b_release_mutex(g_tmadrv.operlock);

    return ret;
}

static int32_t tma525b_hid_set_powermode(uint8_t powermode)
{
    int32_t ret;
    tma525b_hidcmd hidcmd = {
        .opcode = HID_CMD_SET_POWER,
        .reporttype = 0,
        .waitinterrupt = 1,
        .timeoutms = TMA525B_HID_SET_POWER_TIMEOUT,
        .writelen = 0,
    };
    tma525b_coredata *coredata = &(g_tmadrv.coredata);

    hidcmd.powerstate = powermode;
    g_tmadrv.cmdstatus = TMA525B_CMD_SET_WORK_MODE;
    ret = tma525b_hid_exec_cmd_and_wait(&hidcmd);
    if (ret != EXT_ERR_SUCCESS) {
        return EXT_ERR_FAILURE;
    }
    /* validate */
    uint8_t buflen1 = coredata->responsebuf[TMA525B_PM_LENGTH1_OFFSET];
    uint8_t buflen2 = coredata->responsebuf[TMA525B_PM_LENGTH2_OFFSET];
    uint8_t bufreportid = coredata->responsebuf[TMA525B_PM_REPORTID_OFFSET];
    uint8_t bufpowermode = coredata->responsebuf[TMA525B_PM_POWERSTATE_OFFSET];
    uint8_t bufopcode = coredata->responsebuf[TMA525B_PM_OPCODE_OFFSET];
    if ((bufreportid != HID_RESPONSE_REPORT_ID) || ((bufpowermode & TMA525B_POWERMODE_POWERSTATE_MASK) != powermode) ||
        ((bufopcode & TMA525B_POWERMODE_OPCODE_MASK) != HID_CMD_SET_POWER)) {
        tp_print("TOUCH:Setpowermode failed, state %u %u %u %u %u\r\n",
                 buflen1, buflen2, bufreportid, bufpowermode, bufopcode);
    }

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_set_normal_mode(void)
{
    int32_t ret;

    if (g_tmadrv.powermode == TMA525B_POWER_SLEEP) {
        tma525b_reset();
    } else {
        ret = tma525b_hid_set_powermode(TMA525B_POWERMODE_NORMAL);
        if (ret != EXT_ERR_SUCCESS) {
            tp_print("TOUCH:TPIC2_SetPowerNormal fail\r\n");
            return EXT_ERR_FAILURE;
        }
    }
    g_tmadrv.powermode = TMA525B_POWER_NORMAL;

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_set_sleep_mode(void)
{
    int32_t ret;

    ret = tma525b_hid_set_powermode(TMA525B_POWERMODE_SLEEP);
    if (ret != EXT_ERR_SUCCESS) {
        tp_print("TOUCH:TPIC2_SetPowerSleep fail\r\n");
    }
    g_tmadrv.powermode = TMA525B_POWER_SLEEP;
#ifdef TMA525B_GESTURE_WAKEUP
    g_tma525b_drv.gesture_wakeup = TD_FALSE;
#endif
    g_tma525b_drv.work_status = TMA525B_WORK_SLEEP;

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_set_power_easywake(void)
{
    int32_t ret;

    if (g_tmadrv.powermode == TMA525B_POWER_EASYWAKE) {
        return EXT_ERR_SUCCESS;
    }
    /* change status before irq */
    if (g_tmadrv.powermode == TMA525B_POWER_SLEEP) {
        tma525b_reset();
    }
    ret = tma525b_hid_output_enter_easywake();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2_SetPowerEasyWake fail\r\n");
        return EXT_ERR_FAILURE;
    }
    g_tmadrv.powermode = TMA525B_POWER_EASYWAKE;
#ifdef TMA525B_GESTURE_WAKEUP
    g_tma525b_drv.gesture_wakeup = TD_TRUE;
#endif
    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_set_power_mode(tp_work_mode power_mode)
{
    ext_errno ret = EXT_ERR_SUCCESS;

    switch (power_mode) {
        case MC_TP_SLEEP_WORK_MODE:
        case MC_TP_SUSPEND_SCAN:
            ret = tma525b_set_sleep_mode();
            break;
        case MC_TP_NORMAL_WORK_MODE:
        case MC_TP_RESUME_SCAN:
            ret = tma525b_set_normal_mode();
            break;
        case MC_TP_STANDBY_WORK_MODE:
            ret = tma525b_set_power_easywake();
            break;
        default:
            tp_err("TOUCH:tma525b not support powermode 0x%x", power_mode);
            ret = EXT_ERR_FAILURE;
            break;
    }

    return ret;
}

static ext_errno tma525b_standby(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint8_t retry_times;

    if ((g_tma525b_drv.work_status == TMA525B_WORK_STANDBY) || (g_tma525b_drv.work_status == TMA525B_WORK_SLEEP)) {
        tp_print("alread standby or sleep");
        return EXT_ERR_SUCCESS;
    }
    tma525b_set_power_easywake();
    g_tma525b_drv.work_status = TMA525B_WORK_STANDBY;
    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_suspend(void)
{
    ext_errno ret;
    uint16_t reg_write;
    uint8_t retry_times = 0;

    if (g_tma525b_drv.work_status == TMA525B_WORK_SLEEP) {
        tp_print("TOUCH:alread suspend");
        return EXT_ERR_SUCCESS;
    }

    tma525b_set_sleep_mode();
    g_tma525b_drv.work_status = TMA525B_WORK_SLEEP;

    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_resume(void)
{
    ext_errno ret;
    uint8_t retry_times = 0;

    g_tma525b_drv.work_status = TMA525B_WORK_RESUME;

    tma525b_set_normal_mode();

    g_tma525b_drv.work_status = TMA525B_WORK_NORMAL;
#ifdef TMA525B_GESTURE_WAKEUP
    g_tma525b_drv.gesture_wakeup = TD_FALSE;
#endif

    return EXT_ERR_SUCCESS;
}

static int32_t tma525b_touch_init(void)
{
    /* create tp operation semaphore, count is 0 */
    if ((g_tmadrv.osready == true) && g_tmadrv.intsem == NULL) {
        g_tmadrv.intsem = osSemaphoreNew(g_intsem.maxcount, g_intsem.initialcount, &(g_intsem.attr));

        if (g_tmadrv.intsem == NULL) {
            tp_err("TOUCH:TPIC2 init create int semaph err\r\n");
            return ERROR_INIT_1;
        }
    }

    /* create tp operation mutex */
    if ((g_tmadrv.osready == true) && (g_tmadrv.systemlock == NULL)) {
        g_tmadrv.systemlock = osMutexNew((osMutexAttr_t *)NULL);

        if (g_tmadrv.systemlock == NULL) {
            tp_err("TOUCH:TPIC2 init create system lock err\r\n");
            return ERROR_INIT_2;
        }
    }

    /* Init DrvData */
    tma525b_drv_data_init();
    
    int32_t ret = tma525b_touch_reset(RESET_TO_APPLICATION);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2 init reset err\r\n");
    }
    /* Get HID Desc */
    ret = tma525b_hid_get_descriptor();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:TPIC2 init get hid desc err\r\n");
        return ERROR_INIT_GET_HID_DESC;
    }
    g_tmadrv.powermode = TMA525B_POWER_NORMAL;

    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_init(void)
{
    int32_t ret;

    /* create tp operation mutex */
    if ((g_tmadrv.osready == true) && (g_tmadrv.operlock == NULL)) {
        g_tmadrv.operlock = osMutexNew((osMutexAttr_t *)NULL);

        if (g_tmadrv.operlock == NULL) {
            tp_err("TOUCH:Mc TPIC2 Init: Create oper lock err\r\n");
            return EXT_ERR_FAILURE;
        }
    }

    /* get tp operater mutex */
    ret = tma525b_wait_mutex(g_tmadrv.operlock, TMA525B_OPERATER_MUTEX_WAIT_TIME);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:Mc TPIC2 Init: wait oper lock err 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    /* tp hardware reset and init */
    ret = tma525b_touch_init();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:Mc TPIC2 Init: err ret 0x%x\r\n", ret);
        /* No Need Return For Check Upgrade */
    }

    /* release tp operater mutex */
    ret = tma525b_release_mutex(g_tmadrv.operlock);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TOUCH:Mc TPIC2 Init: wait oper lock err 0x%x\r\n", ret);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
