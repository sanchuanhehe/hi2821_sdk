/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Interrupt Abstraction Layer And API Implementation
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

#include "los_hwi_pri.h"
#include "los_hwi.h"
#include "los_memory.h"
#include "los_spinlock.h"
#include "los_trace.h"
#include "los_init.h"

#ifdef LOSCFG_HWI_BOTTOM_HALF
#include "los_task_pri.h"
#endif
#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif
#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
#include "los_sched_debug_pri.h"
#endif
#ifdef LOSCFG_KERNEL_SMP
#include "los_atomic.h"
#endif
#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
#include "los_perf_pri.h"
#endif

/* spinlock for hwi module, only available on SMP mode */
#ifdef LOSCFG_KERNEL_SMP
LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_hwiSpin);
#endif

#ifdef LOSCFG_HWI_BOTTOM_HALF
STATIC UINT32 g_bhTaskId;
STATIC LOS_DL_LIST g_bhworkList;
STATIC LOS_DL_LIST g_bhworkFreeList;
#ifdef LOSCFG_KERNEL_MEM_ALLOC
STATIC HwiBhworkItem *g_bhWorkBuf;
#else
STATIC HwiBhworkItem g_bhWorkBuf[LOSCFG_HWI_BOTTOM_HALF_WORK_LIMIT];
#endif
STATIC EVENT_CB_S g_bhEvent;
LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_hwiBottomHalfSpin);
#define HWI_BH_EVENT_MASK   0x01
#endif

size_t g_intCount[LOSCFG_KERNEL_CORE_NUM] = {0};

#if defined(LOSCFG_CPUP_INCLUDE_IRQ) && defined(LOSCFG_ARCH_INTERRUPT_PREEMPTION)
STATIC UINT32 g_lastIrq[LOSCFG_KERNEL_CORE_NUM] = {
#ifdef LOSCFG_KERNEL_SMP
    [0 ... (LOSCFG_KERNEL_CORE_NUM - 1)] = OS_HWI_INVALID_IRQ,
#else
    OS_HWI_INVALID_IRQ,
#endif
};
#endif

const HwiControllerOps *g_hwiOps = NULL;

typedef VOID (*HWI_PROC_FUNC0)(VOID);
typedef VOID (*HWI_PROC_FUNC2)(INT32, VOID *);

#ifdef LOSCFG_HWI_PRE_POST_PROCESS
enum {
    HWI_PRE_PROC_HOOK = 0,
    HWI_POST_PROC_HOOK,
    HWI_PROC_HOOK_NUM
};
STATIC VOID *g_hwiHookTable[HWI_PROC_HOOK_NUM];

VOID LOS_HwiPreHookReg(HWI_PROC_HOOK intPreHook)
{
    g_hwiHookTable[HWI_PRE_PROC_HOOK] = (VOID *)(UINTPTR)intPreHook;
}

VOID LOS_HwiPostHookReg(HWI_PROC_HOOK intPostHook)
{
    g_hwiHookTable[HWI_POST_PROC_HOOK] = (VOID *)(UINTPTR)intPostHook;
}

STATIC INLINE VOID OsIntPre(UINT32 hwiNum)
{
    HWI_PROC_HOOK intHook = (HWI_PROC_HOOK)(UINTPTR)g_hwiHookTable[HWI_PRE_PROC_HOOK];
    if (intHook != NULL) {
        intHook(hwiNum);
    }
}

STATIC INLINE VOID OsIntPost(UINT32 hwiNum)
{
    HWI_PROC_HOOK intHook = (HWI_PROC_HOOK)(UINTPTR)g_hwiHookTable[HWI_POST_PROC_HOOK];
    if (intHook != NULL) {
        intHook(hwiNum);
    }
}
#endif

STATIC INLINE VOID OsIrqNestingActive(UINT32 hwiNum)
{
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    /* preemption not allowed when handling tick interrupt */
    if (hwiNum != OS_TICK_INT_NUM) {
        (VOID)LOS_IntUnLock();
    }
#else
    (VOID)hwiNum;
#endif
}

STATIC INLINE VOID OsIrqNestingInactive(UINT32 hwiNum)
{
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    if (hwiNum != OS_TICK_INT_NUM) {
        (VOID)LOS_IntLock();
    }
#else
    (VOID)hwiNum;
#endif
}

STATIC INLINE VOID OsHwiRespCountClear(UINT32 *respCount)
{
#ifdef LOSCFG_KERNEL_SMP
    LOS_AtomicSet((Atomic *)respCount, 0);
#else
    *respCount = 0;
#endif
}

STATIC INLINE VOID OsHwiRespCountInc(UINT32 *respCount)
{
#ifdef LOSCFG_KERNEL_SMP
    LOS_AtomicInc((Atomic *)respCount);
#else
    (*respCount)++;
#endif
}

STATIC INLINE UINT32 OsHwiRespCountRead(UINT32 *respCount)
{
#ifdef LOSCFG_KERNEL_SMP
    return (UINT32)LOS_AtomicRead((Atomic *)respCount);
#else
    return *respCount;
#endif
}

size_t OsIrqNestingCntGet(VOID)
{
    return g_intCount[ArchCurrCpuid()];
}

VOID OsIrqNestingCntSet(size_t val)
{
    g_intCount[ArchCurrCpuid()] = val;
}

size_t IntActive(VOID)
{
    size_t intCount;
    UINT32 intSave = LOS_IntLock();

    intCount = g_intCount[ArchCurrCpuid()];
    LOS_IntRestore(intSave);
    return intCount;
}

STATIC INLINE VOID InterruptHandle(HwiHandleInfo *hwiForm)
{
    OsHwiRespCountInc(&hwiForm->respCount);

#ifdef LOSCFG_SHARED_IRQ
    while (hwiForm->next != NULL) {
        hwiForm = hwiForm->next;
#endif
        if (hwiForm->registerInfo) {
            HWI_PROC_FUNC2 func = (HWI_PROC_FUNC2)hwiForm->hook;
            if (func != NULL) {
                UINTPTR *param = (UINTPTR *)(hwiForm->registerInfo);
                func((INT32)(*param), (VOID *)(*(param + 1)));
            }
        } else {
            HWI_PROC_FUNC0 func = (HWI_PROC_FUNC0)hwiForm->hook;
            if (func != NULL) {
                func();
            }
        }
#ifdef LOSCFG_SHARED_IRQ
    }
#endif
}

VOID OsIntHandle(UINT32 hwiNum, HwiHandleInfo *hwiForm)
{
    size_t *intCnt = NULL;
    UINT32 cpuId;
#ifdef LOSCFG_CPUP_INCLUDE_IRQ
    UINT32 prevHwi;
#endif

    /* SHALL update interrupt status first. Otherwise, if a calling of system function have
     * used this interrupt status, then such functions may read incorrect status so they may
     * NOT works as we expected. More to notice, a lms stub may introducing such calling too. */
    cpuId = ArchCurrCpuid();
    intCnt = &g_intCount[cpuId];
    *intCnt = *intCnt + 1;

#ifdef LOSCFG_CPUP_INCLUDE_IRQ
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    prevHwi = g_lastIrq[cpuId];
    g_lastIrq[cpuId] = hwiNum;
#else
    prevHwi = OS_HWI_INVALID_IRQ;
#endif
    OsCpupIrqStart(prevHwi, hwiNum);
#endif

#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
    VOID *state = OsPerfIrqEnter(hwiNum);
#endif

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    OsSchedStatPause();
    OsSchedStatHwi(hwiNum);
#endif

    LOS_TRACE(HWI_RESPONSE_IN, hwiNum);

    OsIrqNestingActive(hwiNum);
#ifdef LOSCFG_HWI_PRE_POST_PROCESS
    OsIntPre(hwiNum);
#endif

    InterruptHandle(hwiForm);

#ifdef LOSCFG_HWI_PRE_POST_PROCESS
    OsIntPost(hwiNum);
#endif
    OsIrqNestingInactive(hwiNum);

    LOS_TRACE(HWI_RESPONSE_OUT, hwiNum);

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    OsSchedStatResume();
#endif

#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
    OsPerfIrqExit(hwiNum, state);
#endif

#ifdef LOSCFG_CPUP_INCLUDE_IRQ
    OsCpupIrqEnd(prevHwi, hwiNum);
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    g_lastIrq[cpuId] = prevHwi;
#endif
#endif

    *intCnt = *intCnt - 1;
}

VOID OsIntEntry(VOID)
{
    if ((g_hwiOps != NULL) && (g_hwiOps->handleIrq != NULL)) {
        g_hwiOps->handleIrq();
    }
    return;
}

#ifdef LOSCFG_HWI_WITH_ARG
STATIC HWI_ARG_T OsHwiCpIrqParam(const HWI_IRQ_PARAM_S *irqParam)
{
    HWI_IRQ_PARAM_S *paramByAlloc = NULL;

    paramByAlloc = (HWI_IRQ_PARAM_S *)LOS_MemAlloc(m_aucSysMem0, sizeof(HWI_IRQ_PARAM_S));
    if (paramByAlloc != NULL) {
        (VOID)memcpy(paramByAlloc, irqParam, sizeof(HWI_IRQ_PARAM_S));
    }

    return (HWI_ARG_T)paramByAlloc;
}
#endif

#ifdef LOSCFG_HWI_BOTTOM_HALF
LITE_OS_SEC_TEXT VOID OsHwiBhTask(VOID)
{
    HwiBhworkItem *bhwork  = NULL;
    UINT32 intSave = 0;
    HWI_BOTTOM_HALF_FUNC func = NULL;
    VOID *data = NULL;

    while (1) {
        if (LOS_ListEmpty(&g_bhworkList)) {
            (VOID)LOS_EventRead(&g_bhEvent, HWI_BH_EVENT_MASK, LOS_WAITMODE_CLR | LOS_WAITMODE_OR, LOS_WAIT_FOREVER);
        }

        LOS_SpinLockSave(&g_hwiBottomHalfSpin, &intSave);
        while (!LOS_ListEmpty(&g_bhworkList)) {
            bhwork = LOS_DL_LIST_ENTRY(g_bhworkList.pstNext, HwiBhworkItem, entry);
            LOS_ListDelInit(g_bhworkList.pstNext);
            func = bhwork->workFunc;
            data = bhwork->data;
            LOS_ListTailInsert(&g_bhworkFreeList, &bhwork->entry);
            LOS_SpinUnlockRestore(&g_hwiBottomHalfSpin, intSave);

            func(data);

            LOS_SpinLockSave(&g_hwiBottomHalfSpin, &intSave);
        }
        LOS_SpinUnlockRestore(&g_hwiBottomHalfSpin, intSave);
    }
}

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
STATIC UINT8 g_bhTaskStack[LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE] LITE_OS_ATTR_ALIGN(LOSCFG_STACK_POINT_ALIGN_SIZE);
#endif
UINT32 OsHwiBottomHalfInit(VOID)
{
    UINT32 ret = LOS_OK;
    TSK_INIT_PARAM_S taskParam = {0};
    UINT32 i = 0;

    LOS_ListInit(&g_bhworkList);
    LOS_ListInit(&g_bhworkFreeList);

#ifdef LOSCFG_KERNEL_MEM_ALLOC
    UINT32 size = (UINT32)sizeof(HwiBhworkItem) * LOSCFG_HWI_BOTTOM_HALF_WORK_LIMIT;
    g_bhWorkBuf = (HwiBhworkItem *)LOS_MemAlloc(m_aucSysMem0, size);
    if (g_bhWorkBuf == NULL) {
        return LOS_NOK;
    }
#endif
    for (i = 0; i < LOSCFG_HWI_BOTTOM_HALF_WORK_LIMIT; i++) {
        LOS_ListTailInsert(&g_bhworkFreeList, &g_bhWorkBuf[i].entry);
    }

    ret = LOS_EventInit(&g_bhEvent);
    if (ret != LOS_OK) {
        goto OUT;
    }

    taskParam.pfnTaskEntry = (TSK_ENTRY_FUNC)OsHwiBhTask;
    taskParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam.pcName = "Int_Bottom_Half_Task";
    taskParam.usTaskPrio = LOS_TASK_PRIORITY_HIGHEST + 1;    // BottomHalf task's pri should be lower than swtmr
    taskParam.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    taskParam.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    ret = LOS_TaskCreateStatic(&g_bhTaskId, &taskParam, g_bhTaskStack);
#else
    ret = LOS_TaskCreate(&g_bhTaskId, &taskParam);
#endif
    if (ret != LOS_OK) {
        goto OUT;
    }

    OS_TCB_FROM_TID(g_bhTaskId)->taskFlags |= OS_TASK_FLAG_SYSTEM;
    return LOS_OK;

OUT:
    LOS_ListInit(&g_bhworkFreeList);
#ifdef LOSCFG_KERNEL_MEM_ALLOC
    if (g_bhWorkBuf != NULL) {
        (VOID)LOS_MemFree(m_aucSysMem0, (VOID *)g_bhWorkBuf);
        g_bhWorkBuf = NULL;
    }
#endif
    return ret;
}
LOS_SYS_INIT(OsHwiBottomHalfInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_3);
#endif

#ifndef LOSCFG_SHARED_IRQ
STATIC UINT32 OsHwiDel(HwiHandleInfo *hwiForm, const HWI_IRQ_PARAM_S *irqParam, UINT32 irqId)
{
    UINT32 intSave;

    (VOID)irqParam;
    HWI_LOCK(intSave);
    hwiForm->hook = NULL;
#ifdef LOSCFG_HWI_WITH_ARG
    if (hwiForm->registerInfo) {
        (VOID)LOS_MemFree(m_aucSysMem0, (VOID *)hwiForm->registerInfo);
    }
#endif
    hwiForm->registerInfo = 0;
    OsHwiRespCountClear(&hwiForm->respCount);

    if (g_hwiOps->disableIrq == NULL) {
        HWI_UNLOCK(intSave);
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    g_hwiOps->disableIrq(irqId);

    HWI_UNLOCK(intSave);
    return LOS_OK;
}

STATIC UINT32 OsHwiCreate(HwiHandleInfo *hwiForm, HWI_MODE_T hwiMode, HWI_PROC_FUNC hwiHandler,
                          const HWI_IRQ_PARAM_S *irqParam)
{
    UINT32 intSave;

    if (hwiMode & IRQF_SHARED) {
        return LOS_ERRNO_HWI_SHARED_ERROR;
    }

#ifndef LOSCFG_HWI_WITH_ARG
    if (irqParam != NULL) {
        return LOS_ERRNO_HWI_ARG_NOT_ENABLED;
    }
#endif

    HWI_LOCK(intSave);
    if (hwiForm->hook == NULL) {
        hwiForm->hook = hwiHandler;
#ifdef LOSCFG_HWI_WITH_ARG
        if (irqParam != NULL) {
            hwiForm->registerInfo = OsHwiCpIrqParam(irqParam);
            if (hwiForm->registerInfo == (HWI_ARG_T)NULL) {
                HWI_UNLOCK(intSave);
                return LOS_ERRNO_HWI_NO_MEMORY;
            }
        }
#endif
    } else {
        HWI_UNLOCK(intSave);
        return LOS_ERRNO_HWI_ALREADY_CREATED;
    }
    HWI_UNLOCK(intSave);
    return LOS_OK;
}
#else /* LOSCFG_SHARED_IRQ */
STATIC UINT32 OsFreeHwiNode(HwiHandleInfo *head, HwiHandleInfo *hwiForm, UINT32 irqId)
{
    UINT32 ret = LOS_OK;

    if (hwiForm->registerInfo != (HWI_ARG_T)NULL) {
        (VOID)LOS_MemFree(m_aucSysMem0, (VOID *)hwiForm->registerInfo);
    }

    (VOID)LOS_MemFree(m_aucSysMem0, hwiForm);

    if (head->next == NULL) {
        head->shareMode = 0;
        OsHwiRespCountClear(&head->respCount);

        if (g_hwiOps->disableIrq == NULL) {
            ret = LOS_ERRNO_HWI_PROC_FUNC_NULL;
            return ret;
        }
        g_hwiOps->disableIrq(irqId);
    }

    return ret;
}

STATIC UINT32 OsHwiDel(HwiHandleInfo *head, const HWI_IRQ_PARAM_S *irqParam, UINT32 irqId)
{
    HwiHandleInfo *hwiFormPrev = NULL;
    HwiHandleInfo *hwiForm = NULL;
    UINT32 intSave;
    UINT32 ret;

    HWI_LOCK(intSave);

    if ((head->shareMode & IRQF_SHARED) && ((irqParam == NULL) || (irqParam->pDevId == NULL))) {
        HWI_UNLOCK(intSave);
        return LOS_ERRNO_HWI_SHARED_ERROR;
    }

    /* Non-shared interrupt. */
    if (!(head->shareMode & IRQF_SHARED)) {
        if (head->next == NULL) {
            HWI_UNLOCK(intSave);
            return LOS_ERRNO_HWI_HWINUM_UNCREATE;
        }

        hwiForm = head->next;
        head->next = NULL;
        ret = OsFreeHwiNode(head, hwiForm, irqId);
        HWI_UNLOCK(intSave);
        return ret;
    }

    /* Shared interrupt. */
    hwiFormPrev = head;
    hwiForm = head->next;
    while (hwiForm != NULL) {
        if (((HWI_IRQ_PARAM_S *)(hwiForm->registerInfo))->pDevId == irqParam->pDevId) {
            break;
        }
        hwiFormPrev = hwiForm;
        hwiForm = hwiForm->next;
    }

    if (hwiForm == NULL) {
        HWI_UNLOCK(intSave);
        return LOS_ERRNO_HWI_HWINUM_UNCREATE;
    }

    hwiFormPrev->next = hwiForm->next;
    ret = OsFreeHwiNode(head, hwiForm, irqId);
    HWI_UNLOCK(intSave);
    return ret;
}

STATIC UINT32 OsHwiCreate(HwiHandleInfo *head, HWI_MODE_T hwiMode, HWI_PROC_FUNC hwiHandler,
                          const HWI_IRQ_PARAM_S *irqParam)
{
    UINT32 intSave;
    HwiHandleInfo *hwiFormNode = NULL;
    HWI_IRQ_PARAM_S *hwiParam = NULL;
    HWI_MODE_T modeResult = hwiMode & IRQF_SHARED;
    HwiHandleInfo *hwiForm = NULL;

    if (modeResult && ((irqParam == NULL) || (irqParam->pDevId == NULL))) {
        return LOS_ERRNO_HWI_SHARED_ERROR;
    }

    HWI_LOCK(intSave);

    if ((head->next != NULL) && ((modeResult == 0) || (!(head->shareMode & IRQF_SHARED)))) {
        HWI_UNLOCK(intSave);
        return LOS_ERRNO_HWI_SHARED_ERROR;
    }

    hwiForm = head;
    while (hwiForm->next != NULL) {
        hwiForm = hwiForm->next;
        hwiParam = (HWI_IRQ_PARAM_S *)(hwiForm->registerInfo);
        if (hwiParam->pDevId == irqParam->pDevId) {
            HWI_UNLOCK(intSave);
            return LOS_ERRNO_HWI_ALREADY_CREATED;
        }
    }

    hwiFormNode = (HwiHandleInfo *)LOS_MemAlloc(m_aucSysMem0, sizeof(HwiHandleInfo));
    if (hwiFormNode == NULL) {
        HWI_UNLOCK(intSave);
        return LOS_ERRNO_HWI_NO_MEMORY;
    }
    OsHwiRespCountClear(&head->respCount);

    if (irqParam != NULL) {
        hwiFormNode->registerInfo = OsHwiCpIrqParam(irqParam);
        if (hwiFormNode->registerInfo == (HWI_ARG_T)NULL) {
            HWI_UNLOCK(intSave);
            (VOID) LOS_MemFree(m_aucSysMem0, hwiFormNode);
            return LOS_ERRNO_HWI_NO_MEMORY;
        }
    } else {
        hwiFormNode->registerInfo = 0;
    }

    hwiFormNode->hook = hwiHandler;
    hwiFormNode->next = (struct tagHwiHandleForm *)NULL;
    hwiForm->next = hwiFormNode;

    head->shareMode = modeResult;

    HWI_UNLOCK(intSave);
    return LOS_OK;
}
#endif

LITE_OS_SEC_TEXT UINT32 LOS_HwiCreate(HWI_HANDLE_T hwiNum,
                                      HWI_PRIOR_T hwiPrio,
                                      HWI_MODE_T hwiMode,
                                      HWI_PROC_FUNC hwiHandler,
                                      HWI_IRQ_PARAM_S *irqParam)
{
    UINT32 ret;
    HwiHandleInfo *hwiForm = NULL;

    if (hwiHandler == NULL) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }

    if ((g_hwiOps == NULL) || (g_hwiOps->getHandleForm == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }

    hwiForm = g_hwiOps->getHandleForm(hwiNum);
    if (hwiForm == NULL) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    LOS_TRACE(HWI_CREATE, hwiNum, hwiPrio, hwiMode, (UINTPTR)hwiHandler);
    ret = OsHwiCreate(hwiForm, hwiMode, hwiHandler, irqParam);
    LOS_TRACE(HWI_CREATE_NODE, hwiNum, (UINTPTR)(irqParam != NULL ? irqParam->pDevId : NULL), ret);
    if (ret != LOS_OK) {
        goto LOS_ERR_EXIT;
    }

    /* priority will be changed if setIrqPriority implemented,
     * but interrupt preemption only allowed when LOSCFG_ARCH_INTERRUPT_PREEMPTION enable */
    if (g_hwiOps->setIrqPriority != NULL) {
        ret = g_hwiOps->setIrqPriority(hwiNum, hwiPrio);
        if (ret != LOS_OK) {
            goto LOS_ERR_HWI_DELETE;
        }
    }

#ifdef LOSCFG_KERNEL_CPUP
    ret = OsCpupCBIrqCreate(hwiNum);
    if (ret != LOS_OK) {
        goto LOS_ERR_HWI_DELETE;
    }
#endif

    return LOS_OK;

LOS_ERR_HWI_DELETE:
    (VOID)OsHwiDel(hwiForm, irqParam, hwiNum);
LOS_ERR_EXIT:
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiDelete(HWI_HANDLE_T hwiNum, HWI_IRQ_PARAM_S *irqParam)
{
    UINT32 ret;
    HwiHandleInfo *hwiForm = NULL;

    if ((g_hwiOps == NULL) || (g_hwiOps->getHandleForm == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }

#if defined(LOSCFG_DEBUG_HWI) && defined(LOSCFG_SHARED_IRQ)
    if (OS_INT_ACTIVE) {
        PRINT_WARN("Delete share interrupt in isr will cause a memory access error.\n");
    }
#endif

    hwiForm = g_hwiOps->getHandleForm(hwiNum);
    if (hwiForm == NULL) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    LOS_TRACE(HWI_DELETE, hwiNum);

    ret = OsHwiDel(hwiForm, irqParam, hwiNum);
    LOS_TRACE(HWI_DELETE_NODE, hwiNum, (UINTPTR)(irqParam != NULL ? irqParam->pDevId : NULL), ret);

    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiTrigger(HWI_HANDLE_T hwiNum)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->triggerIrq == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_TRIGGER, hwiNum);

    return g_hwiOps->triggerIrq(hwiNum);
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiEnable(HWI_HANDLE_T hwiNum)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->enableIrq == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_ENABLE, hwiNum);

    return g_hwiOps->enableIrq(hwiNum);
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiDisable(HWI_HANDLE_T hwiNum)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->disableIrq == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_DISABLE, hwiNum);

    return g_hwiOps->disableIrq(hwiNum);
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiClear(HWI_HANDLE_T hwiNum)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->clearIrq == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_CLEAR, hwiNum);

    return g_hwiOps->clearIrq(hwiNum);
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiSetPriority(HWI_HANDLE_T hwiNum, HWI_PRIOR_T priority)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->setIrqPriority == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_SETPRI, hwiNum, priority);

    return g_hwiOps->setIrqPriority(hwiNum, priority);
}

#ifdef LOSCFG_HWI_BOTTOM_HALF
LITE_OS_SEC_TEXT UINT32 LOS_HwiBhworkAdd(HWI_BOTTOM_HALF_FUNC bhHandler, VOID *data)
{
    HwiBhworkItem *bhwork = NULL;
    UINT32         intSave = 0;

    if (!(OS_INT_ACTIVE)) {
        return LOS_ERRNO_HWI_NOT_INTERRUPT_CONTEXT;
    }

    if (bhHandler == NULL) {
        return OS_ERRNO_HWI_PROC_FUNC_NULL;
    }

    LOS_SpinLockSave(&g_hwiBottomHalfSpin, &intSave);
    if (LOS_ListEmpty(&g_bhworkFreeList)) {
        LOS_SpinUnlockRestore(&g_hwiBottomHalfSpin, intSave);
        return LOS_ERRNO_HWI_NO_MEMORY;
    }
    bhwork = LOS_DL_LIST_ENTRY(g_bhworkFreeList.pstNext, HwiBhworkItem, entry);
    LOS_ListDelInit(g_bhworkFreeList.pstNext);
    bhwork->data = data;
    bhwork->workFunc = bhHandler;
    LOS_ListTailInsert(&g_bhworkList, &bhwork->entry);
    LOS_SpinUnlockRestore(&g_hwiBottomHalfSpin, intSave);

    return LOS_EventWrite(&g_bhEvent, HWI_BH_EVENT_MASK);
}
#endif

#ifdef LOSCFG_KERNEL_SMP
LITE_OS_SEC_TEXT UINT32 LOS_HwiSetAffinity(HWI_HANDLE_T hwiNum, UINT32 cpuMask)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->setIrqCpuAffinity == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_SETAFFINITY, hwiNum, cpuMask);

    return g_hwiOps->setIrqCpuAffinity(hwiNum, cpuMask);
}

LITE_OS_SEC_TEXT UINT32 LOS_HwiSendIpi(HWI_HANDLE_T hwiNum, UINT32 cpuMask)
{
    if ((g_hwiOps == NULL) || (g_hwiOps->sendIpi == NULL)) {
        return LOS_ERRNO_HWI_PROC_FUNC_NULL;
    }
    LOS_TRACE(HWI_SENDIPI, hwiNum, cpuMask);

    return g_hwiOps->sendIpi(cpuMask, hwiNum);
}
#endif

/* Initialization of the hardware interrupt */
LITE_OS_SEC_TEXT_INIT UINT32 OsHwiInit(VOID)
{
    HalIrqInit();
    return LOS_OK;
}
LOS_SYS_INIT(OsHwiInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_1);

LITE_OS_SEC_TEXT UINT32 LOS_HwiRespCntGet(HWI_HANDLE_T hwiNum, UINT32 *respCount)
{
    HwiHandleInfo *hwiForm = NULL;

    if (respCount == NULL) {
        return LOS_ERRNO_HWI_PTR_NULL;
    }

    hwiForm = OsGetHwiForm(hwiNum);
    if (hwiForm == NULL) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    *respCount = OsHwiRespCountRead(&hwiForm->respCount);
    return LOS_OK;
}
