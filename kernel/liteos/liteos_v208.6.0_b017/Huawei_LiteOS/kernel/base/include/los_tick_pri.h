/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Tick Private HeadFile
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

#ifndef _LOS_TICK_PRI_H
#define _LOS_TICK_PRI_H

#include "los_base.h"
#include "los_tick.h"
#include "los_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* spinlock for tick */
extern SPIN_LOCK_S g_tickSpin;
#define TICK_LOCK(state)                       LOS_SpinLockSave(&g_tickSpin, &(state))
#define TICK_UNLOCK(state)                     LOS_SpinUnlockRestore(&g_tickSpin, (state))

/* Count of Ticks */
extern volatile UINT64 g_tickCount[LOSCFG_KERNEL_CORE_NUM];

/* Cycle to nanosecond scale */
extern DOUBLE g_cycle2NsScale;

/* This API is called when the system tick timeout and triggers the interrupt. */
extern VOID OsTickHandler(VOID);
extern UINT32 OsTickInit(VOID);
extern VOID OsTickStart(VOID);

/* Convert from the cycle count to nanosecond. */
#define CYCLE_TO_NS(cycles) ((cycles) * g_cycle2NsScale)

/**
 * Current system timer register is 32 bit, therefore TIMER_MAXLOAD define just in order to avoid ambiguity.
 */
#define TIMER_MAXLOAD 0xffffffff

#ifdef LOSCFG_KERNEL_TICKLESS
#define LOS_TICK_INT_FLAG 0x80000000
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_TICK_PRI_H */
