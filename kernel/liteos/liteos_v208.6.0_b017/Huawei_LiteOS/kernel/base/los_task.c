/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: LiteOS Task Module Implementation
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

#include "los_task_pri.h"
#include "los_priqueue_pri.h"
#include "los_sem_pri.h"
#include "los_mux_debug_pri.h"
#include "los_exc.h"
#include "los_memstat_pri.h"
#include "los_mp_pri.h"
#include "los_spinlock.h"
#include "los_percpu_pri.h"
#include "los_trace.h"
#include "los_init.h"

#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif
#ifdef LOSCFG_BASE_CORE_SWTMR
#include "los_swtmr_pri.h"
#endif
#ifdef LOSCFG_EXC_INTERACTION
#include "los_exc_pri.h"
#endif
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
#include "los_perf_pri.h"
#endif

#ifndef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
#ifndef LOSCFG_TASK_STACK_STATIC_ALLOCATION
#error "no choice for task stack alloc mode"
#endif
#endif

LITE_OS_SEC_BSS LOS_DL_LIST                     g_losFreeTask;
LITE_OS_SEC_BSS LOS_DL_LIST                     g_taskRecycleList;
LITE_OS_SEC_BSS UINT32                          g_taskMaxNum;
LITE_OS_SEC_BSS UINT32                          g_taskScheduled; /* one bit for each cores */
LITE_OS_SEC_BSS LosTaskCB                      *g_osTaskCBArray;
LITE_OS_SEC_BSS UINT8                          *g_osIdleTaskStack[LOSCFG_KERNEL_CORE_NUM];

/* spinlock for task module, only available on SMP mode */
LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_taskSpin);

#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
TSKSWITCHHOOK g_pfnUsrTskSwitchHook = NULL;
#endif

#ifdef LOSCFG_KERNEL_LOWPOWER
STATIC LOWPOWERIDLEHOOK g_lowPowerHook = NULL;
#endif
IDLEHANDLERHOOK g_idleHandlerHook = NULL;

STATIC VOID OsConsoleIDSetHook(UINT32 param1,
                               UINT32 param2) __attribute__((weakref("OsSetConsoleID")));
#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
STATIC VOID OsExcStackCheckHook(VOID) __attribute__((weakref("OsExcStackCheck")));
#endif

STATIC VOID OsTaskStackFree(LosTaskCB *taskCB)
{
#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    if (taskCB->usrStack == 0) {
        VOID *pool = (VOID *)OS_TASK_STACK_POOL;
        UINTPTR taskStack = taskCB->topOfStack;
#ifdef LOSCFG_EXC_INTERACTION
        if (taskStack < (UINTPTR)m_aucSysMem1) {
            pool = (VOID *)m_aucSysMem0;
        }
#endif
        LOS_SpinUnlockNoSched(&g_taskSpin);
        (VOID)LOS_MemFree(pool, (VOID *)taskStack);
        LOS_SpinLock(&g_taskSpin);
    }
#endif
    taskCB->topOfStack = 0;
    LOS_ListAdd(&g_losFreeTask, &taskCB->pendList);
}

#ifdef LOSCFG_TASK_JOINABLE
STATIC VOID OsTaskJoinPost(LosTaskCB *taskCB)
{
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    if (OS_TASK_IS_ALREADY_JOIN(taskCB)) {
        taskCB->joinner->taskStatus &= ~OS_TASK_STATUS_PEND;
        taskCB->joinner->taskStatus |= OS_TASK_STATUS_DELAY;
        OsTaskAdd2TimerList(taskCB->joinner, 1); /* Wake up by 1 tick */
    }
}
#endif

VOID LOS_TaskResRecycle(VOID)
{
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;

    SCHEDULER_LOCK(intSave);

    while (!LOS_ListEmpty(&g_taskRecycleList)) {
        taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&g_taskRecycleList));
        if (OsTaskIsInoperable(taskCB)) {
            /*
            * MQ: The task stack cannot be released because the task is being scheduled, as a
            * result, an infinite loop occurs and the global lock affects the running of
            * other tasks. Therefore, the global lock must be unlocked and then locked
            * before a task is executed in a loop so that other tasks can obtain the lock
            * and run normally.
            */
            LOS_SpinUnlockNoSched(&g_taskSpin);
            LOS_SpinLock(&g_taskSpin);
            continue;
        }

        LOS_ListDelete(LOS_DL_LIST_FIRST(&g_taskRecycleList));
        OsTaskStackFree(taskCB);
    }

    SCHEDULER_UNLOCK(intSave);
}

#if defined(LOSCFG_EXC_INTERACTION) || defined(LOSCFG_DEBUG_SCHED_STATISTICS)
BOOL IsIdleTask(UINT32 taskId)
{
    UINT32 i;

    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        if (taskId == g_percpu[i].idleTaskId) {
            return TRUE;
        }
    }

    return FALSE;
}
#endif

LITE_OS_SEC_TEXT WEAK VOID OsIdleTask(VOID)
{
#ifdef LOSCFG_KERNEL_LOWPOWER
    LOWPOWERIDLEHOOK lowPowerHook;
#endif

    while (1) {
#ifdef LOSCFG_KERNEL_SMP_GC_IDLE
        OsMpCollectTasks();
#endif
        LOS_TaskResRecycle();
        OsIdleHandler();

#ifdef LOSCFG_KERNEL_LOWPOWER
        lowPowerHook = g_lowPowerHook;
        if (lowPowerHook != NULL) {
            lowPowerHook();
        }
#else
        wfi();
#endif
    }
}

/*
 * Description : Add task to sorted delay list.
 * Input       : taskCB  --- task control block
 *               timeout --- wait time, ticks
 */
LITE_OS_SEC_TEXT VOID OsTaskAdd2TimerList(LosTaskCB *taskCB, UINT32 timeout)
{
    SET_SORTLIST_VALUE(&(taskCB->sortList), timeout);
    OsAdd2SortLink(&OsPercpuGet()->taskSortLink, &taskCB->sortList);
#ifdef LOSCFG_KERNEL_SMP
    taskCB->timerCpu = ArchCurrCpuid();
#endif
}

LITE_OS_SEC_TEXT VOID OsTimerListDelete(LosTaskCB *taskCB)
{
    SortLinkAttribute *sortLinkHeader = NULL;

#ifdef LOSCFG_KERNEL_SMP
    /*
     * the task delay timer is on the specific processor,
     * we need delete the timer from that processor's sortlink.
     */
    sortLinkHeader = &g_percpu[taskCB->timerCpu].taskSortLink;
#else
    sortLinkHeader = &g_percpu[0].taskSortLink;
#endif
    OsDeleteSortLink(sortLinkHeader, &taskCB->sortList);
}

LITE_OS_SEC_TEXT VOID OsTaskScan(VOID)
{
    SortLinkList *sortList = NULL;
    LosTaskCB *taskCB = NULL;
    BOOL needSchedule = FALSE;
    UINT16 tempStatus;
    LOS_DL_LIST *listObject = NULL;
    SortLinkAttribute *taskSortLink = NULL;

    taskSortLink = &OsPercpuGet()->taskSortLink;
    SORTLINK_CURSOR_UPDATE(taskSortLink->cursor);
    SORTLINK_LISTOBJ_GET(listObject, taskSortLink);

    /*
     * When task is pended with timeout, the task block is on the timeout sortlink
     * (per cpu) and ipc(mutex,sem and etc.)'s block at the same time, it can be waken
     * up by either timeout or corresponding ipc it's waiting.
     *
     * Now synchronize sortlink procedure is used, therefore the whole task scan needs
     * to be protected, preventing another core from doing sortlink deletion at same time.
     */
    LOS_SpinLock(&g_taskSpin);

    if (LOS_ListEmpty(listObject)) {
        LOS_SpinUnlock(&g_taskSpin);
        return;
    }
    sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
    ROLLNUM_DEC(sortList->idxRollNum);

    while (ROLLNUM(sortList->idxRollNum) == 0) {
        LOS_ListDelete(&sortList->sortLinkNode);
        taskCB = LOS_DL_LIST_ENTRY(sortList, LosTaskCB, sortList);
        taskCB->taskStatus &= ~OS_TASK_STATUS_PEND_TIME;
        tempStatus = taskCB->taskStatus;
        if (tempStatus & OS_TASK_STATUS_PEND) {
            taskCB->taskStatus &= ~OS_TASK_STATUS_PEND;
            taskCB->taskStatus |= OS_TASK_STATUS_TIMEOUT;
            LOS_ListDelete(&taskCB->pendList);
            taskCB->taskSem = NULL;
            taskCB->taskMux = NULL;
        } else {
            taskCB->taskStatus &= ~OS_TASK_STATUS_DELAY;
        }

        if (!(tempStatus & OS_TASK_STATUS_SUSPEND)) {
            taskCB->taskStatus |= OS_TASK_STATUS_READY;
            OsPriQueueEnqueueProtect(&taskCB->pendList, taskCB->priority, PRI_QUEUE_TAIL);
            needSchedule = TRUE;
        }

        if (LOS_ListEmpty(listObject)) {
            break;
        }

        sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
    }

    LOS_SpinUnlock(&g_taskSpin);

    if (needSchedule == TRUE) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }
}


LITE_OS_SEC_BSS STATIC LOS_DL_LIST g_taskSortlink[LOSCFG_KERNEL_CORE_NUM][OS_TSK_SORTLINK_LEN];
LITE_OS_SEC_TEXT_INIT UINT32 OsTaskInit(VOID)
{
    UINT32 index;

    LOS_ListInit(&g_losFreeTask);
    LOS_ListInit(&g_taskRecycleList);
    for (index = 0; index < KERNEL_TSK_LIMIT; index++) {
        g_osTaskCBArray[index].taskStatus = OS_TASK_STATUS_UNUSED;
        g_osTaskCBArray[index].taskId = index;
        LOS_ListTailInsert(&g_losFreeTask, &g_osTaskCBArray[index].pendList);
    }

    OsPriQueueInit();

    /* init sortlink for each core */
    for (index = 0; index < LOSCFG_KERNEL_CORE_NUM; index++) {
        OsSortLinkInit(&g_percpu[index].taskSortLink, g_taskSortlink[index]);
    }

    g_taskMaxNum = KERNEL_TSK_LIMIT;
    return LOS_OK;
}
LOS_SYS_INIT(OsTaskInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_0);

UINT32 OsGetIdleTaskId(VOID)
{
    Percpu *perCpu = OsPercpuGet();
    return perCpu->idleTaskId;
}

LITE_OS_SEC_TEXT_INIT UINT32 OsIdleTaskCreate(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S taskInitParam = {0};
    Percpu *perCpu = OsPercpuGet();
    UINT32 *idleTaskId = &perCpu->idleTaskId;

    taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)OsIdleTask;
    taskInitParam.uwStackSize = KERNEL_TSK_IDLE_STACK_SIZE;
    taskInitParam.pcName = "IdleCore000";
    taskInitParam.usTaskPrio = LOS_TASK_PRIORITY_LOWEST;
#ifdef LOSCFG_KERNEL_SMP
    taskInitParam.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    ret = LOS_TaskCreateStatic(idleTaskId, &taskInitParam, g_osIdleTaskStack[ArchCurrCpuid()]);
#else
    ret = LOS_TaskCreate(idleTaskId, &taskInitParam);
#endif
    if (ret == LOS_OK) {
        OS_TCB_FROM_TID(*idleTaskId)->taskFlags |= OS_TASK_FLAG_SYSTEM;
    }

    return ret;
}
LOS_SYS_INIT(OsIdleTaskCreate, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_3);

/*
 * Description : get id of current running task.
 * Return      : task id
 */
LITE_OS_SEC_TEXT UINT32 LOS_CurTaskIDGet(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();

    if (runTask == NULL) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }
    return runTask->taskId;
}

LITE_OS_SEC_TEXT CHAR *OsCurTaskNameGet(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();

    if (runTask != NULL) {
        return runTask->taskName;
    }

    return NULL;
}

#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
LITE_OS_SEC_TEXT STATIC VOID OsTaskStackCheck(const LosTaskCB *oldTask, const LosTaskCB *newTask)
{
    if (!OS_STACK_MAGIC_CHECK(oldTask->topOfStack)) {
        LOS_PANIC("CURRENT task ID: %s:%u stack overflow!\n", oldTask->taskName, oldTask->taskId);
    }

    if (((UINTPTR)(newTask->stackPointer) <= newTask->topOfStack) ||
        ((UINTPTR)(newTask->stackPointer) > (newTask->topOfStack + newTask->stackSize))) {
        LOS_PANIC("HIGHEST task ID: %s:%u SP error! StackPointer: %p TopOfStack: %p\n",
                  newTask->taskName, newTask->taskId, newTask->stackPointer, (VOID *)newTask->topOfStack);
    }

    if (OsExcStackCheckHook != NULL) {
        OsExcStackCheckHook();
    }
}

LITE_OS_SEC_TEXT_MINOR UINT32 OsTaskMonInit(VOID)
{
    g_pfnUsrTskSwitchHook = NULL;
    return LOS_OK;
}
LOS_SYS_INIT(OsTaskMonInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_0);

LITE_OS_SEC_TEXT_MINOR VOID LOS_TaskSwitchHookReg(TSKSWITCHHOOK hook)
{
    g_pfnUsrTskSwitchHook = hook;
}

LITE_OS_SEC_TEXT_MINOR VOID OsTaskSwitchCheck(const LosTaskCB *oldTask, const LosTaskCB *newTask)
{
    TSKSWITCHHOOK usrTskSwitchHook;

    OsTaskStackCheck(oldTask, newTask);

    usrTskSwitchHook = g_pfnUsrTskSwitchHook;
    if (usrTskSwitchHook != NULL) {
        usrTskSwitchHook(oldTask->taskId, newTask->taskId);
    }
}
#endif /* LOSCFG_BASE_CORE_TSK_MONITOR */

#ifdef LOSCFG_KERNEL_LOWPOWER
LITE_OS_SEC_TEXT_MINOR VOID LOS_LowpowerHookReg(LOWPOWERIDLEHOOK hook)
{
    g_lowPowerHook = hook;
}
#endif

LITE_OS_SEC_TEXT_MINOR VOID LOS_IdleHandlerHookReg(IDLEHANDLERHOOK hook)
{
    g_idleHandlerHook = hook;
}

VOID OsIdleHandler(VOID)
{
    IDLEHANDLERHOOK idleHandlerHook = g_idleHandlerHook;
    if (idleHandlerHook != NULL) {
        idleHandlerHook();
    }
}

/*
 * Description : All task entry
 * Input       : taskId     --- The ID of the task to be run
 */
LITE_OS_SEC_TEXT_INIT VOID OsTaskEntry(UINT32 taskId)
{
    LosTaskCB *taskCB = NULL;
    VOID *ret = NULL;

    LOS_ASSERT(OS_TSK_GET_INDEX(taskId) < g_taskMaxNum);

    /*
     * task scheduler needs to be protected throughout the whole process
     * from interrupt and other cores. release task spinlock and enable
     * interrupt in sequence at the task entry.
     */
    OsSchedUnlock();
    (VOID)LOS_IntUnLock();

    taskCB = OS_TCB_FROM_TID(taskId);

#ifdef LOSCFG_OBSOLETE_API
    ret = taskCB->taskEntry(taskCB->args[0], taskCB->args[1], taskCB->args[2],
        taskCB->args[3]); /* 0~3: just for args array index */
#else
    ret = taskCB->taskEntry(taskCB->args);
#endif

    UINT32 intSave = LOS_IntLock();
    OsPercpuGet()->taskLockCnt = 0;
    LOS_IntRestore(intSave);
    (VOID)OsTaskDelete(taskCB->taskId, (UINTPTR)ret);
}

STATIC UINT32 OsTaskInitParamCheck(const TSK_INIT_PARAM_S *initParam)
{
    if (initParam == NULL) {
        return LOS_ERRNO_TSK_PTR_NULL;
    }

    if (initParam->pcName == NULL) {
        return LOS_ERRNO_TSK_NAME_EMPTY;
    }

    if (initParam->pfnTaskEntry == NULL) {
        return LOS_ERRNO_TSK_ENTRY_NULL;
    }

    if (initParam->usTaskPrio > LOS_TASK_PRIORITY_LOWEST) {
        return LOS_ERRNO_TSK_PRIOR_ERROR;
    }

#ifdef LOSCFG_KERNEL_SMP
    if ((initParam->usCpuAffiMask != 0) &&
        (!(initParam->usCpuAffiMask & LOSCFG_KERNEL_CPU_MASK))) {
        return LOS_ERRNO_TSK_CPU_AFFINITY_MASK_ERR;
    }
#endif

    return LOS_OK;
}

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
STATIC UINT32 OsTaskCreateParamCheckStatic(const UINT32 *taskId,
    const TSK_INIT_PARAM_S *initParam, const VOID *topStack)
{
    UINT32 ret;

    if (taskId == NULL) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (topStack == NULL) {
        return LOS_ERRNO_TSK_PTR_NULL;
    }

    ret = OsTaskInitParamCheck(initParam);
    if (ret != LOS_OK) {
        return ret;
    }

    if ((UINTPTR)topStack & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) {
        return LOS_ERRNO_TSK_STKSZ_NOT_ALIGN;
    }

    if (initParam->uwStackSize & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) {
        return LOS_ERRNO_TSK_STKSZ_NOT_ALIGN;
    }

    if ((initParam->uwStackSize < LOS_TASK_MIN_STACK_SIZE) || (initParam->uwStackSize < sizeof(TaskContext))) {
        return LOS_ERRNO_TSK_STKSZ_TOO_SMALL;
    }

    if (initParam->uwStackSize > ((UINTPTR)(-1) - (UINTPTR)topStack)) {
        return LOS_ERRNO_TSK_STKSZ_TOO_LARGE;
    }
    return LOS_OK;
}
#endif

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
LITE_OS_SEC_TEXT_INIT STATIC UINT32 OsTaskCreateParamCheck(const UINT32 *taskId,
    TSK_INIT_PARAM_S *initParam, VOID **pool)
{
    UINT32 ret;
    *pool = (VOID *)OS_TASK_STACK_POOL;

    if (taskId == NULL) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    ret = OsTaskInitParamCheck(initParam);
    if (ret != LOS_OK) {
        return ret;
    }

#ifdef LOSCFG_EXC_INTERACTION
    if (OsCheckExcInteractionTask(initParam) == LOS_OK) {
        *pool = m_aucSysMem0;
    }
#endif

    if (initParam->uwStackSize == 0) {
        initParam->uwStackSize = KERNEL_TSK_DEFAULT_STACK_SIZE;
    }

    if (initParam->uwStackSize > LOS_MemPoolSizeGet(*pool)) {
        return LOS_ERRNO_TSK_STKSZ_TOO_LARGE;
    }

    initParam->uwStackSize = (UINT32)LOS_Align(initParam->uwStackSize, LOSCFG_STACK_POINT_ALIGN_SIZE);

    if ((initParam->uwStackSize < LOS_TASK_MIN_STACK_SIZE) || (initParam->uwStackSize < sizeof(TaskContext))) {
        return LOS_ERRNO_TSK_STKSZ_TOO_SMALL;
    }

    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT STATIC INLINE VOID *OsTaskStackAlloc(UINT32 stackSize, VOID *pool)
{
    return (VOID *)LOS_MemAllocAlign(pool, stackSize, LOSCFG_STACK_POINT_ALIGN_SIZE);
}
#endif /* LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION */

#ifdef LOSCFG_KERNEL_SMP_TASK_SYNC
STATIC INLINE UINT32 OsTaskSyncCreate(LosTaskCB *taskCB)
{
    UINT32 ret = LOS_SemCreate(0, &taskCB->syncSignal);
    if (ret != LOS_OK) {
        return LOS_ERRNO_TSK_MP_SYNC_RESOURCE;
    }
    return LOS_OK;
}

STATIC INLINE VOID OsTaskSyncDestroy(const LosTaskCB *taskCB)
{
    (VOID)LOS_SemDelete(taskCB->syncSignal);
}

STATIC INLINE UINT32 OsTaskSyncWait(const LosTaskCB *taskCB)
{
    UINT32 ret = LOS_OK;

    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));
    LOS_SpinUnlock(&g_taskSpin);
    /*
     * gc soft timer works every OS_MP_GC_PERIOD period, to prevent this timer
     * triggered right at the timeout has reached, we set the timeout as double
     * of the gc period.
     */
    if (LOS_SemPend(taskCB->syncSignal, OS_MP_GC_PERIOD * 2) != LOS_OK) {
        ret = LOS_ERRNO_TSK_MP_SYNC_FAILED;
    }

    LOS_SpinLock(&g_taskSpin);

    return ret;
}

STATIC INLINE VOID OsTaskSyncWake(const LosTaskCB *taskCB)
{
    UINT32 syncSignal = taskCB->syncSignal;
    /*
     * unlock to let synchronization works.
     * this operation needs task status set to UNUSED, and yet not put
     * back to the recycle or free tcb list.
     */
    LOS_ASSERT(taskCB->taskStatus & OS_TASK_IS_EXIT);
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    LOS_SpinUnlock(&g_taskSpin);

    /*
     * do the sync, because bottom half of LOS_SemPend has no operation on the sem,
     * we can delete this sem after the post.
     */
    (VOID)LOS_SemPost(syncSignal);
    (VOID)LOS_SemDelete(syncSignal);

    LOS_SpinLock(&g_taskSpin);
}
#else
STATIC INLINE UINT32 OsTaskSyncCreate(LosTaskCB *taskCB)
{
    (VOID)taskCB;
    return LOS_OK;
}

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
STATIC INLINE VOID OsTaskSyncDestroy(const LosTaskCB *taskCB)
{
    (VOID)taskCB;
}
#endif

#ifdef LOSCFG_KERNEL_SMP
STATIC INLINE UINT32 OsTaskSyncWait(const LosTaskCB *taskCB)
{
    (VOID)taskCB;
    return LOS_OK;
}
#endif

STATIC INLINE VOID OsTaskSyncWake(const LosTaskCB *taskCB)
{
    (VOID)taskCB;
}
#endif

LITE_OS_SEC_TEXT_INIT STATIC VOID OsTaskDelActionOnRun(LosTaskCB *taskCB)
{
    LosTaskCB *runTask = NULL;

    if (taskCB->usrStack) {
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
        LOS_ListAdd(&g_losFreeTask, &taskCB->pendList);
#endif
    } else {
#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
        LOS_ListTailInsert(&g_taskRecycleList, &taskCB->pendList);
#endif
    }

    runTask = &g_osTaskCBArray[g_taskMaxNum];
    runTask->taskId = taskCB->taskId;
    runTask->taskStatus = taskCB->taskStatus;
    runTask->topOfStack = taskCB->topOfStack;
    runTask->taskName = taskCB->taskName;

    taskCB->taskStatus = OS_TASK_STATUS_UNUSED;
    return;
}

#ifdef LOSCFG_KERNEL_SMP
LITE_OS_SEC_TEXT_MINOR VOID OsTaskProcSignal(VOID)
{
    LosTaskCB *runTask = NULL;
    UINT32    ret;

    /*
     * private and uninterruptible, no protection needed.
     * while this task is always running when others cores see it,
     * so it keeps receiving signals while follow code executing.
     */
    runTask = OsCurrTaskGet();
    if (runTask->mpSignal == SIGNAL_NONE) {
        return;
    }

    if (runTask->mpSignal & SIGNAL_KILL) {
        /*
         * clear the signal, and do the task deletion. if the signaled task has been
         * scheduled out, then this deletion will wait until next run.
         */
        runTask->mpSignal = SIGNAL_NONE;
        ret = LOS_TaskDelete(runTask->taskId);
        if (ret) {
            PRINT_ERR("%s: tsk del fail err:0x%x\n", __FUNCTION__, ret);
        }
    } else if (runTask->mpSignal & SIGNAL_SUSPEND) {
        runTask->mpSignal &= ~SIGNAL_SUSPEND;

        /* suspend killed task may fail, ignore the result */
        (VOID)LOS_TaskSuspend(runTask->taskId);
    } else if (runTask->mpSignal & SIGNAL_AFFI) {
        runTask->mpSignal &= ~SIGNAL_AFFI;

        OsSetSchedFlag(INT_PEND_RESCH);
        /* pri-queue has updated, notify the target cpu */
        LOS_MpSchedule((UINT32)runTask->cpuAffiMask);
#ifndef LOSCFG_COMPAT_POSIX
    }
#else
    } else if (runTask->mpSignal & SIGNAL_CANCEL) {
        /*
         * clear the signal, and do the task deletion. if the signaled task has been
         * scheduled out, then this deletion will wait until next run.
         */
        runTask->mpSignal = SIGNAL_NONE;
        ret = OsTaskDelete(runTask->taskId, (UINTPTR)OS_TASK_CANCEL_VALUE);
        if (ret) {
            PRINT_ERR("%s: tsk del fail err:0x%x\n", __FUNCTION__, ret);
        }
    }
#endif

    LOS_TRACE(TASK_SIGNAL, runTask->taskId, runTask->mpSignal);

    return;
}
#else
LITE_OS_SEC_TEXT_MINOR VOID OsTaskProcSignal(VOID)
{
}
#endif

#ifndef LOSCFG_SCHED_LATENCY
STATIC INLINE UINT32 OsTaskUnlockVerify(UINT32 losTaskLock)
{
    if ((losTaskLock == 0) && ((OsPercpuGet()->schedFlag) == INT_PEND_RESCH) && OS_SCHEDULER_ACTIVE) {
        return LOS_OK;
    }

    return LOS_NOK;
}

#else
STATIC INLINE UINT32 OsTaskUnlockVerify(UINT32 losTaskLock)
{
    if ((losTaskLock == 0) && OS_SCHEDULER_ACTIVE) {
        return LOS_OK;
    }

    return LOS_NOK;
}

#endif

/*
 * Check if needs to do the delete operation in the current task.
 * Return TRUE, if needs to do the deletion.
 * Return FALSE, if meets following circumstances:
 * 1. Do the deletion across cores, if SMP is enabled
 * 2. Do the deletion when preemption is disabled
 * 3. Do the deletion in hard-irq
 * then LOS_TaskDelete will directly return with 'ret' value.
 */
LITE_OS_SEC_TEXT_INIT STATIC BOOL OsTaskDeleteCheckOnRun(LosTaskCB *taskCB, UINT32 *ret, UINTPTR retval)
{
#if !defined(LOSCFG_KERNEL_SMP) && !defined(LOSCFG_COMPAT_POSIX)
    (VOID)retval;
#endif

    /* init default out return value */
    *ret = LOS_OK;

#ifdef LOSCFG_KERNEL_SMP
    /* ASYNCHRONIZED. No need to do task lock checking */
    if (taskCB->currCpu != ArchCurrCpuid()) {
        /*
         * the task is running on another cpu.
         * mask the target task with "kill/cancel" signal, and trigger mp schedule
         * which might not be essential but the deletion could more in time.
         */
#ifdef LOSCFG_COMPAT_POSIX
        OsTaskSignalSet(taskCB, (retval == (UINTPTR)OS_TASK_CANCEL_VALUE) ? SIGNAL_CANCEL : SIGNAL_KILL);
#else
        OsTaskSignalSet(taskCB, SIGNAL_KILL);
#endif
        LOS_MpSchedule(CPUID_TO_AFFI_MASK(taskCB->currCpu));
        *ret = OsTaskSyncWait(taskCB);
        return FALSE;
    }
#endif

    if (!OsPreemptableInSched()) {
        /* If the task is running and scheduler is locked then you can not delete it */
        *ret = LOS_ERRNO_TSK_DELETE_LOCKED;
        return FALSE;
    }

#ifdef LOSCFG_KERNEL_SMP
    if (OS_INT_ACTIVE) {
        /*
         * delete running task in interrupt.
         * mask "kill/cancel" signal and later deletion will be handled.
         */
#ifdef LOSCFG_COMPAT_POSIX
        OsTaskSignalSet(taskCB, (retval == (UINTPTR)OS_TASK_CANCEL_VALUE) ? SIGNAL_CANCEL : SIGNAL_KILL);
#else
        OsTaskSignalSet(taskCB, SIGNAL_KILL);
#endif
        return FALSE;
    }
#else
    (VOID)taskCB;
#endif

    return TRUE;
}

LITE_OS_SEC_TEXT_INIT STATIC VOID OsTaskCBInit(LosTaskCB *taskCB, const TSK_INIT_PARAM_S *initParam,
                                               VOID *stackPtr, const VOID *topStack, BOOL useUsrStack)
{
    taskCB->stackPointer = stackPtr;
#ifdef LOSCFG_OBSOLETE_API
    taskCB->args[0]      = initParam->auwArgs[0]; /* 0~3: just for args array index */
    taskCB->args[1]      = initParam->auwArgs[1];
    taskCB->args[2]      = initParam->auwArgs[2];
    taskCB->args[3]      = initParam->auwArgs[3];
#else
    taskCB->args         = initParam->pArgs;
#endif
    taskCB->topOfStack   = (UINTPTR)topStack;
    taskCB->stackSize    = initParam->uwStackSize;
    taskCB->taskSem      = NULL;
    taskCB->taskMux      = NULL;
    taskCB->priority     = initParam->usTaskPrio;
    taskCB->priBitMap    = 0;
    taskCB->taskEntry    = initParam->pfnTaskEntry;
#ifdef LOSCFG_BASE_IPC_EVENT
    LOS_ListInit(&taskCB->event.stEventList);
    taskCB->event.uwEventID = 0;
    taskCB->eventMask    = 0;
#endif

    taskCB->taskName     = initParam->pcName;
    taskCB->msg          = NULL;

#ifdef LOSCFG_TASK_JOINABLE
    taskCB->taskRetval = NULL;
    taskCB->joined = NULL;
    taskCB->joinner = NULL;
    /* set the task is detached or joinable */
    if ((initParam->uwResved & LOS_TASK_STATUS_JOINABLE) == LOS_TASK_STATUS_JOINABLE) {
        taskCB->taskFlags = OS_TASK_FLAG_JOINABLE;
    } else {
        taskCB->taskFlags = OS_TASK_FLAG_DETACHED;
    }
#else
    taskCB->taskFlags = OS_TASK_FLAG_DETACHED;
#endif

    taskCB->usrStack     = useUsrStack ? 1 : 0; /* 0: dynamically alloc stack space;1: user inputs stack space */

    OsTaskSignalSet(taskCB, SIGNAL_NONE);

#ifdef LOSCFG_KERNEL_SMP
    taskCB->currCpu      = OS_TASK_INVALID_CPUID;
    taskCB->lastCpu      = OS_TASK_INVALID_CPUID;
    taskCB->cpuAffiMask  = (initParam->usCpuAffiMask) ? initParam->usCpuAffiMask : LOSCFG_KERNEL_CPU_MASK;
#endif
#ifdef LOSCFG_BASE_CORE_TIMESLICE
    taskCB->timeSlice    = 0;
#endif
#ifdef LOSCFG_KERNEL_LOCKDEP
    for (UINT32 i = 0; i < LOCK_TYPE_MAX; i++) {
        taskCB->lockDep[i].waitLock = NULL;
        taskCB->lockDep[i].lockDepth = 0;
    }
#endif
#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    (VOID)memset(&taskCB->schedStat, 0, sizeof(SchedStat));
#endif
}

STATIC UINT32 OsTaskGetFreeTaskCB(LosTaskCB **taskCB)
{
    if (LOS_ListEmpty(&g_losFreeTask)) {
        return LOS_ERRNO_TSK_TCB_UNAVAILABLE;
    }

    *taskCB = OS_TCB_FROM_PENDLIST(LOS_DL_LIST_FIRST(&g_losFreeTask));
    if (*taskCB == NULL) {
        return LOS_ERRNO_TSK_PTR_NULL;
    }
    LOS_ListDelete(LOS_DL_LIST_FIRST(&g_losFreeTask));
    return LOS_OK;
}

STATIC UINT32 OsTaskCreateOnly(UINT32 *taskId, TSK_INIT_PARAM_S *initParam, VOID *topStack, BOOL useUsrStack)
{
    UINT32 intSave;
    UINT32 errRet = LOS_OK;
    VOID *stackPtr = NULL;
    LosTaskCB *taskCB = NULL;
#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    VOID *pool = NULL;
#endif

    if (useUsrStack) {
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
        errRet = OsTaskCreateParamCheckStatic(taskId, initParam, topStack);
#endif
    } else {
#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
        errRet = OsTaskCreateParamCheck(taskId, initParam, &pool);
#endif
    }

    if (errRet != LOS_OK) {
        return errRet;
    }

    SCHEDULER_LOCK(intSave);
    errRet = OsTaskGetFreeTaskCB(&taskCB);
    if (errRet != LOS_OK) {
        goto LOS_ERREND;
    }
    SCHEDULER_UNLOCK(intSave);

    errRet = OsTaskSyncCreate(taskCB);
    if (errRet != LOS_OK) {
        goto LOS_ERREND_REWIND_TCB;
    }

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    if (useUsrStack == FALSE) {
        topStack = OsTaskStackAlloc(initParam->uwStackSize, pool);
        if (topStack == NULL) {
            errRet = LOS_ERRNO_TSK_NO_MEMORY;
            OsTaskSyncDestroy(taskCB);
            goto LOS_ERREND_REWIND_TCB;
        }
    }
#endif

    stackPtr = ArchTaskStackInit(taskCB->taskId, initParam->uwStackSize, topStack);
    OsTaskCBInit(taskCB, initParam, stackPtr, topStack, useUsrStack);

    if (OsConsoleIDSetHook != NULL) {
        OsConsoleIDSetHook(taskCB->taskId, OsCurrTaskGet()->taskId);
    }

#ifdef LOSCFG_TRUSTZONE
    if ((initParam->uwResved & LOS_TASK_OS_ALLOC_SECURE) == LOS_TASK_OS_ALLOC_SECURE) {
        taskCB->secureContextSP = ArchAllocSecureContext(LOSCFG_TZ_SECURE_TSK_DEFAULT_STACK_SIZE, taskCB->taskId);
        if (taskCB->secureContextSP == NULL) {
            taskCB->taskFlags |= OS_TASK_FLAG_DETACHED;
            (VOID)LOS_TaskDelete(taskCB->taskId);
            return LOS_ERRNO_TSK_ALLOC_SECURE_FAILED;
        }
    } else {
        taskCB->secureContextSP = NULL;
    }
#endif

    /*
     * The suspend state must be set after the taskCB is fully initialized in case the task is resumed
     * when taskCB is not ready.
     */
    taskCB->taskStatus = OS_TASK_STATUS_SUSPEND;

#ifdef LOSCFG_KERNEL_CPUP
    OsCpupCBTaskCreate(taskCB->taskId, taskCB->taskStatus);
#endif

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OsPerfTaskCreate(taskCB);
#endif

    *taskId = taskCB->taskId;

    return LOS_OK;

LOS_ERREND_REWIND_TCB:
    SCHEDULER_LOCK(intSave);
    LOS_ListAdd(&g_losFreeTask, &taskCB->pendList);
LOS_ERREND:
    SCHEDULER_UNLOCK(intSave);
    return errRet;
}

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
UINT32 LOS_TaskCreateOnlyStatic(UINT32 *taskId, TSK_INIT_PARAM_S *initParam, VOID *topStack)
{
    return OsTaskCreateOnly(taskId, initParam, topStack, TRUE);
}

UINT32 LOS_TaskCreateStatic(UINT32 *taskId, TSK_INIT_PARAM_S *initParam, VOID *topStack)
{
    UINT32 ret;

    ret = LOS_TaskCreateOnlyStatic(taskId, initParam, topStack);
    if (ret != LOS_OK) {
        return ret;
    }

    return LOS_TaskResume(*taskId);
}
#endif

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskCreateOnly(UINT32 *taskId, TSK_INIT_PARAM_S *initParam)
{
    return OsTaskCreateOnly(taskId, initParam, NULL, FALSE);
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskCreate(UINT32 *taskId, TSK_INIT_PARAM_S *initParam)
{
    UINT32 ret;

    ret = LOS_TaskCreateOnly(taskId, initParam);
    if (ret != LOS_OK) {
        return ret;
    }

    return LOS_TaskResume(*taskId);
}
#endif

#ifdef LOSCFG_TRUSTZONE
LITE_OS_SEC_TEXT_INIT STATIC VOID OsTaskFreeSecureContext(LosTaskCB *taskCB, UINT32 *intSave)
{
    UINT32 intLock;
    if (taskCB->secureContextSP != NULL) {
        OsPercpuGet()->taskLockCnt++;
        SCHEDULER_UNLOCK(*intSave);
        (VOID)ArchFreeSecureContext(taskCB->secureContextSP);
        SCHEDULER_LOCK(intLock);
        taskCB->secureContextSP = NULL;
        OsPercpuGet()->taskLockCnt--;
        *intSave = intLock;
    }
}
#endif

LITE_OS_SEC_TEXT_INIT STATIC INLINE VOID OsTaskResourceReclaim(LosTaskCB *taskCB, UINT32 *intSave)
{
#ifdef LOSCFG_BASE_IPC_EVENT
    taskCB->event.uwEventID = OS_INVALID_VALUE;
    taskCB->eventMask = 0;
#endif

#ifdef LOSCFG_TRUSTZONE
    OsTaskFreeSecureContext(taskCB, intSave);
#else
    (VOID)intSave;
#endif

#ifdef LOSCFG_KERNEL_CPUP
    OsCpupCBTaskDelete(taskCB->taskId);
#endif

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OsPerfTaskDelete(taskCB);
#endif

    OS_MEM_CLEAR(taskCB->taskId);

    OsTaskSyncWake(taskCB);
}

LITE_OS_SEC_TEXT_INIT UINT32 OsTaskDelete(UINT32 taskId, UINTPTR retVal)
{
    LosTaskCB *taskCB = NULL;
    UINT32 intSave;
    UINT32 errRet = LOS_OK;
    UINT16 tempStatus;
    BOOL needRetry;

    taskCB = OS_TCB_FROM_TID(taskId);
    if (taskCB->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    LOS_TRACE(TASK_DELETE, taskCB->taskId, taskCB->taskStatus, taskCB->usrStack);
    SCHEDULER_LOCK(intSave);

    do {
        /*
         * By default, the loop is executed only once.
         * If priqueue dequeue fails, or the task cannot be operated,
         * we need to re-judge the task status and perform the corresponding operation.
         */
        needRetry = FALSE;
        if (OsTaskIsInoperable(taskCB)) {
            needRetry = TRUE;
            continue;
        }

        /*
         * It's necessary to use tempStatus to record the original status of the taskCB.
         * If the taskCB is on another core, the task status might be changed,
         * which will bypass all conditional judgments.
         * The task status changing from ready to running is not protected by the g_taskSpin,
         * so it needs to be acquired again after failure in do-while.
         */
        tempStatus = taskCB->taskStatus;
        if (tempStatus & OS_TASK_STATUS_UNUSED) {
            errRet = LOS_ERRNO_TSK_NOT_CREATED;
            goto LOS_UNLOCK;
        }

        if (OS_TASK_IS_ZOMBIE(tempStatus)) {
            errRet = LOS_ERRNO_TSK_IS_ZOMBIE;
            goto LOS_UNLOCK;
        }

        if (tempStatus & OS_TASK_STATUS_RUNNING) {
            if (!OsTaskDeleteCheckOnRun(taskCB, &errRet, retVal)) {
                goto LOS_UNLOCK;
            }
            goto LOS_RESRECYCLE;
        }

        if (tempStatus & OS_TASK_STATUS_READY) {
            needRetry = !OsSchedReadyTaskRemove(taskCB);
        } else if ((tempStatus & OS_TASK_STATUS_PEND) && (!OS_TASK_IS_JOINING(taskCB))) {
            LOS_ListDelete(&taskCB->pendList);
        }
    } while (needRetry);

    OsTaskCancelJoin(taskCB);

    if (taskCB->taskStatus & (OS_TASK_STATUS_DELAY | OS_TASK_STATUS_PEND_TIME)) {
        OsTimerListDelete(taskCB);
    }

    taskCB->taskStatus &= ~OS_TASK_STATUS_SUSPEND;

LOS_RESRECYCLE:
    if (OS_TASK_IS_JOINABLE(taskCB)) {
        taskCB->taskStatus |= OS_TASK_STATUS_ZOMBIE;
    } else {
        taskCB->taskStatus |= OS_TASK_STATUS_UNUSED;
        OsTaskResourceReclaim(taskCB, &intSave);
    }

    if (taskCB->taskStatus & OS_TASK_STATUS_RUNNING) {
        OsTaskExtStatusSet(taskCB, OS_TASK_STATUS_SCHED);
    }

#ifdef LOSCFG_TASK_JOINABLE
    if (OS_TASK_IS_JOINABLE(taskCB)) {
        OsTaskJoinPost(taskCB);
        taskCB->taskRetval = (VOID *)retVal;
    }
#endif

    if (taskCB->taskStatus & OS_TASK_STATUS_RUNNING) {
        if (OS_TASK_IS_DETACHED(taskCB)) {
            OsTaskDelActionOnRun(taskCB);
        }

        OsSchedLockGlobal2Local();

        OsTaskReSched();

        OsSchedUnlockLocalRestore(intSave);
        goto LOS_RETURN;
    } else if (OS_TASK_IS_DETACHED(taskCB)) {
        taskCB->taskStatus = OS_TASK_STATUS_UNUSED;
        OsTaskStackFree(taskCB);
    }

LOS_UNLOCK:
    SCHEDULER_UNLOCK(intSave);

LOS_RETURN:
    return errRet;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskDelete(UINT32 taskId)
{
    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

#ifdef LOSCFG_TASK_JOINABLE
    return OsTaskDelete(taskId, (UINTPTR)(OsCurrTaskGet()->taskId));
#else
    return OsTaskDelete(taskId, 0);
#endif
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskResume(UINT32 taskId)
{
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;
    UINT16 tempStatus;
    UINT32 errRet;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    LOS_TRACE(TASK_RESUME, taskCB->taskId, taskCB->taskStatus, taskCB->priority);

    SCHEDULER_LOCK(intSave);

#ifdef LOSCFG_KERNEL_SMP
    /* clear pending signal */
    taskCB->mpSignal &= ~SIGNAL_SUSPEND;
#endif

    OsTaskWaitCanOperate(taskCB);

    tempStatus = taskCB->taskStatus;
    if (tempStatus & OS_TASK_STATUS_UNUSED) {
        errRet = LOS_ERRNO_TSK_NOT_CREATED;
        goto LOS_ERREND;
    } else if (OS_TASK_IS_ZOMBIE(tempStatus)) {
        errRet = LOS_ERRNO_TSK_IS_ZOMBIE;
        goto LOS_ERREND;
    } else if (!(tempStatus & OS_TASK_STATUS_SUSPEND)) {
        errRet = LOS_ERRNO_TSK_NOT_SUSPENDED;
        goto LOS_ERREND;
    }

    taskCB->taskStatus &= ~OS_TASK_STATUS_SUSPEND;
    if ((taskCB->taskStatus & OS_CHECK_TASK_BLOCK) || OsTaskIsSchedCanBeAborted(taskCB)) {
        errRet = LOS_OK;
        goto LOS_ERREND;
    }

    /* The scheduling lock is opened in the OsSchedResume interface. */
    OsSchedResume(taskCB, intSave);

    if (OS_SCHEDULER_ACTIVE) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }

    return LOS_OK;

LOS_ERREND:
    SCHEDULER_UNLOCK(intSave);
    return errRet;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskSuspend(UINT32 taskId)
{
    LosTaskCB *taskCB = NULL;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    if (taskCB->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    return OsSchedSuspend(taskCB);
}

LITE_OS_SEC_TEXT UINT32 LOS_TaskDelay(UINT32 tick)
{
    UINT32 intSave;
    LosTaskCB *runTask = NULL;

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_TSK_DELAY_IN_INT;
    }

    runTask = OsCurrTaskGet();
    if (runTask->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    if (!OsPreemptable()) {
        return LOS_ERRNO_TSK_DELAY_IN_LOCK;
    }

    if (tick == 0) {
        return LOS_TaskYield();
    } else {
        SCHEDULER_LOCK(intSave);
        OsTaskAdd2TimerList(runTask, tick);
        OsTaskExtStatusSet(runTask, OS_TASK_STATUS_SCHED);
        OsSchedLockGlobal2Local();
        runTask->taskStatus |= OS_TASK_STATUS_DELAY;
        OsSchedResched();
        OsSchedUnlockLocalRestore(intSave);
    }

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT16 LOS_TaskPriGet(UINT32 taskId)
{
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;
    UINT16 priority;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return (UINT16)OS_INVALID;
    }

    taskCB = OS_TCB_FROM_TID(taskId);

    SCHEDULER_LOCK(intSave);
    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        priority = (UINT16)OS_INVALID;
    } else {
        priority = taskCB->priority;
    }

    SCHEDULER_UNLOCK(intSave);
    return priority;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_TaskPriSet(UINT32 taskId, UINT16 taskPrio)
{
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;
    BOOL needSched = FALSE;
    UINT32 ret = LOS_OK;

    if (taskPrio > LOS_TASK_PRIORITY_LOWEST) {
        return LOS_ERRNO_TSK_PRIOR_ERROR;
    }

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    if (taskCB->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    LOS_TRACE(TASK_PRIOSET, taskCB->taskId, taskCB->taskStatus, taskCB->priority, taskPrio);

    SCHEDULER_LOCK(intSave);

    ret = OsSchedPrioSet(taskCB, taskPrio, &needSched);

    SCHEDULER_UNLOCK(intSave);

    /* delete the task and insert with right priority into ready queue */
    if (needSched) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_CurTaskPriSet(UINT16 taskPrio)
{
    return LOS_TaskPriSet(OsCurrTaskGet()->taskId, taskPrio);
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_TaskYield(VOID)
{
    UINT32 intSave;
    LosTaskCB *runTask = NULL;
    UINT32 ret;

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_TSK_YIELD_IN_INT;
    }

    if (!OsPreemptable()) {
        return LOS_ERRNO_TSK_YIELD_IN_LOCK;
    }

    runTask = OsCurrTaskGet();
    if (runTask->taskId >= g_taskMaxNum) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    SCHEDULER_LOCK(intSave);

    ret = OsSchedYield(runTask);

    OsSchedUnlockLocalRestore(intSave);
    return ret;
}

BOOL LOS_TaskIsScheduled(VOID)
{
    return OS_SCHEDULER_ACTIVE;
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_TaskLock(VOID)
{
    UINT32 intSave;
    UINT32 *losTaskLock = NULL;

    intSave = LOS_IntLock();
    losTaskLock = &OsPercpuGet()->taskLockCnt;
    (*losTaskLock)++;
    LOS_IntRestore(intSave);
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_TaskUnlock(VOID)
{
    UINT32 intSave;
    UINT32 *losTaskLock = NULL;
    Percpu *percpu = NULL;

    intSave = LOS_IntLock();

    percpu = OsPercpuGet();
    losTaskLock = &percpu->taskLockCnt;
    if (*losTaskLock > 0) {
        (*losTaskLock)--;
        if (OsTaskUnlockVerify(*losTaskLock) == LOS_OK) {
            OsSetSchedFlag(INT_NO_RESCH);
            LOS_IntRestore(intSave);
            LOS_Schedule();
            return;
        }
    }

    LOS_IntRestore(intSave);
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_TaskUnlockNoSched(VOID)
{
    UINT32 intSave;
    UINT32 *losTaskLock = NULL;
    Percpu *percpu = NULL;

    intSave = LOS_IntLock();

    percpu = OsPercpuGet();
    losTaskLock = &percpu->taskLockCnt;
    if (*losTaskLock > 0) {
        (*losTaskLock)--;
    }

    LOS_IntRestore(intSave);
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_TaskInfoGet(UINT32 taskId, TSK_INFO_S *taskInfo)
{
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;
    errno_t errorCode;

    if (taskInfo == NULL) {
        return LOS_ERRNO_TSK_PTR_NULL;
    }

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    SCHEDULER_LOCK(intSave);

    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        SCHEDULER_UNLOCK(intSave);
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    if ((!OsTaskIsRunOrSched(taskCB)) || OS_INT_ACTIVE) {
        taskInfo->uwSP = (UINTPTR)taskCB->stackPointer;
    } else {
        taskInfo->uwSP = ArchSPGet();
    }

    taskInfo->usTaskStatus = taskCB->taskStatus;
    taskInfo->usTaskPrio = taskCB->priority;
    taskInfo->uwStackSize = taskCB->stackSize;
    taskInfo->uwTopOfStack = taskCB->topOfStack;
#ifdef LOSCFG_BASE_IPC_EVENT
    taskInfo->uwEvent = taskCB->event;
    taskInfo->uwEventMask = taskCB->eventMask;
#endif
    taskInfo->pTaskSem = taskCB->taskSem;
    taskInfo->pTaskMux = taskCB->taskMux;
    taskInfo->uwTaskID = taskId;

    /* It will cause double lock issue that print after SCHEDULER_LOCK,
     * so handle the return value errorCode after SCHEDULER_UNLOCK */
    errorCode = strncpy_s(taskInfo->acName, LOS_TASK_NAMELEN, taskCB->taskName, LOS_TASK_NAMELEN - 1);
    taskInfo->uwBottomOfStack = TRUNCATE(((UINTPTR)taskCB->topOfStack + taskCB->stackSize),
                                         LOSCFG_STACK_POINT_ALIGN_SIZE);
    taskInfo->uwCurrUsed = (UINT32)(taskInfo->uwBottomOfStack - taskInfo->uwSP);

    taskInfo->bOvf = OsStackWaterLineGet((const UINTPTR *)taskInfo->uwBottomOfStack,
                                         (const UINTPTR *)taskInfo->uwTopOfStack, &taskInfo->uwPeakUsed);
    SCHEDULER_UNLOCK(intSave);
    if (errorCode != EOK) {
        PRINT_DEBUG("Task name copy failed, err=%d\n", errorCode);
    }

    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_TaskCpuAffiSet(UINT32 taskId, UINT16 cpuAffiMask)
{
#ifdef LOSCFG_KERNEL_SMP
    LosTaskCB *taskCB = NULL;
    UINT32 intSave;
    BOOL needSched = FALSE;
    UINT16 currCpuMask;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (!(cpuAffiMask & LOSCFG_KERNEL_CPU_MASK)) {
        return LOS_ERRNO_TSK_CPU_AFFINITY_MASK_ERR;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    if (taskCB->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    SCHEDULER_LOCK(intSave);

    OsTaskWaitCanOperate(taskCB);

    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        SCHEDULER_UNLOCK(intSave);
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    if (OS_TASK_IS_ZOMBIE(taskCB->taskStatus)) {
        SCHEDULER_UNLOCK(intSave);
        return LOS_ERRNO_TSK_IS_ZOMBIE;
    }

    taskCB->cpuAffiMask = cpuAffiMask;
    currCpuMask = CPUID_TO_AFFI_MASK(taskCB->currCpu);
    if (!(currCpuMask & cpuAffiMask)) {
        needSched = TRUE;
        OsSchedAffiChange(taskCB);
        OsTaskSignalSet(taskCB, SIGNAL_AFFI);
    }
    SCHEDULER_UNLOCK(intSave);

    if (needSched && OS_SCHEDULER_ACTIVE) {
        LOS_MpSchedule(currCpuMask);
        LOS_Schedule();
    }
    return LOS_OK;

#else
    (VOID)taskId;
    (VOID)cpuAffiMask;
    return LOS_OK;
#endif
}

LITE_OS_SEC_TEXT_MINOR UINT16 LOS_TaskCpuAffiGet(UINT32 taskId)
{
#ifdef LOSCFG_KERNEL_SMP
#define INVALID_CPU_AFFI_MASK   0
    LosTaskCB *taskCB = NULL;
    UINT16 cpuAffiMask;
    UINT32 intSave;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return INVALID_CPU_AFFI_MASK;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    SCHEDULER_LOCK(intSave);
    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        SCHEDULER_UNLOCK(intSave);
        return INVALID_CPU_AFFI_MASK;
    }

    cpuAffiMask = taskCB->cpuAffiMask;
    SCHEDULER_UNLOCK(intSave);

    return cpuAffiMask;
#else
    (VOID)taskId;
    return 1; /* 1: mask of current cpu */
#endif
}

#ifdef LOSCFG_TRUSTZONE
LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskAllocSecureContext(UINT32 taskId, UINT32 size)
{
    LosTaskCB *taskCB = NULL;
    VOID *secPtr = NULL;
    UINT32 errRet = LOS_OK;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }
    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_TSK_ALLOC_SECURE_INT;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    LOS_TaskLock();
    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        errRet = LOS_ERRNO_TSK_NOT_CREATED;
        goto LOS_RETURN;
    }

    if (OS_TASK_IS_ZOMBIE(taskCB->taskStatus)) {
        errRet = LOS_ERRNO_TSK_IS_ZOMBIE;
        goto LOS_RETURN;
    }

    secPtr = taskCB->secureContextSP;
    if (secPtr != NULL) {
        errRet = LOS_ERRNO_TSK_SECURE_ALREADY_ALLOC;
        goto LOS_RETURN;
    }

    secPtr = ArchAllocSecureContext(size, taskId);
    if (secPtr == NULL) {
        errRet = LOS_ERRNO_TSK_ALLOC_SECURE_FAILED;
        goto LOS_RETURN;
    }
    taskCB->secureContextSP = secPtr;

LOS_RETURN:
    LOS_TaskUnlock();
    return errRet;
}
#endif

#ifdef LOSCFG_TASK_JOINABLE
LITE_OS_SEC_TEXT_INIT STATIC INLINE UINT32 OsTaskJoinPend(LosTaskCB *taskCB, UINTPTR *retval, UINT32 *intSave)
{
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    if (!OS_TASK_IS_ZOMBIE(taskCB->taskStatus)) {
        LosTaskCB *runTask = OsCurrTaskGet();

        LOS_ASSERT(runTask->joined == NULL);
        runTask->taskStatus |= OS_TASK_STATUS_PEND;
        runTask->joined = taskCB;
        taskCB->joinner = runTask;
        OsTaskExtStatusSet(runTask, OS_TASK_STATUS_SCHED);
        OsSchedLockGlobal2Local();
        /* Suspend the current task, wait for the joined task.
         * The current task is woken up by the tick mechanism
         * when the joined task exits or deleted by other task. */
        OsSchedResched();
        OsSchedLockLocal2Global();
        runTask->joined = NULL;
        taskCB->joinner = NULL;
    }

    OsTaskWaitCanOperate(taskCB);
    if (retval != NULL) {
        *retval = (UINTPTR)taskCB->taskRetval;
    }
    taskCB->taskStatus = OS_TASK_STATUS_UNUSED;
    OsTaskResourceReclaim(taskCB, intSave);
    OsTaskStackFree(taskCB);

    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskJoin(UINT32 taskId, UINTPTR *retval)
{
    UINT32 ret;
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_TSK_NOT_ALLOW_IN_INT;
    }

    if (!OsPreemptable()) {
        return LOS_ERRNO_TSK_SCHED_LOCKED;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    if (taskCB == OsCurrTaskGet()) {
        return LOS_ERRNO_TSK_NOT_JOIN_SELF;
    }

    if (taskCB->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    SCHEDULER_LOCK(intSave);
    OsTaskWaitCanOperate(taskCB);

    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        ret = LOS_ERRNO_TSK_NOT_CREATED;
        goto LOS_UNLOCK;
    }

    if (OS_TASK_IS_DETACHED(taskCB)) {
        ret = LOS_ERRNO_TSK_IS_DETACHED;
        goto LOS_UNLOCK;
    }

    if (OS_TASK_IS_ALREADY_JOIN(taskCB)) {
        ret =  LOS_ERRNO_TSK_ALREADY_JOIN;
        goto LOS_UNLOCK;
    }

    ret = OsTaskJoinPend(taskCB, retval, &intSave);

LOS_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_TaskDetach(UINT32 taskId)
{
    LosTaskCB *taskCB = NULL;
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    if (OS_TASK_ID_CHECK_INVALID(taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (OS_INT_ACTIVE) {
        return LOS_ERRNO_TSK_NOT_ALLOW_IN_INT;
    }

    taskCB = OS_TCB_FROM_TID(taskId);
    if (taskCB->taskFlags & OS_TASK_FLAG_SYSTEM) {
        return LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK;
    }

    SCHEDULER_LOCK(intSave);
    OsTaskWaitCanOperate(taskCB);
    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        ret = LOS_ERRNO_TSK_NOT_CREATED;
        goto LOS_UNLOCK;
    }

    if (OS_TASK_IS_DETACHED(taskCB)) {
        ret = LOS_ERRNO_TSK_IS_DETACHED;
        goto LOS_UNLOCK;
    }

    if (OS_TASK_IS_ALREADY_JOIN(taskCB)) {
        ret =  LOS_ERRNO_TSK_ALREADY_JOIN;
        goto LOS_UNLOCK;
    }

    if (taskCB->taskStatus & OS_TASK_STATUS_ZOMBIE) {
        ret = OsTaskJoinPend(taskCB, NULL, &intSave);
        goto LOS_UNLOCK;
    }

    taskCB->taskFlags |= OS_TASK_FLAG_DETACHED;

LOS_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return ret;
}
#endif
