/*
 * Copyright (c) @CompanyNameMagicTag 2020-2023. All rights reserved.
 * Description:  HAL LOW FRQ CLOCK DRIVER.
 * Author: @CompanyNameTag
 * Create: 2020-03-17
 */

#include "hal_32k_clock.h"
#include "platform_core.h"
#include "chip_io.h"
#include "tcxo.h"

#define HAL_CALIBRATION_32K_CLOCK_DET_CFG            (PMU2_CMU_CTL_RB_BASE + 0x480)
#define HAL_CALIBRATION_32K_CLOCK_ENABLE             0

#define HAL_CALIBRATION_32K_CLOCK_DET_VAL            (PMU2_CMU_CTL_RB_BASE + 0x484)

#define HAL_CALIBRATION_32K_CLOCK_DET_STS            (PMU2_CMU_CTL_RB_BASE + 0x488)
#define HAL_CALIBRATION_32K_CLOCK_DET_DOING          1
#define HAL_CALIBRATION_32K_CLOCK_DET_DONE           0

#define HAL_CALIBRATION_32K_CLOCK_DET_RES_L          (PMU2_CMU_CTL_RB_BASE + 0x48C)
#define HAL_CALIBRATION_32K_CLOCK_DET_RES_H          (PMU2_CMU_CTL_RB_BASE + 0x490)

#define CALIBRATION_CLOCK_SHIFT                 16
#define CALIBRATION_32K_CLOCK_DELAY             100ULL

void hal_32k_clock_calibration_enable(void)
{
    // If calibration is currently in progress, return
    if (reg16_getbit(HAL_CALIBRATION_32K_CLOCK_DET_STS, HAL_CALIBRATION_32K_CLOCK_DET_DOING)) {
        return;
    }

    reg16_clrbit(HAL_CALIBRATION_32K_CLOCK_DET_CFG, HAL_CALIBRATION_32K_CLOCK_ENABLE);
    uapi_tcxo_delay_us(CALIBRATION_32K_CLOCK_DELAY);
    reg16_setbit(HAL_CALIBRATION_32K_CLOCK_DET_CFG, HAL_CALIBRATION_32K_CLOCK_ENABLE);
    uapi_tcxo_delay_us(CALIBRATION_32K_CLOCK_DELAY);
}

void hal_32k_clock_calibration_disable(void)
{
    reg16_clrbit(HAL_CALIBRATION_32K_CLOCK_DET_CFG, (uint16_t)HAL_CALIBRATION_32K_CLOCK_ENABLE);
}

void hal_32k_clock_calibration_cycle_config(uint16_t det_val)
{
    reg16_write(HAL_CALIBRATION_32K_CLOCK_DET_VAL, det_val);
}

static bool hal_32k_clock_calibration_get_done_status(void)
{
    return (bool)reg16_getbit(HAL_CALIBRATION_32K_CLOCK_DET_STS, HAL_CALIBRATION_32K_CLOCK_DET_DONE);
}

uint32_t hal_read_32k_clock_detect_result_reg(void)
{
    uint32_t result_l, result_h;
    result_l = readw(HAL_CALIBRATION_32K_CLOCK_DET_RES_L);
    result_h = readw(HAL_CALIBRATION_32K_CLOCK_DET_RES_H);
    return (uint32_t)(result_l | (result_h << CALIBRATION_CLOCK_SHIFT));
}

uint32_t hal_32k_clock_get_detect_result(void)
{
    hal_32k_clock_calibration_enable();
    while (!hal_32k_clock_calibration_get_done_status()) {}
    return hal_read_32k_clock_detect_result_reg();
}
