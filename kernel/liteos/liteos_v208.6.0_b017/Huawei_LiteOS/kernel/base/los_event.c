/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Event
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

#include "los_event_pri.h"
#include "los_task_pri.h"
#include "los_spinlock.h"
#include "los_mp_pri.h"
#include "los_percpu_pri.h"
#include "los_trace.h"

LITE_OS_SEC_TEXT_INIT UINT32 LOS_EventInit(PEVENT_CB_S eventCB)
{
    UINT32 intSave;

    LOS_TRACE(EVENT_CREATE, (UINTPTR)eventCB);

    if (eventCB == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    SCHEDULER_LOCK(intSave);
    eventCB->uwEventID = 0;
    LOS_ListInit(&eventCB->stEventList);
    SCHEDULER_UNLOCK(intSave);
    return LOS_OK;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsEventParamCheck(const VOID *ptr, UINT32 eventMask, UINT32 mode)
{
    if (ptr == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    if (eventMask == 0) {
        return LOS_ERRNO_EVENT_EVENTMASK_INVALID;
    }

    if (eventMask & LOS_ERRTYPE_ERROR) {
        return LOS_ERRNO_EVENT_SETBIT_INVALID;
    }

    if (((mode & LOS_WAITMODE_OR) && (mode & LOS_WAITMODE_AND)) ||
        (mode & ~(LOS_WAITMODE_OR | LOS_WAITMODE_AND | LOS_WAITMODE_CLR)) ||
        !(mode & (LOS_WAITMODE_OR | LOS_WAITMODE_AND))) {
        return LOS_ERRNO_EVENT_FLAGS_INVALID;
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsEventPoll(UINT32 *eventId, UINT32 eventMask, UINT32 mode)
{
    UINT32 ret = 0;

    LOS_ASSERT(ArchIntLocked());
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    if (mode & LOS_WAITMODE_OR) {
        if ((*eventId & eventMask) != 0) {
            ret = *eventId & eventMask;
        }
    } else {
        if ((eventMask != 0) && (eventMask == (*eventId & eventMask))) {
            ret = *eventId & eventMask;
        }
    }

    if (ret && (mode & LOS_WAITMODE_CLR)) {
        *eventId = *eventId & ~ret;
    }

    return ret;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsEventReadCheck(const PEVENT_CB_S eventCB, UINT32 eventMask, UINT32 mode)
{
    UINT32 ret;
    LosTaskCB *runTask = NULL;

    ret = OsEventParamCheck(eventCB, eventMask, mode);
    if (ret != LOS_OK) {
        return ret;
    }

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_EVENT_READ_IN_INTERRUPT;
    }

    runTask = OsCurrTaskGet();
    if (runTask->taskFlags & OS_TASK_FLAG_SYSTEM) {
        PRINT_DEBUG("Warning: DO NOT recommend to use LOS_EventRead or OsEventReadOnce in system tasks.\n");
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsEventRead(PEVENT_CB_S eventCB, UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    UINT32 ret;
    LosTaskCB *runTask = OsCurrTaskGet();

    ret = OsEventPoll(&eventCB->uwEventID, eventMask, mode);
    if (ret == 0) {
        if (timeout == 0) {
            return ret;
        }

        if (!OsPreemptableInSched()) {
            return LOS_ERRNO_EVENT_READ_IN_LOCK;
        }

        runTask->eventMask = eventMask;
        runTask->eventMode = mode;

        OsSchedWait(runTask, &eventCB->stEventList, timeout);

        if (runTask->taskStatus & OS_TASK_STATUS_TIMEOUT) {
            runTask->taskStatus &= ~OS_TASK_STATUS_TIMEOUT;
            return LOS_ERRNO_EVENT_READ_TIMEOUT;
        }

        ret = OsEventPoll(&eventCB->uwEventID, eventMask, mode);
    }
    return ret;
}

LITE_OS_SEC_TEXT STATIC UINT32 OsEventWrite(PEVENT_CB_S eventCB, UINT32 events, BOOL once)
{
    LosTaskCB *resumedTask = NULL;
    LosTaskCB *nextTask = NULL;
    UINT32 intSave;
    UINT8 exitFlag = 0;

    if (eventCB == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    if (events & LOS_ERRTYPE_ERROR) {
        return LOS_ERRNO_EVENT_SETBIT_INVALID;
    }

    LOS_TRACE(EVENT_WRITE, (UINTPTR)eventCB, eventCB->uwEventID, events);

    SCHEDULER_LOCK(intSave);

    eventCB->uwEventID |= events;
    if (!LOS_ListEmpty(&eventCB->stEventList)) {
        for (resumedTask = LOS_DL_LIST_ENTRY((&eventCB->stEventList)->pstNext, LosTaskCB, pendList);
             &resumedTask->pendList != &eventCB->stEventList;) {
            nextTask = LOS_DL_LIST_ENTRY(resumedTask->pendList.pstNext, LosTaskCB, pendList);
            if (((resumedTask->eventMode & LOS_WAITMODE_OR) && ((resumedTask->eventMask & events) != 0)) ||
                ((resumedTask->eventMode & LOS_WAITMODE_AND) &&
                 ((resumedTask->eventMask & eventCB->uwEventID) == resumedTask->eventMask))) {
                exitFlag = 1;
                OsSchedWake(resumedTask);
            }
            if (once == TRUE) {
                break;
            }
            resumedTask = nextTask;
        }
    }

    SCHEDULER_UNLOCK(intSave);

    if (exitFlag == 1) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }
    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 LOS_EventPoll(UINT32 *eventId, UINT32 eventMask, UINT32 mode)
{
    UINT32 ret;
    UINT32 intSave;

    ret = OsEventParamCheck((VOID *)eventId, eventMask, mode);
    if (ret != LOS_OK) {
        return ret;
    }

    SCHEDULER_LOCK(intSave);
    ret = OsEventPoll(eventId, eventMask, mode);
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_EventRead(PEVENT_CB_S eventCB, UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    UINT32 ret;
    UINT32 intSave;

    ret = OsEventReadCheck(eventCB, eventMask, mode);
    if (ret != LOS_OK) {
        return ret;
    }

    LOS_TRACE(EVENT_READ, (UINTPTR)eventCB, eventCB->uwEventID, eventMask, mode, timeout);

    SCHEDULER_LOCK(intSave);
    ret = OsEventRead(eventCB, eventMask, mode, timeout);
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_EventWrite(PEVENT_CB_S eventCB, UINT32 events)
{
    return OsEventWrite(eventCB, events, FALSE);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_EventDestroy(PEVENT_CB_S eventCB)
{
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    if (eventCB == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    SCHEDULER_LOCK(intSave);
    if (!LOS_ListEmpty(&eventCB->stEventList)) {
        ret = LOS_ERRNO_EVENT_SHOULD_NOT_DESTORY;
        goto OUT;
    }

    eventCB->uwEventID = 0;
OUT:
    SCHEDULER_UNLOCK(intSave);

    LOS_TRACE(EVENT_DELETE, (UINTPTR)eventCB, ret);
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_EventClear(PEVENT_CB_S eventCB, UINT32 events)
{
    UINT32 intSave;

    if (eventCB == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    LOS_TRACE(EVENT_CLEAR, (UINTPTR)eventCB, eventCB->uwEventID, events);

    SCHEDULER_LOCK(intSave);
    eventCB->uwEventID &= events;
    SCHEDULER_UNLOCK(intSave);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 OsEventWriteOnce(PEVENT_CB_S eventCB, UINT32 events)
{
    return OsEventWrite(eventCB, events, TRUE);
}

#ifdef LOSCFG_COMPAT_POSIX
LITE_OS_SEC_TEXT UINT32 OsEventReadWithCond(const EventCond *cond, PEVENT_CB_S eventCB,
                                            UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    UINT32 ret;
    UINT32 intSave;

    ret = OsEventReadCheck(eventCB, eventMask, mode);
    if (ret != LOS_OK) {
        return ret;
    }

    SCHEDULER_LOCK(intSave);

    if (*cond->realValue != cond->value) {
        eventCB->uwEventID &= cond->clearEvent;
        goto OUT;
    }

    ret = OsEventRead(eventCB, eventMask, mode, timeout);
OUT:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
#endif

LITE_OS_SEC_TEXT UINT32 EventCondRead(PEVENT_CB_S eventCB, UINT32 *timeout)
{
    UINT64 ticksStart = LOS_TickCountGet();
    UINT64 ticksCost;
    LosTaskCB *runTask = OsCurrTaskGet();
    UINT32 ret = LOS_OK;
    UINT32 intSave;

    if (eventCB == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_EVENT_READ_IN_INTERRUPT;
    }

    if (runTask->taskFlags & OS_TASK_FLAG_SYSTEM) {
        PRINT_DEBUG("Warning: DO NOT recommend to use LOS_EventCondRead in system tasks.\n");
    }

    if (*timeout == 0) {
        return LOS_ERRNO_EVENT_READ_TIMEOUT;
    }

    LOS_TRACE(EVENT_COND_READ, (UINTPTR)eventCB, eventCB->uwEventID);

    SCHEDULER_LOCK(intSave);
    if (!OsPreemptableInSched()) {
        ret = LOS_ERRNO_EVENT_READ_IN_LOCK;
        goto UNLOCK_RETURN;
    }

    OsSchedWait(runTask, &eventCB->stEventList, *timeout);

    if (runTask->taskStatus & OS_TASK_STATUS_TIMEOUT) {
        runTask->taskStatus &= ~OS_TASK_STATUS_TIMEOUT;
        ret = LOS_ERRNO_EVENT_READ_TIMEOUT;
        goto UNLOCK_RETURN;
    }

    SCHEDULER_UNLOCK(intSave);

    if (*timeout == LOS_WAIT_FOREVER) {
        return ret;
    }

    ticksCost = LOS_TickCountGet() - ticksStart;
    if (ticksCost >= *timeout) {
        *timeout = 0;
        return LOS_ERRNO_EVENT_READ_TIMEOUT;
    }

    *timeout = *timeout - (UINT32)ticksCost;
    return ret;

UNLOCK_RETURN:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT UINT32 LOS_EventCondWrite(PEVENT_CB_S eventCB)
{
    LosTaskCB *resumedTask = NULL;
    LosTaskCB *nextTask = NULL;
    UINT32 intSave;
    UINT8 exitFlag = 0;

    if (eventCB == NULL) {
        return LOS_ERRNO_EVENT_PTR_NULL;
    }

    LOS_TRACE(EVENT_COND_WRITE, (UINTPTR)eventCB, eventCB->uwEventID);

    SCHEDULER_LOCK(intSave);

    if (!LOS_ListEmpty(&eventCB->stEventList)) {
        for (resumedTask = LOS_DL_LIST_ENTRY((&eventCB->stEventList)->pstNext, LosTaskCB, pendList);
             &resumedTask->pendList != &eventCB->stEventList;) {
            nextTask = LOS_DL_LIST_ENTRY(resumedTask->pendList.pstNext, LosTaskCB, pendList);

            exitFlag = 1;
            OsSchedWake(resumedTask);
            resumedTask = nextTask;
        }
    }

    SCHEDULER_UNLOCK(intSave);

    if (exitFlag == 1) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }
    return LOS_OK;
}
