/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  LOW POWER CONTROLLER HAL.
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#include "hal_lpc.h"
#include "core.h"
#include "arch_barrier.h"

/**
 * Explanation:
 * In iflash.c the security core has a function that attempts to do flash writes.
 * It first checks that both cores are either disabled, or in deep sleep.
 * It then check each core individually to limit the false-positive window, before disabling the core.
 * If the wakes up after the check, but before it gets disabled then it will lose the information in its
 * flash bus - reading it as 0, and skipping it.
 * Because of the risk of instructions getting skipped we have added in NOPs, and two ISB instructions
 * This allows us to lose our bus, without then failing.
 */
void hal_lpc_enter_wfi(void)
{
    dsb();

    wfi();

    isb();
    nop();
    nop();
    nop();
    nop();
}

/* Set a type of sleep mode */
void hal_lpc_set_sleep_mode(hal_lpc_sleep_mode_t mode)
{
#if (ARCH == CM3) || (ARCH == CM7)
    if (mode == HAL_LPC_SLEEP_MODE_LIGHT) {
        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    } else {  // sleep deep
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    }
#else
    UNUSED(mode);
#endif
}

/* Set the sleep on exit mode */
void hal_lpc_set_sleep_on_exit(bool sleep_on_exit)
{
#if (ARCH == CM3) || (ARCH == CM7)
    if (sleep_on_exit) {
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    } else {
        SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    }
#else
    UNUSED(sleep_on_exit);
#endif
}
