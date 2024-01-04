/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  LPC DRIVER
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#include "lpc.h"
#include "hal_lpc.h"
#include "non_os.h"
#include "soc_osal.h"
#include "rtc.h"
#include "panic.h"
#include "arch_port.h"

#include "lpc_core.h"
#if (CORE_NUMS > 1)
#include "watchdog.h"
#endif
#if (CORE == MASTER_BY_ALL)
#include "watchdog.h"
#include "preserve.h"
#endif
#include "watchdog_porting.h"
#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
#include "los_hwi.h"
#endif
#include "lpc.h"

#define LPC_VETO_SLEEP_TIME_MS 25
#define LPC_WAIT_TIME_MS 15000
#define LPC_WAIT_TIME_SEC_TO_MS 1000

/*
 *  Private variable declarations
 */
/** The lpc wake-up timer handle */
static rtc_handle_t g_lpc_wakeup_timer_handle = NULL;
/** Counts the number of sleep vetos set */
static volatile uint16_t g_lpc_sleep_vetos;
/** Counts the number of deep sleep vetos set */
static volatile uint16_t g_lpc_deep_sleep_vetos;

/*
 *  Private function declarations
 */
/**
 * Set the configuration common to all the lpc_sleep_configuration_e configurations
 */
static void lpc_set_common_configuration(void);

/**
 * RTC callback for the Low Power Controller
 */
static void lpc_wakeup_timer_callback(void);

/*
  *  Public function definitions
  */
/* Initialise the Low Power Controller */
void lpc_init(void)
{
    if (non_os_is_driver_initialised(DRIVER_INIT_LPC) == true) {
        return;
    }
    if (non_os_is_driver_initialised(DRIVER_INIT_RTC) != true) {
        uapi_rtc_init();
        uapi_rtc_adapter(RTC_0, RTC_0_IRQN, g_aucIntPri[RTC_0_IRQN]);
    }

    lpc_set_common_configuration();

    uint32_t irq_sts = osal_irq_lock();
    g_lpc_sleep_vetos = 0;
    g_lpc_deep_sleep_vetos = 0;
    non_os_set_driver_initalised(DRIVER_INIT_LPC, true);
    osal_irq_restore(irq_sts);
}

/* Deinitialise LPC */
void lpc_deinit(void)
{
    non_os_set_driver_initalised(DRIVER_INIT_LPC, false);
}

void lpc_add_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();
    // if not running with asserts at least don't wrap
    if (g_lpc_sleep_vetos < UINT16_MAX) {
        g_lpc_sleep_vetos++;
    } else {
        panic(PANIC_LPC_VETO, __LINE__);
    }
    osal_irq_restore(irq_sts);
}

void lpc_remove_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();

    // if not running with asserts at least don't wrap
    if (g_lpc_sleep_vetos > 0) {
        g_lpc_sleep_vetos--;
    } else {
        panic(PANIC_LPC_VETO, __LINE__);
    }
    osal_irq_restore(irq_sts);
}

void lpc_add_deep_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();

    // if not running with asserts at least don't wrap
    if (g_lpc_deep_sleep_vetos < UINT16_MAX) {
        g_lpc_deep_sleep_vetos++;
    } else {
        panic(PANIC_LPC_VETO, __LINE__);
    }
    osal_irq_restore(irq_sts);
}

void lpc_remove_deep_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();

    // if not running with asserts at least don't wrap
    if (g_lpc_deep_sleep_vetos > 0) {
        g_lpc_deep_sleep_vetos--;
    } else {
        panic(PANIC_LPC_VETO, __LINE__);
    }
    osal_irq_restore(irq_sts);
}

/* Set the default and the common parameters */
static void lpc_set_common_configuration(void)
{
    // Set the default and the common parameters
    hal_lpc_set_sleep_mode(HAL_LPC_SLEEP_MODE_LIGHT);
    // Set the common configuration to all sleep modes
    hal_lpc_set_sleep_on_exit(false);
    lpc_core_init_configuration();
}

/* lpc timer callback to indicate that the timer has expired */
static void lpc_wakeup_timer_callback(void)
{
}

/* Adds a timer to be used for waking us up from sleep */
void lpc_start_wakeup_timer(uint32_t period_in_ms)
{
    uint32_t wakeup_time = period_in_ms;
    uint32_t wdg_left_time;
#if (CORE == MASTER_BY_ALL)
    if (uapi_watchdog_enable(WDT_MODE_INTERRUPT) != ERRCODE_SUCC) {
        uapi_watchdog_get_left_time(&wdg_left_time);
        if (wakeup_time >= ((wdg_left_time - 1) * LPC_WAIT_TIME_SEC_TO_MS)) {
            panic(PANIC_LPC_WAKEUP_TIME, period_in_ms);
        }
    }
#else
    uapi_watchdog_get_left_time(&wdg_left_time);
    if ((wdg_left_time != 0) && (wakeup_time >= wdg_left_time)) {
        wakeup_time = wdg_left_time >> 1U;
    }
#endif

    if (!g_lpc_wakeup_timer_handle) {
        uapi_rtc_create(RTC_0, &g_lpc_wakeup_timer_handle);
        uapi_rtc_start(g_lpc_wakeup_timer_handle, wakeup_time, (rtc_callback_t)lpc_wakeup_timer_callback, 0);
    } else {
        uapi_rtc_start(g_lpc_wakeup_timer_handle, wakeup_time, (rtc_callback_t)lpc_wakeup_timer_callback, 0);
    }

#if CORE == MASTER_BY_ALL
    set_excepted_sleep_time(wakeup_time);
#endif
}

void lpc_enter_wfi(void)
{
    hal_lpc_enter_wfi();
}

void lpc_set_sleep_mode(lpc_sleep_mode_e mode)
{
    if (mode == LPC_SLEEP_MODE_LIGHT) {
        hal_lpc_set_sleep_mode(HAL_LPC_SLEEP_MODE_LIGHT);
    } else {
        hal_lpc_set_sleep_mode(HAL_LPC_SLEEP_MODE_DEEP);
    }
}
