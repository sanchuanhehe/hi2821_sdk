/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides adc port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "td_base.h"
#include "chip_core_irq.h"
#include "soc_osal.h"
#include "pinctrl_porting.h"
#include "hal_adc_v153.h"
#include "chip_io.h"
#include "osal_interrupt.h"
#include "pm_clock.h"
#include "adc_porting.h"

#define ADC_BASE_ADDR 0x57036000
#define ADC_ANA_OFFSET 0x3D0
#define ADC_PMU_BASE_ADDR 0x57008700
#define ADC_AON_OFFSET 0x220
#define MCU_DIAG_CTL_BASE_ADDR 0x52004000
#define ADC_DIAG_OFFSET 0x100

uintptr_t g_adc_base_addr = (uintptr_t)ADC_BASE_ADDR;
uintptr_t g_adc_ana_base_addr = (uintptr_t)(ADC_BASE_ADDR + ADC_ANA_OFFSET);
uintptr_t g_adc_pmu_base_addr = (uintptr_t)ADC_PMU_BASE_ADDR;
uintptr_t g_adc_aon_cfg_addr = (uintptr_t)(ULP_AON_CTL_RB_ADDR + ADC_AON_OFFSET);
uintptr_t g_adc_diag_cfg_addr0 = (uintptr_t)(MCU_DIAG_CTL_BASE_ADDR + ADC_DIAG_OFFSET);
uintptr_t g_adc_diag_cfg_addr1 = (uintptr_t)(MCU_DIAG_CTL_BASE_ADDR + ADC_DIAG_OFFSET + ADC_DIAG_OFFSET);
afe_scan_mode_t g_adc_working_afe = AFE_SCAN_MODE_MAX_NUM;
static adc_irq_t g_adc_irq[AFE_SCAN_MODE_MAX_NUM] = { {GADC_DONE_IRQN, GADC_ALARM_IRQN},
                                                      {HADC_DONE_IRQN, HADC_ALARM_IRQN} };
static uint32_t g_irq;
static bool g_afe_cali_flag[AFE_SCAN_MODE_MAX_NUM] = {false, false};

uintptr_t adc_porting_base_addr_get(void)
{
    return g_adc_base_addr;
}

uintptr_t adc_porting_ana_base_addr_get(void)
{
    return g_adc_ana_base_addr;
}

uintptr_t adc_pmu_base_addr_get(void)
{
    return g_adc_pmu_base_addr;
}

uintptr_t adc_aon_cfg_addr_get(void)
{
    return g_adc_aon_cfg_addr;
}

uintptr_t adc_diag_cfg_addr0_get(void)
{
    return g_adc_diag_cfg_addr0;
}

uintptr_t adc_diag_cfg_addr1_get(void)
{
    return g_adc_diag_cfg_addr1;
}

void adc_port_register_hal_funcs(void)
{
    hal_adc_register_funcs(hal_adc_v153_funcs_get());
}

void adc_port_unregister_hal_funcs(void)
{
    hal_adc_unregister_funcs();
}

void adc_port_init_clock(adc_clock_t clock)
{
    uapi_unused(clock);
}

void adc_port_clock_enable(bool on)
{
    if (on) {
        uapi_clock_control(CLOCK_CONTROL_XO_OUT_ENABLE, CLOCK_XO2AFE);
    } else {
        uapi_clock_control(CLOCK_CONTROL_XO_OUT_DISABLE, CLOCK_XO2AFE);
    }
}

static void irq_adc_done_handler(void)
{
    hal_adc_done_irq_handler(g_adc_working_afe);
}

static void irq_adc_alarm_handler(void)
{
    hal_adc_alarm_irq_handler(g_adc_working_afe);
}

void adc_port_register_irq(afe_scan_mode_t afe_scan_mode)
{
    if (afe_scan_mode >= AFE_SCAN_MODE_MAX_NUM) {
        return;
    }
    osal_irq_request(g_adc_irq[afe_scan_mode].done_irqn, (osal_irq_handler)irq_adc_done_handler, NULL, NULL, NULL);
    osal_irq_request(g_adc_irq[afe_scan_mode].alarm_irqn, (osal_irq_handler)irq_adc_alarm_handler, NULL, NULL, NULL);
    osal_irq_enable(g_adc_irq[afe_scan_mode].done_irqn);
    osal_irq_enable(g_adc_irq[afe_scan_mode].alarm_irqn);
    g_adc_working_afe = afe_scan_mode;
}

void adc_port_unregister_irq(afe_scan_mode_t afe_scan_mode)
{
    if (afe_scan_mode >= AFE_SCAN_MODE_MAX_NUM) {
        return;
    }
    osal_irq_disable(g_adc_irq[afe_scan_mode].done_irqn);
    osal_irq_disable(g_adc_irq[afe_scan_mode].alarm_irqn);
    osal_irq_free(g_adc_irq[afe_scan_mode].done_irqn, NULL);
    osal_irq_free(g_adc_irq[afe_scan_mode].alarm_irqn, NULL);
    g_adc_working_afe = AFE_SCAN_MODE_MAX_NUM;
}

void adc_port_power_on(bool on)
{
    uapi_unused(on);
}

void adc_irq_lock(uint8_t channel)
{
    uapi_unused(channel);
    g_irq = osal_irq_lock();
}

void adc_irq_unlock(uint8_t channel)
{
    uapi_unused(channel);
    osal_irq_restore(g_irq);
}

errcode_t adc_set_cali_code(afe_scan_mode_t afe_scan_mode, afe_config_t *afe_config)
{
    if (afe_scan_mode >= AFE_SCAN_MODE_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    uapi_unused(afe_config);
    return ERRCODE_SUCC;
}

errcode_t adc_calibration(afe_scan_mode_t afe_scan_mode, bool os_cali, bool cdac_cali, bool dcoc_cali)
{
    if (g_afe_cali_flag[afe_scan_mode] == true) {
        if (afe_scan_mode == AFE_GADC_MODE) {
            cfg_gadc_data0_t cfg_adc_data0;
            cfg_adc_data0.d32 = g_adc_regs->cfg_gadc_data_0;
            cfg_adc_data0.b.cont_mode = COUNT_MODE;
            g_adc_regs->cfg_gadc_data_0 = cfg_adc_data0.d32;
        }
        return ERRCODE_SUCC;
    }
    errcode_t ret = hal_adc_v153_cali(afe_scan_mode, os_cali, cdac_cali, dcoc_cali);
    if (ret == ERRCODE_SUCC) {
        g_afe_cali_flag[afe_scan_mode] = true;
    }
    return ret;
}

void hal_cpu_trace_restart(void)
{
#if (CONFIG_DRIVER_SUPPORT_CPUTRACE)
    uint32_t temp = osal_irq_lock();
    osal_irq_restore(temp);
#endif
}