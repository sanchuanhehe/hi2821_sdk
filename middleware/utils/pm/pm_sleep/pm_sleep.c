/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides sleep source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-09， Create file. \n
 */

#include "securec.h"
#include "soc_osal.h"
#include "pm_veto.h"
#include "pm_dev.h"
#include "pm_fsm.h"
#include "pm_sleep.h"

typedef enum pm_sleep_state {
    PM_SLEEP_STATE_IDLE,
    PM_SLEEP_STATE_GOTO_DS,
    PM_SLEEP_STATE_WKUP_PRE,
} pm_sleep_state_t;

typedef enum {
    SLP_INFO_TIME_BASE = 0,
    SLP_INFO_LIGHTSLEEP,
    SLP_INFO_DEEPSLEEP,
    SLP_INFO_VETO,
    SLP_INFO_MAX,
} slp_record_info_t;

static sleep_info_t g_sleep_info = { 0 };

#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
#define PM_PC_RECORD_NUM             20
static volatile uint32_t g_pc_record[PM_PC_RECORD_NUM] = {0};
static volatile uint32_t g_pc_cnt = 0;
#define PM_PC_COUNT_RECORD0               0
#define PM_PC_COUNT_RECORD1               1
#define PM_PC_COUNT_RECORD2               2
#define PM_PC_COUNT_RECORD3               3
#define PM_PC_COUNT_RECORD4               4
#define PM_PC_COUNT_RECORD5               5
#endif

#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
static uint8_t g_pm_stop_state = 0;
#endif

static pm_sleep_funcs_t g_pm_sleep_funcs = { NULL };
static pm_enter_udsleep_t g_pm_enter_udsleep = NULL;

void uapi_pm_register_sleep_funcs(pm_sleep_funcs_t *funcs)
{
    if (funcs == NULL) {
        return;
    }
    g_pm_sleep_funcs.start_tickless = funcs->start_tickless;
    g_pm_sleep_funcs.stop_tickless = funcs->stop_tickless;
    g_pm_sleep_funcs.get_sleep_ms = funcs->get_sleep_ms;
    g_pm_sleep_funcs.start_wakeup_timer = funcs->start_wakeup_timer;
    g_pm_sleep_funcs.allow_deepsleep = funcs->allow_deepsleep;
    g_pm_sleep_funcs.lightsleep_config = funcs->lightsleep_config;
    g_pm_sleep_funcs.deepsleep_config = funcs->deepsleep_config;
    g_pm_sleep_funcs.light_wakeup_config = funcs->light_wakeup_config;
    g_pm_sleep_funcs.deep_wakeup_config = funcs->deep_wakeup_config;
    g_pm_sleep_funcs.enter_wfi = funcs->enter_wfi;
#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
    g_pm_sleep_funcs.cpu_suspend = funcs->cpu_suspend;
    g_pm_sleep_funcs.cpu_resume = funcs->cpu_resume;
#endif
}

static void pm_tickless_start(void)
{
    if (g_pm_sleep_funcs.start_tickless != NULL) {
        g_pm_sleep_funcs.start_tickless();
    }
}

static void pm_tickless_stop(uint32_t sleep_ms)
{
    if (g_pm_sleep_funcs.stop_tickless != NULL) {
        g_pm_sleep_funcs.stop_tickless(sleep_ms);
    }
}

static uint32_t pm_get_sleep_ms(void)
{
    if (g_pm_sleep_funcs.get_sleep_ms != NULL) {
        return g_pm_sleep_funcs.get_sleep_ms();
    }
    return 0;
}

static void pm_start_wakeup_timer(uint32_t sleep_ms)
{
    if (g_pm_sleep_funcs.start_wakeup_timer != NULL) {
        g_pm_sleep_funcs.start_wakeup_timer(sleep_ms);
    }
}

static void pm_allow_deepsleep(bool allow)
{
    if (g_pm_sleep_funcs.allow_deepsleep != NULL) {
        g_pm_sleep_funcs.allow_deepsleep(allow);
    }
}

static void pm_lightsleep_config(void)
{
    if (g_pm_sleep_funcs.lightsleep_config != NULL) {
        g_pm_sleep_funcs.lightsleep_config();
    }
}

static void pm_deepsleep_config(void)
{
    if (g_pm_sleep_funcs.deepsleep_config != NULL) {
        g_pm_sleep_funcs.deepsleep_config();
    }
}

static void pm_wakeup_config_from_lightsleep(void)
{
    if (g_pm_sleep_funcs.light_wakeup_config != NULL) {
        g_pm_sleep_funcs.light_wakeup_config();
    }
}

static void pm_wakeup_config_from_deepsleep(void)
{
    if (g_pm_sleep_funcs.deep_wakeup_config != NULL) {
        g_pm_sleep_funcs.deep_wakeup_config();
    }
}

static void pm_enter_wfi(void)
{
    if (g_pm_sleep_funcs.enter_wfi != NULL) {
        g_pm_sleep_funcs.enter_wfi();
    }
}

#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
static void pm_cpu_suspend(void)
{
    if (g_pm_sleep_funcs.cpu_suspend != NULL) {
        g_pm_sleep_funcs.cpu_suspend();
    }
}

static void pm_cpu_resume(void)
{
    if (g_pm_sleep_funcs.cpu_resume != NULL) {
        g_pm_sleep_funcs.cpu_resume();
    }
}
#endif

static void pm_sleep_info_record(slp_record_info_t info)
{
#if defined(CONFIG_PM_SLEEP_RECORD_ENABLE) && (CONFIG_PM_SLEEP_RECORD_ENABLE == 1)
    if (info == SLP_INFO_TIME_BASE) {
        g_sleep_info.sleep_base_time = PM_GET_CURRENT_MS;
    } else if (info == SLP_INFO_LIGHTSLEEP) {
        g_sleep_info.sleep_history[PM_SLEEP_LS].total_slp_count++;
        g_sleep_info.sleep_history[PM_SLEEP_LS].total_slp_time +=
            (PM_GET_CURRENT_MS - g_sleep_info.sleep_base_time);
    } else if (info == SLP_INFO_DEEPSLEEP) {
        g_sleep_info.sleep_history[PM_SLEEP_DS].total_slp_count++;
        g_sleep_info.sleep_history[PM_SLEEP_DS].total_slp_time +=
            (PM_GET_CURRENT_MS - g_sleep_info.sleep_base_time);
    } else if (info == SLP_INFO_VETO) {
        g_sleep_info.sleep_veto.last_veto_count = uapi_pm_veto_get_info()->veto_counts.total_counts;
        g_sleep_info.sleep_veto.last_veto_id = uapi_pm_veto_get_info()->last_veto_id;
    }
    g_sleep_info.event.slp_event = PM_GET_SLEEP_EVENT_STATUS;
    g_sleep_info.event.wkup_event = PM_GET_WKUP_EVENT_STATUS;
#else
    unused(info);
#endif
}

static void pm_sleep_prepare(uint8_t sleep_type)
{
    if (sleep_type == PM_SLEEP_LS) {
        pm_lightsleep_config();
    } else if (sleep_type == PM_SLEEP_DS) {
        pm_deepsleep_config();
    }
}

static void pm_wkup_prepare(uint8_t sleep_type)
{
    if (sleep_type == PM_SLEEP_LS) {
        pm_wakeup_config_from_lightsleep();
    } else if (sleep_type == PM_SLEEP_DS) {
        pm_wakeup_config_from_deepsleep();
    }
}

static void pm_enter_light_sleep(void)
{
    pm_sleep_prepare(PM_SLEEP_LS);
    pm_enter_wfi();
    pm_sleep_info_record(SLP_INFO_LIGHTSLEEP);
    pm_wkup_prepare(PM_SLEEP_LS);
}

static void pm_enter_deep_sleep(void)
{
#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
    g_pc_cnt = 0;
    uapi_pm_dev_suspend();
    g_pm_stop_state = PM_SLEEP_STATE_GOTO_DS;
    pm_sleep_prepare(PM_SLEEP_DS);
    g_pc_record[g_pc_cnt++] = PM_PC_COUNT_RECORD0;
    pm_cpu_suspend();
    g_pc_record[g_pc_cnt++] = PM_PC_COUNT_RECORD1;
    if (g_pm_stop_state == PM_SLEEP_STATE_GOTO_DS) {
        g_pc_record[g_pc_cnt++] = PM_PC_COUNT_RECORD2;
        g_pm_stop_state = PM_SLEEP_STATE_WKUP_PRE;
        pm_allow_deepsleep(true);
        pm_enter_wfi();
        g_pc_record[g_pc_cnt++] = PM_PC_COUNT_RECORD3;
        pm_allow_deepsleep(false);
        pm_cpu_resume();
        uapi_pm_dev_resume();
    } else if (g_pm_stop_state == PM_SLEEP_STATE_WKUP_PRE) {
        pm_allow_deepsleep(false);
        pm_cpu_resume();
        uapi_pm_dev_resume();
    }
    g_pc_record[g_pc_cnt++] = PM_PC_COUNT_RECORD4;
    while (g_pc_cnt != PM_PC_COUNT_RECORD5) {;}
#else
    pm_sleep_prepare(PM_SLEEP_DS);
    pm_allow_deepsleep(true);
    pm_enter_wfi();
    pm_allow_deepsleep(false);
#endif
    pm_wkup_prepare(PM_SLEEP_DS);
    pm_sleep_info_record(SLP_INFO_DEEPSLEEP);
}

void uapi_pm_enter_sleep(void)
{
    uint32_t status = osal_irq_lock();
    osal_kthread_lock();
    if (uapi_pm_get_sleep_veto()) {
        pm_enter_wfi();
        osal_kthread_unlock();
        osal_irq_restore(status);
        return;
    }
    uint32_t sleep_ms = pm_get_sleep_ms();
    if (sleep_ms <= CONFIG_PM_LIGHT_SLEEP_THRESHOLD_MS) {
        pm_enter_wfi();
        osal_kthread_unlock();
        osal_irq_restore(status);
        return;
    }

    pm_sleep_info_record(SLP_INFO_TIME_BASE);
    pm_sleep_info_record(SLP_INFO_VETO);

    pm_tickless_start();
    pm_start_wakeup_timer(sleep_ms);
    if (sleep_ms <= CONFIG_PM_DEEP_SLEEP_THRESHOLD_MS) {
        pm_enter_light_sleep();
    } else {
        pm_enter_deep_sleep();
    }
    pm_tickless_stop(sleep_ms);

    osal_kthread_unlock();
    osal_irq_restore(status);
}

sleep_info_t *uapi_pm_get_sleep_info(void)
{
    return &g_sleep_info;
}

void uapi_pm_register_enter_udsleep_func(pm_enter_udsleep_t func)
{
    g_pm_enter_udsleep = func;
}

void uapi_pm_enter_udsleep(void)
{
    if (g_pm_enter_udsleep != NULL) {
        g_pm_enter_udsleep();
    }
}
