/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm sleep port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "chip_io.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#include "osal_interrupt.h"
#include "los_task_pri.h"
#include "non_os.h"
#include "arch_barrier.h"
#include "arch_encoding.h"
#include "debug_print.h"
#include "systick.h"
#include "ulp_rtc.h"
#include "tcxo.h"
#include "tcxo_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "timer.h"
#include "rtc.h"
#include "pm_sleep_porting.h"

#define ULP_AON_CTL_MCPU_POR_RST_PC_H_REG       (ULP_AON_CTL_RB_ADDR + 0xF0)
#define ULP_AON_CTL_MCPU_POR_RST_PC_L_REG       (ULP_AON_CTL_RB_ADDR + 0xF4)

#define PMU1_CTL_LPM_MCU_ALW_TO_SLP_REG         (PMU1_CTL_RB_BASE + 0x200)

/* Ulp sleep event. */
#define ULP_AON_CTL_ULP_SLP_EVT_STS_REG         (ULP_AON_CTL_RB_ADDR + 0x1C8)
#define ULP_AON_CTL_ULP_SLP_EVT_CLR_REG         (ULP_AON_CTL_RB_ADDR + 0x1CC)
#define ULP_AON_CTL_ULP_SLP_EVT_EN_REG          (ULP_AON_CTL_RB_ADDR + 0x1D0)

/* Ulp sleep interrupt. */
#define ULP_AON_CTL_ULP_SLP_INT_STS_REG         (ULP_AON_CTL_RB_ADDR + 0x1C0)
#define ULP_AON_CTL_ULP_SLP_INT_CLR_REG         (ULP_AON_CTL_RB_ADDR + 0x1C4)
#define ULP_AON_CTL_ULP_SLP_INT_EN_REG          (ULP_AON_CTL_RB_ADDR + 0x1D4)

/* Ulp wakeup interrupt. */
#define ULP_AON_CTL_ULP_WKUP_INT_STS_REG        (ULP_AON_CTL_RB_ADDR + 0x1E0)
#define ULP_AON_CTL_ULP_WKUP_INT_CLR_REG        (ULP_AON_CTL_RB_ADDR + 0x1E4)
#define ULP_AON_CTL_ULP_WKUP_INT_EN_REG         (ULP_AON_CTL_RB_ADDR + 0x1F8)

/* Ulp wakeup event. */
#define ULP_AON_CTL_ULP_WKUP_EVT_STS_REG        (ULP_AON_CTL_RB_ADDR + 0x1E8)
#define ULP_AON_CTL_ULP_WKUP_EVT_CLR_REG        (ULP_AON_CTL_RB_ADDR + 0x1EC)
#define ULP_AON_CTL_ULP_WKUP_EVT_EN_REG         (ULP_AON_CTL_RB_ADDR + 0x1F0)

#define PM_NFC_FIELD_DET_WAKEUP                 4
#define PM_ULP_OSC_EN_WAKEUP                    3
#define PM_ULP_RTC_WAKEUP                       2
#define PM_ULP_GPIO_WAKEUP                      1
#define PM_AON_WKUP_ULP_WAKEUP                  0
#define PM_ULP_WKUP_ALL_MASK                    0x1F
#if defined(NFC_TASK_EXIST)
#define PM_ULP_WKUP_MASK                        (BIT(PM_NFC_FIELD_DET_WAKEUP) | \
                                                BIT(PM_ULP_OSC_EN_WAKEUP) | \
                                                BIT(PM_ULP_GPIO_WAKEUP) | \
                                                BIT(PM_AON_WKUP_ULP_WAKEUP))
#else
#define PM_ULP_WKUP_MASK                        (BIT(PM_ULP_OSC_EN_WAKEUP) | \
                                                BIT(PM_ULP_GPIO_WAKEUP) | \
                                                BIT(PM_AON_WKUP_ULP_WAKEUP))
#endif

/* Wakeup event. */
#define PMU1_CTL_LPM_MCPU_WKUP_EVT_CLR_REG      (PMU1_CTL_RB_BASE + 0x210)
#define PMU1_CTL_LPM_MCPU_WKUP_EVT_EN_REG       (PMU1_CTL_RB_BASE + 0x214)
#define PMU1_CTL_LPM_MCPU_WKUP_EVT_STS_REG      (PMU1_CTL_RB_BASE + 0x218)
/* Wakeup interrupt. */
#define PMU1_CTL_LPM_MCPU_WKUP_INT_CLR_REG      (PMU1_CTL_RB_BASE + 0x220)
#define PMU1_CTL_LPM_MCPU_WKUP_INT_EN_REG       (PMU1_CTL_RB_BASE + 0x224)
#define PMU1_CTL_LPM_MCPU_WKUP_INT_STS_REG      (PMU1_CTL_RB_BASE + 0x228)

#define PM_LPM_MCPU_CWDT_INT_WAKEUP             14
#define PM_LPM_MCPU_ULP_INT_WAKEUP              13
#define PM_LPM_MCPU_BT_OSC_EN_WAKEUP            12
#define PM_LPM_MCPU_DAP_WAKEUP                  11
#define PM_LPM_MCPU_SSI_WAKEUP                  10
#define PM_LPM_MCPU_GPIO_WAKEUP                 9
#define PM_LPM_MCPU_M_RTC_WAKEUP                8
#define PM_LPM_MCPU_SPI1_INT_WAKEUP             7
#define PM_LPM_MCPU_UART_L1_RX_WAKEUP           6
#define PM_LPM_MCPU_UART_H0_RX_WAKEUP           5
#define PM_LPM_MCPU_UART_L0_RX_WAKEUP           4
#define PM_LPM_MCPU_SPI2_INT_WAKEUP             3
#define PM_LPM_MCPU_QDEC_INT_WAKEUP             2
#define PM_LPM_MCPU_KEYSCAN_INT_WAKEUP          0
#define PM_LPM_MCPU_WKUP_ALL_MASK               0xFFFF
#define PM_LPM_MCPU_WKUP_MASK                   (BIT(PM_LPM_MCPU_CWDT_INT_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_ULP_INT_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_BT_OSC_EN_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_DAP_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_SSI_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_GPIO_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_M_RTC_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_SPI1_INT_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_UART_L1_RX_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_UART_H0_RX_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_UART_L0_RX_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_SPI2_INT_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_QDEC_INT_WAKEUP) | \
                                                BIT(PM_LPM_MCPU_KEYSCAN_INT_WAKEUP))

/* Sleep event. */
#define PMU1_CTL_LPM_MCPU_SLP_EVT_CLR_REG       (PMU1_CTL_RB_BASE + 0x240)
#define PMU1_CTL_LPM_MCPU_SLP_EVT_EN_REG        (PMU1_CTL_RB_BASE + 0x244)
#define PMU1_CTL_LPM_MCPU_SLP_EVT_STS_REG       (PMU1_CTL_RB_BASE + 0x248)
/* Sleep int. */
#define PMU1_CTL_LPM_MCPU_SLP_INT_CLR_REG       (PMU1_CTL_RB_BASE + 0x250)
#define PMU1_CTL_LPM_MCPU_SLP_INT_EN_REG        (PMU1_CTL_RB_BASE + 0x254)
#define PMU1_CTL_LPM_MCPU_SLP_INT_STS_REG       (PMU1_CTL_RB_BASE + 0x258)

#define PM_LPM_MCPU_BT_OSC_EN_SLP               2
#define PM_LPM_MCPU_SLEEPING_SLP                1
#define PM_LPM_MCPU_SLEEPDEEP_SLP               0
#define PM_LPM_MCPU_SLP_ALL_MASK                0x7
#define PM_LPM_MCPU_SLP_MASK                    BIT(PM_LPM_MCPU_SLEEPING_SLP)

#define PMU1_CTL_USB_WKUP_EVT_SEL_REG           (PMU1_CTL_RB_BASE + 0x700)
#define PMU1_CTL_USB_WKUP_EVT_EN_REG            (PMU1_CTL_RB_BASE + 0x704)
#define PMU1_CTL_USB_WKUP_EVT_CLR_REG           (PMU1_CTL_RB_BASE + 0x708)
#define PMU1_CTL_USB_WKUP_EVT_STS_REG           (PMU1_CTL_RB_BASE + 0x70C)

#define PMU1_CTL_USB_WKUP_INT_EN_REG            (PMU1_CTL_RB_BASE + 0x710)
#define PMU1_CTL_USB_WKUP_INT_CLR_REG           (PMU1_CTL_RB_BASE + 0x714)
#define PMU1_CTL_USB_WKUP_INT_STS_REG           (PMU1_CTL_RB_BASE + 0x718)

#define PMU1_CTL_RST_MAN_REG                    (PMU1_CTL_RB_BASE + 0xE4)
#define PM_RST_PWR_C1_CRG_N_FRC_ON              2
#define PM_RST_PWR_C1_LOGIC_N_FRC_ON            1
#define PM_RST_PWR_C1_CPU_N_FRC_ON              0
#define PM_RST_PWR_C1_FRC_ON_MASK               (BIT(PM_RST_PWR_C1_CRG_N_FRC_ON) | \
                                                BIT(PM_RST_PWR_C1_LOGIC_N_FRC_ON) | \
                                                BIT(PM_RST_PWR_C1_CPU_N_FRC_ON))

#define ULP_AON_CTL_PMU_SYSLDO_ECO_EN_CFG_REG   (ULP_AON_CTL_RB_ADDR + 0xFC)
#define ULP_AON_CTL_PMU_REF1_IBG_EN_CFG_REG     (ULP_AON_CTL_RB_ADDR + 0x104)
#define ULP_AON_CTL_PMU_UVLO_EN_CFG_REG         (ULP_AON_CTL_RB_ADDR + 0x108)
#define ULP_AON_CTL_PMU_BUCK_EN_CFG_REG         (ULP_AON_CTL_RB_ADDR + 0x110)
#define ULP_AON_CTL_PMU_CLDO_SW_CFG_REG         (ULP_AON_CTL_RB_ADDR + 0x124)
#define ULP_AON_CTL_VDD0P7_TO_SYS_ISO_CFG_REG   (ULP_AON_CTL_RB_ADDR + 0x130)
#define ULP_AON_CTL_RST_BOOT_32K_CFG_REG        (ULP_AON_CTL_RB_ADDR + 0x134)
#define ULP_AON_CTL_ULP_WKUP_AON_CFG_REG        (ULP_AON_CTL_RB_ADDR + 0x138)
#define ULP_AON_CTL_PMU_FLASHLDO_EN_CFG_REG     (ULP_AON_CTL_RB_ADDR + 0x13C)
#define ULP_AON_CTL_RST_BOOT_32K_N_CFG_REG      (ULP_AON_CTL_RB_ADDR + 0x134)
#define ULP_AON_CTL_PMU_CLDO_EN_CFG_REG         (ULP_AON_CTL_RB_ADDR + 0x128)
#define ULP_AON_CTL_CLDO_MAN_REG                (ULP_AON_CTL_RB_ADDR + 0x200)
#define PM_PMU_CLDO_EN_MAN_ON                   0x111
#define PM_PMU_CLDO_EN_AUTO                     0

#define ULP_AON_CTL_PAD_CONTROL_REG             (ULP_AON_CTL_RB_ADDR + 0x840)
#define PM_PAD_CONTROL_BY_ULP                   1
#define PM_PAD_CONTROL_BY_AON                   0

#define ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG        (ULP_AON_CTL_RB_ADDR + 0x24)
#define PM_PCLK_INTR_SEL_BIT                    1
#define PM_PCLK_INTR_EN_BIT                     0

#define PM_OS_TICKS_PER_S   LOSCFG_BASE_CORE_TICK_PER_SECOND
#define PM_OS_TICKS_PER_MS  (PM_OS_TICKS_PER_S / LOSCFG_BASE_CORE_TICK_PER_SECOND)
static uint64_t g_entry_sleep_time = 0;
static uint64_t g_exit_sleep_time = 0;
static uint64_t g_32k_time_calibrate = 0;

#define PM_SLEEP_CPU_SUSPEND_REG_NUM 128
uint32_t g_cpu_suspend_regs[PM_SLEEP_CPU_SUSPEND_REG_NUM] = { 0 };

#define EARLY_WKUP_MS             2
#if defined(CONFIG_RTC_SUPPORT_LPM)
static uint64_t g_rtc_resume_current_count = 0;
#endif
#if defined(CONFIG_TIMER_SUPPORT_LPM)
static uint64_t g_timer_resume_current_count = 0;
#endif

#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
static uint32_t g_cpu_reset_pc;
static uint64_t g_suspend_current_count = 0;
static uint64_t g_resume_current_count = 0;

#define DTCM_SHARE_MODE           0xF90
#define SHARE_MODE_GT             12
#define SHARE_MODE_CFG            8

static void pm_em_enable_cfg(void)
{
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_GT, 0x2, 0x0);
#ifdef EM_32K_SUPPORT
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_CFG, 0x2, 0x3);
#else
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_CFG, 0x2, 0x1);
#endif
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_GT, 0x2, 0x3);
}

typedef struct reg_cfg {
    uint32_t reg_addr;
    uint16_t value;
} reg_cfg_t;

static const reg_cfg_t g_sleep_wait_time_config[] = {
    { 0x570042A0, 0x3 },    // AON_SLP_RDY_SLP_TIME
    { 0x570042A4, 0x3 },    // REF2_EN_BG_SLP_TIME_SOC
    { 0x570042A8, 0x3 },    // EN_REFBUFFER_SLP_TIME_SOC
    { 0x570042AC, 0x2 },    // EN_INTLDO2_SLP_TIME_SOC
    { 0x570042B0, 0x3 },    // EN_IBG_SLP_TIME_SOC
    { 0x570042B4, 0x3 },    // EN_IPOLY_SLP_TIME_SOC
    { 0x570042B8, 0x3 },    // EN_ITUNE_SLP_TIME_SOC
    { 0x570042C4, 0x3 },    // EN_CLKLDO1_SLP_TIME
    { 0x570042C8, 0x2 },    // EN_XLDO_SLP_TIME
    { 0x570042D0, 0x2 },    // XO_CORE_PD_SLP_TIME
    { 0x570042D4, 0x2 },    // FAST_XO_ISO_SLP_TIME
    { 0x570042D8, 0x2 },    // RC_PD_SLP_TIME
    { 0x570042DC, 0x2 },    // RC_RSTN_SLP_TIME
    { 0x570042E8, 0x2 },    // FAST_XO_LOOP_RSTN_SLP_TIME
    { 0x570042EC, 0x2 },    // XO2DBB_CLKOUT_EN_SLP_TIME
    { 0x570042F8, 0x2 },    // RC2_DBB_PD_SLP_TIME
    { 0x570042FC, 0x2 },    // RCCLK2_EN_SLP_TIME
    { 0x57004300, 0x2 },    // RC2_CLKEN_SLP_TIME
    { 0x57004304, 0x2 },    // B32M_CLKEN_SLP_TIME
    { 0x57004308, 0x2 },    // A32M_CLKEN_SLP_TIME
    { 0x5700430C, 0x2 },    // TCXO_EN_SLP_TIME
    { 0x57004310, 0x1 },    // APERP_32K_SEL_SLP_TIME
    { 0x57004314, 0x1 },    // RST_PWR_C1_CRG_N_SLP_TIME
    { 0x57004318, 0x1 },    // RST_PWR_C1_LGC_N_SLP_TIME
    { 0x5700431C, 0x1 },    // RST_PWR_C1_CPU_N_SLP_TIME
};

static void pm_sleep_wait_time_config(void)
{
    for (uint32_t i = 0; i < sizeof(g_sleep_wait_time_config) / sizeof(reg_cfg_t); i++) {
        writew(g_sleep_wait_time_config[i].reg_addr, g_sleep_wait_time_config[i].value);
    }
}
#else
static rtc_handle_t g_pm_rtc = NULL;

static void pm_rtc1_irq(uintptr_t data)
{
    unused(data);
}
#endif

static void pm_clear_slp_wkup_event(void)
{
    writew(PMU1_CTL_LPM_MCPU_SLP_EVT_EN_REG, 0);                                // Disable.
    do {
        writew(PMU1_CTL_LPM_MCPU_SLP_EVT_CLR_REG, PM_LPM_MCPU_SLP_ALL_MASK);    // Clear status.
    } while (readw(PMU1_CTL_LPM_MCPU_SLP_EVT_STS_REG) != 0);                    // Get status.

    writew(PMU1_CTL_LPM_MCPU_WKUP_EVT_EN_REG, 0);                               // Disable.
    do {
        writew(PMU1_CTL_LPM_MCPU_WKUP_EVT_CLR_REG, PM_LPM_MCPU_WKUP_ALL_MASK);  // Clear status.
    } while (readw(PMU1_CTL_LPM_MCPU_WKUP_EVT_STS_REG) != 0);                   // Get status.

    writew(ULP_AON_CTL_ULP_SLP_EVT_EN_REG, 0);                                // Disable.
    do {
        writew(ULP_AON_CTL_ULP_SLP_EVT_CLR_REG, 0x1);                         // Clear status.
    } while (readw(ULP_AON_CTL_ULP_SLP_EVT_STS_REG) != 0);                    // Get status.

    writew(ULP_AON_CTL_ULP_WKUP_EVT_EN_REG, 0);                               // Disable.
    do {
        writew(ULP_AON_CTL_ULP_WKUP_EVT_CLR_REG, PM_ULP_WKUP_ALL_MASK);  // Clear status.
    } while (readw(ULP_AON_CTL_ULP_WKUP_EVT_STS_REG) != 0);                   // Get status.
}

void pm_wakeup_rtc_init(void)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    ulp_rtc_init();
#else
    uapi_rtc_init();
    uapi_rtc_adapter(RTC_1, RTC_1_IRQN, g_aucIntPri[RTC_1_IRQN]);
    uapi_rtc_create(RTC_1, &g_pm_rtc);
#endif
}

void pm_wakeup_rtc_start(uint32_t time_ms)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    ulp_rtc_start(time_ms);
#else
    if (g_pm_rtc == NULL) {
        uapi_rtc_create(RTC_1, &g_pm_rtc);
    } else {
        uapi_rtc_start(g_pm_rtc, time_ms, pm_rtc1_irq, 1);
    }
#endif
}

void pm_port_start_tickless(void)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    g_entry_sleep_time = 0; // ulp_rtc_get_count获取的值未重置，先按0计算即可
#else
    g_entry_sleep_time = uapi_systick_get_count();
#endif
    os_tick_timer_disable();
}

static void pm_port_tickless_compensation(uint32_t ticks)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    g_exit_sleep_time = ulp_rtc_get_count();
#else
    g_exit_sleep_time = uapi_systick_get_count();
#endif
    uint64_t actual_sleep_ms = (((g_exit_sleep_time - g_entry_sleep_time) * PM_OS_TICKS_PER_S) +
                                 g_32k_time_calibrate) >> 0xF;
    g_32k_time_calibrate = actual_sleep_ms & 0x7FFF;

    if (actual_sleep_ms > ticks) {
        oal_ticks_restore(ticks);
    } else {
        oal_ticks_restore(actual_sleep_ms);
    }

#if defined(PM_SLEEP_DEBUG_ENABLE) && (PM_SLEEP_DEBUG_ENABLE == YES)
    PRINT("ticks = %d, actual_sleep_ms = %d, g_resume_current_count = %d, g_exit_sleep_time = %d\r\n",
          ticks, (uint32_t)actual_sleep_ms, (uint32_t)g_resume_current_count, (uint32_t)g_exit_sleep_time);
#if defined(CONFIG_RTC_SUPPORT_LPM)
    PRINT("g_rtc_resume_current_count = %d\r\n", (uint32_t)g_rtc_resume_current_count);
#endif
#if defined(CONFIG_TIMER_SUPPORT_LPM)
    PRINT("g_timer_resume_current_count = %d\r\n", (uint32_t)g_timer_resume_current_count);
#endif
    PRINT("0x5702C1E8 = 0x%x\r\n", readw(0x5702C1E8));
    PRINT("0x57004218 = 0x%x\r\n", readw(0x57004218));
#endif
    pm_clear_slp_wkup_event();
}

void pm_port_stop_tickless(uint32_t sleep_ms)
{
    uint32_t ticks = 0;
    /* Avoid overcompensation. */
    if (PM_OS_TICKS_PER_MS == 1) {
        ticks = (sleep_ms - 1) * PM_OS_TICKS_PER_MS;
    } else {
        ticks = (sleep_ms - (PM_OS_TICKS_PER_MS >> 1)) * PM_OS_TICKS_PER_MS;
    }
    os_tick_timer_enable();
    pm_port_tickless_compensation(ticks);
}

uint32_t pm_port_get_sleep_ms(void)
{
#if defined(CONFIG_RTC_SUPPORT_LPM)
    uint32_t nxt_ms = uapi_rtc_get_latest_timeout();
#else
    uint32_t nxt_ms = UINT32_MAX;
#endif /* CONFIG_RTC_SUPPORT_LPM */
    uint32_t os_nxt_ms = oal_get_sleep_ticks() / PM_OS_TICKS_PER_MS;
    uint32_t slp_ms = min(nxt_ms, os_nxt_ms);
    return slp_ms <= EARLY_WKUP_MS ? EARLY_WKUP_MS : slp_ms - EARLY_WKUP_MS;
}

void pm_port_allow_deepsleep(bool allow)
{
    writew(PMU1_CTL_LPM_MCU_ALW_TO_SLP_REG, (uint16_t)allow);
}

void pm_port_enter_wfi(void)
{
    dsb();
    wfi();
    isb();
    nop();
    nop();
    nop();
    nop();
}

void pm_port_start_wakeup_timer(uint32_t sleep_ms)
{
    pm_wakeup_rtc_start(sleep_ms);
}

void pm_port_lightsleep_config(void)
{
    return;
}

void pm_port_light_wakeup_config(void)
{
    return;
}

void pm_port_sleep_config_int(void)
{
#if defined(CONFIG_PM_ENABLE_WAKEUP_INTERRUPT)
    /* Aon wakeup interrupt. */
    writew(PMU1_CTL_LPM_MCPU_WKUP_INT_EN_REG, 0);                           // Disable.
    writew(PMU1_CTL_LPM_MCPU_WKUP_INT_CLR_REG, PM_LPM_MCPU_WKUP_ALL_MASK);  // Clear status.

    writew(ULP_AON_CTL_ULP_WKUP_INT_EN_REG, 0);                     // Disable.
    writew(ULP_AON_CTL_ULP_WKUP_INT_CLR_REG, PM_ULP_WKUP_ALL_MASK);     // Clear status.
    writew(ULP_AON_CTL_ULP_WKUP_INT_EN_REG, PM_ULP_WKUP_MASK);      // Enable.
#endif
    /* Ulp sleep event. */
    writew(ULP_AON_CTL_ULP_SLP_EVT_EN_REG, 0);  // Disable.
    writew(ULP_AON_CTL_ULP_SLP_EVT_CLR_REG, 1); // Clear status.
    writew(ULP_AON_CTL_ULP_SLP_EVT_EN_REG, 1);  // Enable.
    /* Ulp wakeup event. */
    writew(ULP_AON_CTL_ULP_WKUP_EVT_EN_REG, 0);                     // Disable.
    writew(ULP_AON_CTL_ULP_WKUP_EVT_CLR_REG, PM_ULP_WKUP_ALL_MASK); // Clear status.
    writew(ULP_AON_CTL_ULP_WKUP_EVT_EN_REG, PM_ULP_WKUP_ALL_MASK);      // Enable.
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    /* Turn off cldo during sleep. */
    writew(PMU1_CTL_RST_MAN_REG, 0);    // 复位后默认即0
    writew(ULP_AON_CTL_RST_BOOT_32K_N_CFG_REG, 0);
    writew(ULP_AON_CTL_PMU_CLDO_EN_CFG_REG, 0);
    writew(ULP_AON_CTL_CLDO_MAN_REG, PM_PMU_CLDO_EN_AUTO);
#endif
}

static void pm_port_handle_before_enter_deepsleep(void)
{
    // Enable slp/wkup event.
    writew(PMU1_CTL_LPM_MCPU_SLP_EVT_CLR_REG, PM_LPM_MCPU_SLP_ALL_MASK);      // Clear status.
    writew(PMU1_CTL_LPM_MCPU_SLP_EVT_EN_REG, PM_LPM_MCPU_SLP_ALL_MASK);           // Enable.
    writew(PMU1_CTL_LPM_MCPU_WKUP_EVT_CLR_REG, PM_LPM_MCPU_WKUP_ALL_MASK);  // Clear status.
    writew(PMU1_CTL_LPM_MCPU_WKUP_EVT_EN_REG, PM_LPM_MCPU_WKUP_ALL_MASK);       // Enable.

#if defined(CONFIG_PM_ENABLE_WAKEUP_INTERRUPT)
    // Clear wkup int.
    writew(ULP_AON_CTL_ULP_WKUP_INT_CLR_REG, 0x17);
#endif

    // Enable ulp slp/wkup event.
    writew(ULP_AON_CTL_ULP_SLP_EVT_CLR_REG, 0x1); // Clear status.
    writew(ULP_AON_CTL_ULP_SLP_EVT_EN_REG, 0x1);  // Enable.
    writew(ULP_AON_CTL_ULP_WKUP_EVT_CLR_REG, PM_ULP_WKUP_ALL_MASK); // Clear status.
    writew(ULP_AON_CTL_ULP_WKUP_EVT_EN_REG, PM_ULP_WKUP_ALL_MASK);      // Enable.
}

void pm_port_deepsleep_config(void)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    pm_sleep_wait_time_config();
#endif
    pm_port_handle_before_enter_deepsleep();
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    /* Pad control by ulp. */
    writew(ULP_AON_CTL_PAD_CONTROL_REG, PM_PAD_CONTROL_BY_ULP);
    reg16_setbits(0x5702C230, 0x8, 0x3, 0x0); // CLDO关闭时需要关闭bit[8-10]，否则漏电
#else
    /* Turn on cldo during sleep. */
    writew(PMU1_CTL_RST_MAN_REG, PM_RST_PWR_C1_FRC_ON_MASK);
    writew(ULP_AON_CTL_RST_BOOT_32K_N_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_CLDO_EN_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_REF1_IBG_EN_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_UVLO_EN_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_BUCK_EN_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_CLDO_SW_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_FLASHLDO_EN_CFG_REG, 0x1);
    writew(ULP_AON_CTL_PMU_SYSLDO_ECO_EN_CFG_REG, 0x1);
    writew(ULP_AON_CTL_RST_BOOT_32K_CFG_REG, 0x1);
    writew(ULP_AON_CTL_ULP_WKUP_AON_CFG_REG, 0x1);
    writew(ULP_AON_CTL_VDD0P7_TO_SYS_ISO_CFG_REG, 0);
    writew(ULP_AON_CTL_CLDO_MAN_REG, PM_PMU_CLDO_EN_MAN_ON);
#endif
    /* ULP_GPIO interrupt sampling clock: 32k. */
    reg16_setbit(ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG, PM_PCLK_INTR_SEL_BIT);
    reg16_setbit(ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG, PM_PCLK_INTR_EN_BIT);
}

void pm_port_deep_wakeup_config(void)
{
    /* ULP_GPIO interrupt sampling clock: pclk. */
    reg16_clrbit(ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG, (uint16_t)PM_PCLK_INTR_SEL_BIT);
    reg16_setbit(ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG, PM_PCLK_INTR_EN_BIT);
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    /* Pad control by aon. */
    writew(ULP_AON_CTL_PAD_CONTROL_REG, PM_PAD_CONTROL_BY_AON);
    reg16_setbits(0x5702C230, 0x8, 0x3, 0x7); // CLDO打开时需要打开bit[8-10]，ADC、NFC、RF需要
#endif
}

void pm_port_cpu_suspend(void)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
#if defined(CONFIG_SYSTICK_SUPPORT_LPM)
    uapi_systick_suspend(0);
#endif
#if defined(CONFIG_TCXO_SUPPORT_LPM)
    uapi_tcxo_suspend(0);
#endif
#if defined(CONFIG_RTC_SUPPORT_LPM)
    uapi_rtc_suspend(0);
#endif
#if defined(CONFIG_TIMER_SUPPORT_LPM)
    uapi_timer_suspend(0);
#endif
    g_suspend_current_count = 0;

    // pc = pc_h << 16 + pc_l
    g_cpu_reset_pc = (readl(ULP_AON_CTL_MCPU_POR_RST_PC_H_REG) << 16) | (readl(ULP_AON_CTL_MCPU_POR_RST_PC_L_REG));
    // pc_h: pc >> 16
    writew(ULP_AON_CTL_MCPU_POR_RST_PC_H_REG, (uint16_t)((uint32_t)(uintptr_t)lowpower_cpu_resume >> 16));
    writew(ULP_AON_CTL_MCPU_POR_RST_PC_L_REG, (uint16_t)((uint32_t)(uintptr_t)lowpower_cpu_resume));
    lowpower_cpu_suspend(); // Must be placed at the end of this function
#endif
}

#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
static void pm_port_timers_resume(void)
{
#if defined(CONFIG_SYSTICK_SUPPORT_LPM)
    uint64_t sys_differential_value = (g_resume_current_count - g_suspend_current_count) * 0x1;
    uapi_systick_resume((uintptr_t)&sys_differential_value);
#endif
#if defined(CONFIG_TCXO_SUPPORT_LPM)
    // 1000000: 1s = 1000000us
    uint64_t tcxo_differential_value = ((g_resume_current_count - g_suspend_current_count) * 1000000) >> 0xF;
    uapi_tcxo_resume((uintptr_t)&tcxo_differential_value);
#endif
#if defined(CONFIG_RTC_SUPPORT_LPM)
    g_rtc_resume_current_count = ulp_rtc_get_count();
    uint64_t rtc_differential_value = (g_rtc_resume_current_count - g_suspend_current_count) * 0x1;
    uapi_rtc_resume((uintptr_t)&rtc_differential_value);
#endif
#if defined(CONFIG_TIMER_SUPPORT_LPM)
    g_timer_resume_current_count = ulp_rtc_get_count();
    uint64_t timer_differential_value = (g_timer_resume_current_count - g_suspend_current_count) * 1000;
    uapi_timer_resume((uintptr_t)&timer_differential_value);
#endif
}
#endif

void pm_port_cpu_resume(void)
{
#if (CONFIG_PM_POWER_GATING_ENABLE == 1)
    writew(ULP_AON_CTL_MCPU_POR_RST_PC_H_REG, (uint16_t)((uint32_t)(uintptr_t)g_cpu_reset_pc >> 16)); // pc_h: pc >> 16
    writew(ULP_AON_CTL_MCPU_POR_RST_PC_L_REG, (uint16_t)((uint32_t)(uintptr_t)g_cpu_reset_pc));

    writel(0x52000A40, 1);
    writel(0x5200007C, 0x2); // 先关en
    writel(0x5200007C, 0x1); // 再配分频系数
    writel(0x5200007C, 0x11); // 再开en
    writel(0x52000548, 0x0); // 关闭MCU_CRG时钟输出
    uint16_t value = reg16_getbits(0x570000FC, 12, 2); // 回读bit 13:12，01代表0路，10代表1路
    if (value == 0x1) {
        writel(0x570000FC, 0x3);  // clk_mcu_core切CLK_XO，当前在0路执行
        writel(0x57000100, 0x1);  // mcu_core_ch_sel_nor=1，当前在0路执行
    } else if (value == 0x2) {
        writel(0x570000FC, 0x3);  // clk_mcu_core切CLK_XO，当前在1路执行
        writel(0x57000100, 0x0);  // mcu_core_ch_sel_nor=0，当前在1路执行
    }
    writel(0x52000040, 0x3CF); // 关闭uart_h0_clken
    writel(0x52000558, 0x3); // uart切CLK_XO
    writel(0x52000040, 0x7CF); // 开启uart_h0_clken
    writel(0x52000554, 0x3); // perp_ls切XO时钟 32M
    writel(0x52000044, 0x8391); // 关闭spi0/1/2_clken
    writel(0x5200055C, 0x3); // spi切CLK_XO
    writel(0x52000044, 0x839F); // 开启spi0/1/2_clken
    writel(0x52000588, 0x51); // clk_nfc切CLK_RC/2
    writel(0x52000608, 0x93); // clk_i2s切CLK_XO/4
    writel(0x52000574, 0x3); // clk_qspi切CLK_XO
    writel(0x52000548, 0xFEE); // 开启C_CRG时钟输出
    writel(0x57000068, 0x400); // clk_aon_bus选择XO时钟
    writel(0x52000540, 0x4); // 关闭com_dll2

    (void)ulp_rtc_get_count();  // 前两次读的可能不对，等SOC解决澄清再删除这里
    (void)ulp_rtc_get_count();
    g_resume_current_count = ulp_rtc_get_count();
    pm_port_timers_resume();
    pm_em_enable_cfg();
#endif

    return;
}

uint16_t pm_port_get_sleep_event_status(void)
{
    return readw(PMU1_CTL_LPM_MCPU_SLP_EVT_STS_REG);
}

uint16_t pm_port_get_wakeup_event_status(void)
{
    return readw(ULP_AON_CTL_ULP_WKUP_EVT_STS_REG);
}