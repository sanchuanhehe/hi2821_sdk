/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: CMSIS Interface V1.0
 * Author: Huawei LiteOS Team
 * Create: 2023-05-20
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
#include "cmsis_os_pri.h"
#include "los_membox.h"
#include "los_hwi.h"

#ifdef LOSCFG_COMPAT_CMSIS_VER_1

#ifndef LOSCFG_COMPAT_CMSIS_VER_2
#define PRIORITY_WIN 4
#endif

#if !defined(LOSCFG_LIB_CONFIGURABLE) && (LOSCFG_BASE_CORE_TICK_PER_SECOND == OS_SYS_MS_PER_SECOND)
STATIC INLINE UINT32 Ms2Tick(UINT32 ms)
{
    return ms;
}
#else
STATIC INLINE UINT32 Ms2Tick(UINT32 ms)
{
    return LOS_MS2Tick(ms);
}
#endif

#ifndef LOSCFG_COMPAT_CMSIS_VER_2
int32_t osKernelRunning(void)
#else
int32_t osKernelIsRunning(void)
#endif
{
    return (int32_t)g_taskScheduled;
}

uint32_t osKernelSysTick(void)
{
    return (uint32_t)LOS_TickCountGet();
}

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
osThreadId osThreadCreate(const osThreadDef_t *thread_def, void *argument)
{
    osThreadAttr_t attr = {0};

    if (CMSIS_INT_ACTIVE || (thread_def == NULL) ||
        (thread_def->tpriority < osPriorityIdle) ||
        (thread_def->tpriority > osPriorityRealtime)) {
        return NULL;
    }

    attr.name = thread_def->name;
    attr.stack_size = thread_def->stacksize;

    if (thread_def->tpriority < osPriorityLow3) {
        attr.priority = osPriorityLow3;
    } else if (thread_def->tpriority > osPriorityHigh) {
        attr.priority = osPriorityHigh;
    } else {
        attr.priority = thread_def->tpriority;
    }

    return osThreadNew((osThreadFunc_t)thread_def->pthread, argument, &attr);
}
#else /* LOSCFG_COMPAT_CMSIS_VER_2 */
osThreadId osThreadCreate(const osThreadDef_t *thread_def, void *argument)
{
    UINT32 ret;
    UINT32 taskId = 0;
    TSK_INIT_PARAM_S taskInitParam = {0};

    if (CMSIS_INT_ACTIVE || (thread_def == NULL) ||
        (thread_def->tpriority < osPriorityIdle) ||
        (thread_def->tpriority > osPriorityRealtime)) {
        return NULL;
    }

    taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)(UINTPTR)(thread_def->pthread);
    taskInitParam.uwStackSize  = thread_def->stacksize;
    taskInitParam.pcName       = thread_def->name;
    taskInitParam.uwResved     = LOS_TASK_STATUS_DETACHED;
    taskInitParam.usTaskPrio   = (UINT16)(PRIORITY_WIN - (INT16)thread_def->tpriority);  /* 1~7 */
    LOS_TASK_PARAM_INIT_ARG(taskInitParam, argument);

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    ret = KAL_ThreadCreate(&taskId,  &taskInitParam, (VOID *)thread_def->stackmem);
#else
    ret = KAL_ThreadCreate(&taskId,  &taskInitParam, NULL);
#endif
    if (ret == LOS_OK) {
        return THREAD_LITEOS2CMSIS(taskId);
    } else {
        return NULL;
    }
}

osThreadId osThreadGetId(void)
{
    return THREAD_LITEOS2CMSIS(LOS_CurTaskIDGet());
}

osStatus osThreadTerminate(osThreadId thread_id)
{
    UINT32 ret;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = KAL_TaskDelete(THREAD_CMSIS2LITEOS(thread_id), NULL);
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_TSK_ID_INVALID) ||
               (ret == LOS_ERRNO_TSK_NOT_CREATED) ||
               ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus osThreadYield(void)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_TaskYield();
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority)
{
    UINT32 ret;
    UINT16 losPrio;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    if ((priority < osPriorityIdle) || (priority > osPriorityRealtime)) {
        return osErrorValue;
    }

    losPrio = (UINT16)(PRIORITY_WIN - (INT16)priority);
    ret = LOS_TaskPriSet(THREAD_CMSIS2LITEOS(thread_id), losPrio);
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_TSK_ID_INVALID) || (ret == LOS_ERRNO_TSK_NOT_CREATED) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osPriority osThreadGetPriority(osThreadId thread_id)
{
    UINT16 losPrio;
    INT16 priorityRet;

    if (CMSIS_INT_ACTIVE || (thread_id == NULL)) {
        return osPriorityError;
    }

    losPrio = LOS_TaskPriGet(THREAD_CMSIS2LITEOS(thread_id));
    priorityRet = (INT16)(PRIORITY_WIN - (INT16)losPrio);
    if ((priorityRet < (INT16)osPriorityIdle) || (priorityRet > (INT16)osPriorityRealtime)) {
        return osPriorityError;
    }

    return (osPriority)priorityRet;
}
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */

UINT32 osThreadGetPId(osThreadId thread_id)
{
    if (thread_id == NULL) {
        return (UINT32)-1;
    }
    return KAL_ThreadGetPid(THREAD_CMSIS2LITEOS(thread_id));
}

//  ==== Semaphore Management Functions ====
#ifdef LOSCFG_BASE_IPC_SEM
osSemaphoreId osBinarySemaphoreCreate(const osSemaphoreDef_t *semaphore_def, int32_t count)
{
    UINT32 ret;
    UINT32 semId = 0;
    (VOID)semaphore_def;

    if ((count > OS_SEM_BINARY_COUNT_MAX) || (count < 0) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    ret = LOS_BinarySemCreate((UINT16)count, &semId);
    if (ret == LOS_OK) {
        return SEM_LITEOS2CMSIS(semId);
    } else {
        return NULL;
    }
}

osSemaphoreId osSemaphoreCreate(const osSemaphoreDef_t *semaphore_def, int32_t count)
{
    UINT32 ret;
    UINT32 semId = 0;
    (VOID)semaphore_def;

    if ((count > LOS_SEM_COUNT_MAX) || (count < 0) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    ret = LOS_SemCreate((UINT16)count, &semId);
    if (ret == LOS_OK) {
        return SEM_LITEOS2CMSIS(semId);
    } else {
        return NULL;
    }
}

int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE || (semaphore_id == NULL)) {
        return -1;
    }

    ret = KAL_SemPendAndGetCount(SEM_CMSIS2LITEOS(semaphore_id), Ms2Tick(millisec));
    if ((ret & LOS_ERRTYPE_ERROR) == 0) {
        return (int32_t)ret;
    } else {
        return -1;
    }
}

#ifndef LOSCFG_COMPAT_CMSIS_VER_2
osStatus osSemaphoreRelease(osSemaphoreId semaphore_id)
{
    if (semaphore_id == NULL) {
        return osErrorParameter;
    }
    UINT32 ret = LOS_SemPost(SEM_CMSIS2LITEOS(semaphore_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_SEM_INVALID || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus osSemaphoreDelete(osSemaphoreId semaphore_id)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }
    if (semaphore_id == NULL) {
        return osErrorParameter;
    }
    ret = LOS_SemDelete(SEM_CMSIS2LITEOS(semaphore_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_SEM_INVALID || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */
#endif /* LOSCFG_BASE_IPC_SEM */

//  ==== Mutex Management ====
#ifdef LOSCFG_BASE_IPC_MUX
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
osMutexId osMutexCreate(const osMutexDef_t *mutex_def)
{
    (VOID)mutex_def;
    return osMutexNew(NULL);
}

osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec)
{
    return osMutexAcquire(mutex_id, Ms2Tick(millisec));
}
#else /* LOSCFG_COMPAT_CMSIS_VER_2 */
osMutexId osMutexCreate(const osMutexDef_t *mutex_def)
{
    UINT32 ret;
    UINT32 muxId = 0;
    (VOID)mutex_def;

    if (CMSIS_INT_ACTIVE) {
        return NULL;
    }
    ret = LOS_MuxCreate(&muxId);
    if (ret == LOS_OK) {
        return MUX_LITEOS2CMSIS(muxId);
    } else {
        return NULL;
    }
}

osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec)
{
    UINT32 ret;

    if (mutex_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_MuxPend(MUX_CMSIS2LITEOS(mutex_id), Ms2Tick(millisec));
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_MUX_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else if (ret == LOS_ERRNO_MUX_TIMEOUT) {
        return osErrorTimeoutResource;
    } else {
        return osErrorResource;
    }
}

osStatus osMutexRelease(osMutexId mutex_id)
{
    UINT32 ret;

    if (mutex_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_MuxPost(MUX_CMSIS2LITEOS(mutex_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_MUX_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus osMutexDelete(osMutexId mutex_id)
{
    UINT32 ret;

    if (mutex_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_MuxDelete(MUX_CMSIS2LITEOS(mutex_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_MUX_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */
#endif /* LOSCFG_BASE_IPC_MUX */

//  ==== Memory Pool Management Functions ====
osPoolId osPoolCreate(const osPoolDef_t *pool_def)
{
    if ((pool_def == NULL) || CMSIS_INT_ACTIVE || (pool_def->pool_sz == 0) || (pool_def->item_sz == 0)) {
        return NULL;
    }

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    /* 1: here pool_sz means total pool size */
    if (LOS_MemboxInit(pool_def->pool, pool_def->pool_sz, pool_def->item_sz) == LOS_OK) {
        return pool_def->pool;
    }
    return NULL;
#else
    /* 2: here pool_sz means block number, which is different with 1 */
    return (osPoolId)KAL_PoolCreate(pool_def->pool_sz, pool_def->item_sz);
#endif
}

void *osPoolAlloc(osPoolId pool_id)
{
    if (pool_id == NULL) {
        return NULL;
    }

    return LOS_MemboxAlloc(pool_id);
}

void *osPoolCAlloc(osPoolId pool_id)
{
    void *ptr = NULL;

    if (pool_id == NULL) {
        return NULL;
    }

    ptr = LOS_MemboxAlloc(pool_id);
    if (ptr != NULL) {
        LOS_MemboxClr(pool_id, ptr);
    }
    return ptr;
}

osStatus osPoolFree(osPoolId pool_id, void *block)
{
    UINT32 ret;

    if (pool_id == NULL) {
        return osErrorParameter;
    }

    ret = LOS_MemboxFree(pool_id, block);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorValue;
    }
}

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
osStatus osPoolDelete(osPoolId pool_id)
{
    (VOID)pool_id;
    return osOK;
}
#else
osStatus osPoolDelete(osPoolId pool_id)
{
    LOS_MEMBOX_INFO *memBox = (LOS_MEMBOX_INFO *)pool_id;
    UINT32 ret;

    if (memBox == NULL) {
        return osErrorParameter;
    }
    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }
    if (memBox->uwBlkCnt != 0) {
        return osErrorResource;
    }
    ret = KAL_PoolDelete(memBox);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorValue;
    }
}
#endif

#ifdef LOSCFG_BASE_IPC_QUEUE
//  ==== Message Queue Management Functions ====
osMessageQId osMessageCreate(const osMessageQDef_t *queue_def, osThreadId thread_id)
{
    (VOID)thread_id;
    UINT32 queueId = 0;
    UINT32 ret;

    if ((queue_def == NULL) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    ret = KAL_QueueCreate(&queueId, queue_def->pool, queue_def->queue_sz, sizeof(UINT32));
    if (ret == LOS_OK) {
        return QUEUE_LITEOS2CMSIS(queueId);
    } else {
        return NULL;
    }
}

osStatus osMessagePutHead(const osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
    UINT32 ret;

    if ((CMSIS_INT_ACTIVE && (millisec != 0)) || (info == 0) || (queue_id == NULL)) {
        return osErrorParameter;
    }
    ret = LOS_QueueWriteHeadCopy(QUEUE_CMSIS2LITEOS(queue_id), (VOID *)(UINTPTR)&info,
                                 sizeof(UINT32), Ms2Tick(millisec));
    return MappingQueueWriteRet(ret);
}

osStatus osMessagePut(const osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
    UINT32 ret;

    if ((CMSIS_INT_ACTIVE && (millisec != 0)) || (queue_id == NULL) || (info == 0)) {
        return osErrorParameter;
    }
    ret = LOS_QueueWriteCopy(QUEUE_CMSIS2LITEOS(queue_id), (VOID *)(UINTPTR)&info, sizeof(UINT32), Ms2Tick(millisec));
    return MappingQueueWriteRet(ret);
}

osEvent osMessageGet(osMessageQId queue_id, uint32_t millisec)
{
    osEvent event = {0};
    UINT32 ret;
    UINT32 bufferSize = (UINT32)sizeof(UINT32);

    if ((CMSIS_INT_ACTIVE && (millisec != 0)) || (queue_id == NULL)) {
        event.status = osErrorParameter;
        return event;
    }
    ret = LOS_QueueReadCopy(QUEUE_CMSIS2LITEOS(queue_id), &(event.value.p), &bufferSize, Ms2Tick(millisec));
    event.status = MappingQueueReadRet(ret);
    return event;
}

osStatus osMessageDelete(const osMessageQId queue_id)
{
    UINT32 ret;

    if (queue_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    ret = KAL_QueueDelete(QUEUE_CMSIS2LITEOS(queue_id), FALSE);
#else
    ret = KAL_QueueDelete(QUEUE_CMSIS2LITEOS(queue_id), TRUE);
#endif
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osErrorResource;
    }
}

//  ==== Mail Queue Management Functions ====
osMailQId osMailCreate(const osMailQDef_t *queue_def, osThreadId thread_id)
{
    UINT32 ret;
    UINT32 queueId = 0;
    struct osMailQ *mailQ = NULL;
    (VOID)thread_id;

    if ((queue_def == NULL) || (queue_def->pool == NULL) ||
        (queue_def->queue_sz == 0) || (queue_def->item_sz == 0) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    mailQ = (struct osMailQ *)(queue_def->pool);
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    ret = KAL_QueueCreate(&queueId, (UINT8 *)mailQ->pool + mailQ->m_sz, queue_def->queue_sz, sizeof(VOID*));
#else
    ret = KAL_QueueCreate(&queueId, NULL, queue_def->queue_sz, sizeof(VOID*));
#endif
    if (ret != LOS_OK) {
        return NULL;
    }

#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    mailQ->pool = KAL_MailPoolCreate(queueId, mailQ->pool, queue_def->queue_sz, queue_def->item_sz);
#else
    mailQ->pool = KAL_MailPoolCreate(queueId, NULL, queue_def->queue_sz, queue_def->item_sz);
#endif
    if (mailQ->pool == NULL) {
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
        (VOID)KAL_QueueDelete(queueId, FALSE);
#else
        (VOID)KAL_QueueDelete(queueId, TRUE);
#endif
        return NULL;
    }
    mailQ->queue = queueId;
    return (osMailQId)mailQ;
}

void *osMailAlloc(osMailQId queue_id, uint32_t millisec)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    if (mailQ == NULL) {
        return NULL;
    }
    return KAL_QueueMailAlloc(mailQ->queue, mailQ->pool, Ms2Tick(millisec));
}

void *osMailCAlloc(osMailQId queue_id, uint32_t millisec)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    if (mailQ == NULL) {
        return NULL;
    }
    return KAL_QueueMailCAlloc(mailQ->queue, mailQ->pool, Ms2Tick(millisec));
}

osStatus osMailFree(osMailQId queue_id, void *mail)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UINT32 ret;

    if (mailQ == NULL) {
        return osErrorParameter;
    }
    ret = KAL_QueueMailFree(mailQ->queue, mailQ->pool, mail);
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_QUEUE_MAIL_FREE_ERROR) {
        return osErrorValue;
    } else {
        return osErrorParameter;
    }
}

osStatus osMailPutHead(osMailQId queue_id, void *mail)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UINT32 ret;

    if (mailQ == NULL) {
        return osErrorParameter;
    }
    if (mail == NULL) {
        return osErrorValue;
    }
    ret = KAL_QueueMailPut(mailQ->queue, mailQ->pool, mail, OS_QUEUE_HEAD);
    return MappingQueueWriteRet(ret);
}

osStatus osMailPut(osMailQId queue_id, void *mail)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    UINT32 ret;

    if (mailQ == NULL) {
        return osErrorParameter;
    }
    if (mail == NULL) {
        return osErrorValue;
    }
    ret = KAL_QueueMailPut(mailQ->queue, mailQ->pool, mail, OS_QUEUE_TAIL);
    return MappingQueueWriteRet(ret);
}

osEvent osMailGet(osMailQId queue_id, uint32_t millisec)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    osEvent event = {0};
    osStatus status;
    UINT32 ret;

    if ((mailQ == NULL) || (CMSIS_INT_ACTIVE && (millisec != 0))) {
        event.status = osErrorParameter;
        return event;
    }

    ret = KAL_QueueMailGet(mailQ->queue, mailQ->pool, &(event.value.p), Ms2Tick(millisec));
    status = MappingQueueReadRet(ret);
    event.status = (status == (osStatus)osEventMessage) ? (osStatus)osEventMail : status;
    return event;
}

osStatus osMailClear(osMailQId queue_id)
{
    osEvent event = {0};
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;
    if (mailQ == NULL) {
        return osErrorParameter;
    }

    UINT32 ret = KAL_QueueMailClear(mailQ->queue, mailQ->pool);
    event.status = MappingQueueReadRet(ret);
    if (event.status == (osStatus)osEventTimeout) {
        return osOK;
    }
    return event.status;
}

osStatus osMailDelete(osMailQId queue_id)
{
    struct osMailQ *mailQ = (struct osMailQ *)queue_id;

    if (mailQ == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    return KAL_QueueMailDelete(mailQ->queue, mailQ->pool);
}
#endif /* LOSCFG_BASE_IPC_QUEUE */

//  ==== Signal Management ====
#ifdef LOSCFG_BASE_IPC_EVENT
// can only use 31 bits at most
#if (osFeature_Signals > 0x1E)
#error exceed max event bit count
#endif

#define SIGNAL_ERR         ((int32_t)0x80000000)
#define VALID_EVENT_MASK   ((UINT32)((1U << osFeature_Signals) - 1))
#define INVALID_EVENT_MASK ((UINT32)(~VALID_EVENT_MASK))

int32_t osSignalSet(osThreadId thread_id, int32_t signals)
{
    UINT32 ret;

    if (((UINT32)signals & INVALID_EVENT_MASK) || (thread_id == NULL)) {
        return SIGNAL_ERR;
    }

    ret = KAL_TaskEventSetAndGetEvent(THREAD_CMSIS2LITEOS(thread_id),
                                      (UINT32)signals, OS_EVENT_MODE_RETURN_BEFORE);
    if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return SIGNAL_ERR;
    }

    return (int32_t)ret;
}

int32_t osSignalClear(osThreadId thread_id, int32_t signals)
{
    UINT32 ret;
    if (((UINT32)signals & INVALID_EVENT_MASK) || CMSIS_INT_ACTIVE || (thread_id == NULL)) {
        return SIGNAL_ERR;
    }
    ret = KAL_TaskEventClearAndGetEvent(THREAD_CMSIS2LITEOS(thread_id), ~(UINT32)signals);
    if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return SIGNAL_ERR;
    }

    return (int32_t)ret;
}

osEvent osSignalWait(int32_t signals, uint32_t millisec)
{
    osEvent evt = {0};
    UINT32 events;
    UINT32 ret;
    UINT32 flags = 0;

    if (CMSIS_INT_ACTIVE) {
        evt.status = osErrorISR;
        return evt;
    }
    if ((UINT32)signals & INVALID_EVENT_MASK) {
        evt.status = osErrorValue;
        return evt;
    }

    if (signals != 0) {
        events = (UINT32)signals;
        flags |= LOS_WAITMODE_AND;
    } else {
        events = VALID_EVENT_MASK;
        flags |= LOS_WAITMODE_OR;
    }

    ret = KAL_TaskEventRead(events, flags | LOS_WAITMODE_CLR, Ms2Tick(millisec));
    if (ret & LOS_ERRTYPE_ERROR) {
        if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
            evt.status = osEventTimeout;
        } else {
            evt.status = osErrorResource;
        }
    } else {
        if (ret == 0) {
            evt.status = osOK;
        } else {
            evt.status = osEventSignal;
            evt.value.signals = (int32_t)ret;
        }
    }
    return evt;
}
#endif /* LOSCFG_BASE_IPC_EVENT */

//  ==== Timer Management Functions ====
#ifdef LOSCFG_BASE_CORE_SWTMR
osTimerId osTimerCreate(const osTimerDef_t *timer_def, os_timer_type type, void *argument)
{
    UINT32 ret;
    UINT16 swtmrId = 0;
    if ((timer_def == NULL) || (timer_def->ptimer == NULL) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    ret = LOS_SwtmrCreate(1, (UINT8)type, (SWTMR_PROC_FUNC)(INTPTR)(timer_def->ptimer), &swtmrId, (UINTPTR)argument);
    if (ret == LOS_OK) {
        return SWTMR_LITEOS2CMSIS(swtmrId);
    } else {
        return NULL;
    }
}

#ifndef LOSCFG_COMPAT_CMSIS_VER_2
osStatus osTimerStart(osTimerId timer_id, uint32_t millisec)
{
    UINT32 interval = Ms2Tick(millisec);
    UINT32 ret;

    if ((interval == 0) || (timer_id == NULL)) {
        return osErrorParameter;
    }
    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }
    ret = KAL_SwtmrStart(SWTMR_CMSIS2LITEOS(timer_id), interval, 0);
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_SWTMR_ID_INVALID) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus osTimerStop(osTimerId timer_id)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    if (timer_id == NULL) {
        return osErrorParameter;
    }

    ret = LOS_SwtmrStop(SWTMR_CMSIS2LITEOS(timer_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_SWTMR_ID_INVALID) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus osTimerDelete(osTimerId timer_id)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    if (timer_id == NULL) {
        return osErrorParameter;
    }

    ret = LOS_SwtmrDelete(SWTMR_CMSIS2LITEOS(timer_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_SWTMR_ID_INVALID) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */

osStatus osTimerRestart(osTimerId timer_id, uint32_t millisec, uint8_t strict)
{
    osStatus status;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    if (timer_id == NULL) {
        return osErrorParameter;
    }

    status = osTimerStart(timer_id, millisec);
    if (status != osOK) {
        return status;
    }

    return osOK;
}
#endif /* LOSCFG_BASE_CORE_SWTMR */

#ifndef LOSCFG_COMPAT_CMSIS_VER_2
osStatus osDelay(uint32_t millisec)
{
    UINT32 ret;

    if (millisec == 0) {
        return osErrorParameter;
    }

    ret = LOS_TaskDelay(Ms2Tick(millisec));
    if (ret == LOS_OK) {
        return osEventTimeout;
    } else if (ret == LOS_ERRNO_TSK_DELAY_IN_INT) {
        return osErrorISR;
    } else {
        return osErrorOS;
    }
}
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */

osEvent osWait(uint32_t millisec)
{
    osEvent evt = {0};
    UINT32 interval;
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        evt.status = osErrorISR;
        return evt;
    }

    if (millisec == 0) {
        evt.status = osOK;
        return evt;
    }

    /* osEventSignal, osEventMessage, osEventMail */
    interval = Ms2Tick(millisec);

    ret = LOS_TaskDelay(interval);
    if (ret == LOS_OK) {
        evt.status = osEventTimeout;
    } else {
        evt.status = osErrorResource;
    }
    return evt;
}
#endif /* LOSCFG_COMPAT_CMSIS_VER_1 */
