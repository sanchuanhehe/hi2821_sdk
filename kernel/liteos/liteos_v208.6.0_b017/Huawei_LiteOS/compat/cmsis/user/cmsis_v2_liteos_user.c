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
#include "los_task_pri.h"
#include "los_hwi.h"
#ifdef LOSCFG_COMPAT_CMSIS_VER_2

#define LOS_PRIORITY_WIN   10
#define OS_API_VERSION      20010016
#define OS_KERNEL_VERSION   20010016

#define KERNEL_ID   "Huawei LiteOS"
/* priority: 1~30 */
#define LOS_PRIORITY(cmsisPriority)   (LOS_TASK_PRIORITY_LOWEST - ((cmsisPriority) - LOS_PRIORITY_WIN))
#define ISVALID_LOS_PRIORITY(losPrio) (((losPrio) > LOS_TASK_PRIORITY_HIGHEST) && \
                                      ((losPrio) < LOS_TASK_PRIORITY_LOWEST))

#ifndef LITE_OS_SEC_RODATA_USRSPACE
#define LITE_OS_SEC_RODATA_USRSPACE
#endif

osStatus_t osKernelGetInfo(osVersion_t *version, char *id_buf, uint32_t id_size)
{
    errno_t ret;

    if ((version == NULL) || (id_buf == NULL) || (id_size == 0)) {
        return osError;
    }

    ret = memcpy_s(id_buf, id_size, KERNEL_ID, sizeof(KERNEL_ID));
    if (ret == EOK) {
        version->api = OS_API_VERSION;
        version->kernel = OS_KERNEL_VERSION;
        return osOK;
    } else {
        PRINT_ERR("[%s] memcpy_s failed, error type = %d\n", __func__, ret);
        return osError;
    }
}

osKernelState_t osKernelGetState(void)
{
    if (g_taskScheduled == 0) {
        if (g_kernelState == osKernelReady) {
            return osKernelReady;
        } else {
            return osKernelInactive;
        }
    } else if (KAL_GetPreemptableStatus()) {
        return osKernelLocked;
    } else {
        return osKernelRunning;
    }
}

uint32_t osKernelGetTickCount(void)
{
    return (uint32_t)LOS_TickCountGet();
}

uint64_t osKernelGetTick2ms(void)
{
    return LOS_TickCountGet() * OS_SYS_MS_PER_SECOND / KERNEL_TICK_PER_SECOND;
}

uint64_t osMs2Tick(uint64_t ms)
{
    return (ms * KERNEL_TICK_PER_SECOND + OS_SYS_MS_PER_SECOND - 1) / OS_SYS_MS_PER_SECOND;
}

uint32_t osKernelGetTickFreq(void)
{
    return (uint32_t)KERNEL_TICK_PER_SECOND;
}

uint32_t osKernelGetSysTimerCount(void)
{
    UINT32 countHigh = 0;
    UINT32 countLow = 0;

    LOS_GetCpuCycle((UINT32 *)&countHigh, (UINT32 *)&countLow);
    return countLow;
}

uint32_t osKernelGetSysTimerFreq(void)
{
    return (uint32_t)OS_SYS_CLOCK;
}

//  ==== Thread Management Functions ====
osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    UINT32 taskId = 0;
    UINT16 priority;
    TSK_INIT_PARAM_S taskInitParam = {0};

    if ((func == NULL) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    priority = (attr != NULL) ? (UINT16)LOS_PRIORITY((UINT32)attr->priority) : LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    if (!ISVALID_LOS_PRIORITY(priority)) {
        PRINT_ERR("osThreadNew Fail, NOT in adapt priority range: [osPriorityLow3 : osPriorityHigh].\n");
        return NULL;
    }

    if (attr != NULL) {
        taskInitParam.uwStackSize = (UINT32)attr->stack_size;
        taskInitParam.pcName = (CHAR *)attr->name;
#ifdef LOSCFG_TASK_JOINABLE
        taskInitParam.uwResved = ((attr->attr_bits & osThreadJoinable) == osThreadJoinable) ? \
                                 LOS_TASK_STATUS_JOINABLE : LOS_TASK_STATUS_DETACHED;
#else
        taskInitParam.uwResved = LOS_TASK_STATUS_DETACHED;
#endif
    } else {
        taskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
        taskInitParam.pcName = "undefined";
        taskInitParam.uwResved = LOS_TASK_STATUS_DETACHED;
    }

    taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)(UINTPTR)func;
    taskInitParam.usTaskPrio = priority;
    LOS_TASK_PARAM_INIT_ARG(taskInitParam, argument);
    UINT32 ret = KAL_ThreadCreate(&taskId,  &taskInitParam, NULL);
    if (ret != LOS_OK) {
        return NULL;
    }
    return THREAD_LITEOS2CMSIS(taskId);
}

#ifdef LOSCFG_TASK_JOINABLE
/* Wait for specified thread to terminate. */
osStatus_t osThreadJoin(osThreadId_t thread_id)
{
    UINT32 ret;
    osStatus_t status = osOK;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    ret = LOS_TaskJoin(THREAD_CMSIS2LITEOS(thread_id), NULL);
    if (ret == LOS_ERRNO_TSK_ID_INVALID) {
        status = osErrorParameter;
    } else if (ret == LOS_ERRNO_TSK_NOT_ALLOW_IN_INT) {
        status = osErrorISR;
    } else if (ret != LOS_OK) {
        status = osErrorResource;
    }
    return status;
}

/* Detach a thread (thread storage can be reclaimed when thread terminates). */
osStatus_t osThreadDetach(osThreadId_t thread_id)
{
    UINT32 ret;
    osStatus_t status = osOK;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    ret = LOS_TaskDetach(THREAD_CMSIS2LITEOS(thread_id));
    if (ret == LOS_ERRNO_TSK_ID_INVALID) {
        status = osErrorParameter;
    } else if (ret == LOS_ERRNO_TSK_NOT_ALLOW_IN_INT) {
        status = osErrorISR;
    } else if (ret != LOS_OK) {
        status = osErrorResource;
    }

    return status;
}
#endif

const char *osThreadGetName(osThreadId_t thread_id)
{
    if (CMSIS_INT_ACTIVE || (thread_id == NULL)) {
        return NULL;
    }

    return KAL_GetTaskName(THREAD_CMSIS2LITEOS(thread_id));
}

osThreadId_t osThreadGetId(void)
{
    return THREAD_LITEOS2CMSIS(KAL_GetCurTaskId());
}

osThreadState_t osThreadGetState(osThreadId_t thread_id)
{
    osThreadState_t state;
    UINT16 taskStatus = 0;
    if (CMSIS_INT_ACTIVE || (thread_id == NULL) ||
        (KAL_GetTaskState(THREAD_CMSIS2LITEOS(thread_id), &taskStatus) != LOS_OK)) {
        return osThreadError;
    }

    if (taskStatus & OS_TASK_STATUS_RUNNING) {
        state = osThreadRunning;
    } else if (taskStatus & OS_TASK_STATUS_READY) {
        state = osThreadReady;
    } else if ((taskStatus & OS_CHECK_TASK_BLOCK) != 0) {
        state = osThreadBlocked;
    } else if (taskStatus & OS_TASK_STATUS_UNUSED) {
        state = osThreadInactive;
    } else {
        state = osThreadError;
    }

    return state;
}

osStatus_t osThreadSetPriority(osThreadId_t thread_id, osPriority_t priority)
{
    UINT32 ret;
    UINT16 losPrio;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    if ((priority < osPriorityLow3) || (priority > osPriorityHigh)) {
        PRINT_ERR("[%s] Fail, NOT in adapt priority range: [osPriorityLow3 : osPriorityHigh].\n", __func__);
        return osErrorParameter;
    }

    losPrio = (UINT16)(LOS_TASK_PRIORITY_LOWEST - ((UINT32)priority - LOS_PRIORITY_WIN));
    ret = LOS_TaskPriSet(THREAD_CMSIS2LITEOS(thread_id), losPrio);
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_TSK_ID_INVALID) || (ret == LOS_ERRNO_TSK_PRIOR_ERROR) ||
               ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osPriority_t osThreadGetPriority(osThreadId_t thread_id)
{
    UINT16 losPrio;
    UINT16 priorityRet;

    if (CMSIS_INT_ACTIVE || (thread_id == NULL)) {
        return osPriorityError;
    }

    losPrio = LOS_TaskPriGet(THREAD_CMSIS2LITEOS(thread_id));
    if (losPrio > LOS_TASK_PRIORITY_LOWEST) {
        return osPriorityError;
    }

    priorityRet = (UINT16)((LOS_TASK_PRIORITY_LOWEST - losPrio) + LOS_PRIORITY_WIN);
    return (osPriority_t)priorityRet;
}

osStatus_t osThreadYield(void)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_TaskYield();
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osError;
    }
}

osStatus_t osThreadSuspend(osThreadId_t thread_id)
{
    UINT32 ret;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_TaskSuspend(THREAD_CMSIS2LITEOS(thread_id));
    if ((ret == LOS_OK) || (ret == LOS_ERRNO_TSK_ALREADY_SUSPENDED)) {
        return osOK;
    } else if (ret == LOS_ERRNO_TSK_ID_INVALID || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus_t osThreadResume(osThreadId_t thread_id)
{
    UINT32 ret;

    if (thread_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_TaskResume(THREAD_CMSIS2LITEOS(thread_id));
    if ((ret == LOS_OK) || (ret == LOS_ERRNO_TSK_NOT_SUSPENDED)) {
        return osOK;
    } else if ((ret == LOS_ERRNO_TSK_ID_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus_t osThreadTerminate(osThreadId_t thread_id)
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
    } else if (ret == LOS_ERRNO_TSK_ID_INVALID || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

uint32_t osThreadGetStackSize(osThreadId_t thread_id)
{
    if (CMSIS_INT_ACTIVE || (thread_id == NULL)) {
        return 0;
    }

    return KAL_GetTaskStackSize(THREAD_CMSIS2LITEOS(thread_id));
}

uint32_t osThreadGetStackSpace(osThreadId_t thread_id)
{
    if (CMSIS_INT_ACTIVE || (thread_id == NULL)) {
        return 0;
    }

    return KAL_GetTaskStackSpace(THREAD_CMSIS2LITEOS(thread_id));
}

uint32_t osThreadGetCount(void)
{
    if (CMSIS_INT_ACTIVE) {
        return 0;
    }
    return KAL_GetTaskCount();
}

//  ==== Thread Flags Functions ====
#ifdef LOSCFG_BASE_IPC_EVENT
STATIC VOID EventModeConfig(UINT32 options, UINT32 *mode)
{
    if ((options & osFlagsWaitAll) == osFlagsWaitAll) {
        *mode |= LOS_WAITMODE_AND;
    } else {
        *mode |= LOS_WAITMODE_OR;
    }

    if ((options & osFlagsNoClear) == osFlagsNoClear) {
        *mode &= ~LOS_WAITMODE_CLR;
    } else {
        *mode |= LOS_WAITMODE_CLR;
    }
}

uint32_t osThreadFlagsSet(osThreadId_t thread_id, uint32_t flags)
{
    UINT32 ret;
    if (thread_id == NULL) {
        return (uint32_t)osFlagsErrorParameter;
    }

    ret = KAL_TaskEventSetAndGetEvent(THREAD_CMSIS2LITEOS(thread_id), flags,
                                      OS_EVENT_MODE_RETURN_AFTER);
    if ((ret == LOS_ERRNO_EVENT_SETBIT_INVALID) || (ret == LOS_ERRNO_TSK_ID_INVALID) ||
        ERROR_IS_INVALID_SID(ret)) {
        return (uint32_t)osFlagsErrorParameter;
    } else if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return (uint32_t)osFlagsErrorResource;
    }
    return ret;
}

uint32_t osThreadFlagsClear(uint32_t flags)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return (uint32_t)osFlagsErrorUnknown;
    }

    ret = KAL_CurTaskEventClearAndGetEvent(~flags);
    if ((ret & LOS_ERRTYPE_ERROR) != 0) {
        return (uint32_t)osFlagsErrorResource;
    }

    return ret;
}

uint32_t osThreadFlagsGet(void)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return (uint32_t)osFlagsErrorUnknown;
    }

    ret = KAL_CurTaskEventClearAndGetEvent(OS_NULL_INT);
    if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return (uint32_t)osFlagsErrorResource;
    }
    return ret;
}

uint32_t osThreadFlagsWait(uint32_t flags, uint32_t options, uint32_t timeout)
{
    UINT32 mode = 0;
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return (uint32_t)osFlagsErrorUnknown;
    }

    if (options > (osFlagsWaitAny | osFlagsWaitAll | osFlagsNoClear)) {
        return (uint32_t)osFlagsErrorParameter;
    }

    EventModeConfig(options, &mode);

    ret = KAL_TaskEventRead(flags, mode, (UINT32)timeout);
    if ((ret & LOS_ERRTYPE_ERROR) == 0) {
        return (uint32_t)ret;
    }

    return MappingEventRet(ret);
}

//  ==== Event Flags Management Functions ====
osEventFlagsId_t osEventFlagsNew(const osEventFlagsAttr_t *attr)
{
    (VOID)attr;

    UINT32 ret;
    UINTPTR eventId = 0;

    if (CMSIS_INT_ACTIVE) {
        return NULL;
    }

    ret = KAL_EventCreate(&eventId);
    if (ret != LOS_OK) {
        return NULL;
    }
    return EVENT_LITEOS2CMSIS(eventId);
}

uint32_t osEventFlagsSet(osEventFlagsId_t ef_id, uint32_t flags)
{
    if (ef_id == NULL) {
        return (UINT32)osFlagsErrorParameter;
    }

    UINT32 ret = KAL_EventSetAndGetEvent(EVENT_CMSIS2LITEOS(ef_id), flags, OS_EVENT_MODE_RETURN_AFTER);
    if ((ret == LOS_ERRNO_EVENT_PTR_NULL) || (ret == LOS_ERRNO_EVENT_SETBIT_INVALID) ||
        ERROR_IS_INVALID_SID(ret)) {
        return (uint32_t)osFlagsErrorParameter;
    } else if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return (uint32_t)osFlagsErrorResource;
    }

    return ret;
}

uint32_t osEventFlagsClear(osEventFlagsId_t ef_id, uint32_t flags)
{
    if (ef_id == NULL) {
        return (UINT32)osFlagsErrorParameter;
    }

    UINT32 ret = KAL_EventClearAndGetEvent(EVENT_CMSIS2LITEOS(ef_id), ~flags);
    if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return (uint32_t)osFlagsErrorResource;
    }

    return ret;
}

uint32_t osEventFlagsGet(osEventFlagsId_t ef_id)
{
    if (ef_id == NULL) {
        return (UINT32)osFlagsErrorParameter;
    }
    UINT32 ret = KAL_EventClearAndGetEvent(EVENT_CMSIS2LITEOS(ef_id), OS_NULL_INT);
    if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return 0;
    }

    return ret;
}

uint32_t osEventFlagsWait(osEventFlagsId_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout)
{
    UINT32 ret;
    UINT32 mode = 0;

    if (CMSIS_INT_ACTIVE && (timeout != 0)) {
        return (uint32_t)osFlagsErrorParameter;
    }
    if (options > (osFlagsWaitAny | osFlagsWaitAll | osFlagsNoClear)) {
        return (uint32_t)osFlagsErrorParameter;
    }

    EventModeConfig(options, &mode);

    ret = KAL_EventRead(EVENT_CMSIS2LITEOS(ef_id), flags, mode, (UINT32)timeout);
    if ((ret & LOS_ERRTYPE_ERROR) == 0) {
        return (uint32_t)ret;
    }

    return MappingEventRet(ret);
}

osStatus_t osEventFlagsDelete(osEventFlagsId_t ef_id)
{
    UINT32 ret;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }
    if (ef_id == NULL) {
        return osErrorParameter;
    }

    ret = KAL_EventDelete(EVENT_CMSIS2LITEOS(ef_id));
    if ((ret & LOS_ERRTYPE_ERROR) > 0) {
        return osErrorResource;
    }

    return osOK;
}

const char *osEventFlagsGetName(osEventFlagsId_t ef_id)
{
    (VOID)ef_id;
    if (CMSIS_INT_ACTIVE) {
        return NULL;
    }
    return NULL;
}

#endif /* LOSCFG_BASE_IPC_EVENT */

//  ==== Generic Wait Functions ====
osStatus_t osDelay(uint32_t ticks)
{
    UINT32 ret;

    if (ticks == 0) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_TaskDelay(ticks);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osError;
    }
}

osStatus_t osDelayUntil(uint32_t ticks)
{
    UINT32 ret;
    UINT32 ticksGap;
    UINT32 tickCount;

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    tickCount = (UINT32)LOS_TickCountGet();
    ticksGap = ticks - tickCount;
	/* the maximum delay is limited to (2^31)-1 ticks */
    if ((ticksGap >> 31) > 0) {
        return osError;
    }

    ret = LOS_TaskDelay(ticksGap);
    if (ret == LOS_OK) {
        return osOK;
    } else {
        return osError;
    }
}

//  ==== Timer Management Functions ====
#ifdef LOSCFG_BASE_CORE_SWTMR
osTimerId_t osTimerNew(osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr)
{
    (VOID)attr;
    UINT32 ret;
    UINT16 swtmrId = 0;
    UINT8 mode;

    if ((func == NULL) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    if (type == osTimerOnce) {
        mode = LOS_SWTMR_MODE_NO_SELFDELETE;
    } else if (type == osTimerPeriodic) {
        mode = LOS_SWTMR_MODE_PERIOD;
    } else {
        return NULL;
    }

    ret = LOS_SwtmrCreate(1, mode, (SWTMR_PROC_FUNC)(UINTPTR)func, &swtmrId, (UINTPTR)argument);
    if (ret == LOS_OK) {
        return SWTMR_LITEOS2CMSIS(swtmrId);
    } else {
        return NULL;
    }
}

const char *osTimerGetName(osTimerId_t timer_id)
{
    (VOID)timer_id;
    return NULL;
}

osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks)
{
    UINT32 ret;

    if ((ticks == 0) || (timer_id == NULL)) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = KAL_SwtmrStart(SWTMR_CMSIS2LITEOS(timer_id), ticks, 0);
    if (ret == LOS_OK) {
        return osOK;
    } else if (ret == LOS_ERRNO_SWTMR_ID_INVALID) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

osStatus_t osTimerStop(osTimerId_t timer_id)
{
    UINT32 ret;

    if (timer_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
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

uint32_t osTimerIsRunning(osTimerId_t timer_id)
{
    if ((timer_id == NULL) || CMSIS_INT_ACTIVE) {
        return 0;
    }

    return (uint32_t)KAL_SwtmrIsRunning(SWTMR_CMSIS2LITEOS(timer_id));
}

osStatus_t osTimerDelete(osTimerId_t timer_id)
{
    UINT32 ret;
    if (timer_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
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
#endif /* LOSCFG_BASE_CORE_SWTMR */

//  ==== Mutex Management Functions ====
#ifdef LOSCFG_BASE_IPC_MUX
osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    (VOID)attr;
    UINT32 ret;
    UINT32 muxId = 0;

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

const char *osMutexGetName(osMutexId_t mutex_id)
{
    (VOID)mutex_id;
    return NULL;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
    UINT32 ret;

    if (mutex_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = LOS_MuxPend(MUX_CMSIS2LITEOS(mutex_id), timeout);
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_MUX_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else if (ret == LOS_ERRNO_MUX_TIMEOUT) {
        return osErrorTimeout;
    } else {
        return osErrorResource;
    }
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
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

osThreadId_t osMutexGetOwner(osMutexId_t mutex_id)
{
    if (mutex_id == NULL) {
        return NULL;
    }
    return EVENT_LITEOS2CMSIS(KAL_MutexGetOwner(MUX_CMSIS2LITEOS(mutex_id)));
}

osStatus_t osMutexDelete(osMutexId_t mutex_id)
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
#endif /* LOSCFG_BASE_IPC_MUX */

//  ==== Semaphore Management Functions ====
#ifdef LOSCFG_BASE_IPC_SEM
osSemaphoreId_t osSemaphoreNew(uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr)
{
    (VOID)attr;
    UINT32 ret;
    UINT32 semId = 0;

    if ((initial_count > max_count) || (max_count > LOS_SEM_COUNT_MAX) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    if (max_count == 1) {
        ret = LOS_BinarySemCreate((UINT16)initial_count, &semId);
    } else {
        ret = LOS_SemCreate((UINT16)initial_count, &semId);
    }
    if (ret == LOS_OK) {
        return SEM_LITEOS2CMSIS(semId);
    } else {
        return NULL;
    }
}

const char *osSemaphoreGetName(osSemaphoreId_t semaphore_id)
{
    (VOID)semaphore_id;
    return NULL;
}

osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout)
{
    UINT32 ret;

    if ((CMSIS_INT_ACTIVE && (timeout != 0)) || (semaphore_id == NULL)) {
        return osErrorParameter;
    }

    ret = LOS_SemPend(SEM_CMSIS2LITEOS(semaphore_id), timeout);
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_SEM_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else if (ret == LOS_ERRNO_SEM_TIMEOUT) {
        return osErrorTimeout;
    } else {
        return osErrorResource;
    }
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id)
{
    UINT32 ret;
    if (semaphore_id == NULL) {
        return osErrorParameter;
    }
    ret = LOS_SemPost(SEM_CMSIS2LITEOS(semaphore_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_SEM_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}

uint32_t osSemaphoreGetCount(osSemaphoreId_t semaphore_id)
{
    if (semaphore_id == NULL) {
        return 0;
    }
    return KAL_SemGetCount(SEM_CMSIS2LITEOS(semaphore_id));
}

osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id)
{
    UINT32 ret;

    if (semaphore_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }
    ret = LOS_SemDelete(SEM_CMSIS2LITEOS(semaphore_id));
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_SEM_INVALID) || ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}
#endif /* LOSCFG_BASE_IPC_SEM */

//  ==== Message Queue Management Functions ====
#ifdef LOSCFG_BASE_IPC_QUEUE
typedef enum {
    ATTR_CAPACITY = 0,
    ATTR_MSGSIZE = 1,
    ATTR_COUNT = 2,
    ATTR_SPACE = 3
} QueueAttribute;

osMessageQueueId_t osMessageQueueNew(uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr)
{
    (VOID)attr;
    UINT32 ret;
    UINT32 queueId = 0;

    if ((msg_count > OS_NULL_SHORT) || (msg_size > OS_NULL_SHORT) || CMSIS_INT_ACTIVE) {
        return NULL;
    }

    ret = KAL_QueueCreate(&queueId, NULL, msg_count, msg_size);
    if (ret == LOS_OK) {
        return QUEUE_LITEOS2CMSIS(queueId);
    } else {
        return NULL;
    }
}

const char *osMessageQueueGetName(osMessageQueueId_t mq_id)
{
    (VOID)mq_id;
    return NULL;
}

STATIC osStatus_t osMessageQueueOp(osMessageQueueId_t mq_id, VOID *msg_ptr, UINT32 timeout, UINT32 rw)
{
    UINT32 ret;

    if ((msg_ptr == NULL) || (mq_id == NULL) || (CMSIS_INT_ACTIVE && (timeout != 0))) {
        return osErrorParameter;
    }

    if (rw == OS_QUEUE_WRITE) {
        ret = KAL_QueueWriteCopy(QUEUE_CMSIS2LITEOS(mq_id), msg_ptr, timeout);
    } else {
        ret = KAL_QueueReadCopy(QUEUE_CMSIS2LITEOS(mq_id), msg_ptr, timeout);
    }

    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_QUEUE_INVALID) || (ret == LOS_ERRNO_QUEUE_NOT_CREATE) ||
                ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else if (ret == LOS_ERRNO_QUEUE_TIMEOUT) {
        return osErrorTimeout;
    } else {
        return osErrorResource;
    }
}

osStatus_t osMessageQueuePut(osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout)
{
    (VOID)msg_prio;
    return osMessageQueueOp(mq_id, (VOID *)msg_ptr, (UINT32)timeout, OS_QUEUE_WRITE);
}

osStatus_t osMessageQueueGet(osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout)
{
    (VOID)msg_prio;
    return osMessageQueueOp(mq_id, (VOID *)msg_ptr, (UINT32)timeout, OS_QUEUE_READ);
}

STATIC UINT16 osMessageQueueGetAttr(osMessageQueueId_t mq_id, QueueAttribute attr)
{
    QUEUE_INFO_S queueInfo = {0};
    UINT16 attrVal = 0;

    if (mq_id == NULL) {
        return 0;
    }

    if (LOS_QueueInfoGet(QUEUE_CMSIS2LITEOS(mq_id), &queueInfo) != LOS_OK) {
        return 0;
    }

    switch (attr) {
        case ATTR_CAPACITY:
            attrVal = GET_QUEUEINFO_LEN(queueInfo);
            break;
        case ATTR_MSGSIZE:
            attrVal = GET_QUEUEINFO_MSGSIZE(queueInfo);
            break;
        case ATTR_COUNT:
            attrVal = GET_QUEUEINFO_COUNT(queueInfo);
            break;
        case ATTR_SPACE:
            attrVal = GET_QUEUEINFO_SPACE(queueInfo);
            break;
        default:
            break;
    }

    return attrVal;
}

uint32_t osMessageQueueGetCapacity(osMessageQueueId_t mq_id)
{
    return (uint32_t)osMessageQueueGetAttr(mq_id, ATTR_CAPACITY);
}

uint32_t osMessageQueueGetMsgSize(osMessageQueueId_t mq_id)
{
    return (uint32_t)osMessageQueueGetAttr(mq_id, ATTR_MSGSIZE);
}

uint32_t osMessageQueueGetCount(osMessageQueueId_t mq_id)
{
    return (uint32_t)osMessageQueueGetAttr(mq_id, ATTR_COUNT);
}

uint32_t osMessageQueueGetSpace(osMessageQueueId_t mq_id)
{
    return (uint32_t)osMessageQueueGetAttr(mq_id, ATTR_SPACE);
}

osStatus_t osMessageQueueDelete(osMessageQueueId_t mq_id)
{
    UINT32 ret;

    if (mq_id == NULL) {
        return osErrorParameter;
    }

    if (CMSIS_INT_ACTIVE) {
        return osErrorISR;
    }

    ret = KAL_QueueDelete(QUEUE_CMSIS2LITEOS(mq_id), TRUE);
    if (ret == LOS_OK) {
        return osOK;
    } else if ((ret == LOS_ERRNO_QUEUE_INVALID) || (ret == LOS_ERRNO_QUEUE_NOT_CREATE) ||
               ERROR_IS_INVALID_SID(ret)) {
        return osErrorParameter;
    } else {
        return osErrorResource;
    }
}
#endif /* LOSCFG_BASE_IPC_QUEUE */
#endif /* LOSCFG_COMPAT_CMSIS_VER_2 */
