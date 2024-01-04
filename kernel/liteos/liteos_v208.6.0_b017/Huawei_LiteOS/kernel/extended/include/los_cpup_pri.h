/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Cpup HeadFile
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

#ifndef _LOS_CPUP_PRI_H
#define _LOS_CPUP_PRI_H

#include "los_cpup.h"
#include "los_task_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef LOSCFG_CPUP_IRQ_CB_NUM
#define LOSCFG_CPUP_IRQ_CB_NUM LOSCFG_PLATFORM_HWI_LIMIT
#endif
#ifndef LOSCFG_CPUP_SAMPLE_PERIOD
#define LOSCFG_CPUP_SAMPLE_PERIOD LOSCFG_BASE_CORE_TICK_PER_SECOND
#endif
#ifndef LOSCFG_CPUP_HISTORY_RECORD_NUM
#define LOSCFG_CPUP_HISTORY_RECORD_NUM 10
#endif

#define LOS_CPUP_PRECISION           1000
#define LOS_CPUP_PRECISION_MULT      (LOS_CPUP_PRECISION / 100)

#define OS_CPUP_TICK_TO_S_PART(ticks)     ((ticks) / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#define OS_CPUP_TICK_TO_MS_PART(ticks)    ((LOS_CPUP_PRECISION_MULT * ((ticks) % LOSCFG_BASE_CORE_TICK_PER_SECOND)) \
   / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#define OS_CPUP_HISTORY_RECORD_NUM   (LOSCFG_CPUP_HISTORY_RECORD_NUM + 1)

#define OS_CPUP_PERIOD_S   OS_CPUP_TICK_TO_S_PART(LOSCFG_CPUP_SAMPLE_PERIOD)
#define OS_CPUP_PERIOD_MS  OS_CPUP_TICK_TO_MS_PART(LOSCFG_CPUP_SAMPLE_PERIOD)

#define OS_CPUP_MULTI_PERIOD_S   OS_CPUP_TICK_TO_S_PART(LOSCFG_CPUP_SAMPLE_PERIOD * LOSCFG_CPUP_HISTORY_RECORD_NUM)
#define OS_CPUP_MULTI_PERIOD_MS  OS_CPUP_TICK_TO_MS_PART(LOSCFG_CPUP_SAMPLE_PERIOD * LOSCFG_CPUP_HISTORY_RECORD_NUM)

typedef struct {
    UINT64 allTime;                                   /* Total running time by the end of last task switch */
    UINT64 curTime;                                   /* Total running time by the end of last soft timer run */
    UINT32 historyTime[OS_CPUP_HISTORY_RECORD_NUM];   /* Historical running time */
    UINT16 status;                                    /* Task status */
} OsCpupCB;

extern OsCpupCB *OsCpupCBGet(UINT32 index);
extern UINT32 OsCpupInit(VOID);
extern VOID OsCpupSetCycle(UINT64 startCycles);
extern UINT64 OsCpupGetCycle(VOID);
extern VOID OsCpupStartToRun(UINT32 taskId);
extern VOID OsTaskCycleEndStart(const LosTaskCB *newTask);

extern VOID OsCpupCBTaskCreate(UINT32 taskId, UINT16 taskStatus);
extern VOID OsCpupCBTaskDelete(UINT32 taskId);
extern UINT32 OsCpupCBIrqCreate(HWI_HANDLE_T intNum);

extern UINT32 OsCpupIrqCBIdGet(UINT32 hwiNum, BOOL isLocal);

#ifdef LOSCFG_CPUP_INCLUDE_IRQ
VOID OsCpupIrqStart(UINT32, UINT32);
VOID OsCpupIrqEnd(UINT32, UINT32);
#endif

#ifdef LOSCFG_SCHED_LOAD_BALANCE_CPUP
extern BOOL OsCpupCurrAllCpuUsage(UINT32 *cpuUsage);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_CPUP_PRI_H */
