/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "pm_sleep.h"
#include "pm_veto.h"
#include "pm_sleep_porting.h"
#include "pm_dev.h"
#include "pm.h"
#include "lpc.h"
#include "sfc.h"
#include "cpu_trace.h"
#include "watchdog.h"
#include "pinctrl.h"
#include "uart.h"
#include "los_task_pri.h"
#include "ulp_gpio.h"

#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
#if defined(CONFIG_UART_SUPPORT_LPM)
static pm_dev_ops_t g_dev_uart_register = {
    .suspend = uapi_uart_suspend,
    .suppend_arg = 1,
    .resume = uapi_uart_resume,
    .resume_arg = 1,
};
#endif  /* CONFIG_UART_SUPPORT_LPM */

#if defined(CONFIG_PINCTRL_SUPPORT_LPM)
static pm_dev_ops_t g_dev_pin_register = {
    .suspend = uapi_pin_suspend,
    .suppend_arg = 1,
    .resume = uapi_pin_resume,
    .resume_arg = 1,
};
#endif  /* CONFIG_PINCTRL_SUPPORT_LPM */

#if defined(CONFIG_SFC_SUPPORT_LPM)
static pm_dev_ops_t g_dev_sfc_register = {
    .suspend = uapi_sfc_suspend,
    .suppend_arg = 1,
    .resume = uapi_sfc_resume,
    .resume_arg = 1,
};
#endif  /* CONFIG_SFC_SUPPORT_LPM */

#if defined(CONFIG_WATCHDOG_SUPPORT_LPM)
static pm_dev_ops_t g_dev_watchdog_register = {
    .suspend = uapi_watchdog_suspend,
    .suppend_arg = 1,
    .resume = watchdog_resume,
    .resume_arg = 1,
};
#endif  /* CONFIG_WDT_SUPPORT_LPM */

#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
static pm_dev_ops_t g_dev_cache_register = {
    .suspend = mpu_cache_suspend,
    .suppend_arg = 1,
    .resume = mpu_cache_resume,
    .resume_arg = 1,
};

static pm_dev_ops_t g_dev_cpu_trace_register = {
    .suspend = cpu_trace_suspend,
    .suppend_arg = 1,
    .resume = cpu_trace_resume,
    .resume_arg = 1,
};
#endif
#endif

#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
static uint32_t g_pm_time_before_sleep = 0;
static uint32_t g_pm_time_after_sleep = 0;
static uint32_t g_pm_total_idle_time = 0;
static uint32_t g_pm_total_work_time = 0;
#endif

#if (PM_ULP_GPIO_WKUP_ENABLE == YES)
ulp_gpio_int_wkup_cfg_t g_wk_cfg[] = {
    { 0, 20, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, NULL }, // ULP_GPIO0: S_MGPIO20(20)
    { 1, 33, true, ULP_GPIO_INTERRUPT_RISING_EDGE, NULL },  // ULP_GPIO1: SWD_IO(33)
};
#endif

typedef struct reg_cfg {
    uint32_t reg_addr;
    uint16_t value;
} reg_cfg_t;

static const reg_cfg_t g_wakeup_wait_time_config[] = {
    { 0x5702C14C, 0x1 },    // PMU_SYSLDO_ECO_EN_BOOT_TIME
    { 0x5702C154, 0x1 },    // PMU_REF1_IBG_EN_BOOT_TIME
    { 0x5702C198, 0x1 },    // RST_BOOT_32K_N_WKUP_TIME
    { 0x5702C19C, 0x1 },    // ULP_WKUP_AON_WKUP_TIME
    { 0x5702C634, 0x1 },    // AON_SLP_RDY_WKUP_TIME_SOC
    { 0x5702C638, 0x1 },    // RST_AON_CRG_WKUP_TIME_SOC
    { 0x5702C17C, 0x1 },    // PMU_CLDO_VSET_ECO_BOOT_TIME
    { 0x5702C644, 0x1 },    // REF2_EN_BG_WKUP_TIME_SOC
    { 0x5702C63C, 0x2 },    // GLB_CLK_FORCE_ON_WKUP_TIME_SOC
    { 0x5702C648, 0x2 },    // EN_REFBUFFER_WKUP_TIME_SOC
    { 0x5702C640, 0x3 },    // RST_AON_LGC_WKUP_TIME_SOC
    { 0x5702C64C, 0x3 },    // EN_INTLDO2_WKUP_TIME_SOC
    { 0x5702C1A0, 0x6 },    // ULP_SLP_HLD_TIME
    { 0x5702C16C, 0x7 },    // PMU_MICLDO_EN_BOOT_TIME
    { 0x5702C650, 0x8 },    // EN_IBG_WKUP_TIME_SOC
    { 0x5702C654, 0x8 },    // EN_IPOLY_WKUP_TIME_SOC
    { 0x5702C658, 0x8 },    // EN_ITUNE_WKUP_TIME_SOC
    { 0x5702C65C, 0x8 },    // EN_XLDO_WKUP_TIME_SOC
    { 0x5702C158, 0xB },    // PMU_UVLO_EN_BOOT_TIME
    { 0x5702C15C, 0xB },    // TCM_VDDC_POWER_ON_SEL_BOOT_TIME
    { 0x5702C160, 0xB },    // BUCK_EN_BOOT_TIME
    { 0x5702C164, 0xB },    // BUCK_VSET_ECO_BOOT_TIME
    { 0x5702C168, 0xB },    // BUCK_SLEEP_BOOT_TIME
    { 0x5702C174, 0xD },    // PMU_CLDO_SW_BOOT_TIME
    { 0x5702C178, 0xD },    // PMU_CLDO_EN_BOOT_TIME
    { 0x5702C660, 0xE },    // XO_CORE_PD_WKUP_TIME_SOC
    { 0x5702C664, 0xF },    // FAST_XO_ISO_WKUP_TIME_SOC
    { 0x5702C668, 0xF },    // RC_PD_WKUP_TIME_SOC
    { 0x5702C66C, 0xF },    // RC_RSTN_WKUP_TIME_SOC
    { 0x5702C678, 0xF },    // FAST_XO_LOOP_RSTN_WKUP_TIME_SOC
    { 0x5702C170, 0x12 },   // PMU_SYS_OUT_SEL_BOOT_TIME
    { 0x5702C180, 0x13 },   // VDD0P7_TO_SYS_ISO_EN_BOOT_TIME
    { 0x5702C184, 0x13 },   // RST_BOOT_32K_N_BOOT_TIME
    { 0x5702C188, 0x13 },   // ULP_WKUP_AON_BOOT_TIME
    { 0x5702C18C, 0x13 },   // PMU_FLASHLDO_EN_BOOT_TIME
    { 0x5702C67C, 0x17 },   // XO2DBB_CLKOUT_EN_WKUP_TIME_SOC
    { 0x5702C688, 0x17 },   // RC2_DBB_PD_WKUP_TIME_SOC
    { 0x5702C68C, 0x17 },   // RCCLK2_EN_WKUP_TIME_SOC
    { 0x5702C690, 0x17 },   // RC2_CLKEN_WKUP_TIME_SOC
    { 0x5702C694, 0x18 },   // B32M_CLKEN_WKUP_TIME_SOC
    { 0x5702C698, 0x18 },   // A32M_CLKEN_WKUP_TIME_SOC
    { 0x5702C69C, 0x18 },   // TCXO_EN_WKUP_TIME_SOC
    { 0x5702C6A0, 0x18 },   // APERP_32K_SEL_WKUP_TIME_SOC
    { 0x5702C0E8, 0x1A },   // ram_test1_pulse_boot_time
    { 0x5702C6A4, 0x19 },   // RST_PWR_C1_CRG_N_WKUP_TIME_SOC
    { 0x5702C6A8, 0x1A },   // RST_PWR_C1_LGC_N_WKUP_TIME_SOC
    { 0x5702C6AC, 0x1B },   // RST_PWR_C1_CPU_N_WKUP_TIME_SOC
    { 0x5702C6B0, 0x1B },   // M_SYS_WKING_COMPLETE_TIME
};

static const reg_cfg_t g_sleep_wait_time_config[] = {
    { 0x5702C0E4, 0xF },    // PMU_SYSLDO_ECO_EN_SLP_TIME_SOC
    { 0x5702C1A8, 0xF },    // PMU_REF1_IBG_EN_SLP_TIME_SOC
    { 0x5702C1AC, 0xE },    // PMU_UVLO_EN_SLP_TIME_SOC
    { 0x5702C1B0, 0xD },    // TCM_VDDC_POWER_ON_SEL_SLP_TIME_SOC
    { 0x5702C1B4, 0xD },    // BUCK_EN_SLP_TIME_SOC
    { 0x5702C0C0, 0xD },    // PMU_MICLDO_EN_SLP_TIME_SOC
    { 0x5702C0C4, 0xC },    // PMU_SYS_OUT_SEL_SLP_TIME_SOC
    { 0x5702C0C8, 0xB },    // PMU_CLDO_SW_SLP_TIME_SOC
    { 0x5702C0D0, 0xC },    // PMU_CLDO_EN_SLP_TIME_SOC
    { 0x5702C1B8, 0x8 },    // BUCK_VSET_ECO_SLP_TIME_SOC
    { 0x5702C1BC, 0x8 },    // BUCK_SLEEP_SLP_TIME_SOC
    { 0x5702C0D4, 0x2 },    // PMU_CLDO_VSET_ECO_SLP_TIME_SOC
    { 0x5702C0D8, 0x2 },    // VDD0P7_TO_SYS_ISO_EN_SLP_TIME_SOC
    { 0x5702C0DC, 0x1 },    // RST_BOOT_32K_N_SLP_TIME_SOC
    { 0x5702C0E0, 0x1 },    // ULP_WKUP_AON_SLP_TIME_SOC
    { 0x5702C0EC, 0x1 },    // PMU_FLASHLDO_EN_SLP_TIME_SOC
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

static void pm_wakeup_wait_time_config(void)
{
    for (uint32_t i = 0; i < sizeof(g_wakeup_wait_time_config) / sizeof(reg_cfg_t); i++) {
        writew(g_wakeup_wait_time_config[i].reg_addr, g_wakeup_wait_time_config[i].value);
    }
}

static void pm_sleep_wait_time_config(void)
{
    for (uint32_t i = 0; i < sizeof(g_sleep_wait_time_config) / sizeof(reg_cfg_t); i++) {
        writew(g_sleep_wait_time_config[i].reg_addr, g_sleep_wait_time_config[i].value);
    }
}

static void pm_auto_cg_config(bool on)
{
    if (on) {
        writew(0x57000a00, 0x0);    // AON_AUTO_CG_CFG
        writew(0x52000190, 0x0);    // DAP_H2P_AUTOCG_BYPASS
        writew(0x52000194, 0x0);    // DMA_LP_CTL
        writew(0x52000a2c, 0x0);    // BUS_CG_CTL0
        writew(0x52000a30, 0x0);    // BUS_CG_CTL1
        writew(0x52000a34, 0x0);    // BUS_CG_CTL2
        writew(0x52000ba4, 0x4);    // PWM_AUTO_CG_BYPASS_EN
        writew(0x52000bb0, 0x0);    // SPI_AUTO_CG_CFG
        writew(0x52000bb4, 0x0);    // SPI_SSI_CLK_AUTO_CG_CFG
        writew(0x52000bb8, 0x0);    // DMA_AUTO_CG_CFG
    } else {
        writew(0x57000a00, 0x333);  // AON_AUTO_CG_CFG
        writew(0x52000190, 0x1);    // DAP_H2P_AUTOCG_BYPASS
        writew(0x52000194, 0x1);    // DMA_LP_CTL
        writew(0x52000a2c, 0x377);  // BUS_CG_CTL0
        writew(0x52000a30, 0x7);    // BUS_CG_CTL1
        writew(0x52000a34, 0x377);  // BUS_CG_CTL2
        writew(0x52000ba0, 0x7);    // UART_AUTO_CG_BYPASS_EN
        writew(0x52000ba4, 0x7);    // PWM_AUTO_CG_BYPASS_EN
        writew(0x52000bb0, 0x3f);   // SPI_AUTO_CG_CFG
        writew(0x52000bb8, 0xf);    // DMA_AUTO_CG_CFG
    }
}

void uapi_pm_lpc_init(void)
{
    pm_sleep_funcs_t funcs = {
        .start_tickless            = pm_port_start_tickless,
        .stop_tickless             = pm_port_stop_tickless,
        .get_sleep_ms              = pm_port_get_sleep_ms,
        .start_wakeup_timer        = pm_port_start_wakeup_timer,
        .allow_deepsleep           = pm_port_allow_deepsleep,
        .lightsleep_config         = pm_port_lightsleep_config,
        .deepsleep_config          = pm_port_deepsleep_config,
        .light_wakeup_config       = pm_port_light_wakeup_config,
        .deep_wakeup_config        = pm_port_deep_wakeup_config,
        .enter_wfi                 = pm_port_enter_wfi,
#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
        .cpu_suspend               = pm_port_cpu_suspend,
        .cpu_resume                = pm_port_cpu_resume,
#endif
    };
    uapi_pm_register_sleep_funcs(&funcs);

    uapi_pm_veto_init();
    pm_wakeup_rtc_init();

    pm_wakeup_wait_time_config();
    pm_sleep_wait_time_config();
    pm_auto_cg_config(true);
    pm_port_sleep_config_int();

#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
#if defined(CONFIG_WATCHDOG_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_M_WDT, &g_dev_watchdog_register);
#endif  /* CONFIG_WDT_SUPPORT_LPM */
#if defined(CONFIG_UART_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_M_UART, &g_dev_uart_register);
#endif  /* CONFIG_UART_SUPPORT_LPM */
#if defined(CONFIG_PINCTRL_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_M_PINCTRL, &g_dev_pin_register);
#endif  /* CONFIG_PINCTRL_SUPPORT_LPM */
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_M_CPU_TRACE, &g_dev_cpu_trace_register);
    uapi_pm_register_dev_ops(PM_DEV_M_CACHE, &g_dev_cache_register);
#endif
#if defined(CONFIG_SFC_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_M_SFC, &g_dev_sfc_register);
#endif  /* CONFIG_SFC_SUPPORT_LPM */
#endif

#if (PM_ULP_GPIO_WKUP_ENABLE == YES)
    ulp_gpio_init();
    ulp_gpio_int_wkup_config(g_wk_cfg, sizeof(g_wk_cfg) / sizeof(ulp_gpio_int_wkup_cfg_t));
#endif
    uapi_pm_add_sleep_veto(PM_VETO_ID_MCU);
}

#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
void pm_record_time_before_sleep(void)
{
    g_pm_time_before_sleep = (uint32_t)uapi_systick_get_ms();
    g_pm_total_work_time += (g_pm_time_before_sleep - g_pm_time_after_sleep);
}

void pm_record_time_after_sleep(void)
{
    g_pm_time_after_sleep = (uint32_t)uapi_systick_get_ms();
    g_pm_total_idle_time += (g_pm_time_after_sleep - g_pm_time_before_sleep);
}

uint32_t pm_get_time_before_sleep(void)
{
    return g_pm_time_before_sleep;
}

uint32_t pm_get_total_work_time(void)
{
    return g_pm_total_work_time;
}

uint32_t pm_get_time_after_sleep(void)
{
    return g_pm_time_after_sleep;
}

uint32_t pm_get_total_idle_time(void)
{
    return g_pm_total_idle_time;
}
#endif