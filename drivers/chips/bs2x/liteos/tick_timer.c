/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: RISCV31 Machine timer config for LiteOS
 * Author: @CompanyNameTag
 * Create: 2021-9-27
 */
#include "hal_hwi.h"
#include "los_hwi.h"
#include "los_tick_pri.h"
#include "chip_io.h"
#include "arch_barrier.h"
#include "oal_interface.h"
#include "osal_interrupt.h"
#include "osal_timer.h"
#include "platform_core.h"
#include "idle_config.h"
#include "interrupt.h"
#include "tcxo.h"
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "pm_sys.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MACHINE_INT_NUM                 NUM_HAL_INTERRUPT_TIMER

#define MACHINE_TIMER_BASE              TICK_TIMER_BASE_ADDR
#define MACHINE_TIMER_LOAD_COUNT        0x00
#define MACHINE_TIMER_LOAD_COUNT_L      0x00
#define MACHINE_TIMER_LOAD_COUNT_H      0x04
#define MACHINE_TIMER_CURRENT_VALUE_L   0x08
#define MACHINE_TIMER_CURRENT_VALUE_H   0x0C
#define MACHINE_TIMER_CONTROL_REG       0x10
#define MACHINE_TIMER_EOI               0x14
#define MACHINE_TIMER_INT_STATUS        0x18

#define MACHINE_TIMER_ENABLE_USRMODE    0x03


#define OS_CYCLE_PER_TICK_PRI      ((OS_SYS_CLOCK) / (LOSCFG_BASE_CORE_TICK_PER_SECOND))

#define MACHINE_TIMER_CNT_LOCK_REQ_BIT  5
#define MACHINE_TIMER_CNT_LOCKED_BIT    6
#define MACHINE_TIMER_CNT_LOCK_TIMEOUT  0xFFFF

static inline void machine_timer_init(void)
{
    // before config timer, disable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), 0);

    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK_PRI);
    // user define mode and enable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

void os_tick_timer_disable(void)
{
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), 0);
}

void os_tick_timer_enable(void)
{
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT), OS_CYCLE_PER_TICK_PRI);
    // user define mode and enable it
    writel((MACHINE_TIMER_BASE + MACHINE_TIMER_CONTROL_REG), MACHINE_TIMER_ENABLE_USRMODE);
}

static inline uint32_t machine_timer_get_cur_count(void)
{
    uint32_t timeout = 0;

    /* request lock timer current count */
    reg_setbit(MACHINE_TIMER_BASE, MACHINE_TIMER_CONTROL_REG, MACHINE_TIMER_CNT_LOCK_REQ_BIT);

    /* wait lock succ */
    while (reg_getbits(MACHINE_TIMER_BASE, MACHINE_TIMER_CONTROL_REG, MACHINE_TIMER_CNT_LOCKED_BIT, 1) == 0) {
        timeout++;
        if (timeout > MACHINE_TIMER_CNT_LOCK_TIMEOUT) {
            return UINT32_MAX;
        }
    }
    return readl(MACHINE_TIMER_BASE + MACHINE_TIMER_CURRENT_VALUE_L);
}

static inline uint32_t machine_timer_get_load_count(void)
{
    return readl(MACHINE_TIMER_BASE + MACHINE_TIMER_LOAD_COUNT);
}

void HalTickEntry(void)
{
    // clear the timer interrupt
    writel(MACHINE_TIMER_BASE + MACHINE_TIMER_EOI, 1);
    int_clear_pending_irq(MACHINE_INT_NUM);
    OsTickHandler();
#if defined(CONFIG_PM_SYS_SUPPORT)
    pm_tick_handler_entry();
#endif
}

LITE_OS_SEC_TEXT_INIT void HalClockStart(void)
{
    uint32_t ret;
    uint32_t lock_int_save;

    if ((OS_SYS_CLOCK == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND == 0) ||
        (OS_SYS_CLOCK < LOSCFG_BASE_CORE_TICK_PER_SECOND)) {
        return;
    }

    lock_int_save = osal_irq_lock();
    ret = osal_irq_request(MACHINE_INT_NUM, (osal_irq_handler)HalTickEntry, NULL, NULL, NULL);
    osal_irq_set_priority(MACHINE_INT_NUM,  OS_HWI_PRIO_LOWEST);
    osal_irq_enable(MACHINE_INT_NUM);
    if (ret != 0) {
        PRINT_ERR("%s %d: oal_int_create ret:0x%x\n", __FUNCTION__, __LINE__, ret);
    }

    machine_timer_init();
    idle_task_config();
    osal_irq_restore(lock_int_save);
}

void HalClockInit(void)
{
    SET_SYS_CLOCK(OS_SYS_CLOCK);
}

void HalDelayUs(uint32_t usecs)
{
    uint64_t tmo = osal_sched_clock() + (uint64_t)usecs * OS_SYS_NS_PER_US;

    while (osal_sched_clock() < tmo) {
        nop();
    }
}

uint64_t HalClockGetCycles(void)
{
    uint64_t tick_count;
    uint64_t cycle;
    uint32_t cycle_in_tick;
    uint32_t lock_int_save;
    uint32_t load_tick;
    uint32_t cur_tick;

    lock_int_save = osal_irq_lock();
    tick_count = osal_get_jiffies();
    load_tick = machine_timer_get_load_count();
    cur_tick = machine_timer_get_cur_count();
    uint8_t isPending = 0;
    if ((HalIrqPendingGet(MACHINE_INT_NUM, &isPending) == LOS_OK) && (isPending != 0)) {
        cur_tick = machine_timer_get_cur_count();
        tick_count++;
    }
    cycle_in_tick = load_tick - cur_tick;
    cycle = tick_count * load_tick + cycle_in_tick;
    osal_irq_restore(lock_int_save);
    return cycle;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
