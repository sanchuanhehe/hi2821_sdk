/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Tick
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "los_tick_pri.h"
#include "los_swtmr_pri.h"
#include "los_task_pri.h"
#include "los_sched_pri.h"
#include "los_init.h"

LITE_OS_SEC_BSS volatile UINT64 g_tickCount[LOSCFG_KERNEL_CORE_NUM] = {0};
LITE_OS_SEC_DATA_INIT UINT32 g_sysClock;
LITE_OS_SEC_DATA_INIT UINT32 g_tickPerSecond;
LITE_OS_SEC_BSS DOUBLE g_cycle2NsScale;

/* spinlock for task module */
LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_tickSpin);

/*
 * Description : Tick interruption handler
 */
LITE_OS_SEC_TEXT VOID OsTickHandler(VOID)
{
    UINT32 intSave;

    TICK_LOCK(intSave);
    g_tickCount[ArchCurrCpuid()]++;
    TICK_UNLOCK(intSave);

#ifdef LOSCFG_BASE_CORE_TIMESLICE
    OsTimesliceCheck();
#endif

    OsTaskScan(); /* task timeout scan */

#ifdef LOSCFG_BASE_CORE_SWTMR
    OsSwtmrScan();
#endif
}

LITE_OS_SEC_TEXT_INIT UINT32 OsTickInit(VOID)
{
    if ((GET_SYS_CLOCK() == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND == 0) ||
        (LOSCFG_BASE_CORE_TICK_PER_SECOND > GET_SYS_CLOCK())) {
        return LOS_ERRNO_TICK_CFG_INVALID;
    }

    HalClockInit();

    return LOS_OK;
}
LOS_SYS_INIT(OsTickInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_2);

LITE_OS_SEC_TEXT_INIT VOID OsTickStart(VOID)
{
    HalClockStart();
}

LITE_OS_SEC_TEXT_MINOR UINT64 LOS_TickCountGet(VOID)
{
    UINT32 intSave;
    UINT64 tick;

    /*
     * use core0's tick as system's timeline,
     * the tick needs to be atomic.
     */
    TICK_LOCK(intSave);
    tick = g_tickCount[0];
    TICK_UNLOCK(intSave);

    return tick;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_CyclePerTickGet(VOID)
{
    return g_sysClock / KERNEL_TICK_PER_SECOND;
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_GetCpuCycle(UINT32 *highCnt, UINT32 *lowCnt)
{
    UINT64 cycle;

    if ((highCnt == NULL) || (lowCnt == NULL)) {
        return;
    }
    cycle = HalClockGetCycles();

    /* get the high 32 bits */
    *highCnt = (UINT32)(cycle >> 32);
    /* get the low 32 bits */
    *lowCnt = (UINT32)(cycle & 0xFFFFFFFFULL);
}

LITE_OS_SEC_TEXT_MINOR UINT64 LOS_CurrNanosec(VOID)
{
    UINT64 nanos;
    UINT64 cycle = HalClockGetCycles();
    nanos = (cycle / g_sysClock) * (UINT64)OS_SYS_NS_PER_SECOND;
    nanos += cycle % g_sysClock * (UINT64)OS_SYS_NS_PER_SECOND / g_sysClock;
    return nanos;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_MS2Tick(UINT32 millisec)
{
    UINT64 temp;
    UINT64 delayTick;

    if (millisec == UINT32_MAX) {
        return UINT32_MAX;
    }

    temp = (UINT64)millisec * KERNEL_TICK_PER_SECOND;
    delayTick = (temp + OS_SYS_MS_PER_SECOND - 1) / OS_SYS_MS_PER_SECOND;

    /* Returns UINT32_MAX if flipping occurs */
    if (delayTick > UINT32_MAX) {
        return UINT32_MAX;
    } else {
        return (UINT32)delayTick;
    }
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_Tick2MS(UINT32 tick)
{
    UINT64 delayMs;

    delayMs = ((UINT64)tick * OS_SYS_MS_PER_SECOND) / KERNEL_TICK_PER_SECOND;

    /* Returns UINT32_MAX if flipping occurs */
    if (delayMs > UINT32_MAX) {
        return UINT32_MAX;
    } else {
        return (UINT32)delayMs;
    }
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_Udelay(UINT32 usecs)
{
    HalDelayUs(usecs);
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_Mdelay(UINT32 msecs)
{
    UINT32 delayUs = (UINT32_MAX / OS_SYS_US_PER_MS) * OS_SYS_US_PER_MS;

    while (msecs > UINT32_MAX / OS_SYS_US_PER_MS) {
        HalDelayUs(delayUs);
        msecs -= (UINT32_MAX / OS_SYS_US_PER_MS);
    }
    HalDelayUs(msecs * OS_SYS_US_PER_MS);
}
