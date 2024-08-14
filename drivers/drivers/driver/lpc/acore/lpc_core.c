/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  LPC CORE SPECIFIC FUNCTIONS
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#include "soc_osal.h"
#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
#include "los_hwi.h"
#endif
#include "non_os.h"
#if (CORE_NUMS > 1)
#include "ipc.h"
#endif
#if (CORE_NUMS > 1)
#include "pmu_cmu.h"
#endif
#include "platform_driver.h"
#include "panic.h"
#include "lpc_core.h"

/** Counts the number of deep sleep vetos set */
static volatile lpc_veto_type g_lpc_sys_sleep_vetos[LPC_ID_MAX] = { 0 };
static volatile uint32_t g_lpc_sys_deepsleep_vetos = 0;
static volatile uint32_t g_lpc_mcpu_i2c_sleep_vetos = 0;
static volatile uint32_t g_lpc_mcpu_mprep_sleep_vetos = 0;

static lpc_fsm_callback_handler g_lpc_fsm_callback_handlers[LPC_ID_MAX][LPC_CONFIGURATION_MAX];

/*
 * Add a callback to the handler array.
 */
void lpc_fsm_callback_register_handler(lpc_sleep_id_e id, lpc_work_states_e state, lpc_fsm_callback_handler handler)
{
    if (id >= LPC_ID_MAX) {
        return;
    }

    if (state >= LPC_CONFIGURATION_MAX) {
        return;
    }

    if (handler == NULL) {
        return;
    }

    // Simply update the entry.
    g_lpc_fsm_callback_handlers[id][state] = handler;
}

void lpc_work_state_callback(lpc_sleep_id_e id, lpc_work_states_e config)
{
    if (id >= LPC_ID_MAX) {
        return;
    }

    if (config >= LPC_CONFIGURATION_MAX) {
        return;
    }

    if (g_lpc_fsm_callback_handlers[id][config] != NULL) {
        g_lpc_fsm_callback_handlers[id][config]();
    }
}

void lpc_sys_sleep_veto(lpc_sleep_id_e sys_id)
{
    if (sys_id >= LPC_ID_MAX) {
        return;
    }

    uint32_t lr = __return_address();
    uint32_t irq_sts = osal_irq_lock();

    // if not running with asserts at least don't wrap
    if (g_lpc_sys_sleep_vetos[sys_id].lpc_sleep_vetos < UINT16_MAX) {
        g_lpc_sys_sleep_vetos[sys_id].lpc_sleep_vetos++;
        g_lpc_sys_sleep_vetos[sys_id].lr_pointer = lr;
    } else {
        panic(PANIC_LPC_VETO, __LINE__);
    }
    osal_irq_restore(irq_sts);
}

void lpc_sys_remove_sleep_veto(lpc_sleep_id_e sys_id)
{
    if (sys_id >= LPC_ID_MAX) {
        return;
    }

    uint32_t irq_sts = osal_irq_lock();

    // if not running with asserts at least don't wrap
    if (g_lpc_sys_sleep_vetos[sys_id].lpc_sleep_vetos > 0) {
        g_lpc_sys_sleep_vetos[sys_id].lpc_sleep_vetos--;
    } else {
        panic(PANIC_LPC_VETO, __LINE__);
    }
    osal_irq_restore(irq_sts);
}

uint16_t lpc_get_id_sleep_veto(lpc_sleep_id_e sys_id)
{
    if (sys_id >= LPC_ID_MAX) {
        panic(PANIC_LPC, __LINE__);
        return (uint16_t)sys_id;
    }

    return g_lpc_sys_sleep_vetos[sys_id].lpc_sleep_vetos;
}

uint32_t lpc_get_id_sleep_lr(lpc_sleep_id_e sys_id)
{
    if (sys_id >= LPC_ID_MAX) {
        panic(PANIC_LPC, __LINE__);
        return (uint16_t)sys_id;
    }

    return g_lpc_sys_sleep_vetos[sys_id].lr_pointer;
}

uint32_t lpc_get_sleep_veto(void)
{
    uint8_t sys_id;
    uint16_t sleep_vote;
    g_lpc_sys_deepsleep_vetos = 0;

    for (sys_id = (uint8_t)LPC_ID_A2B; sys_id < (uint8_t)LPC_ID_MAX; sys_id++) {
        sleep_vote = g_lpc_sys_sleep_vetos[sys_id].lpc_sleep_vetos;
        g_lpc_sys_deepsleep_vetos += sleep_vote;
    }

    return g_lpc_sys_deepsleep_vetos;
}

void lpc_mcpu_i2c_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();
    g_lpc_mcpu_i2c_sleep_vetos++;
    osal_irq_restore(irq_sts);
}

void lpc_mcpu_i2c_remove_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();
    // if not running with asserts at least don't wrap
    if (g_lpc_mcpu_i2c_sleep_vetos > 0) {
        g_lpc_mcpu_i2c_sleep_vetos--;
    }
    osal_irq_restore(irq_sts);
}

uint32_t lpc_get_mcpu_i2c_sleep_veto(void)
{
    return g_lpc_mcpu_i2c_sleep_vetos;
}

volatile lpc_veto_type *lpc_get_sys_sleep_veto(void)
{
    return g_lpc_sys_sleep_vetos;
}

void lpc_mcpu_mprep_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();
    g_lpc_mcpu_mprep_sleep_vetos++;
    osal_irq_restore(irq_sts);
}

void lpc_mcpu_mprep_remove_sleep_veto(void)
{
    uint32_t irq_sts = osal_irq_lock();
    // if not running with asserts at least don't wrap
    if (g_lpc_mcpu_mprep_sleep_vetos > 0) {
        g_lpc_mcpu_mprep_sleep_vetos--;
    }
    osal_irq_restore(irq_sts);
}

uint32_t lpc_get_mcpu_mprep_sleep_veto(void)
{
    return g_lpc_mcpu_mprep_sleep_vetos;
}

#if (CORE_NUMS > 1) && defined(IPC_NEW)
#elif (CORE_NUMS > 1)
void lpc_wakeup_trigger(void)
{
    ipc_status_t ipc_returned_value;

    ipc_returned_value = ipc_spin_send_message_timeout(CORES_BT_CORE,
                                                       IPC_ACTION_WAKUP_INFO,
                                                       NULL,
                                                       0,
                                                       IPC_PRIORITY_HIGHEST,
                                                       false,
                                                       // 'Payload' in output_message is an IPC command
                                                       IPC_SPIN_SEND_DEFAULT_TIMEOUT);
    if (ipc_returned_value != IPC_STATUS_OK) {
        panic(PANIC_LPC_WKUP_FAIL, ipc_returned_value);
    }
}
#endif

static void platform_suspend(void)
{
    platform_device_suspend();
}

static void platform_resume(void)
{
    platform_device_resume();
}

/* Allows the routing of the clock */
void lpc_core_init_configuration(void)
{
#if (CORE_NUMS > 1)
    hal_lpc_init();
#endif
    UNUSED(platform_suspend);
    UNUSED(platform_resume);
}
