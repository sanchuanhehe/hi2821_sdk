/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Mutex
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

#include "los_mux_pri.h"
#include "los_mux_debug_pri.h"
#include "los_lockdep.h"
#include "los_lockdep_pri.h"
#include "los_bitmap.h"
#include "los_exc.h"
#include "los_spinlock.h"
#include "los_mp_pri.h"
#include "los_percpu_pri.h"
#include "los_trace.h"

LITE_OS_SEC_BSS LosMuxCB *g_osAllMux = NULL;
LITE_OS_SEC_BSS STATIC LOS_DL_LIST g_unusedMuxList;

STATIC INLINE VOID OsMuxNodeRecycle(LosMuxCB *muxNode)
{
    muxNode->muxStat = LOS_UNUSED;
    LOS_ListTailInsert(&g_unusedMuxList, &muxNode->muxList);
}

LITE_OS_SEC_TEXT UINT32 OsMuxInit(VOID)
{
    LosMuxCB *muxNode = NULL;
    UINT32 index;

    LOS_ListInit(&g_unusedMuxList);

    for (index = 0; index < KERNEL_MUX_LIMIT; index++) {
        muxNode = g_osAllMux + index;
        muxNode->muxId = index;
        OsMuxNodeRecycle(muxNode);
    }

    if (OsMuxDbgInitHook() != LOS_OK) {
        return LOS_ERRNO_MUX_NO_MEMORY;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 LOS_MuxCreate(UINT32 *muxHandle)
{
    UINT32 intSave;
    LosMuxCB *muxCreated = NULL;
    LOS_DL_LIST *unusedMux = NULL;

    if (muxHandle == NULL) {
        return LOS_ERRNO_MUX_PTR_NULL;
    }

    SCHEDULER_LOCK(intSave);
    if (LOS_ListEmpty(&g_unusedMuxList)) {
        SCHEDULER_UNLOCK(intSave);
        OsMutexCheckHook();
        return LOS_ERRNO_MUX_ALL_BUSY;
    }

    unusedMux = LOS_DL_LIST_FIRST(&g_unusedMuxList);
    LOS_ListDelete(unusedMux);
    muxCreated = LOS_DL_LIST_ENTRY(unusedMux, LosMuxCB, muxList);
    muxCreated->muxCount = 0;
    muxCreated->muxStat = LOS_USED;
    muxCreated->owner = NULL;
    LOS_ListInit(&muxCreated->muxList);
    *muxHandle = muxCreated->muxId;

    OsMuxDbgUpdateHook(muxCreated->muxId, OsCurrTaskGet()->taskEntry);
    MUXDEP_CHECK_INIT(muxCreated->muxId);
    SCHEDULER_UNLOCK(intSave);

    LOS_TRACE(MUX_CREATE, muxCreated->muxId);
    return LOS_OK;
}

#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
STATIC INLINE UINT32 OsMuxStateVerify(const LosMuxCB *muxCB, UINT32 muxHandle)
{
    if ((muxCB->muxId != muxHandle) || (muxCB->muxStat == LOS_UNUSED)) {
        return LOS_ERRNO_MUX_INVALID;
    }

    return LOS_OK;
}
STATIC INLINE VOID OsMuxIdUpdate(LosMuxCB *muxDeleted)
{
    muxDeleted->muxId = SET_MUX_ID(GET_MUX_COUNT(muxDeleted->muxId) + 1, GET_MUX_INDEX(muxDeleted->muxId));
}
#else
STATIC INLINE UINT32 OsMuxStateVerify(const LosMuxCB *muxCB, UINT32 muxHandle)
{
    (VOID)muxHandle;

    if (muxCB->muxStat == LOS_UNUSED) {
        return LOS_ERRNO_MUX_INVALID;
    }

    return LOS_OK;
}

STATIC INLINE VOID OsMuxIdUpdate(LosMuxCB *muxDeleted)
{
    (VOID)muxDeleted;
}
#endif

LITE_OS_SEC_TEXT UINT32 LOS_MuxDelete(UINT32 muxHandle)
{
    UINT32 intSave;
    LosMuxCB *muxDeleted = NULL;
    UINT32 ret;

    if (GET_MUX_INDEX(muxHandle) >= (UINT32)KERNEL_MUX_LIMIT) {
        return LOS_ERRNO_MUX_INVALID;
    }

    muxDeleted = GET_MUX(muxHandle);

    LOS_TRACE(MUX_DELETE, muxHandle, muxDeleted->muxStat, muxDeleted->muxCount,
        ((muxDeleted->owner == NULL) ? 0xFFFFFFFF : muxDeleted->owner->taskId));

    SCHEDULER_LOCK(intSave);

    ret = OsMuxStateVerify(muxDeleted, muxHandle);
    if (ret != LOS_OK) {
        goto OUT;
    }

    if (!LOS_ListEmpty(&muxDeleted->muxList) || muxDeleted->muxCount) {
        ret = LOS_ERRNO_MUX_PENDED;
        goto OUT;
    }
    MUXDEP_CHECK_DEINIT(muxDeleted->muxId);
    OsMuxIdUpdate(muxDeleted);
    OsMuxNodeRecycle(muxDeleted);

    OsMuxDbgUpdateHook(muxDeleted->muxId, NULL);

 OUT:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsMuxParaCheck(const LosMuxCB *muxCB, UINT32 muxHandle)
{
    UINT32 ret = OsMuxStateVerify(muxCB, muxHandle);
    if (ret != LOS_OK) {
        return ret;
    }

    OsMuxDbgTimeUpdateHook(muxCB->muxId);

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_MUX_PEND_INTERR;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT STATIC VOID OsMuxBitmapSet(const LosTaskCB *runTask, const MuxBaseCB *muxPended)
{
    if (GET_MUX_OWNER(muxPended)->priority > runTask->priority) {
        LOS_BitmapSet(&(GET_MUX_OWNER(muxPended)->priBitMap), GET_MUX_OWNER(muxPended)->priority);
        OsSchedPrioModify(GET_MUX_OWNER(muxPended), runTask->priority);
    }
}

LITE_OS_SEC_TEXT STATIC VOID OsMuxBitmapRestore(const LosTaskCB *runTask, LosTaskCB *owner)
{
    UINT16 bitMapPri;

    if (owner->priority >= runTask->priority) {
        bitMapPri = LOS_LowBitGet(owner->priBitMap);
        if (bitMapPri != LOS_INVALID_BIT_INDEX) {
            LOS_BitmapClr(&(owner->priBitMap), bitMapPri);
            OsSchedPrioModify(owner, bitMapPri);
        }
    } else {
        if (LOS_HighBitGet(owner->priBitMap) != runTask->priority) {
            LOS_BitmapClr(&(owner->priBitMap), runTask->priority);
        }
    }
}

#ifdef LOSCFG_MUTEX_WAITMODE_PRIO
LITE_OS_SEC_TEXT STATIC LOS_DL_LIST *OsMuxPendFindPosSub(const LosTaskCB *runTask, const MuxBaseCB *muxPended)
{
    LosTaskCB *pendedTask = NULL;
    LOS_DL_LIST *node = NULL;

    LOS_DL_LIST_FOR_EACH_ENTRY(pendedTask, &(muxPended->muxList), LosTaskCB, pendList) {
        if (pendedTask->priority < runTask->priority) {
            continue;
        } else if (pendedTask->priority > runTask->priority) {
            node = &pendedTask->pendList;
            break;
        } else {
            node = pendedTask->pendList.pstNext;
            break;
        }
    }

    return node;
}

LITE_OS_SEC_TEXT STATIC LOS_DL_LIST *OsMuxPendFindPos(const LosTaskCB *runTask, MuxBaseCB *muxPended)
{
    LOS_DL_LIST *node = NULL;
    LosTaskCB *pendedTask1 = NULL;
    LosTaskCB *pendedTask2 = NULL;

    if (LOS_ListEmpty(&muxPended->muxList)) {
        node = &muxPended->muxList;
    } else {
        pendedTask1 = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&muxPended->muxList));
        pendedTask2 = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_LAST(&muxPended->muxList));
        if ((pendedTask1 != NULL) && (pendedTask1->priority > runTask->priority)) {
            node = muxPended->muxList.pstNext;
        } else if ((pendedTask2 != NULL) && (pendedTask2->priority <= runTask->priority)) {
            node = &muxPended->muxList;
        } else {
            node = OsMuxPendFindPosSub(runTask, muxPended);
        }
    }
    return node;
}
#else
LITE_OS_SEC_TEXT STATIC LOS_DL_LIST *OsMuxPendFindPos(const LosTaskCB *runTask, MuxBaseCB *muxPended)
{
    (VOID)runTask;
    LOS_DL_LIST *node = NULL;
    node = &muxPended->muxList;
    return node;
}
#endif

LITE_OS_SEC_TEXT UINT32 OsMuxPendOp(LosTaskCB *runTask, MuxBaseCB *muxPended, UINT32 timeout)
{
    LOS_DL_LIST *node = NULL;
    UINT32 ret = LOS_OK;
    LosTaskCB *owner = GET_MUX_OWNER(muxPended);

    runTask->taskMux = (VOID *)muxPended;
    node = OsMuxPendFindPos(runTask, muxPended);

    OsSchedWait(runTask, node, timeout);

    if (runTask->taskStatus & OS_TASK_STATUS_TIMEOUT) {
        runTask->taskStatus &= ~OS_TASK_STATUS_TIMEOUT;
        ret = LOS_ERRNO_MUX_TIMEOUT;
    }

    if (timeout != LOS_WAIT_FOREVER) {
        OsMuxBitmapRestore(runTask, owner);
    }

    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_MuxPend(UINT32 muxHandle, UINT32 timeout)
{
    UINT32 ret, intSave;
    LosMuxCB *muxPended = NULL;
    LosTaskCB *runTask = NULL;

    if (GET_MUX_INDEX(muxHandle) >= (UINT32)KERNEL_MUX_LIMIT) {
        return LOS_ERRNO_MUX_INVALID;
    }

    muxPended = GET_MUX(muxHandle);

    LOS_TRACE(MUX_PEND, muxHandle, muxPended->muxCount,
        ((muxPended->owner == NULL) ? 0xFFFFFFFF : GET_MUX_OWNER(muxPended)->taskId), timeout);

    runTask = OsCurrTaskGet();
    if (runTask->taskFlags & OS_TASK_FLAG_SYSTEM) {
        PRINT_DEBUG("Warning: DO NOT recommend to use %s in system tasks.\n", __FUNCTION__);
    }
    SCHEDULER_LOCK(intSave);

    ret = OsMuxParaCheck(muxPended, muxHandle);
    if (ret != LOS_OK) {
        goto OUT_UNLOCK;
    }

    if (muxPended->owner == runTask) {
        muxPended->muxCount++;
        goto OUT_UNLOCK;
    }

    MUXDEP_CHECK_IN(muxHandle);

    if (muxPended->muxCount == 0) {
        muxPended->muxCount++;
        muxPended->owner = runTask;
        goto DEP_RECORD;
    }

    if (!timeout) {
        ret = LOS_ERRNO_MUX_UNAVAILABLE;
        goto OUT_UNLOCK;
    }

    if (!OsPreemptableInSched()) {
        SCHEDULER_UNLOCK(intSave);
        PRINT_DEBUG("!!!LOS_ERRNO_MUX_PEND_IN_LOCK!!!\n");
        return LOS_ERRNO_MUX_PEND_IN_LOCK;
    }

    OsMuxBitmapSet(runTask, (MuxBaseCB *)muxPended);
    ret = OsMuxPendOp(runTask, (MuxBaseCB *)muxPended, timeout);
    if (ret != LOS_OK) {
        goto OUT_UNLOCK;
    }

DEP_RECORD:
    MUXDEP_RECORD(muxHandle);
OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT STATIC VOID OsMuxPostOpSub(LosTaskCB *runTask, MuxBaseCB *muxPosted)
{
    LosTaskCB *pendedTask = NULL;
    UINT16 bitMapPri;

    if (!LOS_ListEmpty(&muxPosted->muxList)) {
        bitMapPri = LOS_HighBitGet(runTask->priBitMap);
        LOS_DL_LIST_FOR_EACH_ENTRY(pendedTask, (&muxPosted->muxList), LosTaskCB, pendList) {
            if (bitMapPri != pendedTask->priority) {
                LOS_BitmapClr(&runTask->priBitMap, pendedTask->priority);
            }
        }
    }
    bitMapPri = LOS_LowBitGet(runTask->priBitMap);
    LOS_BitmapClr(&runTask->priBitMap, bitMapPri);
    OsSchedPrioModify(GET_MUX_OWNER(muxPosted), bitMapPri);
}

LITE_OS_SEC_TEXT UINT32 OsMuxPostOp(LosTaskCB *runTask, MuxBaseCB *muxPosted)
{
    LosTaskCB *resumedTask = NULL;

    if (LOS_ListEmpty(&muxPosted->muxList)) {
        muxPosted->owner = NULL;
        return MUX_NO_SCHEDULE;
    }

    resumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&(muxPosted->muxList)));
#ifdef LOSCFG_MUTEX_WAITMODE_PRIO
    if (resumedTask->priority > runTask->priority) {
        if (LOS_HighBitGet(runTask->priBitMap) != resumedTask->priority) {
            LOS_BitmapClr(&runTask->priBitMap, resumedTask->priority);
        }
    } else if (runTask->priBitMap != 0) {
        OsMuxPostOpSub(runTask, muxPosted);
    }
#else
    if (runTask->priBitMap != 0) {
        OsMuxPostOpSub(runTask, muxPosted);
    }
#endif

    muxPosted->muxCount = 1;
    muxPosted->owner = resumedTask;
    resumedTask->taskMux = NULL;

    OsSchedWake(resumedTask);

    return MUX_SCHEDULE;
}

LITE_OS_SEC_TEXT UINT32 LOS_MuxPost(UINT32 muxHandle)
{
    UINT32 ret;
    LosTaskCB *runTask = NULL;
    LosMuxCB *muxPosted = NULL;
    UINT32 intSave;

    if (GET_MUX_INDEX(muxHandle) >= (UINT32)KERNEL_MUX_LIMIT) {
        return LOS_ERRNO_MUX_INVALID;
    }

    muxPosted = GET_MUX(muxHandle);

    LOS_TRACE(MUX_POST, muxHandle, muxPosted->muxCount,
        ((muxPosted->owner == NULL) ? 0xFFFFFFFF : muxPosted->owner->taskId));

    SCHEDULER_LOCK(intSave);

    ret = OsMuxParaCheck(muxPosted, muxHandle);
    if (ret != LOS_OK) {
        goto OUT_UNLOCK;
    }

    runTask = OsCurrTaskGet();
    if ((muxPosted->muxCount == 0) || (muxPosted->owner != runTask)) {
        ret = LOS_ERRNO_MUX_INVALID;
        goto OUT_UNLOCK;
    }

    if (--muxPosted->muxCount != 0) {
        goto OUT_UNLOCK;
    }

    ret = OsMuxPostOp(runTask, (MuxBaseCB *)muxPosted);
    MUXDEP_CHECK_OUT(muxHandle);
    SCHEDULER_UNLOCK(intSave);
    if (ret == MUX_SCHEDULE) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }

    return LOS_OK;

OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
