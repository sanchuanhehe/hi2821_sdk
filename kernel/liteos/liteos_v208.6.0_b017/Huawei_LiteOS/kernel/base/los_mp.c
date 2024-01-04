/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Muti-core processing
 * Author: Huawei LiteOS Team
 * Create: 2018-07-10
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

#include "los_mp_pri.h"
#include "los_task_pri.h"
#include "los_percpu_pri.h"
#include "los_sched_pri.h"
#include "los_swtmr.h"
#include "los_init.h"

#ifdef LOSCFG_KERNEL_SMP

#ifdef LOSCFG_KERNEL_SMP_CALL
LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_mpCallSpin);
#define MP_CALL_LOCK(state)       LOS_SpinLockSave(&g_mpCallSpin, &(state))
#define MP_CALL_UNLOCK(state)     LOS_SpinUnlockRestore(&g_mpCallSpin, (state))
#endif

VOID LOS_MpSchedule(UINT32 target)
{
    UINT32 ret;
    UINT32 cpuid = ArchCurrCpuid();
    target &= ~(1U << cpuid);
    ret = HalIrqSendIpi(target, LOS_MP_IPI_SCHEDULE);
    if (ret != LOS_OK) {
        return;
    }
    return;
}

VOID OsMpWakeHandler(VOID)
{
    /* generic wakeup ipi, do nothing */
}

VOID OsMpScheduleHandler(VOID)
{
    /*
     * set schedule flag to differ from wake function,
     * so that the scheduler can be triggered at the end of irq.
     */
    OsSetSchedFlag(INT_PEND_RESCH);
}

VOID OsMpHaltHandler(VOID)
{
    (VOID)LOS_IntLock();
    OsPercpuGet()->excFlag = CPU_HALT;

    while (1) {}
}

VOID OsMpCollectTasks(VOID)
{
    LosTaskCB *taskCB = NULL;
    UINT32 taskId = 0;
    UINT32 ret;

    /* recursive checking all the available task */
    for (; taskId <= g_taskMaxNum; taskId++) {
        taskCB = &g_osTaskCBArray[taskId];

        if ((taskCB->taskStatus & OS_TASK_IS_EXIT) ||
            (taskCB->taskStatus & OS_TASK_STATUS_RUNNING)) {
            continue;
        }

        /*
         * though task status is not atomic, this check may success but not accomplish
         * the deletion; this deletion will be handled until the next run.
         */
        if (taskCB->mpSignal & SIGNAL_KILL) {
            ret = LOS_TaskDelete(taskId);
            if ((ret != LOS_OK) && (ret != LOS_ERRNO_TSK_NOT_CREATED)) {
                PRINT_WARN("GC collect task failed err:0x%x\n", ret);
            }
        }
    }
}

#ifdef LOSCFG_KERNEL_SMP_CALL
#ifdef LOSCFG_SMP_CALL_STATIC_ALLOCATION
STATIC LOS_DL_LIST_HEAD(g_mpCallFreeObj);
STATIC MpCallFunc g_mpCallFuncObjs[LOSCFG_SMP_CALL_OBJ_NUM];
#endif

STATIC INLINE MpCallFunc *OsMpFuncAllocObj(VOID)
{
#ifdef LOSCFG_SMP_CALL_STATIC_ALLOCATION
    UINT32 intSave;
    MP_CALL_LOCK(intSave);
    if (LOS_ListEmpty(&g_mpCallFreeObj)) {
        MP_CALL_UNLOCK(intSave);
        return NULL;
    }

    LOS_DL_LIST *obj = LOS_DL_LIST_FIRST(&g_mpCallFreeObj);
    LOS_ListDelete(obj);
    MP_CALL_UNLOCK(intSave);
    return LOS_DL_LIST_ENTRY(obj, MpCallFunc, node);
#else
    return (MpCallFunc *)LOS_MemAlloc(m_aucSysMem0, sizeof(MpCallFunc));
#endif
}

STATIC INLINE VOID OsMpFuncFreeObj(MpCallFunc *obj)
{
#ifdef LOSCFG_SMP_CALL_STATIC_ALLOCATION
    UINT32 intSave;
    MP_CALL_LOCK(intSave);
    LOS_ListAdd(&g_mpCallFreeObj, &obj->node);
    MP_CALL_UNLOCK(intSave);
#else
    (VOID)LOS_MemFree(m_aucSysMem0, obj);
#endif
}

VOID OsMpFuncCall(UINT32 target, SMP_FUNC_CALL func, VOID *args)
{
    UINT32 index;
    UINT32 intSave;
    UINT32 ret;

    if (func == NULL) {
        return;
    }

    if (!(target & OS_MP_CPU_ALL)) {
        return;
    }

    for (index = 0; index < LOSCFG_KERNEL_CORE_NUM; index++) {
        if (CPUID_TO_AFFI_MASK(index) & target) {
            MpCallFunc *mpCallFunc = OsMpFuncAllocObj();
            if (mpCallFunc == NULL) {
                PRINT_ERR("smp func call malloc failed\n");
                return;
            }
            mpCallFunc->func = func;
            mpCallFunc->args = args;

            MP_CALL_LOCK(intSave);
            LOS_ListAdd(&g_percpu[index].funcLink, &(mpCallFunc->node));
            MP_CALL_UNLOCK(intSave);
        }
    }
    ret = HalIrqSendIpi(target, LOS_MP_IPI_FUNC_CALL);
    if (ret != LOS_OK) {
        return;
    }
    return;
}

VOID OsMpFuncCallHandler(VOID)
{
    UINT32 intSave;
    UINT32 cpuid = ArchCurrCpuid();
    LOS_DL_LIST *list = NULL;
    MpCallFunc* mpCallFunc = NULL;

    MP_CALL_LOCK(intSave);
    while (!LOS_ListEmpty(&g_percpu[cpuid].funcLink)) {
        list = LOS_DL_LIST_FIRST(&g_percpu[cpuid].funcLink);
        LOS_ListDelete(list);
        MP_CALL_UNLOCK(intSave);

        mpCallFunc = LOS_DL_LIST_ENTRY(list, MpCallFunc, node);
        mpCallFunc->func(mpCallFunc->args);
        OsMpFuncFreeObj(mpCallFunc);

        MP_CALL_LOCK(intSave);
    }
    MP_CALL_UNLOCK(intSave);
}

VOID OsMpFuncCallInit(VOID)
{
    UINT32 index;
    /* init funclink for each core */
    for (index = 0; index < LOSCFG_KERNEL_CORE_NUM; index++) {
        LOS_ListInit(&g_percpu[index].funcLink);
    }
#ifdef LOSCFG_SMP_CALL_STATIC_ALLOCATION
    /* add all func call objects to free list */
    for (index = 0; index < LOSCFG_SMP_CALL_OBJ_NUM; index++) {
        LOS_ListAdd(&g_mpCallFreeObj, &g_mpCallFuncObjs[index].node);
    }
#endif
}
#endif /* LOSCFG_KERNEL_SMP_CALL */

UINT32 OsMpInit(VOID)
{
#ifdef LOSCFG_KERNEL_SMP_GC_SWTMR
    UINT16 swtmrId;
    UINT32 ret;

    ret = LOS_SwtmrCreate(OS_MP_GC_PERIOD, LOS_SWTMR_MODE_PERIOD,
                          (SWTMR_PROC_FUNC)OsMpCollectTasks, &swtmrId, 0);
    if (ret != LOS_OK) {
        PRINT_ERR("Swtmr Create failed err:0x%x\n", ret);
        return ret;
    }
    ret = LOS_SwtmrStart(swtmrId);
    if (ret != LOS_OK) {
        PRINT_ERR("Swtmr Start failed err:0x%x\n", ret);
        return ret;
    }
#endif
#ifdef LOSCFG_KERNEL_SMP_CALL
    OsMpFuncCallInit();
#endif
    return LOS_OK;
}
LOS_SYS_INIT(OsMpInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_4);
#endif
