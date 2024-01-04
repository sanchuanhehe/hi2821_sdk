/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides bs21 ulp_rtc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-31, Create file. \n
 */
#include "common_def.h"
#include "chip_io.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#include "osal_interrupt.h"
#include "debug_print.h"
#include "ulp_rtc.h"

/* Ulp ulp_rtc event. */
#define ULP_INT_CFG                             (ULP_AON_CTL_RB_ADDR + 0x90)
#define ULP_AON_CTL_ULP_NP_RTC_EN_REG           (ULP_AON_CTL_RB_ADDR + 0x940)
#define ULP_AON_CTL_ULP_NP_RTC_CLK_DIV_L_REG    (ULP_AON_CTL_RB_ADDR + 0x94C)
#define PMU_D_CORE_RTCDR0                       (PMU_D_CORE_RB_ADDR + 0xC00)
#define PMU_D_CORE_RTCDR1                       (PMU_D_CORE_RB_ADDR + 0xC04)
#define PMU_D_CORE_RTCDR2                       (PMU_D_CORE_RB_ADDR + 0xC08)
#define PMU_D_CORE_RTCDR3                       (PMU_D_CORE_RB_ADDR + 0xC0C)
#define PMU_D_CORE_RTCMR0                       (PMU_D_CORE_RB_ADDR + 0xC10)
#define PMU_D_CORE_RTCMR1                       (PMU_D_CORE_RB_ADDR + 0xC14)
#define PMU_D_CORE_RTCMR2                       (PMU_D_CORE_RB_ADDR + 0xC18)
#define PMU_D_CORE_RTCMR3                       (PMU_D_CORE_RB_ADDR + 0xC1C)
#define PMU_D_CORE_RTCLR0                       (PMU_D_CORE_RB_ADDR + 0xC20)
#define PMU_D_CORE_RTCLR1                       (PMU_D_CORE_RB_ADDR + 0xC24)
#define PMU_D_CORE_RTCLR2                       (PMU_D_CORE_RB_ADDR + 0xC28)
#define PMU_D_CORE_RTCLR3                       (PMU_D_CORE_RB_ADDR + 0xC2C)
#define ULP_RTC_MATCH_REG_MASK                  0xFF
#define ULP_RTC_MATCH_REG_LEN                   0x8
#define ULP_RTC_MATCH_REG_LEN1                  0x10
#define ULP_RTC_MATCH_REG_LEN2                  0x18

#define ULP_GPIO_INT_EN_BIT                     0

static uint32_t ulp_rtc_get_mr_count(void)
{
    uint32_t current_count = 0;
    current_count = current_count + readl(PMU_D_CORE_RTCMR0);
    current_count = current_count + (readl(PMU_D_CORE_RTCMR1) << ULP_RTC_MATCH_REG_LEN);
    current_count = current_count + (readl(PMU_D_CORE_RTCMR2) << ULP_RTC_MATCH_REG_LEN1);
    current_count = current_count + (readl(PMU_D_CORE_RTCMR3) << ULP_RTC_MATCH_REG_LEN2);
    return current_count;
}

static void ulp_rtc_handler(void)
{
    UNUSED(ulp_rtc_get_mr_count);
    uint32_t status = osal_irq_lock();
#if defined(PM_SLEEP_DEBUG_ENABLE) && (PM_SLEEP_DEBUG_ENABLE == YES)
    PRINT("[ulp rtc irq]: MR = 0x%x, DR = 0x%x\r\n", ulp_rtc_get_mr_count(), ulp_rtc_get_count());
#endif
    osal_irq_clear(ULP_INT_IRQN);
    osal_irq_restore(status);
}

void ulp_rtc_init(void)
{
    reg16_clrbit(ULP_INT_CFG, ULP_GPIO_INT_EN_BIT);
    osal_irq_request(ULP_INT_IRQN, (osal_irq_handler)ulp_rtc_handler, NULL, NULL, NULL);
    osal_irq_set_priority(ULP_INT_IRQN, irq_prio(ULP_INT_IRQN));
    osal_irq_enable(ULP_INT_IRQN);
    writel(ULP_AON_CTL_ULP_NP_RTC_EN_REG, 0x0); // Disable ulp_rtc.
    writel(ULP_AON_CTL_ULP_NP_RTC_EN_REG, 0x1); // Enable ulp_rtc.
    writel(ULP_AON_CTL_ULP_NP_RTC_CLK_DIV_L_REG, 0x1);  // 0x1: 实际上是2分频
}

void ulp_rtc_deinit(void)
{
    osal_irq_disable(ULP_INT_IRQN);
    osal_irq_free(ULP_INT_IRQN, NULL);
    writel(ULP_AON_CTL_ULP_NP_RTC_EN_REG, 0x0); // Disable ulp_rtc.
}

void ulp_rtc_start(uint32_t time_ms)
{
    uint32_t count = (uint32_t)((uint64_t)time_ms << 0xE) / 1000; // 1s == 1000ms, rtc freq:32768.
    writew(PMU_D_CORE_RTCLR0, 0x0);
    writew(PMU_D_CORE_RTCLR1, 0x0);
    writew(PMU_D_CORE_RTCLR2, 0x0);
    writew(PMU_D_CORE_RTCLR3, 0x0);

    writew(PMU_D_CORE_RTCMR0, count & ULP_RTC_MATCH_REG_MASK);
    count = count >> ULP_RTC_MATCH_REG_LEN;
    writew(PMU_D_CORE_RTCMR1, count & ULP_RTC_MATCH_REG_MASK);
    count = count >> ULP_RTC_MATCH_REG_LEN;
    writew(PMU_D_CORE_RTCMR2, count & ULP_RTC_MATCH_REG_MASK);
    count = count >> ULP_RTC_MATCH_REG_LEN;
    writew(PMU_D_CORE_RTCMR3, count & ULP_RTC_MATCH_REG_MASK);
}

uint64_t ulp_rtc_get_count(void)
{
    uint64_t current_count = 0;
    current_count = current_count + readl(PMU_D_CORE_RTCDR0);
    current_count = current_count + (readl(PMU_D_CORE_RTCDR1) << ULP_RTC_MATCH_REG_LEN);
    current_count = current_count + (readl(PMU_D_CORE_RTCDR2) << ULP_RTC_MATCH_REG_LEN1);
    current_count = current_count + (readl(PMU_D_CORE_RTCDR3) << ULP_RTC_MATCH_REG_LEN2);
    return current_count * 0x2; // 因2分频
}