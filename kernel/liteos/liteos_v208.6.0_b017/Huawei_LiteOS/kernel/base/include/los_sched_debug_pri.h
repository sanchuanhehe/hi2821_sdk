/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: Schedule statistics Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2018-11-16
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

#ifndef _LOS_SCHED_DEBUG_PRI_H
#define _LOS_SCHED_DEBUG_PRI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    UINT64      runtime;
    UINT32      contextSwitch;
#ifdef LOSCFG_KERNEL_SMP
    UINT32      migrationIn;
#endif
} SchedPercpu;

typedef struct {
    UINT64      startRuntime;
    UINT64      allRuntime;
    UINT32      allContextSwitch;
#ifdef LOSCFG_KERNEL_SMP
    UINT32      allMigration;
#endif
    SchedPercpu schedPercpu[LOSCFG_KERNEL_CORE_NUM];
} SchedStat;

typedef struct {
    DOUBLE      pastTime;
    DOUBLE      idleTaskTimePercent; /* Percentage of running duration of idle task */
    UINT32      taskSwiNum;
    UINT32      hwiNum;
    DOUBLE      noIdleTaskSwiAvgPri;
    DOUBLE      hiTaskTimePercent;   /* Percentage of running duration of high-priority tasks */
    UINT32      hiTaskSwiNum;
    DOUBLE      hiTaskAvgRunTime;
#ifdef LOSCFG_KERNEL_SMP
    UINT32      smpHwiNum;
    UINT32      smpMigrationNum;
#endif
} SchedStatCpuInfo;

typedef struct {
    UINT32      cpuId;
    DOUBLE      cpuRunTime;
    UINT32      cpuTaskSwiNum;
#ifdef LOSCFG_KERNEL_SMP
    UINT32      cpuSmpMigrationNum;
#endif
} SchedStatTaskCpuInfo;

typedef struct {
    CHAR        *taskName;
    DOUBLE      pastTime;
    DOUBLE      allRuntime;
    UINT32      allTaskSwiNum;
#ifdef LOSCFG_KERNEL_SMP
    UINT32      allSmpMigrationNum;
#endif
    UINT32      taskUseCpuNum;
    SchedStatTaskCpuInfo    cpuInfo[LOSCFG_KERNEL_CORE_NUM];
} SchedStatTaskInfo;

extern VOID OsSchedStatHwi(size_t intNum);

/* This API is used for Scheduling statistics output */
extern UINT32 OsSchedStatCpuInfoGet(SchedStatCpuInfo *info, UINT32 cpuId);
extern UINT32 OsSchedStatTaskInfoGet(SchedStatTaskInfo *info, UINT32 taskId);

/* This API is used for temporary pause and resume task sched statistics */
extern VOID OsSchedStatPause(VOID);
extern VOID OsSchedStatResume(VOID);

/* This API is used for shell command execution function */
extern VOID OsSchedStatStart(VOID);
extern VOID OsSchedStatStop(VOID);
extern VOID OsSchedStatInfo(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_SCHED_DEBUG_PRI_H */
