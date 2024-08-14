/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm sys API \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-28， Create file. \n
 */

#include "los_tick_pri.h"
#include "soc_osal.h"
#include "pm_veto.h"
#include "app_init.h"
#include "osal_debug.h"
#include "osal_msgqueue.h"
#include "pm_sys.h"

#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
#define TASK_PRIORITY_PM_SYS    28
#define PM_SYS_STACK_SIZE       0x600
#define PM_SYS_MSG_COUNT        5
static uint64_t g_pm_sys_queue;

typedef struct pm_notify_mail {
    uint8_t pm_state;
} pm_notify_mail_t;
#endif

#define PM_STATE_WORK       0
#define PM_STATE_STANDBY    1
#define PM_STATE_SLEEP  2

typedef enum pm_state_trans {
    PM_STATE_WORK_TO_STANDBY = 0,
    PM_STATE_STANDBY_TO_SLEEP = 1,
    PM_STATE_STANDBY_TO_WORK = 2,
    PM_STATE_SLEEP_TO_WORK = 3,
    PM_STATE_TRANS_NUM = 4,
} pm_state_trans_t;

#define PM_CPU_CUR_TICK_CNT         (g_tickCount[ArchCurrCpuid()])

static pm_state_trans_func_t g_pm_state_trans_func[PM_STATE_TRANS_NUM] = { NULL };
static uint8_t g_pm_cur_state = PM_STATE_WORK;
static uint64_t g_pm_work_base_tick_cnt = 0;
static uint64_t g_pm_standby_base_tick_cnt = 0;

static uint32_t g_pm_work_duration_ms = 0xFFFFFFFF;
static uint32_t g_pm_standby_duration_ms = 0xFFFFFFFF;

errcode_t uapi_pm_state_trans_handler_register(pm_state_trans_handler_t *handler)
{
    g_pm_state_trans_func[PM_STATE_WORK_TO_STANDBY] = handler->work_to_standby;
    g_pm_state_trans_func[PM_STATE_STANDBY_TO_SLEEP] = handler->standby_to_sleep;
    g_pm_state_trans_func[PM_STATE_STANDBY_TO_WORK] = handler->standby_to_work;
    g_pm_state_trans_func[PM_STATE_SLEEP_TO_WORK] = handler->sleep_to_work;
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_set_state_trans_duration(uint32_t work_to_standby, uint32_t standby_to_sleep)
{
    g_pm_work_duration_ms = work_to_standby;
    g_pm_standby_duration_ms = standby_to_sleep;
    return ERRCODE_SUCC;
}

static void pm_state_work_to_standby(void)
{
    uint32_t status = osal_irq_lock();
    if (g_pm_state_trans_func[PM_STATE_WORK_TO_STANDBY] != NULL) {
#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
        pm_notify_mail_t mail = {0};
        mail.pm_state = PM_STATE_WORK_TO_STANDBY;
        if (osal_msg_queue_write_copy(g_pm_sys_queue, (void *)&mail, sizeof(pm_notify_mail_t), 0) != OSAL_SUCCESS) {
            osal_printk("[pm_sys]pm state %d send failed\n", (uint32_t)PM_STATE_WORK_TO_STANDBY);
        }
#else
        g_pm_state_trans_func[PM_STATE_WORK_TO_STANDBY](0);
        uapi_pm_remove_sleep_veto(PM_VETO_ID_SYS);
        osal_printk("[pm_sys]from work to standby.\n");
#endif
    }
    g_pm_cur_state = PM_STATE_STANDBY;
    g_pm_standby_base_tick_cnt = PM_CPU_CUR_TICK_CNT;
    osal_irq_restore(status);
}

static void pm_state_standby_to_sleep(void)
{
    uint32_t status = osal_irq_lock();
    if (g_pm_state_trans_func[PM_STATE_STANDBY_TO_SLEEP] != NULL) {
#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
        pm_notify_mail_t mail = {0};
        mail.pm_state = PM_STATE_STANDBY_TO_SLEEP;
        if (osal_msg_queue_write_copy(g_pm_sys_queue, (void *)&mail, sizeof(pm_notify_mail_t), 0) != OSAL_SUCCESS) {
            osal_printk("[pm_sys]pm state %d send failed\n", (uint32_t)PM_STATE_STANDBY_TO_SLEEP);
        }
#else
        uapi_pm_add_sleep_veto(PM_VETO_ID_SYS); // 睡眠否决：防止在处理期间CPU让出期间进睡眠
        g_pm_state_trans_func[PM_STATE_STANDBY_TO_SLEEP](0);
        uapi_pm_remove_sleep_veto(PM_VETO_ID_SYS);
        osal_printk("[pm_sys]from standby to sleep.\n");
#endif
    }
    g_pm_cur_state = PM_STATE_SLEEP;
    osal_irq_restore(status);
}

static void pm_state_standby_to_work(void)
{
    uint32_t status = osal_irq_lock();
    g_pm_work_base_tick_cnt = PM_CPU_CUR_TICK_CNT;
    g_pm_cur_state = PM_STATE_WORK;
    if (g_pm_state_trans_func[PM_STATE_STANDBY_TO_WORK] != NULL) {
#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
        pm_notify_mail_t mail = {0};
        mail.pm_state = PM_STATE_STANDBY_TO_WORK;
        if (osal_msg_queue_write_copy(g_pm_sys_queue, (void *)&mail, sizeof(pm_notify_mail_t), 0) != OSAL_SUCCESS) {
            osal_printk("[pm_sys]pm state %d send failed\n", (uint32_t)PM_STATE_STANDBY_TO_WORK);
        }
#else
        uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
        g_pm_state_trans_func[PM_STATE_STANDBY_TO_WORK](0);
        osal_printk("[pm_sys]from standby to wrok.\n");
#endif
    }
    osal_irq_restore(status);
}

static void pm_state_sleep_to_work(void)
{
    uint32_t status = osal_irq_lock();
    g_pm_work_base_tick_cnt = PM_CPU_CUR_TICK_CNT;
    g_pm_cur_state = PM_STATE_WORK;
    if (g_pm_state_trans_func[PM_STATE_SLEEP_TO_WORK] != NULL) {
#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
        pm_notify_mail_t mail = {0};
        mail.pm_state = PM_STATE_SLEEP_TO_WORK;
        if (osal_msg_queue_write_copy(g_pm_sys_queue, (void *)&mail, sizeof(pm_notify_mail_t), 0) != OSAL_SUCCESS) {
            osal_printk("[pm_sys]pm state %d send failed\n", (uint32_t)PM_STATE_SLEEP_TO_WORK);
        }
#else
        uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
        g_pm_state_trans_func[PM_STATE_SLEEP_TO_WORK](0);
        osal_printk("[pm_sys]from sleep to wrok.\n");
#endif
    }
    osal_irq_restore(status);
}

errcode_t uapi_pm_work_state_reset(void)
{
    g_pm_work_base_tick_cnt = PM_CPU_CUR_TICK_CNT;
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_wkup_process(uintptr_t arg)
{
    unused(arg);
    switch (g_pm_cur_state) {
        case PM_STATE_STANDBY:
            pm_state_standby_to_work();
            break;

        case PM_STATE_SLEEP:
            pm_state_sleep_to_work();
            break;

        default:
            break;
    }
    return ERRCODE_SUCC;
}

void pm_tick_handler_entry(void)
{
    switch (g_pm_cur_state) {
        case PM_STATE_WORK:
            if ((PM_CPU_CUR_TICK_CNT - g_pm_work_base_tick_cnt) >= g_pm_work_duration_ms) {
                pm_state_work_to_standby();
            }
            break;

        case PM_STATE_STANDBY:
            if ((PM_CPU_CUR_TICK_CNT - g_pm_standby_base_tick_cnt) >= g_pm_standby_duration_ms) {
                pm_state_standby_to_sleep();
            }
            break;

        default:
            break;
    }
}

#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
static void pm_sys_task(void const *arg)
{
    unused(arg);
    int32_t ret;
    pm_notify_mail_t mail;
    uint32_t msgsize = (uint32_t)sizeof(pm_notify_mail_t);

    while (1) {
        ret = osal_msg_queue_read_copy(g_pm_sys_queue, (void *)&mail, &msgsize,
                                       OSAL_MSGQ_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            continue;
        }

        switch (mail.pm_state) {
            case PM_STATE_WORK_TO_STANDBY:
                g_pm_state_trans_func[PM_STATE_WORK_TO_STANDBY](0);
                uapi_pm_remove_sleep_veto(PM_VETO_ID_SYS);
                break;

            case PM_STATE_STANDBY_TO_SLEEP:
                uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
                g_pm_state_trans_func[PM_STATE_STANDBY_TO_SLEEP](0);
                uapi_pm_remove_sleep_veto(PM_VETO_ID_SYS);
                break;

            case PM_STATE_STANDBY_TO_WORK:
                uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
                g_pm_state_trans_func[PM_STATE_STANDBY_TO_WORK](0);
                break;

            case PM_STATE_SLEEP_TO_WORK:
                uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
                g_pm_state_trans_func[PM_STATE_SLEEP_TO_WORK](0);
                break;

            default:
                break;
        }
        osal_printk("[pm_sys]mail.state switch: %d\n", mail.pm_state);
    }
}
#endif

void pm_sys_entry(void)
{
#if defined(CONFIG_PM_SYS_SUPPORT_MSGQUEUE)
    if (osal_msg_queue_create("pm_sys_queue", PM_SYS_MSG_COUNT, (unsigned long *)&g_pm_sys_queue,
                              0, sizeof(pm_notify_mail_t)) != ERRCODE_SUCC) {
        osal_printk("[pm_sys] pm_sys create mail fail\n");
        return;
    }


    osal_task *task_handle = osal_kthread_create((osal_kthread_handler)pm_sys_task, NULL, "pm_sys", PM_SYS_STACK_SIZE);
    if (task_handle == NULL) {
        osal_msg_queue_delete(g_pm_sys_queue);
        osal_printk("[pm_sys] pm_sys create thread fail\n");
        return;
    }
    osal_kthread_set_priority(task_handle, TASK_PRIORITY_PM_SYS);
#endif
    // 投否决票，默认不进睡眠
    uapi_pm_add_sleep_veto(PM_VETO_ID_SYS);
    uapi_pm_remove_sleep_veto(PM_VETO_ID_MCU);
    g_pm_work_base_tick_cnt = PM_CPU_CUR_TICK_CNT;
    osal_printk("[pm_sys]pm_sys_entry.\n");
}

app_run(pm_sys_entry);