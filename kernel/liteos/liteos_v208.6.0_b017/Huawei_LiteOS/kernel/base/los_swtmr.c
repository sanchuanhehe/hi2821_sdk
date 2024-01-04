/* ---------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Software Timer Manager
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

#include "los_swtmr_pri.h"
#include "los_sortlink_pri.h"
#include "los_task_pri.h"
#include "los_trace.h"
#include "los_init.h"

#ifdef LOSCFG_BASE_CORE_SWTMR
LITE_OS_SEC_BSS LosSwtmrCB *g_osSwtmrCBArray = NULL;        /* First address in Timer memory space */
#ifndef LOSCFG_BASE_CORE_SWTMR_IN_ISR
LITE_OS_SEC_BSS UINT8 *g_osSwtmrTaskStack[LOSCFG_KERNEL_CORE_NUM];
#endif
LITE_OS_SEC_BSS LOS_DL_LIST     g_swtmrFreeList;            /* Free list of Software Timer */

/* spinlock for swtmr module, only available on SMP mode */
LITE_OS_SEC_BSS  SPIN_LOCK_INIT(g_swtmrSpin);
#define SWTMR_LOCK(state)       LOS_SpinLockSave(&g_swtmrSpin, &(state))
#define SWTMR_UNLOCK(state)     LOS_SpinUnlockRestore(&g_swtmrSpin, (state))

/*
 * Description: Start Software Timer
 * Input      : swtmr --- Need to start software timer
 */
LITE_OS_SEC_TEXT VOID OsSwtmrStart(LosSwtmrCB *swtmr)
{
    if ((swtmr->overrun == 0) && ((swtmr->mode == LOS_SWTMR_MODE_ONCE) ||
        (swtmr->mode == LOS_SWTMR_MODE_OPP) ||
        (swtmr->mode == LOS_SWTMR_MODE_NO_SELFDELETE))) {
        SET_SORTLIST_VALUE(&(swtmr->sortList), swtmr->expiry);
    } else {
        SET_SORTLIST_VALUE(&(swtmr->sortList), swtmr->interval);
    }

    OsAdd2SortLink(&OsPercpuGet()->swtmrSortLink, &swtmr->sortList);

    swtmr->state = OS_SWTMR_STATUS_TICKING;

#ifdef LOSCFG_KERNEL_SMP
    swtmr->cpuid = ArchCurrCpuid();
#endif
}

STATIC INLINE VOID OsSwtmrDelete(LosSwtmrCB *swtmr)
{
#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
    if (swtmr->timerId < (OS_SWTMR_MAX_TIMERID - KERNEL_SWTMR_LIMIT)) {
        swtmr->timerId += KERNEL_SWTMR_LIMIT;
    } else {
        swtmr->timerId %= KERNEL_SWTMR_LIMIT;
    }
#endif
    /* insert to free list */
    LOS_ListTailInsert(&g_swtmrFreeList, &swtmr->sortList.sortLinkNode);
    swtmr->state = OS_SWTMR_STATUS_UNUSED;
}

STATIC INLINE VOID OsSwtmrUpdate(LosSwtmrCB *swtmr)
{
    if (swtmr->mode == LOS_SWTMR_MODE_ONCE) {
        swtmr->state = OS_SWTMR_STATUS_DELETING;
    } else if (swtmr->mode == LOS_SWTMR_MODE_NO_SELFDELETE) {
        swtmr->state = OS_SWTMR_STATUS_CREATED;
    } else {
        swtmr->overrun++;
        OsSwtmrStart(swtmr);
    }
}

STATIC INLINE VOID OsSwtmrCheckSelfDelete(LosSwtmrCB *swtmr)
{
    if ((swtmr->state == OS_SWTMR_STATUS_DELETING) && (swtmr->inProcess == 0)) {
        OsSwtmrDelete(swtmr);
    }
}

#ifndef LOSCFG_BASE_CORE_SWTMR_IN_ISR
LITE_OS_SEC_TEXT VOID OsSwtmrTask(VOID)
{
    LosSwtmrCB *swtmr = NULL;
    UINT32 intSave;

#if defined(LOSCFG_TRUSTZONE) && defined(LOSCFG_SWTMR_ACCESS_SECURE)
    UINT32 ret = LOS_TaskAllocSecureContext(OsPercpuGet()->swtmrTaskId, LOSCFG_TSK_SWTMR_SECURE_STACK_SIZE);
    if (ret != LOS_OK) {
        PRINT_ERR("OsSwtmrTask alloc secure stack failed!\n");
        return;
    }
#endif

    intSave = LOS_IntLock();

    for (;;) {
        (VOID)OsSysTaskSuspend(OsCurrTaskGet());
        while (OsPercpuGet()->expiryList.free != KERNEL_SWTMR_LIMIT) { // check have timeout
            swtmr = (LosSwtmrCB *)OsPercpuGet()->expiryList.buf[OsPercpuGet()->expiryList.head]; // get swtmr
            LOS_IntRestore(intSave);

            if (swtmr->handler != NULL) {
                swtmr->handler(swtmr->arg);
            }

            intSave = LOS_IntLock();
            OsPercpuGet()->expiryList.head++;
            OsPercpuGet()->expiryList.head %= KERNEL_SWTMR_LIMIT;
            OsPercpuGet()->expiryList.free++;
            LOS_SpinLock(&g_swtmrSpin);
            swtmr->inProcess--;
            OsSwtmrCheckSelfDelete(swtmr);
            LOS_SpinUnlock(&g_swtmrSpin);
        }
    }
}

#ifdef LOSCFG_EXC_INTERACTION
BOOL IsSwtmrTask(UINT32 taskId)
{
    UINT32 i;

    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        if (taskId == g_percpu[i].swtmrTaskId) {
            return TRUE;
        }
    }

    return FALSE;
}
#endif

LITE_OS_SEC_TEXT_INIT STATIC UINT32 OsSwtmrTaskCreate(VOID)
{
    UINT32 ret, swtmrTaskId;
    TSK_INIT_PARAM_S swtmrTask = {0};
    UINT32 cpuid = ArchCurrCpuid();

    swtmrTask.pfnTaskEntry = (TSK_ENTRY_FUNC)OsSwtmrTask;
    swtmrTask.uwStackSize = KERNEL_TSK_SWTMR_STACK_SIZE;
    swtmrTask.pcName = "Swt_Task";
    swtmrTask.usTaskPrio = LOS_TASK_PRIORITY_HIGHEST;
    swtmrTask.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    swtmrTask.usCpuAffiMask = CPUID_TO_AFFI_MASK(cpuid);
#endif

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    ret = LOS_TaskCreateStatic(&swtmrTaskId, &swtmrTask, g_osSwtmrTaskStack[cpuid]);
#else
    ret = LOS_TaskCreate(&swtmrTaskId, &swtmrTask);
#endif
    if (ret == LOS_OK) {
        g_percpu[cpuid].swtmrTaskId = swtmrTaskId;
        OS_TCB_FROM_TID(swtmrTaskId)->taskFlags |= OS_TASK_FLAG_SYSTEM;
    }

    return ret;
}
#endif

LITE_OS_SEC_BSS STATIC LOS_DL_LIST g_swtmrSortlink[LOSCFG_KERNEL_CORE_NUM][OS_TSK_SORTLINK_LEN];
LITE_OS_SEC_TEXT_INIT UINT32 OsSwtmrInit(VOID)
{
    UINT16 index;
    UINT32 cpuid = ArchCurrCpuid();
    if (cpuid == 0) {
        LOS_ListInit(&g_swtmrFreeList);
        for (index = 0; index < KERNEL_SWTMR_LIMIT; index++) {
            g_osSwtmrCBArray[index].timerId = index;
            LOS_ListTailInsert(&g_swtmrFreeList, &g_osSwtmrCBArray[index].sortList.sortLinkNode);
        }
    }

#ifndef LOSCFG_BASE_CORE_SWTMR_IN_ISR
    g_percpu[cpuid].expiryList.free = KERNEL_SWTMR_LIMIT;

    if (OsSwtmrTaskCreate() != LOS_OK) {
        return LOS_ERRNO_SWTMR_TASK_CREATE_FAILED;
    }
#endif

    OsSortLinkInit(&g_percpu[cpuid].swtmrSortLink, g_swtmrSortlink[cpuid]);

    return LOS_OK;
}
LOS_SYS_INIT(OsSwtmrInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_3);

/*
 * Description: Tick interrupt interface module of software timer
 * Return     : LOS_OK on success or error code on failure
 */
LITE_OS_SEC_TEXT VOID OsSwtmrScan(VOID)
{
    SortLinkList *sortList = NULL;
    LosSwtmrCB *swtmr = NULL;
    LOS_DL_LIST *listObject = NULL;
    SortLinkAttribute* swtmrSortLink = &OsPercpuGet()->swtmrSortLink;

    SORTLINK_CURSOR_UPDATE(swtmrSortLink->cursor);
    SORTLINK_LISTOBJ_GET(listObject, swtmrSortLink);

    /*
     * it needs to be carefully coped with, since the swtmr is in specific sortlink
     * while other cores still has the chance to process it, like stop the timer.
     */
    LOS_SpinLock(&g_swtmrSpin);

    if (LOS_ListEmpty(listObject)) {
        LOS_SpinUnlock(&g_swtmrSpin);
        return;
    }
    sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
    ROLLNUM_DEC(sortList->idxRollNum);

    while (ROLLNUM(sortList->idxRollNum) == 0) {
        sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
        LOS_ListDelete(&sortList->sortLinkNode);
        swtmr = LOS_DL_LIST_ENTRY(sortList, LosSwtmrCB, sortList);

        LOS_TRACE(SWTMR_EXPIRED, swtmr->timerId);
#ifdef LOSCFG_BASE_CORE_SWTMR_IN_ISR
        SWTMR_PROC_FUNC handler = swtmr->handler;
        UINTPTR arg = swtmr->arg;
        OsSwtmrUpdate(swtmr);
        if (handler != NULL) {
            swtmr->inProcess++;
            LOS_SpinUnlock(&g_swtmrSpin);

            handler(arg); /* do swtmr callback */

            LOS_SpinLock(&g_swtmrSpin);
            swtmr->inProcess--;
        }
        OsSwtmrCheckSelfDelete(swtmr);
#else
        if (OsPercpuGet()->expiryList.free != 0) {
            swtmr->inProcess++;
            OsPercpuGet()->expiryList.buf[OsPercpuGet()->expiryList.tail] = (UINTPTR)swtmr;
            OsPercpuGet()->expiryList.tail++;
            OsPercpuGet()->expiryList.tail %= KERNEL_SWTMR_LIMIT;
            OsPercpuGet()->expiryList.free--;
            (VOID)LOS_TaskResume(OsPercpuGet()->swtmrTaskId);
        }
        OsSwtmrUpdate(swtmr);
#endif

        if (LOS_ListEmpty(listObject)) {
            break;
        }

        sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
    }

    LOS_SpinUnlock(&g_swtmrSpin);
}

/*
 * Description: Get next timeout
 * Return     : Count of the Timer list
 */
LITE_OS_SEC_TEXT UINT32 OsSwtmrGetNextTimeout(VOID)
{
    return OsSortLinkGetNextExpireTime(&OsPercpuGet()->swtmrSortLink);
}

/*
 * Description: Stop of Software Timer interface
 * Input      : swtmr --- the software timer control handler
 */
LITE_OS_SEC_TEXT STATIC VOID OsSwtmrStop(LosSwtmrCB *swtmr)
{
    SortLinkAttribute *sortLinkHeader = NULL;

#ifdef LOSCFG_KERNEL_SMP
    /*
     * the timer is running on the specific processor,
     * we need delete the timer from that processor's sortlink.
     */
    sortLinkHeader = &g_percpu[swtmr->cpuid].swtmrSortLink;
#else
    sortLinkHeader = &g_percpu[0].swtmrSortLink;
#endif
    OsDeleteSortLink(sortLinkHeader, &swtmr->sortList);

    swtmr->state = OS_SWTMR_STATUS_CREATED;
    swtmr->overrun = 0;
}

/*
 * Description: Get next software timer expiretime
 * Input      : swtmr --- the software timer control handler
 */
LITE_OS_SEC_TEXT STATIC UINT32 OsSwtmrTimeGet(const LosSwtmrCB *swtmr)
{
    SortLinkAttribute *sortLinkHeader = NULL;

#ifdef LOSCFG_KERNEL_SMP
    /*
     * the timer is running on the specific processor,
     * we need search the timer from that processor's sortlink.
     */
    sortLinkHeader = &g_percpu[swtmr->cpuid].swtmrSortLink;
#else
    sortLinkHeader = &g_percpu[0].swtmrSortLink;
#endif

    return OsSortLinkGetTargetExpireTime(sortLinkHeader, &swtmr->sortList);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_SwtmrCreate(UINT32 interval,
                                             UINT8 mode,
                                             SWTMR_PROC_FUNC handler,
                                             UINT16 *swtmrId,
                                             UINTPTR arg)
{
    LosSwtmrCB *swtmr = NULL;
    UINT32 intSave;
    SortLinkList *sortList = NULL;

    if (interval == 0) {
        return LOS_ERRNO_SWTMR_INTERVAL_NOT_SUITED;
    }

    if ((mode != LOS_SWTMR_MODE_ONCE) && (mode != LOS_SWTMR_MODE_PERIOD) &&
        (mode != LOS_SWTMR_MODE_NO_SELFDELETE)) {
        return LOS_ERRNO_SWTMR_MODE_INVALID;
    }

    if (handler == NULL) {
        return LOS_ERRNO_SWTMR_PTR_NULL;
    }

    if (swtmrId == NULL) {
        return LOS_ERRNO_SWTMR_RET_PTR_NULL;
    }

    SWTMR_LOCK(intSave);
    if (LOS_ListEmpty(&g_swtmrFreeList)) {
        SWTMR_UNLOCK(intSave);
        return LOS_ERRNO_SWTMR_MAXSIZE;
    }

    sortList = LOS_DL_LIST_ENTRY(g_swtmrFreeList.pstNext, SortLinkList, sortLinkNode);
    swtmr = LOS_DL_LIST_ENTRY(sortList, LosSwtmrCB, sortList);
    LOS_ListDelete(LOS_DL_LIST_FIRST(&g_swtmrFreeList));
    SWTMR_UNLOCK(intSave);

    swtmr->handler = handler;
    swtmr->mode = mode;
    swtmr->overrun = 0;
    swtmr->interval = interval;
    swtmr->expiry = interval;
    swtmr->arg = arg;
    swtmr->state = OS_SWTMR_STATUS_CREATED;
    SET_SORTLIST_VALUE(&(swtmr->sortList), 0);
    *swtmrId = swtmr->timerId;
    LOS_TRACE(SWTMR_CREATE, swtmr->timerId);

    return LOS_OK;
}

#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
LosSwtmrCB *OsSwtmrIdVerify(UINT16 swtmrId)
{
    LosSwtmrCB *swtmr = OS_SWT_FROM_SWTID(swtmrId);
    if (swtmr->timerId != swtmrId) {
        return NULL;
    }

    return swtmr;
}
#else

LosSwtmrCB *OsSwtmrIdVerify(UINT16 swtmrId)
{
    if (swtmrId >= KERNEL_SWTMR_LIMIT) {
        return NULL;
    }

    return OS_SWT_FROM_SWTID(swtmrId);
}
#endif

LITE_OS_SEC_TEXT STATIC INLINE VOID OsSwtmrSetTimerParms(LOSBLD_ATTRIB_UNUSED LosSwtmrCB *swtmr,
    LOSBLD_ATTRIB_UNUSED UINT32 interval, LOSBLD_ATTRIB_UNUSED UINT32 expiry)
{
#ifdef LOSCFG_COMPAT_POSIX
    if (expiry > 0) {
        swtmr->expiry = expiry;
        swtmr->interval = interval;
        if (interval == 0) {
            swtmr->mode = LOS_SWTMR_MODE_NO_SELFDELETE;
        } else {
            swtmr->mode = LOS_SWTMR_MODE_OPP;
        }
        return;
    }
#endif
#ifdef LOSCFG_COMPAT_CMSIS
    if (interval > 0) {
        swtmr->interval = interval;
        swtmr->expiry = interval;
    }
#endif
}

LITE_OS_SEC_TEXT UINT32 OsSwtmrStartTimer(UINT16 swtmrId, UINT32 interval, UINT32 expiry)
{
    LosSwtmrCB *swtmr = NULL;
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    SWTMR_LOCK(intSave);
    swtmr = OsSwtmrIdVerify(swtmrId);
    if (swtmr == NULL) {
        SWTMR_UNLOCK(intSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (swtmr->state) {
        case OS_SWTMR_STATUS_UNUSED:
            /* fall-through */
        case OS_SWTMR_STATUS_DELETING:
            ret = LOS_ERRNO_SWTMR_NOT_CREATED;
            break;
        /*
         * If the status of swtmr is timing, it should stop the swtmr first,
         * then start the swtmr again.
         */
        case OS_SWTMR_STATUS_TICKING:
            OsSwtmrStop(swtmr);
            /* fall-through */
        case OS_SWTMR_STATUS_CREATED:
            OsSwtmrSetTimerParms(swtmr, interval, expiry);
            OsSwtmrStart(swtmr);
            break;
        default:
            ret = LOS_ERRNO_SWTMR_STATUS_INVALID;
            break;
    }

    SWTMR_UNLOCK(intSave);
    LOS_TRACE(SWTMR_START, swtmr->timerId, swtmr->mode, swtmr->overrun, swtmr->interval, swtmr->expiry);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_SwtmrStart(UINT16 swtmrId)
{
    return OsSwtmrStartTimer(swtmrId, 0, 0);
}

LITE_OS_SEC_TEXT UINT32 LOS_SwtmrStop(UINT16 swtmrId)
{
    LosSwtmrCB *swtmr = NULL;
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    SWTMR_LOCK(intSave);
    swtmr = OsSwtmrIdVerify(swtmrId);
    if (swtmr == NULL) {
        SWTMR_UNLOCK(intSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (swtmr->state) {
        case OS_SWTMR_STATUS_UNUSED:
            /* fall-through */
        case OS_SWTMR_STATUS_DELETING:
            ret = LOS_ERRNO_SWTMR_NOT_CREATED;
            break;
        case OS_SWTMR_STATUS_CREATED:
            ret = LOS_ERRNO_SWTMR_NOT_STARTED;
            break;
        case OS_SWTMR_STATUS_TICKING:
            OsSwtmrStop(swtmr);
            break;
        default:
            ret = LOS_ERRNO_SWTMR_STATUS_INVALID;
            break;
    }

    SWTMR_UNLOCK(intSave);
    LOS_TRACE(SWTMR_STOP, swtmr->timerId);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_SwtmrTimeGet(UINT16 swtmrId, UINT32 *tick)
{
    LosSwtmrCB *swtmr = NULL;
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    if (tick == NULL) {
        return LOS_ERRNO_SWTMR_TICK_PTR_NULL;
    }

    SWTMR_LOCK(intSave);
    swtmr = OsSwtmrIdVerify(swtmrId);
    if (swtmr == NULL) {
        SWTMR_UNLOCK(intSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (swtmr->state) {
        case OS_SWTMR_STATUS_UNUSED:
            /* fall-through */
        case OS_SWTMR_STATUS_DELETING:
            ret = LOS_ERRNO_SWTMR_NOT_CREATED;
            break;
        case OS_SWTMR_STATUS_CREATED:
            ret = LOS_ERRNO_SWTMR_NOT_STARTED;
            break;
        case OS_SWTMR_STATUS_TICKING:
            *tick = OsSwtmrTimeGet(swtmr);
            break;
        default:
            ret = LOS_ERRNO_SWTMR_STATUS_INVALID;
            break;
    }
    SWTMR_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsSwtmrDeleteOption(UINT16 swtmrId, BOOL isSync)
{
    LosSwtmrCB *swtmr = NULL;
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    SWTMR_LOCK(intSave);
    swtmr = OsSwtmrIdVerify(swtmrId);
    if (swtmr == NULL) {
        SWTMR_UNLOCK(intSave);
        return LOS_ERRNO_SWTMR_ID_INVALID;
    }

    switch (swtmr->state) {
        case OS_SWTMR_STATUS_UNUSED:
#ifndef LOSCFG_SWTMR_SYNC_DELETE
            /* fall-through */
        case OS_SWTMR_STATUS_DELETING:
#endif
            ret = LOS_ERRNO_SWTMR_NOT_CREATED;
            break;
        case OS_SWTMR_STATUS_TICKING:
            OsSwtmrStop(swtmr);
            /* fall-through */
        case OS_SWTMR_STATUS_CREATED:
            swtmr->state = OS_SWTMR_STATUS_DELETING;
#ifdef LOSCFG_SWTMR_SYNC_DELETE
            /* fall-through */
        case OS_SWTMR_STATUS_DELETING:
            if (isSync) {
                while (swtmr->inProcess != 0) {
                    SWTMR_UNLOCK(intSave);
                    LOS_TaskDelay(1);
                    SWTMR_LOCK(intSave);
                }
                if (swtmr->timerId != swtmrId) {
                    goto EXIT;
                }
            } else {
#endif
                (VOID)isSync;
                if (swtmr->inProcess != 0) {
                    goto EXIT;
                }
#ifdef LOSCFG_SWTMR_SYNC_DELETE
            }
#endif
            OsSwtmrDelete(swtmr);
            break;
        default:
            ret = LOS_ERRNO_SWTMR_STATUS_INVALID;
            break;
    }

EXIT:
    SWTMR_UNLOCK(intSave);
    LOS_TRACE(SWTMR_DELETE, swtmr->timerId);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_SwtmrDelete(UINT16 swtmrId)
{
    return OsSwtmrDeleteOption(swtmrId, FALSE);
}

#ifdef LOSCFG_SWTMR_SYNC_DELETE
LITE_OS_SEC_TEXT UINT32 LOS_SwtmrSyncDelete(UINT16 swtmrId)
{
#ifdef LOSCFG_BASE_CORE_SWTMR_IN_ISR
    if (OS_INT_ACTIVE) {
#else
    if (OS_INT_ACTIVE || (LOS_CurTaskIDGet() == OsPercpuGet()->swtmrTaskId)) {
#endif
        return LOS_ERRNO_SWTMR_INVALID_SYNCDEL;
    }
    return OsSwtmrDeleteOption(swtmrId, TRUE);
}
#endif /* LOSCFG_SWTMR_SYNC_DELETE */

#endif /* LOSCFG_BASE_CORE_SWTMR */
