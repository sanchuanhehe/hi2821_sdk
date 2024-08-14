/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Queue
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

#include "los_queue_pri.h"
#include "los_queue_debug_pri.h"
#include "los_task_pri.h"
#include "los_spinlock.h"
#include "los_mp_pri.h"
#include "los_percpu_pri.h"
#include "los_trace.h"

#if !defined(LOSCFG_QUEUE_DYNAMIC_ALLOCATION) && !defined(LOSCFG_QUEUE_STATIC_ALLOCATION)
#error "MUST choose one mode for creating a queue!"
#endif

LITE_OS_SEC_BSS LosQueueCB *g_osAllQueue = NULL;
LITE_OS_SEC_BSS STATIC LOS_DL_LIST g_freeQueueList;

/* Error number for OsQueueBufferOperate */
#define OS_QUEUE_OPERATE_ERROR_INVALID_TYPE       1
#define OS_QUEUE_OPERATE_ERROR_MEMCPYS_MSG2BUF    2
#define OS_QUEUE_OPERATE_ERROR_MEMCPYS_STRMSG     3

STATIC INLINE VOID OsQueueNodeRecycle(LosQueueCB *queueNode)
{
    queueNode->queueState = LOS_UNUSED;
    LOS_ListTailInsert(&g_freeQueueList, &queueNode->readWriteList[OS_QUEUE_WRITE]);
}

LITE_OS_SEC_TEXT_INIT UINT32 OsQueueInit(VOID)
{
    LosQueueCB *queueNode = NULL;
    UINT32 index;

    LOS_ListInit(&g_freeQueueList);

    for (index = 0; index < KERNEL_QUEUE_LIMIT; index++) {
        queueNode = g_osAllQueue + index;
        queueNode->queueId = index;
        OsQueueNodeRecycle(queueNode);
    }

    if (OsQueueDbgInitHook() != LOS_OK) {
        return LOS_ERRNO_QUEUE_NO_MEMORY;
    }
    return LOS_OK;
}

STATIC INLINE UINT32 OsQueueCreateParameterCheck(UINT16 len, const UINT32 *queueId, UINT16 maxMsgSize)
{
    if (queueId == NULL) {
        return LOS_ERRNO_QUEUE_CREAT_PTR_NULL;
    }

    if ((len == 0) || (maxMsgSize == 0)) {
        return LOS_ERRNO_QUEUE_PARA_ISZERO;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT STATIC UINT32 OsQueueCreateInternal(UINT16 len, UINT32 *queueId, UINT16 msgSize,
                                                          UINT8 *queue, UINT8 queueMemType)
{
    LosQueueCB *queueCB = NULL;
    LOS_DL_LIST *unusedQueue = NULL;
    UINT32 intSave;

    SCHEDULER_LOCK(intSave);
    if (LOS_ListEmpty(&g_freeQueueList)) {
        SCHEDULER_UNLOCK(intSave);
        OsQueueCheckHook();
        return LOS_ERRNO_QUEUE_CB_UNAVAILABLE;
    }

    unusedQueue = LOS_DL_LIST_FIRST(&g_freeQueueList);
    LOS_ListDelete(unusedQueue);
    queueCB = GET_QUEUE_LIST(unusedQueue);
    queueCB->queueLen = len;
    queueCB->queueSize = msgSize;
    queueCB->queueHandle = queue;
    queueCB->queueState = LOS_USED;
    queueCB->queueMemType = queueMemType;
    queueCB->readWriteableCnt[OS_QUEUE_READ] = 0;
    queueCB->readWriteableCnt[OS_QUEUE_WRITE] = len;
    queueCB->queueHead = 0;
    queueCB->queueTail = 0;
    LOS_ListInit(&queueCB->readWriteList[OS_QUEUE_READ]);
    LOS_ListInit(&queueCB->readWriteList[OS_QUEUE_WRITE]);
    LOS_ListInit(&queueCB->memList);

    OsQueueDbgUpdateHook(queueCB->queueId, OsCurrTaskGet()->taskEntry);
    SCHEDULER_UNLOCK(intSave);

    *queueId = queueCB->queueId;

    LOS_TRACE(QUEUE_CREATE, *queueId, len, msgSize, (UINTPTR)queue, queueMemType);
    return LOS_OK;
}

#ifdef LOSCFG_QUEUE_STATIC_ALLOCATION
LITE_OS_SEC_TEXT_INIT UINT32 LOS_QueueCreateStatic(const CHAR *queueName,
                                                   UINT16 len,
                                                   UINT32 *queueId,
                                                   UINT32 flags,
                                                   UINT16 maxMsgSize,
                                                   VOID *queueMem,
                                                   UINT16 memSize)
{
    UINT32 ret;
    UINT32 msgSize;
    (VOID)queueName;
    (VOID)flags;

    ret = OsQueueCreateParameterCheck(len, queueId, maxMsgSize);
    if (ret != LOS_OK) {
        return ret;
    }

    if (queueMem == NULL) {
        return LOS_ERRNO_QUEUE_CREAT_PTR_NULL;
    }

    msgSize = (UINT32)maxMsgSize + (UINT32)sizeof(QueueMsgHead);
    if ((memSize / msgSize) < len) {
        return LOS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }

    return OsQueueCreateInternal(len, queueId, maxMsgSize, queueMem, OS_QUEUE_ALLOC_STATIC);
}
#endif

#ifdef LOSCFG_QUEUE_DYNAMIC_ALLOCATION
LITE_OS_SEC_TEXT_INIT UINT32 LOS_QueueCreate(const CHAR *queueName, UINT16 len, UINT32 *queueId,
                                             UINT32 flags, UINT16 maxMsgSize)
{
    UINT32 ret;
    UINT8 *queueMem = NULL;
    UINT32 msgSize;
    (VOID)queueName;
    (VOID)flags;

    ret = OsQueueCreateParameterCheck(len, queueId, maxMsgSize);
    if (ret != LOS_OK) {
        return ret;
    }

    msgSize = (UINT32)maxMsgSize + (UINT32)sizeof(QueueMsgHead);
    if ((UINT32_MAX / msgSize) < len) {
        return LOS_ERRNO_QUEUE_SIZE_TOO_BIG;
    }
    /*
     * Memory allocation is time-consuming, to shorten the time of disable interrupt,
     * move the memory allocation to here.
     */
    queueMem = (UINT8 *)LOS_MemAlloc(m_aucSysMem1, (UINT32)len * msgSize);
    if (queueMem == NULL) {
        return LOS_ERRNO_QUEUE_CREATE_NO_MEMORY;
    }

    ret = OsQueueCreateInternal(len, queueId, maxMsgSize, queueMem, OS_QUEUE_ALLOC_DYNAMIC);
    if (ret != LOS_OK) {
        (VOID)LOS_MemFree(m_aucSysMem1, queueMem);
        return ret;
    }

    return LOS_OK;
}
#endif

LITE_OS_SEC_TEXT STATIC UINT32 OsQueueReadParameterCheck(UINT32 queueId, const VOID *bufferAddr,
                                                         const UINT32 *bufferSize, UINT32 timeout)
{
    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }

    if ((bufferAddr == NULL) || (bufferSize == NULL)) {
        return LOS_ERRNO_QUEUE_READ_PTR_NULL;
    }

    OsQueueDbgTimeUpdateHook(queueId);

    if (timeout != LOS_NO_WAIT) {
        if (OS_INT_ACTIVE) {
            return LOS_ERRNO_QUEUE_READ_IN_INTERRUPT;
        }
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsQueueWriteParameterCheck(UINT32 queueId, const VOID *bufferAddr,
                                                          const UINT32 *bufferSize, UINT32 timeout)
{
    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }

    if (bufferAddr == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }

    if (*bufferSize == 0) {
        return LOS_ERRNO_QUEUE_WRITESIZE_ISZERO;
    }

    OsQueueDbgTimeUpdateHook(queueId);

    if (timeout != LOS_NO_WAIT) {
        if (OS_INT_ACTIVE) {
            return LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT;
        }
    }
    return LOS_OK;
}

STATIC UINT32 OsQueueBufferOperate(LosQueueCB *queueCB, UINT32 operateType, VOID *bufferAddr, UINT32 *bufferSize)
{
    UINT8 *queueMsgHeadNode = NULL;
    UINT8 *queueNode = NULL;
    QueueMsgHead msgDataSize;
    UINT16 queuePos;

    /* get the queue position */
    switch (OS_QUEUE_OPERATE_GET(operateType)) {
        case OS_QUEUE_READ_HEAD:
            queuePos = queueCB->queueHead;
            queueCB->queueHead = (UINT16)(((queueCB->queueHead + 1) == queueCB->queueLen) ?
                                          0 : (queueCB->queueHead + 1));
            break;
        case OS_QUEUE_WRITE_HEAD:
            queueCB->queueHead = (UINT16)((queueCB->queueHead == 0) ?
                                          (queueCB->queueLen - 1) : (queueCB->queueHead - 1));
            queuePos = queueCB->queueHead;
            break;
        case OS_QUEUE_WRITE_TAIL:
            queuePos = queueCB->queueTail;
            queueCB->queueTail = (UINT16)(((queueCB->queueTail + 1) == queueCB->queueLen) ?
                                          0 : (queueCB->queueTail + 1));
            break;
        default:  /* read tail, reserved. */
            return OS_QUEUE_OPERATE_ERROR_INVALID_TYPE;
    }

    queueMsgHeadNode = &(queueCB->queueHandle[queuePos * sizeof(QueueMsgHead)]);
    queueNode = &(queueCB->queueHandle[(queueCB->queueLen) * sizeof(QueueMsgHead) + queuePos * (queueCB->queueSize)]);

    if (OS_QUEUE_IS_READ(operateType)) {
        msgDataSize = *((QueueMsgHead *)(UINTPTR)(queueMsgHeadNode));
        if (memcpy_s(bufferAddr, *bufferSize, queueNode, msgDataSize) != EOK) {
            return OS_QUEUE_OPERATE_ERROR_MEMCPYS_MSG2BUF;
        }

        *bufferSize = msgDataSize;
    } else {
        *((QueueMsgHead *)(UINTPTR)(queueMsgHeadNode)) = *bufferSize;
        if (memcpy_s(queueNode, queueCB->queueSize, bufferAddr, *bufferSize) != EOK) {
            return OS_QUEUE_OPERATE_ERROR_MEMCPYS_STRMSG;
        }
    }
    return LOS_OK;
}

STATIC VOID OsQueueBufferOperateErrProcess(UINT32 errorCode)
{
    if (errorCode != LOS_OK) {
        PRINT_DEBUG("msg opt fail %u\n", errorCode);
    }
}

#ifdef LOSCFG_BASE_CORE_SYS_RES_CHECK
STATIC INLINE UINT32 OsQueueStateVerify(const LosQueueCB *queueCB, UINT32 queueId)
{
    if ((queueCB->queueId != queueId) || (queueCB->queueState == LOS_UNUSED)) {
        return LOS_NOK;
    }

    return LOS_OK;
}

STATIC INLINE VOID OsQueueIdUpdate(LosQueueCB *queueCB)
{
    queueCB->queueId = SET_QUEUE_ID(GET_QUEUE_COUNT(queueCB->queueId) + 1, GET_QUEUE_INDEX(queueCB->queueId));
}
#else
STATIC INLINE UINT32 OsQueueStateVerify(const LosQueueCB *queueCB, UINT32 queueId)
{
    (VOID)queueId;

    if (queueCB->queueState == LOS_UNUSED) {
        return LOS_NOK;
    }

    return LOS_OK;
}

STATIC INLINE VOID OsQueueIdUpdate(LosQueueCB *queueCB)
{
    (VOID)queueCB;
}
#endif

STATIC UINT32 OsQueueOperateParamCheck(const LosQueueCB *queueCB, UINT32 queueId,
                                       UINT32 operateType, const UINT32 *bufferSize)
{
    UINT32 ret = OsQueueStateVerify(queueCB, queueId);
    if (ret != LOS_OK) {
        return LOS_ERRNO_QUEUE_NOT_CREATE;
    }

    if (OS_QUEUE_IS_READ(operateType) && (*bufferSize < (queueCB->queueSize))) {
        return LOS_ERRNO_QUEUE_READ_SIZE_TOO_SMALL;
    } else if (OS_QUEUE_IS_WRITE(operateType) && (*bufferSize > (queueCB->queueSize))) {
        return LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG;
    }
    return LOS_OK;
}

STATIC UINT32 OsQueueOperate(UINT32 queueId, UINT32 operateType, VOID *bufferAddr, UINT32 *bufferSize, UINT32 timeout)
{
    LosQueueCB *queueCB = (LosQueueCB *)GET_QUEUE_HANDLE(queueId);
    UINT32 readWrite = OS_QUEUE_READ_WRITE_GET(operateType);
    UINT32 errorCode = LOS_OK;
    UINT32 intSave, ret;
    LosTaskCB *runTask = NULL;

    LOS_TRACE(QUEUE_RW, queueId, queueCB->queueSize, *bufferSize, operateType,
        queueCB->readWriteableCnt[OS_QUEUE_READ], queueCB->readWriteableCnt[OS_QUEUE_WRITE], timeout);

    SCHEDULER_LOCK(intSave);
    ret = OsQueueOperateParamCheck(queueCB, queueId, operateType, bufferSize);
    if (ret != LOS_OK) {
        goto OUT_UNLOCK;
    }

    if (queueCB->readWriteableCnt[readWrite] == 0) {
        if (timeout == LOS_NO_WAIT) {
            ret = OS_QUEUE_IS_READ(operateType) ? LOS_ERRNO_QUEUE_ISEMPTY : LOS_ERRNO_QUEUE_ISFULL;
            goto OUT_UNLOCK;
        }

        if (!OsPreemptableInSched()) {
            ret = LOS_ERRNO_QUEUE_PEND_IN_LOCK;
            goto OUT_UNLOCK;
        }

        runTask = OsCurrTaskGet();
        OsSchedWait(runTask, &queueCB->readWriteList[readWrite], timeout);

        if (runTask->taskStatus & OS_TASK_STATUS_TIMEOUT) {
            runTask->taskStatus &= ~OS_TASK_STATUS_TIMEOUT;
            ret = LOS_ERRNO_QUEUE_TIMEOUT;
            goto OUT_UNLOCK;
        }
    } else {
        queueCB->readWriteableCnt[readWrite]--;
    }

    /* It will cause double lock issue that print after SCHEDULER_LOCK,
     * so handle the return value errorCode after SCHEDULER_UNLOCK */
    errorCode = OsQueueBufferOperate(queueCB, operateType, bufferAddr, bufferSize);

    if (!LOS_ListEmpty(&queueCB->readWriteList[!readWrite])) {
        LosTaskCB *resumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&queueCB->readWriteList[!readWrite]));
        OsSchedWake(resumedTask);
        SCHEDULER_UNLOCK(intSave);
        OsQueueBufferOperateErrProcess(errorCode);
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
        return LOS_OK;
    } else {
        queueCB->readWriteableCnt[!readWrite]++;
    }

OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    OsQueueBufferOperateErrProcess(errorCode);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueReadCopy(UINT32 queueId,
                                          VOID *bufferAddr,
                                          UINT32 *bufferSize,
                                          UINT32 timeout)
{
    UINT32 ret;
    UINT32 operateType;

    ret = OsQueueReadParameterCheck(queueId, bufferAddr, bufferSize, timeout);
    if (ret != LOS_OK) {
        return ret;
    }

    operateType = OS_QUEUE_OPERATE_TYPE(OS_QUEUE_READ, OS_QUEUE_HEAD);
    return OsQueueOperate(queueId, operateType, bufferAddr, bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWriteHeadCopy(UINT32 queueId,
                                               VOID *bufferAddr,
                                               UINT32 bufferSize,
                                               UINT32 timeout)
{
    UINT32 ret;
    UINT32 operateType;

    ret = OsQueueWriteParameterCheck(queueId, bufferAddr, &bufferSize, timeout);
    if (ret != LOS_OK) {
        return ret;
    }

    operateType = OS_QUEUE_OPERATE_TYPE(OS_QUEUE_WRITE, OS_QUEUE_HEAD);
    return OsQueueOperate(queueId, operateType, bufferAddr, &bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWriteCopy(UINT32 queueId,
                                           VOID *bufferAddr,
                                           UINT32 bufferSize,
                                           UINT32 timeout)
{
    UINT32 ret;
    UINT32 operateType;

    ret = OsQueueWriteParameterCheck(queueId, bufferAddr, &bufferSize, timeout);
    if (ret != LOS_OK) {
        return ret;
    }

    operateType = OS_QUEUE_OPERATE_TYPE(OS_QUEUE_WRITE, OS_QUEUE_TAIL);
    return OsQueueOperate(queueId, operateType, bufferAddr, &bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueRead(UINT32 queueId, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeout)
{
    return LOS_QueueReadCopy(queueId, bufferAddr, &bufferSize, timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWrite(UINT32 queueId, VOID *bufferAddr, UINT32 bufferSize, UINT32 timeout)
{
    (VOID)bufferSize;
    if (bufferAddr == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }
    return LOS_QueueWriteCopy(queueId, &bufferAddr, sizeof(CHAR *), timeout);
}

LITE_OS_SEC_TEXT UINT32 LOS_QueueWriteHead(UINT32 queueId,
                                           VOID *bufferAddr,
                                           UINT32 bufferSize,
                                           UINT32 timeout)
{
    if (bufferAddr == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }
    bufferSize = (UINT32)sizeof(CHAR *);
    return LOS_QueueWriteHeadCopy(queueId, &bufferAddr, bufferSize, timeout);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_QueueDelete(UINT32 queueId)
{
    LosQueueCB *queueCB = NULL;
    UINT32 intSave;
    UINT32 ret;

    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_NOT_FOUND;
    }

    queueCB = (LosQueueCB *)GET_QUEUE_HANDLE(queueId);

    LOS_TRACE(QUEUE_DELETE, queueId, queueCB->queueState, queueCB->readWriteableCnt[OS_QUEUE_READ]);

    SCHEDULER_LOCK(intSave);

    ret = OsQueueStateVerify(queueCB, queueId);
    if (ret != LOS_OK) {
        ret = LOS_ERRNO_QUEUE_NOT_CREATE;
        goto OUT_UNLOCK;
    }

    if ((!LOS_ListEmpty(&queueCB->readWriteList[OS_QUEUE_READ])) ||
        (!LOS_ListEmpty(&queueCB->readWriteList[OS_QUEUE_WRITE])) ||
        (!LOS_ListEmpty(&queueCB->memList))) {
        ret = LOS_ERRNO_QUEUE_IN_TSKUSE;
        goto OUT_UNLOCK;
    }

    if ((queueCB->readWriteableCnt[OS_QUEUE_WRITE] + queueCB->readWriteableCnt[OS_QUEUE_READ]) !=
        queueCB->queueLen) {
        ret = LOS_ERRNO_QUEUE_IN_TSKWRITE;
        goto OUT_UNLOCK;
    }

#ifdef LOSCFG_QUEUE_DYNAMIC_ALLOCATION
    if (queueCB->queueMemType == OS_QUEUE_ALLOC_DYNAMIC) {
        ret = LOS_MemFree(m_aucSysMem1, (VOID *)queueCB->queueHandle);
    }
#endif
    queueCB->queueHandle = NULL;

    OsQueueIdUpdate(queueCB);
    OsQueueNodeRecycle(queueCB);

    OsQueueDbgUpdateHook(queueCB->queueId, NULL);

OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_QueueInfoGet(UINT32 queueId, QUEUE_INFO_S *queueInfo)
{
    UINT32 intSave;
    UINT32 ret;
    LosQueueCB *queueCB = NULL;
    LosTaskCB *tskCB = NULL;

    if (queueInfo == NULL) {
        return LOS_ERRNO_QUEUE_PTR_NULL;
    }

    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }

    (VOID)memset((VOID *)queueInfo, 0, sizeof(QUEUE_INFO_S));
    queueCB = (LosQueueCB *)GET_QUEUE_HANDLE(queueId);

    SCHEDULER_LOCK(intSave);
    ret = OsQueueStateVerify(queueCB, queueId);
    if (ret != LOS_OK) {
        ret = LOS_ERRNO_QUEUE_NOT_CREATE;
        goto OUT_UNLOCK;
    }

    queueInfo->uwQueueID = queueId;
    queueInfo->usQueueLen = queueCB->queueLen;
    queueInfo->usQueueSize = queueCB->queueSize;
    queueInfo->usQueueHead = queueCB->queueHead;
    queueInfo->usQueueTail = queueCB->queueTail;
    queueInfo->usReadableCnt = queueCB->readWriteableCnt[OS_QUEUE_READ];
    queueInfo->usWritableCnt = queueCB->readWriteableCnt[OS_QUEUE_WRITE];

    LOS_DL_LIST_FOR_EACH_ENTRY(tskCB, &queueCB->readWriteList[OS_QUEUE_READ], LosTaskCB, pendList) {
        queueInfo->uwWaitReadTask |= (1ULL << tskCB->taskId);
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(tskCB, &queueCB->readWriteList[OS_QUEUE_WRITE], LosTaskCB, pendList) {
        queueInfo->uwWaitWriteTask |= (1ULL << tskCB->taskId);
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(tskCB, &queueCB->memList, LosTaskCB, pendList) {
        queueInfo->uwWaitMemTask |= (1ULL << tskCB->taskId);
    }

OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

#ifdef LOSCFG_COMPAT_CMSIS_VER_1
/*
 * Description : Mail allocate memory
 * Input       : queueId  --- QueueID
 *             : mailPool --- The memory poll that stores the mail
 *             : timeout  --- Expiry time. The value range is [0,LOS_WAIT_FOREVER]
 * Return      : pointer if success otherwise NULL
 */
LITE_OS_SEC_TEXT VOID *OsQueueMailAlloc(UINT32 queueId, VOID *mailPool, UINT32 timeout)
{
    VOID *mem = NULL;
    LosQueueCB *queueCB = NULL;
    LosTaskCB *runTask = NULL;
    UINT32 intSave;
    UINT32 ret;

    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return NULL;
    }

    if (mailPool == NULL) {
        return NULL;
    }

    queueCB = GET_QUEUE_HANDLE(queueId);

    OsQueueDbgTimeUpdateHook(queueId);

    if (timeout != LOS_NO_WAIT) {
        if (OS_INT_ACTIVE) {
            return NULL;
        }
    }

    SCHEDULER_LOCK(intSave);
    ret = OsQueueStateVerify(queueCB, queueId);
    if (ret != LOS_OK) {
        goto OUT_UNLOCK;
    }

    mem = LOS_MemboxAlloc(mailPool);
    if (mem == NULL) {
        if (timeout == LOS_NO_WAIT) {
            goto OUT_UNLOCK;
        }

        runTask = OsCurrTaskGet();
        OsSchedWait(runTask, &queueCB->memList, timeout);

        if (runTask->taskStatus & OS_TASK_STATUS_TIMEOUT) {
            runTask->taskStatus &= ~OS_TASK_STATUS_TIMEOUT;
            goto OUT_UNLOCK;
        } else {
            /*
             * When enters the current branch, means the current task already got an available membox,
             * so the runTsk->msg can not be NULL.
             */
            mem = runTask->msg;
            runTask->msg = NULL;
        }
    }

OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return mem;
}

/*
 * Description : Mail free memory
 * Input       : queueId  --- QueueID
 *             : mailPool --- The mail memory poll address
 *             : mailMem  --- The mail memory block address
 * Return      : LOS_OK on success or error code on failure
 */
LITE_OS_SEC_TEXT UINT32 OsQueueMailFree(UINT32 queueId, VOID *mailPool, VOID *mailMem)
{
    VOID *mem = NULL;
    LosQueueCB *queueCB = NULL;
    LosTaskCB *resumedTask = NULL;
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_MAIL_HANDLE_INVALID;
    }

    if (mailPool == NULL) {
        return LOS_ERRNO_QUEUE_MAIL_PTR_INVALID;
    }

    queueCB = GET_QUEUE_HANDLE(queueId);

    OsQueueDbgTimeUpdateHook(queueId);

    SCHEDULER_LOCK(intSave);

    if (LOS_MemboxFree(mailPool, mailMem)) {
        ret = LOS_ERRNO_QUEUE_MAIL_FREE_ERROR;
        goto OUT_UNLOCK;
    }

    ret = OsQueueStateVerify(queueCB, queueId);
    if (ret != LOS_OK) {
        ret = LOS_ERRNO_QUEUE_NOT_CREATE;
        goto OUT_UNLOCK;
    }

    OsQueueDbgTimeUpdateHook(queueId);

    if (!LOS_ListEmpty(&queueCB->memList)) {
        resumedTask = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&queueCB->memList));
        OsSchedWake(resumedTask);
        mem = LOS_MemboxAlloc(mailPool);
        /* At the state of LOS_IntLock, the allocation can not be failed after releasing successfully. */
        resumedTask->msg = mem;
        SCHEDULER_UNLOCK(intSave);
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
        return LOS_OK;
    }

OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
#endif /* LOSCFG_COMPAT_CMSIS */
