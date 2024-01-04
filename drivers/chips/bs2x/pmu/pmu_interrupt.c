/*
 * Copyright (c) @CompanyNameMagicTag 2018-2022. All rights reserved.
 * Description:   BT LPC CORE SPECIFIC FUNCTIONS
 * Author: @CompanyNameTag
 * Create: 2019-12-19
 */

#ifndef BUILD_APPLICATION_ATE
#ifndef BUILD_FLASHBOOT
#include "lpc_core.h"
#include "tcxo.h"
#include "debug_print.h"
#include "non_os.h"
#ifdef BUILD_APPLICATION_STANDARD
#include "log_oam_logger.h"
#include "log_def.h"
#include "log_printf.h"
#endif
#ifdef USE_CMSIS_OS
#include "los_hwi.h"
#else
#include "non_os.h"
#endif
#include "panic.h"
#include "hal_reg_config.h"
#include "oal_interface.h"
#include "soc_osal.h"
#include "osal_interrupt.h"
#include "pm_fsm.h"
#include "pm_veto.h"
#include "pmu_interrupt.h"

#define PMU_LDO_TIMEOUT 1000

#define PMU1_CTL_RB_ANA_STATUS_GRM_INT_EN       (PMU1_CTL_RB_BASE + 0x364)
#define PMU_ANA_STATUS_GRM_INT_MASK             0x7F

#define PMU1_CTL_RB_ANA_STATUS_0                (PMU1_CTL_RB_BASE + 0x320)
#define PMU1_CTL_RB_ANA_STATUS_1                (PMU1_CTL_RB_BASE + 0x324)
#define PMU1_CTL_RB_ANA_STATUS_STICK_CLR        (PMU1_CTL_RB_BASE + 0x328)
#define PMU_ANA_STATUS_STICK_CLR_MASK           0x3FF

#define PMU1_CTL_RB_UNLOCK_MCU_STATUS           (PMU1_CTL_RB_BASE + 0x348)
#define PMU1_CTL_RB_UNLOCK_MCU_STICK_CLR        (PMU1_CTL_RB_BASE + 0x34C)
#define PMU_UNLOCK_MCU_STICK_CLR_MASK           0x3

#define PMU1_CTL_RB_UNLOCK_ULPFLL_STATUS        (PMU1_CTL_RB_BASE + 0x350)
#define PMU1_CTL_RB_UNLOCK_ULPFLL_STICK_CLR     (PMU1_CTL_RB_BASE + 0x354)
#define PMU_UNLOCK_ULPFLL_STICK_CLR_MASK        0x3

#define PMU1_CTL_LPM_MCPU_WKUP_INT_CLR_REG      (PMU1_CTL_RB_BASE + 0x220)
#define PMU1_CTL_LPM_MCPU_WKUP_INT_EN_REG       (PMU1_CTL_RB_BASE + 0x224)
#define PMU1_CTL_LPM_MCPU_WKUP_INT_STS_REG      (PMU1_CTL_RB_BASE + 0x228)
#define PM_LPM_MCPU_WKUP_INT_MASK               0xEFFF
#define PM_LPM_MCPU_BT_OSC_EN_WAKEUP            12
#define PM_LPM_MCPU_UART_H0_RX_WAKEUP           5
#define PM_LPM_MCPU_UART_L0_RX_WAKEUP           4

#define ULP_AON_CTL_ULP_WKUP_INT_STS_REG        (ULP_AON_CTL_RB_ADDR + 0x1E0)
#define ULP_AON_CTL_ULP_WKUP_INT_CLR_REG        (ULP_AON_CTL_RB_ADDR + 0x1E4)
#define ULP_AON_CTL_ULP_WKUP_INT_EN_REG         (ULP_AON_CTL_RB_ADDR + 0x1F8)
#define PM_NFC_FIELD_DET_WAKEUP                 4
#define PM_ULP_OSC_EN_WAKEUP                    3
#define PM_ULP_RTC_WAKEUP                       2
#define PM_ULP_GPIO_WAKEUP                      1
#define PM_AON_WKUP_ULP_WAKEUP                  0

#define PM_NON_SLEEP_TIME_AFTER_GPIO_WKUP       2000

static void pmu_ana_status_grm_int_enable(void)
{
    writew(PMU1_CTL_RB_ANA_STATUS_GRM_INT_EN, PMU_ANA_STATUS_GRM_INT_MASK);
}

__IRQ void pmu_wakeup_handler(void)
{
    uint32_t status = osal_irq_lock();
    uint16_t wkup_int_sts = readw(PMU1_CTL_LPM_MCPU_WKUP_INT_STS_REG);
#if defined(PM_SLEEP_DEBUG_ENABLE) && (PM_SLEEP_DEBUG_ENABLE == YES)
    PRINT("pmu wkup isr: 0x%x\n", wkup_int_sts);
#endif

    if ((wkup_int_sts & BIT(PM_LPM_MCPU_BT_OSC_EN_WAKEUP)) != 0) {
        uapi_pm_process_fsm_handler(PM_FSM_ID_BTC, PM_STATE_WORK);
        do {
            writew(PMU1_CTL_LPM_MCPU_WKUP_INT_CLR_REG, BIT(PM_LPM_MCPU_BT_OSC_EN_WAKEUP));
        } while (reg16_getbit(PMU1_CTL_LPM_MCPU_WKUP_INT_STS_REG, PM_LPM_MCPU_BT_OSC_EN_WAKEUP) != 0);
    }

    do {
        writew(PMU1_CTL_LPM_MCPU_WKUP_INT_CLR_REG, PM_LPM_MCPU_WKUP_INT_MASK);
    } while ((readw(PMU1_CTL_LPM_MCPU_WKUP_INT_STS_REG) & PM_LPM_MCPU_WKUP_INT_MASK) != 0);

    osal_irq_clear(M_WAKEUP_IRQN);
    osal_irq_restore(status);
}

static void pmu_cmu_interrupt_handler(void)
{
    uint16_t ana_status_0, ana_status_1, unlock_mcu_status, unlock_ulpfll_status;

    uint32_t status = osal_irq_lock();
    uint64_t base_time = uapi_tcxo_get_ms();
    do {
        ana_status_0 = readw(PMU1_CTL_RB_ANA_STATUS_0);
        ana_status_1 = readw(PMU1_CTL_RB_ANA_STATUS_1);
        unlock_mcu_status = readw(PMU1_CTL_RB_UNLOCK_MCU_STATUS);
        unlock_ulpfll_status = readw(PMU1_CTL_RB_UNLOCK_ULPFLL_STATUS);
        if ((ana_status_0 != 0) || (ana_status_1 != 0) || (unlock_mcu_status != 0) || (unlock_ulpfll_status != 0)) {
            if ((base_time + PMU_LDO_TIMEOUT) < uapi_tcxo_get_ms()) {
#if defined(BUILD_APPLICATION_STANDARD)
                oml_pf_log_print4(LOG_BCORE_PLT_LPM_SWITCH_CLK, LOG_NUM_LPM_SWITCH_CLK, LOG_LEVEL_ERROR, \
                    "[LOW_POWER], ana_status_0:0x%x, ana_status_1:0x%x, fnpll:0x%x, ulpfll:0x%x\r\n",
                    ana_status_0, ana_status_1, unlock_mcu_status, unlock_ulpfll_status);
#endif
                panic(PANIC_PMU_LDO, __LINE__);
            } else {
                writew(PMU1_CTL_RB_ANA_STATUS_STICK_CLR, PMU_ANA_STATUS_STICK_CLR_MASK);
                writew(PMU1_CTL_RB_UNLOCK_MCU_STICK_CLR, PMU_UNLOCK_MCU_STICK_CLR_MASK);
                writew(PMU1_CTL_RB_UNLOCK_ULPFLL_STICK_CLR, PMU_UNLOCK_ULPFLL_STICK_CLR_MASK);
            }
        } else {
            break;
        }
    } while (true);
    osal_irq_clear(PMU_CMU_ERR_IRQN);
    osal_irq_restore(status);
}

static void pmu_ulp_wkup_int_handler(void)
{
    uint32_t status = osal_irq_lock();
    uint16_t wkup_int_sts = readw(ULP_AON_CTL_ULP_WKUP_INT_STS_REG);
#if defined(PM_SLEEP_DEBUG_ENABLE) && (PM_SLEEP_DEBUG_ENABLE == YES)
    PRINT("ulp wkup isr: 0x%x\n", wkup_int_sts);
#endif

    if ((wkup_int_sts & BIT(PM_ULP_OSC_EN_WAKEUP)) != 0) {
        uapi_pm_process_fsm_handler(PM_FSM_ID_BTC, PM_STATE_WORK);
        do {
            writew(ULP_AON_CTL_ULP_WKUP_INT_CLR_REG, BIT(PM_ULP_OSC_EN_WAKEUP));
        } while (reg16_getbit(ULP_AON_CTL_ULP_WKUP_INT_STS_REG, PM_ULP_OSC_EN_WAKEUP) != 0);
    }
#if defined(NFC_TASK_EXIST)
    if ((wkup_int_sts & BIT(PM_NFC_FIELD_DET_WAKEUP)) != 0) {
        uapi_pm_process_fsm_handler(PM_FSM_ID_NFC, PM_STATE_WORK);
        do {
            writew(ULP_AON_CTL_ULP_WKUP_INT_CLR_REG, BIT(PM_NFC_FIELD_DET_WAKEUP));
        } while (reg16_getbit(ULP_AON_CTL_ULP_WKUP_INT_STS_REG, PM_NFC_FIELD_DET_WAKEUP) != 0);
    }
#endif
#if defined(PM_ULP_GPIO_WKUP_ENABLE) && (PM_ULP_GPIO_WKUP_ENABLE == YES)
    if ((readw(ULP_AON_CTL_ULP_WKUP_INT_STS_REG) & BIT(PM_ULP_GPIO_WAKEUP)) != 0) {
        uapi_pm_add_sleep_veto_with_timeout(PM_VETO_ULP_GPIO, PM_NON_SLEEP_TIME_AFTER_GPIO_WKUP);
    }
#endif

    do {
        writew(ULP_AON_CTL_ULP_WKUP_INT_CLR_REG, 0x17); // Ignore bt_osc_en_int
    } while ((readw(ULP_AON_CTL_ULP_WKUP_INT_STS_REG) & 0x17) != 0);

    osal_irq_clear(ULP_WKUP_INT_IRQN);
    osal_irq_restore(status);
}

void pmu_init_interrupts(void)
{
    // Register and enable mcpu core wakeup interrupts only
    osal_irq_request(M_WAKEUP_IRQN, (osal_irq_handler)pmu_wakeup_handler, NULL, NULL, NULL);
    osal_irq_set_priority(M_WAKEUP_IRQN, irq_prio(M_WAKEUP_IRQN));
    osal_irq_enable(M_WAKEUP_IRQN);

    // Register and enable mcpu core wakeup interrupts only
    osal_irq_request(ULP_WKUP_INT_IRQN, (osal_irq_handler)pmu_ulp_wkup_int_handler, NULL, NULL, NULL);
    osal_irq_set_priority(ULP_WKUP_INT_IRQN, irq_prio(ULP_WKUP_INT_IRQN));
    osal_irq_enable(ULP_WKUP_INT_IRQN);

    pmu_ana_status_grm_int_enable();
    // Register and enable pmu pll unlock status interrupts
    osal_irq_request(PMU_CMU_ERR_IRQN, (osal_irq_handler)pmu_cmu_interrupt_handler, NULL, NULL, NULL);
    osal_irq_set_priority(PMU_CMU_ERR_IRQN, irq_prio(PMU_CMU_ERR_IRQN));
    osal_irq_enable(PMU_CMU_ERR_IRQN);
}
#endif
#endif