/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Semaphore
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

#include "los_sem_pri.h"
#include "los_sem_debug_pri.h"
#include "los_lockdep_pri.h"
#include "los_task_pri.h"
#include "los_spinlock.h"
#include "los_mp_pri.h"
#include "los_percpu_pri.h"
#include "los_trace.h"

LITE_OS_SEC_BSS LosSemCB *g_osAllSem = NULL;
LITE_OS_SEC_BSS STATIC LOS_DL_LIST g_unusedSemList;

STATIC INLINE VOID OsSemNodeRecycle(LosSemCB *semNode)
{
    semNode->semStat = LOS_UNUSED;
    LOS_ListTailInsert(&g_unusedSemList, &semNode->semList);
}

LITE_OS_SEC_TEXT_INIT UINT32 OsSemInit(VOID)
{
    LosSemCB *semNode = NULL;
    UINT32 index;

    LOS_ListInit(&g_unusedSemList);

    for (index = 0; index < KERNEL_SEM_LIMIT; index++) {
        semNode = g_osAllSem + index;
        semNode->semId = index;
        OsSemNodeRecycle(semNode);
    }

    if (OsSemDbgInitHook() != LOS_OK) {
        return LOS_ERRNO_SEM_NO_MEMORY;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT STATIC UINT32 OsSemCreate(UINT16 count, UINT8 type, UINT32 *semHandle)
{
    UINT32 intSave;
    LosSemCB *semCreated = NULL;
    LOS_DL_LIST *unusedSem = NULL;

    if (semHandle == NULL) {
        return LOS_ERRNO_SEM_PTR_NULL;
    }

    SCHEDULER_LOCK(intSave);

    if (LOS_ListEmpty(&g_unusedSemList)) {
        SCHEDULER_UNLOCK(intSave);
        OsSemInfoGetFullDataHook();
        return LOS_ERRNO_SEM_ALL_BUSY;
    }

    unusedSem = LOS_DL_LIST_FIRST(&g_unusedSemList);
    LOS_ListDelete(unusedSem);
    semCreated = GET_SEM_LIST(unusedSem);
    semCreated->semStat = LOS_USED;
    semCreated->semType = type;
    semCreated->semCount = count;
    LOS_ListInit(&semCreated->semList);
    *semHandle = semCreated->semId;

    OsSemDbgUpdateHook(semCreated->semId, OsCurrTaskGet()->taskEntry, count);
    SEMDEP_CHECK_INIT(type, semCreated->semId);

    SCHEDULER_UNLOCK(intSave);

    LOS_TRACE(SEM_CREATE, semCreated->semId, type, count);
    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_SemCreate(UINT16 count, UINT32 *semHandle)
{
    if (count > LOS_SEM_COUNT_MAX) {
        return LOS_ERRNO_SEM_OVERFLOW;
    }
    return OsSemCreate(count, OS_SEM_COUNTING, semHandle);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_BinarySemCreate(UINT16 count, UINT32 *semHandle)
{
    if (count > OS_SEM_BINARY_COUNT_MAX) {
        return LOS_ERRNO_SEM_OVERFLOW;
    }
    return OsSemCreate(count, OS_SEM_BINARY, semHandle);
}

#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
STATIC INLINE UINT32 OsSemStateVerify(UINT32 semId, const LosSemCB *semNode)
{
    if ((semNode->semStat == LOS_UNUSED) || (semNode->semId != semId)) {
        return LOS_ERRNO_SEM_INVALID;
    }
    return LOS_OK;
}

STATIC INLINE VOID OsSemIdUpdate(LosSemCB *semDeleted)
{
    semDeleted->semId = SET_SEM_ID(GET_SEM_COUNT(semDeleted->semId) + 1, GET_SEM_INDEX(semDeleted->semId));
}
#else
STATIC INLINE UINT32 OsSemStateVerify(UINT32 semId, const LosSemCB *semNode)
{
    (VOID)semId;

    if (semNode->semStat == LOS_UNUSED) {
        return LOS_ERRNO_SEM_INVALID;
    }
    return LOS_OK;
}

STATIC INLINE VOID OsSemIdUpdate(LosSemCB *semDeleted)
{
    (VOID)semDeleted;
}
#endif

LITE_OS_SEC_TEXT_INIT UINT32 LOS_SemDelete(UINT32 semHandle)
{
    UINT32 intSave;
    LosSemCB *semDeleted = NULL;
    UINT32 ret;

    if (GET_SEM_INDEX(semHandle) >= (UINT32)KERNEL_SEM_LIMIT) {
        return LOS_ERRNO_SEM_INVALID;
    }

    semDeleted = GET_SEM(semHandle);

    SCHEDULER_LOCK(intSave);

    ret = OsSemStateVerify(semHandle, semDeleted);
    if (ret != LOS_OK) {
        goto OUT;
    }

    if (!LOS_ListEmpty(&semDeleted->semList)) {
        ret = LOS_ERRNO_SEM_PENDED;
        goto OUT;
    }
    SEMDEP_CHECK_DEINIT(semDeleted->semType, semHandle);

    OsSemIdUpdate(semDeleted);
    OsSemNodeRecycle(semDeleted);

    OsSemDbgUpdateHook(semDeleted->semId, NULL, 0);

OUT:
    SCHEDULER_UNLOCK(intSave);

    LOS_TRACE(SEM_DELETE, semHandle, ret);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_SemPend(UINT32 semHandle, UINT32 timeout)
{
    UINT32 intSave;
    LosSemCB *semPended = NULL;
    UINT32 ret;
    LosTaskCB *runTask = NULL;

    if (GET_SEM_INDEX(semHandle) >= (UINT32)KERNEL_SEM_LIMIT) {
        return LOS_ERRNO_SEM_INVALID;
    }

    semPended = GET_SEM(semHandle);

    LOS_TRACE(SEM_PEND, semHandle, semPended->semCount, timeout);

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_SEM_PEND_INTERR;
    }

    runTask = OsCurrTaskGet();
    if (runTask->taskFlags & OS_TASK_FLAG_SYSTEM) {
        PRINT_DEBUG("Warning: DO NOT recommend to use %s in system tasks.\n", __FUNCTION__);
    }

    if (!OsPreemptable()) {
        return LOS_ERRNO_SEM_PEND_IN_LOCK;
    }

    SCHEDULER_LOCK(intSave);

    ret = OsSemStateVerify(semHandle, semPended);
    if (ret != LOS_OK) {
        goto OUT;
    }
    SEMDEP_CHECK_IN(semPended->semType, semHandle);
    /* Update the operate time, no matter the actual Pend success or not */
    OsSemDbgTimeUpdateHook(semHandle);
    if (semPended->semCount > 0) {
        semPended->semCount--;
        goto OUT;
    } else if (!timeout) {
        ret = LOS_ERRNO_SEM_UNAVAILABLE;
        goto OUT;
    }

    runTask->taskSem = (VOID *)semPended;

    OsSchedWait(runTask, &semPended->semList, timeout);

    if (runTask->taskStatus & OS_TASK_STATUS_TIMEOUT) {
        runTask->taskStatus &= ~OS_TASK_STATUS_TIMEOUT;
        ret = LOS_ERRNO_SEM_TIMEOUT;
    }

OUT:
    if (ret == LOS_OK) {
        SEMDEP_RECORD(semPended->semType, semHandle);
    }
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_SemPost(UINT32 semHandle)
{
    UINT32 intSave;
    LosSemCB *semPosted = NULL;
    LosTaskCB *resumedTask = NULL;
    UINT16 maxCount;
    UINT32 ret;

    if (GET_SEM_INDEX(semHandle) >= (UINT32)KERNEL_SEM_LIMIT) {
        return LOS_ERRNO_SEM_INVALID;
    }

    semPosted = GET_SEM(semHandle);

    LOS_TRACE(SEM_POST, semHandle, semPosted->semType, semPosted->semCount);

    SCHEDULER_LOCK(intSave);

    ret = OsSemStateVerify(semHandle, semPosted);
    if (ret != LOS_OK) {
        goto OUT;
    }

    /* Update the operate time, no matter the actual Post success or not */
    OsSemDbgTimeUpdateHook(semHandle);

    maxCount = (semPosted->semType == OS_SEM_COUNTING) ? LOS_SEM_COUNT_MAX : OS_SEM_BINARY_COUNT_MAX;
    if (semPosted->semCount >= maxCount) {
        ret = LOS_ERRNO_SEM_OVERFLOW;
        goto OUT;
    }

    if (LOS_ListEmpty(&semPosted->semList)) {
        semPosted->semCount++;
        goto OUT;
    }

    resumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&(semPosted->semList)));
    resumedTask->taskSem = NULL;

    OsSchedWake(resumedTask);

    SEMDEP_CHECK_OUT(semPosted->semType, semHandle);
    SCHEDULER_UNLOCK(intSave);
    LOS_MpSchedule(OS_MP_CPU_ALL);
    LOS_Schedule();
    return LOS_OK;

OUT:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
