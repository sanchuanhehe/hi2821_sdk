/*
 * Copyright (c) @CompanyNameMagicTag 2020-2022. All rights reserved.
 * Description: CLOCK CALIBRATION DRIVER.
 * Author: @CompanyNameTag
 * Create: 2020-03-17
 */

#include "clock_calibration.h"
#include "platform_core.h"
#include "hal_32k_clock.h"
#include "otp_map.h"
#include "efuse.h"
#if CORE != WIFI
#include "otp.h"
#endif
#include "hal_tcxo.h"
#include "chip_io.h"
#include "non_os.h"
#include "tcxo.h"
#include "tcxo_porting.h"
#include "uart.h"
#include "debug_print.h"

#define CALIBRATION_XO_CORE_TRIM_REG XO_CORE_TRIM_REG
#define CALIBRATION_XO_CORE_CTRIM_REG XO_CORE_CTRIM_REG

#define CALIBRATION_XO_CORE_TRIM_DEFAULT 1

#define CALIBRATION_XO_CORE_CTRIM_BIT 0
#define CALIBRATION_XO_CORE_CTRIM_LEN 8
#define CALIBRATION_XO_CORE_CTRIM_MAX 0xFF
#define CALIBRATION_XO_CORE_CTRIM_MIN 0x0
#define CALIBRATION_XO_CORE_CTRIM_POS 0xFF

#define CALIBRATION_CLOCK_MUL 100
#define CALIBRATION_CLOCK_FREQ_32K_100 3276800

#define XO_CTRIM_CONFIG_WAIT_US 20ULL
#define XO_CTRIM_ADJUST_INTERVAL_VALUE 8
#define XO_CTRIM_LOW_BIT_ADJUST_MASK 7
#define XO_CTRIM_VALUE_DEFAULT 0x0

static uint32_t g_clock_32k = CALIBRATION_CLOCK_FREQ_32K_100;

uint32_t calibration_get_clock_frq(uint16_t cali_cycle)
{
    uint32_t result;
    hal_32k_clock_calibration_cycle_config(cali_cycle);
    result = hal_32k_clock_get_detect_result();
    if (result != 0) {
        g_clock_32k = (uint32_t)(((uint64_t)(cali_cycle)*HAL_TCXO_TICKS_PER_SECOND * CALIBRATION_CLOCK_MUL) / result);
    }

    return g_clock_32k; // return 32K clock
}

uint32_t calibration_get_clock_frq_result(void)
{
    return g_clock_32k;
}

#ifdef XO_32M_CALI
void calibration_xo_core_trim_init(void)
{
    // need use trim value from otp
    // the xo_trim and xo_ctrim should update at same time like this:
    // xo_trim:    0x1  0x03  ...  0x0F
    // xo_ctrim:   0x53 0x56  ...  0x76
    writew(CALIBRATION_XO_CORE_TRIM_REG, CALIBRATION_XO_CORE_TRIM_DEFAULT);
}

void calibration_xo_core_ctrim_init(void)
{
    uint8_t xo_ctrim_value = 0;
    calibration_read_xo_core_ctrim(&xo_ctrim_value);

    if (xo_ctrim_value == 0) {
        // use default value if NO otp config
        xo_ctrim_value = XO_CTRIM_VALUE_DEFAULT;
    }
    calibration_set_xo_core_ctrim(xo_ctrim_value);
}

void calibration_set_xo_core_ctrim(uint8_t xo_ctrim_value)
{
    reg16_setbits(ULP_AON_CTL_RB_ADDR + 0x1004, 0xF, 0x1, 0x1);
    reg8_setbits(CALIBRATION_XO_CORE_CTRIM_REG, CALIBRATION_XO_CORE_CTRIM_BIT, CALIBRATION_XO_CORE_CTRIM_LEN,
        xo_ctrim_value);
}

void calibration_save_xo_core_ctrim(uint8_t xo_ctrim_value)
{
    uint8_t xo_ctrim_value_rd = 0;
    errcode_t read_ret = uapi_efuse_read_buffer(&xo_ctrim_value_rd, XO_CORE_CTRIM, 0x1);
    if (read_ret != ERRCODE_SUCC) {
        PRINT("[save]read efuse fail! ret:%d\r\n", read_ret);
    }
    if (xo_ctrim_value_rd == 0) {
        errcode_t write_ret = uapi_efuse_write_buffer(XO_CORE_CTRIM, &xo_ctrim_value, 0x1);
        if (write_ret != ERRCODE_SUCC) {
            PRINT("write efuse fail! ret:%d\r\n", write_ret);
        }
    }
}

void calibration_read_xo_core_ctrim(uint8_t *xo_ctrim_value)
{
    if (xo_ctrim_value == NULL) {
        return;
    }
    uint8_t xo_ctrim_value_rd = 0;
    errcode_t read_ret = uapi_efuse_read_buffer(&xo_ctrim_value_rd, XO_CORE_CTRIM, 0x1);
    if (read_ret != ERRCODE_SUCC) {
        PRINT("read efuse fail! ret:%d\r\n", read_ret);
    }
    *xo_ctrim_value = xo_ctrim_value_rd & CALIBRATION_XO_CORE_CTRIM_POS;
}

void calibration_xo_core_ctrim_algorithm(bool increase, uint8_t step_num)
{
    reg16_setbits(ULP_AON_CTL_RB_ADDR + 0x1004, 0xF, 0x1, 0x1);
    uint8_t xo_ctrim_value =
        reg8_getbits(CALIBRATION_XO_CORE_CTRIM_REG, CALIBRATION_XO_CORE_CTRIM_BIT, CALIBRATION_XO_CORE_CTRIM_LEN);
    if (increase) {
        if ((xo_ctrim_value + step_num) > CALIBRATION_XO_CORE_CTRIM_MAX) {
            xo_ctrim_value = CALIBRATION_XO_CORE_CTRIM_MAX;
        } else {
            xo_ctrim_value = xo_ctrim_value + step_num;
        }
    } else {
        if (xo_ctrim_value < step_num) {
            xo_ctrim_value = CALIBRATION_XO_CORE_CTRIM_MIN;
        } else {
            xo_ctrim_value = xo_ctrim_value - step_num;
        }
    }
    reg8_setbits(CALIBRATION_XO_CORE_CTRIM_REG, CALIBRATION_XO_CORE_CTRIM_BIT, CALIBRATION_XO_CORE_CTRIM_LEN,
        xo_ctrim_value);
}

void calibration_get_xo_core_ctrim_reg(uint8_t *xo_ctrim_value)
{
    if (xo_ctrim_value == NULL) {
        return;
    }
    reg16_setbits(ULP_AON_CTL_RB_ADDR + 0x1004, 0xF, 0x1, 0x1);
    *xo_ctrim_value =
        reg8_getbits(CALIBRATION_XO_CORE_CTRIM_REG, CALIBRATION_XO_CORE_CTRIM_BIT, CALIBRATION_XO_CORE_CTRIM_LEN);
}

void calibration_set_xo_core_trim_reg(uint8_t xo_trim)
{
    non_os_enter_critical();
    writew(CALIBRATION_XO_CORE_TRIM_REG, xo_trim);
    non_os_exit_critical();
}
#endif