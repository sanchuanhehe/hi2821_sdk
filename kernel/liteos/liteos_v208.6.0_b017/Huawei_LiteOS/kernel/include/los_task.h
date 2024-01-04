/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: LiteOS Task Module Implementation HeadFile
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

/**
 * @defgroup los_task Task
 * @ingroup kernel
 */

#ifndef _LOS_TASK_H
#define _LOS_TASK_H

#include "los_base.h"
#include "los_list.h"
#include "los_sys.h"
#include "los_tick.h"
#include "los_event.h"
#include "los_memory.h"
#include "los_err.h"
#include "arch/task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CPUID_TO_AFFI_MASK(cpuid)               (0x1u << (cpuid))

/**
 * @ingroup los_task
 * Highest task priority
 */
#define LOS_TASK_PRIORITY_HIGHEST    0

/**
 * @ingroup los_task
 * Lowest task priority
 */
#define LOS_TASK_PRIORITY_LOWEST     31

/**
 * @ingroup los_task
 * Set the bits of task parameters during task creation.
 */
enum TaskAttrBits {
    LOS_TASK_OS_ALLOC_SECURE = 1 << 0,  /**< Flag that indicates the the security task stack is allocated by the OS.
                                             The setting is valid only when the trust feature of ARMv8-M is enabled. */
    LOS_TASK_STATUS_JOINABLE = 1 << 7,  /**< Flag that indicates the task or task control block status. This is
                                             means the task is in the non-auto-deleted state. In this state,
                                             the task will not be deleted automatically after the task is done. */
    LOS_TASK_STATUS_DETACHED = 1 << 8,  /**< Flag that indicates the task or task control block status. This is
                                             means the task is in the auto-deleted state. In this state,
                                             the task will be deleted automatically after the task is done. */
};

/**
 * @ingroup los_task
 * Task error code: Insufficient memory for task creation.
 *
 * Value: 0x03000200.
 *
 * Solution: Allocate bigger memory partition to task creation.
 */
#define LOS_ERRNO_TSK_NO_MEMORY                 LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x00)

/**
 * @ingroup los_task
 * Task error code: Null parameter.
 *
 * Value: 0x02000201.
 *
 * Solution: Check the parameter.
 */
#define LOS_ERRNO_TSK_PTR_NULL                  LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x01)

/**
 * @ingroup los_task
 * Task error code: The task stack is not aligned.
 *
 * Value: 0x02000202.
 *
 * Solution: Align the task stack.
 */
#define LOS_ERRNO_TSK_STKSZ_NOT_ALIGN           LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x02)

/**
 * @ingroup los_task
 * Task error code: Incorrect task priority.
 *
 * Value: 0x02000203.
 *
 * Solution: Re-configure the task priority by referring to the priority range.
 */
#define LOS_ERRNO_TSK_PRIOR_ERROR               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x03)

/**
 * @ingroup los_task
 * Task error code: The task entrance is NULL.
 *
 * Value: 0x02000204.
 *
 * Solution: Define the task entrance function.
 */
#define LOS_ERRNO_TSK_ENTRY_NULL                LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x04)

/**
 * @ingroup los_task
 * Task error code: The task name is NULL.
 *
 * Value: 0x02000205.
 *
 * Solution: Set the task name.
 */
#define LOS_ERRNO_TSK_NAME_EMPTY                LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x05)

/**
 * @ingroup los_task
 * Task error code: The task stack size is too small.
 *
 * Value: 0x02000206.
 *
 * Solution: Expand the task stack.
 */
#define LOS_ERRNO_TSK_STKSZ_TOO_SMALL           LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x06)

/**
 * @ingroup los_task
 * Task error code: Invalid task ID.
 *
 * Value: 0x02000207.
 *
 * Solution: Check the task ID.
 */
#define LOS_ERRNO_TSK_ID_INVALID                LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x07)

/**
 * @ingroup los_task
 * Task error code: The task is already suspended.
 *
 * Value: 0x02000208.
 *
 * Solution: Suspend the task after it is resumed.
 */
#define LOS_ERRNO_TSK_ALREADY_SUSPENDED         LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x08)

/**
 * @ingroup los_task
 * Task error code: The task is not suspended.
 *
 * Value: 0x02000209.
 *
 * Solution: Suspend the task.
 */
#define LOS_ERRNO_TSK_NOT_SUSPENDED             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x09)

/**
 * @ingroup los_task
 * Task error code: The task is not created.
 *
 * Value: 0x0200020a.
 *
 * Solution: Create the task.
 */
#define LOS_ERRNO_TSK_NOT_CREATED               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0a)

/**
 * @ingroup los_task
 * Task error code: The task is locked when it is being deleted.
 *
 * Value: 0x0300020b.
 *
 * Solution: Unlock the task.
 */
#define LOS_ERRNO_TSK_DELETE_LOCKED             LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x0b)

/**
 * @ingroup los_task
 * Task error code: The task message is nonzero.
 *
 * Value: 0x0200020c.
 *
 * Solution: This error code is not in use temporarily.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_MSG_NONZERO               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0c)

/**
 * @ingroup los_task
 * Task error code: The task delay occurs during an interrupt.
 *
 * Value: 0x0300020d.
 *
 * Solution: Perform this operation after exiting from the interrupt.
 */
#define LOS_ERRNO_TSK_DELAY_IN_INT              LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x0d)

/**
 * @ingroup los_task
 * Task error code: The task delay occurs when the task is locked.
 *
 * Value: 0x0200020e.
 *
 * Solution: Perform this operation after unlocking the task.
 */
#define LOS_ERRNO_TSK_DELAY_IN_LOCK             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0e)

/**
 * @ingroup los_task
 * Task error code: The task yield occurs when the task is locked.
 *
 * Value: 0x0200020f.
 *
 * Solution: Check the task.
 */
#define LOS_ERRNO_TSK_YIELD_IN_LOCK             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x0f)

/**
 * @ingroup los_task
 * Task error code: Only one task or no task is available for scheduling.
 *
 * Value: 0x02000210.
 *
 * Solution: Increase the number of tasks.
 */
#define LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK     LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x10)

/**
 * @ingroup los_task
 * Task error code: No free task control block is available.
 *
 * Value: 0x02000211.
 *
 * Solution: Increase the number of task control blocks.
 */
#define LOS_ERRNO_TSK_TCB_UNAVAILABLE           LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x11)

/**
 * @ingroup los_task
 * Task error code: The task hook function is not matchable.
 *
 * Value: 0x02000212.
 *
 * Solution: This error code is not in use temporarily.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_HOOK_NOT_MATCH            LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x12)

/**
 * @ingroup los_task
 * Task error code: The number of task hook functions exceeds the permitted upper limit.
 *
 * Value: 0x02000213.
 *
 * Solution: This error code is not in use temporarily.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_HOOK_IS_FULL              LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x13)

/**
 * @ingroup los_task
 * Task error code: The operation is performed on the system-level task.
 *       old usage: The operation is performed on the idle task (LOS_ERRNO_TSK_OPERATE_IDLE)
 *
 * Value: 0x02000214.
 *
 * Solution: Check the task ID and do not operate the system-level task.
 */
#define LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK       LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x14)

/**
 * @ingroup los_task
 * Task error code: The task that is being suspended is locked.
 *
 * Value: 0x03000215.
 *
 * Solution: Suspend the task after unlocking the task.
 */
#define LOS_ERRNO_TSK_SUSPEND_LOCKED            LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x15)

/**
 * @ingroup los_task
 * Task error code: The task stack fails to be freed.
 *
 * Value: 0x02000217
 *
 * Solution: This error code is not in use temporarily.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_FREE_STACK_FAILED         LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x17)

/**
 * @ingroup los_task
 * Task error code: The task stack area is too small.
 *
 * Value: 0x02000218
 *
 * Solution: This error code is not in use temporarily.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_STKAREA_TOO_SMALL         LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x18)

/**
 * @ingroup los_task
 * Task error code: The task fails to be activated.
 *
 * Value: 0x03000219.
 *
 * Solution: Perform task switching after creating an idle task.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_ACTIVE_FAILED             LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x19)

/**
 * @ingroup los_task
 * Task error code: Too many task configuration items.
 *
 * Value: 0x0200021a
 *
 * Solution: This error code is not in use temporarily.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_CONFIG_TOO_MANY           LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1a)

/**
 * @ingroup los_task
 * Task error code: This error code is not in use temporarily.
 *
 * Value: 0x0200021b
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_CP_SAVE_AREA_NOT_ALIGN    LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1b)

/**
 * @ingroup los_task
 * Task error code: This error code is not in use temporarily.
 *
 * Value: 0x0200021d
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_MSG_Q_TOO_MANY            LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1d)

/**
 * @ingroup los_task
 * Task error code: This error code is not in use temporarily.
 *
 * Value: 0x0200021e
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_CP_SAVE_AREA_NULL         LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1e)

/**
 * @ingroup los_task
 * Task error code: This error code is not in use temporarily.
 *
 * Value: 0x0200021f
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_SELF_DELETE_ERR           LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x1f)

/**
 * @ingroup los_task
 * Task error code: The task stack size is too large.
 *
 * Value: 0x02000220.
 *
 * Solution: shrink the task stack size parameter.
 */
#define LOS_ERRNO_TSK_STKSZ_TOO_LARGE           LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x20)

/**
 * @ingroup los_task
 * Task error code: Suspending software timer task is not allowed.
 *
 * Value: 0x02000221.
 *
 * Solution: Check the task ID and do not suspend software timer task.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_TSK_SUSPEND_SWTMR_NOT_ALLOWED LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x21)

/**
 * @ingroup los_task
 * Task error code: The cpu affinity mask is incorrect.
 *
 * Value: 0x03000223.
 *
 * Solution: Please set the correct cpu affinity mask.
 */
#define LOS_ERRNO_TSK_CPU_AFFINITY_MASK_ERR     LOS_ERRNO_OS_FATAL(LOS_MOD_TSK, 0x23)

/**
 * @ingroup los_task
 * Task error code: Task yield in interrupt is not permitted, which will result in an unexpected result.
 *
 * Value: 0x02000224.
 *
 * Solution: Don't call LOS_TaskYield in Interrupt.
 */
#define LOS_ERRNO_TSK_YIELD_IN_INT              LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x24)

/**
 * @ingroup los_task
 * Task error code: Task sync resource (semaphore) allocated failed.
 *
 * Value: 0x02000225.
 *
 * Solution: Expand LOSCFG_BASE_IPC_SEM_LIMIT.
 */
#define LOS_ERRNO_TSK_MP_SYNC_RESOURCE          LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x25)

/**
 * @ingroup los_task
 * Task error code: Task sync failed on operating running task across cores.
 *
 * Value: 0x02000226.
 *
 * Solution: Check task delete can be handled in user's scenario.
 */
#define LOS_ERRNO_TSK_MP_SYNC_FAILED            LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x26)

/**
 * @ingroup los_task
 * Task error code: Apply for the security task stack in interrupt.
 *
 * Value: 0x02000227.
 *
 * Solution: Don't apply for the security task stack in interrupt.
 */
#define LOS_ERRNO_TSK_ALLOC_SECURE_INT          LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x27)

/**
 * @ingroup los_task
 * Task error code: The security task stack is already allocated.
 *
 * Value: 0x02000228.
 *
 * Solution: Don't reapply.
 */
#define LOS_ERRNO_TSK_SECURE_ALREADY_ALLOC      LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x28)

/**
 * @ingroup los_task
 * Task error code: Failed to apply for the security task stack.
 *
 * Value: 0x02000229.
 *
 * Solution: Check the usage scenario and the size of the secure memory pool.
 */
#define LOS_ERRNO_TSK_ALLOC_SECURE_FAILED       LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x29)

/**
 * @ingroup los_task
 * Task error code: Failed to free the security task stack.
 *
 * Value: 0x0200022a.
 *
 * Solution: Check the usage scenario and parameter.
 */
#define LOS_ERRNO_TSK_FREE_SECURE_FAILED        LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x2a)

/**
 * @ingroup los_task
 * Task error code: This task operation is not allowed to be performed in an interrupt.
 *
 * Value: 0x0200022b.
 *
 * Solution:  Check whether the interface is used in interrupts.
 */
#define LOS_ERRNO_TSK_NOT_ALLOW_IN_INT          LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x2b)

/**
 * @ingroup los_task
 * Task error code: Locked scheduling does not allow tasks to be blocked.
 *
 * Value: 0x0200022c.
 *
 * Solution: Check for faulty lock scheduling logic.
 */
#define LOS_ERRNO_TSK_SCHED_LOCKED              LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x2c)

/**
 * @ingroup los_task
 * Task error code: A task cannot be waiting for multiple tasks.
 *
 * Value: 0x0200022d.
 *
 * Solution: Check whether other tasks are waiting for this task.
 */
#define LOS_ERRNO_TSK_ALREADY_JOIN              LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x2d)

/**
 * @ingroup los_task
 * Task error code: This task is detached attr.
 *
 * Value: 0x0200022e.
 *
 * Solution: Check the task properties and whether it is waiting for other tasks to finish.
 */
#define LOS_ERRNO_TSK_IS_DETACHED               LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x2e)

/**
 * @ingroup los_task
 * Task error code: Tasks can't join himself.
 *
 * Value: 0x0200022f.
 *
 * Solution: Check whether the task ID is the current running task.
 */
#define LOS_ERRNO_TSK_NOT_JOIN_SELF             LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x2f)

/**
 * @ingroup los_task
 * Task error code: An "zombie" task cannot be operated.
 *
 * Value: 0x02000230.
 *
 * Solution: Check whether a Joinable task exists. If so, call LOS_TaskJoin to reclaim resources.
 */
#define LOS_ERRNO_TSK_IS_ZOMBIE                 LOS_ERRNO_OS_ERROR(LOS_MOD_TSK, 0x30)

/**
 * @ingroup los_task
 * Minimum stack size.
 *
 * LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE bytes, configured in menuconfig.
 * LOS_TASK_MIN_STACK_SIZE bytes, aligned on a boundary of LOSCFG_STACK_POINT_ALIGN_SIZE.
 */
#define LOS_TASK_MIN_STACK_SIZE (LOS_Align(KERNEL_TSK_MIN_STACK_SIZE, LOSCFG_STACK_POINT_ALIGN_SIZE))

#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
/**
 * @ingroup los_task
 * @brief Define the type of the task switching hook function.
 *
 * @par Description:
 * This API is used to define the type of the task switching hook function.
 * @attention The function type is defined only when #LOSCFG_BASE_CORE_TSK_MONITOR is set to YES.
 *
 * @param  oldTaskId [IN] Type #UINT32 The Id of task which is switched out.
 * @param  newTaskId [IN] Type #UINT32 The Id of task which will switch in.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V100R001C00
 */
typedef VOID (*TSKSWITCHHOOK)(UINT32 oldTaskId, UINT32 newTaskId);

/**
 * @ingroup los_task
 * @brief User task switching hook function.
 *
 * @par Description:
 * This API is a user task switching hook register function.
 * @attention The function is used only when #LOSCFG_BASE_CORE_TSK_MONITOR is set to YES.
 *
 * @param  hook [IN] Type #TSKSWITCHHOOK. The user defined hook for task switch.
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R005C10
 */
extern VOID LOS_TaskSwitchHookReg(TSKSWITCHHOOK hook);
#endif

/**
 * @ingroup  los_task
 * @brief Define the type of a task entrance function.
 *
 * @par Description:
 * This API is used to define the type of a task entrance function and call it after a task is created and triggered.
 * @attention If LOSCFG_OBSOLETE_API is not defined, one parameter which its type is VOID * will be instead of these
 *            four parameters of the API.
 *
 * @param  param1 [IN] Type #UINTPTR The first parameter passed to the task handling function.
 * @param  param2 [IN] Type #UINTPTR The second parameter passed to the task handling function.
 * @param  param3 [IN] Type #UINTPTR The third parameter passed to the task handling function.
 * @param  param4 [IN] Type #UINTPTR The fourth parameter passed to the task handling function.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V100R001C00
 */
#ifdef LOSCFG_OBSOLETE_API
typedef VOID *(*TSK_ENTRY_FUNC)(UINTPTR param1,
                                UINTPTR param2,
                                UINTPTR param3,
                                UINTPTR param4);
#else
typedef VOID *(*TSK_ENTRY_FUNC)(VOID *param);
#endif
/**
 * @ingroup los_task
 * Define the structure of the parameters used for task creation.
 *
 * Information of specified parameters passed in during task creation.
 */
typedef struct tagTskInitParam {
    TSK_ENTRY_FUNC  pfnTaskEntry;  /**< Task entrance function */
    UINT16          usTaskPrio;    /**< Task priority */
#ifdef LOSCFG_OBSOLETE_API
    UINTPTR         auwArgs[4];    /**< Task parameters, of which the maximum number is four.
                                        If LOSCFG_OBSOLETE_API is not defined, auwArgs[4] will
                                        be replaced by pArgs which its type is void *.  */
#else
    VOID            *pArgs;        /**< Task Parameter, of which the type is void * */
#endif
    UINT32          uwStackSize;   /**< Task stack size */
    CHAR            *pcName;       /**< Task name */
#ifdef LOSCFG_KERNEL_SMP
    UINT16          usCpuAffiMask; /**< Task cpu affinity mask. It is defined only when LOSCFG_KERNEL_SMP is defined. */
#endif
    UINT32          uwResved;      /**< bit[0]: If this bit is set to 1, indicates that this task will access the
                                                secure world. This bit is valid only when the trustzone feature of
                                                ARMv8-M is enabled, #LOS_TASK_OS_ALLOC_SECURE could be used to set it.
                                        bit[7]: If this bit is set to 1, indicates that this task is not automatically
                                                deleted, otherwise task will be deleted automatically,
                                                #LOS_TASK_STATUS_JOINABLE could be used to set it.
                                        bit[8]: If this bit is set to 1,
                                                indicates that this task is automatically deleted,
                                                #LOS_TASK_STATUS_DETACHED could be used to set it.
                                        other bits are reserved. */
} TSK_INIT_PARAM_S;

/**
 * @ingroup los_task
 * Task name length
 *
 */
#define LOS_TASK_NAMELEN                        32

/**
 * @ingroup los_task
 * Task information structure.
 *
 */
typedef struct tagTskInfo {
    CHAR                acName[LOS_TASK_NAMELEN];   /**< Task name, the default value of
	                                                     #LOS_TASK_NAMELEN is 32  */
    UINT32              uwTaskID;                   /**< Task ID                  */
    UINT16              usTaskStatus;               /**< Task status              */
    UINT16              usTaskPrio;                 /**< Task priority            */
    VOID                *pTaskSem;                  /**< Semaphore pointer        */
    VOID                *pTaskMux;                  /**< Mutex pointer            */
    EVENT_CB_S          uwEvent;                    /**< Event                    */
    UINT32              uwEventMask;                /**< Event mask               */
    UINT32              uwStackSize;                /**< Task stack size          */
    UINTPTR             uwTopOfStack;               /**< Task stack top           */
    UINTPTR             uwBottomOfStack;            /**< Task stack bottom        */
    UINTPTR             uwSP;                       /**< Task SP pointer          */
    UINT32              uwCurrUsed;                 /**< Current task stack usage */
    UINT32              uwPeakUsed;                 /**< Task stack usage peak    */
    BOOL                bOvf;                       /**< Flag that indicates whether a task stack overflow
	                                                     occurs or not            */
} TSK_INFO_S;

#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
/**
 * @ingroup  los_task
 * @brief Create a task with User defines stack space and suspend .
 *
 * @par Description:
 * This API is used to create a task and suspend it. This task will not be added to the queue of ready tasks before
 * resume it.
 * User should allocate memory for task's stack and assign its addr to para topStack, the uwStackSize in taskInitParam
 * must fit the stack memory size. When the task is deleted, the stack's memory should be free in time by users.
 *
 * @attention
 * <ul>
 * <li>The task name is a pointer and is not allocated memory.</li>
 * <li>If the size of the task stack of the task to be created is 0, configure #LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
 * to specify the default task stack size. The stack size should be a reasonable value, if the size is too large, may
 * cause memory exhaustion.</li>
 * <li>The task stack size must be aligned on the boundary of 8 bytes. The size is determined by whether it is big
 * enough to avoid task stack overflow.</li>
 * <li>Less parameter value indicates higher task priority.</li>
 * <li>The task name cannot be null.</li>
 * <li>The pointer to the task executing function cannot be null.</li>
 * <li>The two parameters of this interface is pointer, it should be a correct value, otherwise, the system may be
 * abnormal.</li>
 * <li>If user mode is enabled, user should input user stack pointer and size, the size must fit the stack pointer,
 * uwStackSize remain as the kernel stack size.</li>
 * <li>This function is defined only when LOSCFG_TASK_STACK_STATIC_ALLOCATION is defined.</li>
 * </ul>
 *
 * @param  taskId        [OUT] Type  #UINT32 * Task Id.
 * @param  initParam     [IN]  Type  #TSK_INIT_PARAM_S * Parameter for task creation.
 * @param  topStack      [IN]  Type  #VOID* Parameter for task's top of stack address.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID            Invalid Task ID, param puwTaskID is NULL.
 * @retval #LOS_ERRNO_TSK_PTR_NULL              Param pstInitParam is NULL.
 * @retval #LOS_ERRNO_TSK_NAME_EMPTY            The task name is NULL.
 * @retval #LOS_ERRNO_TSK_ENTRY_NULL            The task entrance is NULL.
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR           Incorrect task priority.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_LARGE       The task stack size is too large.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_SMALL       The task stack size is too small.
 * @retval #LOS_ERRNO_TSK_TCB_UNAVAILABLE       No free task control block is available.
 * @retval #LOS_ERRNO_TSK_NO_MEMORY             Insufficient memory for task creation.
 * @retval #LOS_ERRNO_TSK_ALLOC_SECURE_FAILED   Failed to apply for the security task stack.
 * @retval #LOS_OK                              The task is successfully created.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * <ul><li>los_config.h: the header file that contains system configuration items.</li></ul>
 * @see LOS_TaskCreateStatic | LOS_TaskDeleteStatic
 * @since Huawei LiteOS V200R005C10
 */
extern UINT32 LOS_TaskCreateOnlyStatic(UINT32 *taskId, TSK_INIT_PARAM_S *initParam, VOID *topStack);

/**
 * @ingroup  los_task
 * @brief Create a task.
 *
 * @par Description:
 * This API is used to create a task. If the priority of the task created after system initialized is higher than
 * the current task and task scheduling is not locked, it is scheduled for running.
 * User should allocate memory for task's stack and assign its addr to para topStack, the uwStackSize in taskInitParam
 * must fit the stack memory size. When the task is deleted, the stack's memory should be free in time by users.
 * If not, the created task is added to the queue of ready tasks.
 *
 * @attention
 * <ul>
 * <li>The task name is a pointer and is not allocated memory.</li>
 * <li>If the size of the task stack of the task to be created is 0, configure #LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
 * to specify the default task stack size.</li>
 * <li>The task stack size must be aligned on the boundary of 8 bytes. The size is determined by whether it is big
 * enough to avoid task stack overflow.</li>
 * <li>Less parameter value indicates higher task priority.</li>
 * <li>The task name cannot be null.</li>
 * <li>The pointer to the task executing function cannot be null.</li>
 * <li>The two parameters of this interface is pointer, it should be a correct value, otherwise, the system may be
 * abnormal.</li>
 * <li>If user mode is enabled, user should input user stack pointer and size, the size must fit the stack pointer,
 * uwStackSize remain as the kernel stack size.</li>
 * <li>If LOSCFG_STATIC_ALLOC_MEM is enabled, user should define a static stack memory and assign to stack pointer,
 * the uwStackSize must fit the stack memory size.</li>
 * <li>This function is defined only when LOSCFG_TASK_STACK_STATIC_ALLOCATION is defined.</li>
 * </ul>
 *
 * @param  taskId        [OUT] Type  #UINT32 * Task Id.
 * @param  initParam     [IN]  Type  #TSK_INIT_PARAM_S * Parameter for task creation.
 * @param  topStack      [IN]  Type  #VOID * Parameter for task's top of stack address.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID            Invalid Task ID, param puwTaskID is NULL.
 * @retval #LOS_ERRNO_TSK_PTR_NULL              Param pstInitParam is NULL.
 * @retval #LOS_ERRNO_TSK_NAME_EMPTY            The task name is NULL.
 * @retval #LOS_ERRNO_TSK_ENTRY_NULL            The task entrance is NULL.
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR           Incorrect task priority.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_LARGE       The task stack size is too large.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_SMALL       The task stack size is too small.
 * @retval #LOS_ERRNO_TSK_TCB_UNAVAILABLE       No free task control block is available.
 * @retval #LOS_ERRNO_TSK_NO_MEMORY             Insufficient memory for task creation.
 * @retval #LOS_ERRNO_TSK_ALLOC_SECURE_FAILED   Failed to apply for the security task stack.
 * @retval #LOS_OK                              The task is successfully created.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * <ul><li>los_config.h: the header file that contains system configuration items.</li></ul>
 * @see LOS_TaskCreateOnlyStatic | LOS_TaskDeleteStatic
 * @since Huawei LiteOS V200R005C10
 */
extern UINT32 LOS_TaskCreateStatic(UINT32 *taskId, TSK_INIT_PARAM_S *initParam, VOID *topStack);
#endif

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
/**
 * @ingroup  los_task
 * @brief Create a task and suspend.
 *
 * @par Description:
 * This API is used to create a task and suspend it. This task will not be added to the queue of ready tasks
 * before resume it.
 *
 * @attention
 * <ul>
 * <li>During task creation, the task control block and task stack of the task that is previously automatically deleted
 * are deallocated.</li>
 * <li>The task name is a pointer and is not allocated memory.</li>
 * <li>If the size of the task stack of the task to be created is 0, configure #LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
 * to specify the default task stack size. The stack size should be a reasonable value, if the size is too large, may
 * cause memory exhaustion.</li>
 * <li>The task stack size must be aligned on the boundary of 8 bytes. The size is determined by whether it is big
 * enough to avoid task stack overflow.</li>
 * <li>Less parameter value indicates higher task priority.</li>
 * <li>The task name cannot be null.</li>
 * <li>The pointer to the task executing function cannot be null.</li>
 * <li>The two parameters of this interface is pointer, it should be a correct value, otherwise, the system may be
 * abnormal.</li>
 * <li>This function is defined only when LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION is defined.</li>
 * </ul>
 *
 * @param  taskId    [OUT] Type  #UINT32 * Task ID.
 * @param  initParam [IN]  Type  #TSK_INIT_PARAM_S * Parameter for task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID            Invalid Task ID, param taskId is NULL.
 * @retval #LOS_ERRNO_TSK_PTR_NULL              Param initParam is NULL.
 * @retval #LOS_ERRNO_TSK_NAME_EMPTY            The task name is NULL.
 * @retval #LOS_ERRNO_TSK_ENTRY_NULL            The task entrance is NULL.
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR           Incorrect task priority.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_LARGE       The task stack size is too large.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_SMALL       The task stack size is too small.
 * @retval #LOS_ERRNO_TSK_TCB_UNAVAILABLE       No free task control block is available.
 * @retval #LOS_ERRNO_TSK_NO_MEMORY             Insufficient memory for task creation.
 * @retval #LOS_ERRNO_TSK_ALLOC_SECURE_FAILED   Failed to apply for the security task stack.
 * @retval #LOS_ERRNO_TSK_IS_ZOMBIE             The task is in a "zombie" state.
 * @retval #LOS_OK                              The task is successfully created.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * <ul><li>los_config.h: the header file that contains system configuration items.</li></ul>
 * @see LOS_TaskDelete | LOS_TaskCreate
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskCreateOnly(UINT32 *taskId, TSK_INIT_PARAM_S *initParam);

/**
 * @ingroup  los_task
 * @brief Create a task.
 *
 * @par Description:
 * This API is used to create a task. If the priority of the task created after system initialized is higher than
 * the current task and task scheduling is not locked, it is scheduled for running.
 * If not, the created task is added to the queue of ready tasks.
 *
 * @attention
 * <ul>
 * <li>During task creation, the task control block and task stack of the task that is previously automatically
 * deleted are deallocated.</li>
 * <li>The task name is a pointer and is not allocated memory.</li>
 * <li>If the size of the task stack of the task to be created is 0, configure #LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
 * to specify the default task stack size.</li>
 * <li>The task stack size must be aligned on the boundary of 8 bytes. The size is determined by whether it is big
 * enough to avoid task stack overflow.</li>
 * <li>Less parameter value indicates higher task priority.</li>
 * <li>The task name cannot be null.</li>
 * <li>The pointer to the task executing function cannot be null.</li>
 * <li>The two parameters of this interface is pointer, it should be a correct value, otherwise, the system may be
 * abnormal.</li>
 * <li>This function is defined only when LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION is defined.</li>
 * </ul>
 *
 * @param  taskId    [OUT] Type  #UINT32 * Task ID.
 * @param  initParam [IN]  Type  #TSK_INIT_PARAM_S * Parameter for task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID            Invalid Task ID, param taskId is NULL.
 * @retval #LOS_ERRNO_TSK_PTR_NULL              Param initParam is NULL.
 * @retval #LOS_ERRNO_TSK_NAME_EMPTY            The task name is NULL.
 * @retval #LOS_ERRNO_TSK_ENTRY_NULL            The task entrance is NULL.
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR           Incorrect task priority.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_LARGE       The task stack size is too large.
 * @retval #LOS_ERRNO_TSK_STKSZ_TOO_SMALL       The task stack size is too small.
 * @retval #LOS_ERRNO_TSK_TCB_UNAVAILABLE       No free task control block is available.
 * @retval #LOS_ERRNO_TSK_NO_MEMORY             Insufficient memory for task creation.
 * @retval #LOS_ERRNO_TSK_ALLOC_SECURE_FAILED   Failed to apply for the security task stack.
 * @retval #LOS_OK                              The task is successfully created.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * <ul><li>los_config.h: the header file that contains system configuration items.</li></ul>
 * @see LOS_TaskDelete | LOS_TaskCreateOnly
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskCreate(UINT32 *taskId, TSK_INIT_PARAM_S *initParam);
#endif

/**
 * @ingroup  los_task
 * @brief Resume a task.
 *
 * @par Description:
 * This API is used to resume a suspended task.
 *
 * @attention
 * <ul>
 * <li>If the task is delayed or blocked, resume the task without adding it to the queue of ready tasks.</li>
 * <li>If the priority of the task resumed after system initialized is higher than the current task and task scheduling
 * is not locked, it is scheduled for running.</li>
 * </ul>
 *
 * @param  taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID        Invalid Task ID
 * @retval #LOS_ERRNO_TSK_NOT_CREATED       The task is not created.
 * @retval #LOS_ERRNO_TSK_NOT_SUSPENDED     The task is not suspended.
 * @retval #LOS_ERRNO_TSK_IS_ZOMBIE         The task is in a "zombie" state.
 * @retval #LOS_OK                          The task is successfully resumed.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskSuspend
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskResume(UINT32 taskId);

/**
 * @ingroup  los_task
 * @brief Suspend a task.
 *
 * @par Description:
 * This API is used to suspend a specified task, and the task will be removed from the queue of ready tasks.
 *
 * @attention
 * <ul>
 * <li>The task that is running and locked cannot be suspended.</li>
 * <li>The idle task and swtmr task cannot be suspended.</li>
 * </ul>
 *
 * @param  taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                    Invalid Task ID
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK           Check the task ID and do not operate the system-level
 *                                                      task, like idle or swtmr task.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED                   The task is not created.
 * @retval #LOS_ERRNO_TSK_ALREADY_SUSPENDED             The task is already suspended.
 * @retval #LOS_ERRNO_TSK_SUSPEND_LOCKED                The task being suspended is current task and task
 *                                                      scheduling is locked.
 * @retval #LOS_ERRNO_TSK_IS_ZOMBIE                     The task is in a "zombie" state.
 * @retval #LOS_OK                                      The task is successfully suspended.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskResume
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskSuspend(UINT32 taskId);

/**
 * @ingroup  los_task
 * @brief Delete a task.
 *
 * @par Description:
 * This API is used to delete a specified task and release the resources for its task stack and task control block.
 *
 * @attention
 * <ul>
 * <li>The idle task and swtmr task cannot be deleted.</li>
 * <li>If delete current task maybe cause unexpected error.</li>
 * <li>If a task get a mutex is deleted or automatically deleted before release this mutex, other tasks pended
 * this mutex maybe never be scheduled.</li>
 * </ul>
 *
 * @param  taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                    Invalid Task ID
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK           Check the task ID and do not operate the system-level
 *                                                      task, like idle or swtmr task.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED                   The task is not created.
 * @retval #LOS_ERRNO_TSK_DELETE_LOCKED                 The task being deleted is current task and task scheduling
 *                                                      is locked.
 * @retval #LOS_ERRNO_TSK_FREE_SECURE_FAILED            Failed to free the security task stack.
 * @retval #LOS_OK                                      The task is successfully deleted.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskCreate | LOS_TaskCreateOnly
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskDelete(UINT32 taskId);

/**
 * @ingroup  los_task
 * @brief Delay a task.
 *
 * @par Description:
 * This API is used to delay the execution of the current task. The task is able to be scheduled after it is delayed
 * for a specified number of Ticks.
 *
 * @attention
 * <ul>
 * <li>The task fails to be delayed if it is being delayed during interrupt processing or it is locked.</li>
 * <li>If 0 is passed in and the task scheduling is not locked, execute the next task in the queue of tasks with
 * the same priority of the current task.
 * If no ready task with the priority of the current task is available, the task scheduling will not occur, and the
 * current task continues to be executed.</li>
 * <li>Using the interface before system initialized is not allowed.</li>
 * <li>DO NOT call this API in software timer callback. </li>
 * </ul>
 *
 * @param  tick [IN] Type #UINT32 Number of Ticks for which the task is delayed.
 *
 * @retval #LOS_ERRNO_TSK_DELAY_IN_INT              The task delay occurs during an interrupt.
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK       The current task is a system-level task, like idle or swtmr
 *                                                  task. This is not allowed.
 * @retval #LOS_ERRNO_TSK_DELAY_IN_LOCK             The task delay occurs when the task scheduling is locked.
 * @retval #LOS_ERRNO_TSK_ID_INVALID                Invalid Task ID
 * @retval #LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK     No tasks with the same priority is available for scheduling.
 * @retval #LOS_OK                                  The task is successfully delayed.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskDelay(UINT32 tick);

/**
 * @ingroup  los_task
 * @brief To check if the current running core is scheduled.
 *
 * @par Description:
 * This API is used to check if the current running core is scheduled.
 *
 * @attention
 * <ul>
 * <li>If the core is scheduled, some API can be used, like LOS_TaskDelay, which maybe cause task switching.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval True                      The current running core is scheduled.
 * @retval False                     The current running core is start-up, not int multi-task environment.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskUnlock LOS_TaskLock
 * @since Huawei LiteOS 206.1.0
 */
extern BOOL LOS_TaskIsScheduled(VOID);

/**
 * @ingroup  los_task
 * @brief Lock the task scheduling.
 *
 * @par Description:
 * This API is used to lock the task scheduling. Task switching will not occur if the task scheduling is locked.
 *
 * @attention
 * <ul>
 * <li>If the task scheduling is locked, but interrupts are not disabled, tasks are still able to be interrupted.</li>
 * <li>One is added to the number of task scheduling locks if this API is called. The number of locks is decreased by
 * one if the task scheduling is unlocked. Therefore, this API should be used together with LOS_TaskUnlock or
 * LOS_TaskUnlockNoSched.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskUnlock
 * @since Huawei LiteOS V100R001C00
 */
extern VOID LOS_TaskLock(VOID);

/**
 * @ingroup  los_task
 * @brief Unlock the task scheduling.
 *
 * @par Description:
 * This API is used to unlock the task scheduling. Calling this API will decrease the number of task locks by one.
 * If a task is locked more than once, the task scheduling will be unlocked only when the number of locks becomes zero.
 *
 * @attention
 * <ul>
 * <li>The number of locks is decreased by one if this API is called, and the number of locks increases by one only
 * in LOS_TaskLock. Therefore, this API should be used together with LOS_TaskLock.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskLock
 * @since Huawei LiteOS V100R001C00
 */
extern VOID LOS_TaskUnlock(VOID);

/**
 * @ingroup  los_task
 * @brief Unlock the task scheduling and no scheduling is generated.
 *
 * @par Description:
 * This API is used to unlock the task scheduling. Calling this API will decrease the number of task locks by one,
 * and then unlocked the task scheduling.
 *
 * @attention
 * <ul>
 * <li>The number of locks is decreased by one if this API is called, and the number of locks increases by one only
 * in LOS_TaskLock. Therefore, this API should be used together with LOS_TaskLock.</li>
 * <li>This function is used only by LOS_SpinUnlockNoSched. For details, see the comments of
 * LOS_SpinUnlockNoSched.</li>
 * </ul>
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskLock
 * @since Huawei LiteOS 207.0.0
 */
extern VOID LOS_TaskUnlockNoSched(VOID);

/**
 * @ingroup  los_task
 * @brief Set a task priority.
 *
 * @par Description:
 * This API is used to set the priority of a specified task.
 *
 * @attention
 * <ul>
 * <li>If the set priority is higher than the priority of the current running task, task scheduling
 * probably occurs.</li>
 * <li>Changing the priority of the current running task also probably causes task scheduling.</li>
 * <li>Using the interface to change the priority of software timer task and idle task is not allowed.</li>
 * <li>Using the interface in the interrupt is not allowed.</li>
 * <li>Before the system starts, it is not allowed to call, otherwise the interface behavior is undefined.</li>
 * </ul>
 *
 * @param  taskId   [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 * @param  taskPrio [IN] Type #UINT16 Task priority.
 *
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR          Incorrect task priority.Re-configure the task priority
 * @retval #LOS_ERRNO_TSK_ID_INVALID           Invalid Task ID
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK  Check the task ID and do not operate the system-level
 *                                             task, like idle or swtmr task.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED          The task is not created.
 * @retval #LOS_ERRNO_TSK_IS_ZOMBIE            The task is in a "zombie" state.
 * @retval #LOS_OK                             The priority of the current running task is successfully
 *                                             set to a specified priority.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskPriGet
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskPriSet(UINT32 taskId, UINT16 taskPrio);

/**
 * @ingroup  los_task
 * @brief Set the priority of the current running task to a specified priority.
 *
 * @par Description:
 * This API is used to set the priority of the current running task to a specified priority.
 *
 * @attention
 * <ul>
 * <li>Changing the priority of the current running task probably causes task scheduling.</li>
 * <li>Using the interface to change the priority of software timer task and idle task is not allowed.</li>
 * <li>Using the interface in the interrupt is not allowed.</li>
 * </ul>
 *
 * @param  taskPrio [IN] Type #UINT16 Task priority.
 *
 * @retval #LOS_ERRNO_TSK_PRIOR_ERROR          Incorrect task priority.Re-configure the task priority.
 * @retval #LOS_ERRNO_TSK_ID_INVALID           The current task ID is invalid.
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK  The current task is a system-level task, like idle or swtmr
 *                                             task. This is not allowed.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED          The task is not created.
 * @retval #LOS_OK                             The priority of the current running task is successfully set
 *                                             to a specified priority.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskPriSet | LOS_TaskPriGet
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_CurTaskPriSet(UINT16 taskPrio);

/**
 * @ingroup  los_task
 * @brief Change the scheduling sequence of tasks with the same priority.
 *
 * @par Description:
 * This API is used to move current task in a queue of tasks with the same priority to the tail of the queue of ready
 * tasks.
 *
 * @attention
 * At least two ready tasks need to be included in the queue of ready tasks with the same priority. If the
 * less than two ready tasks are included in the queue, an error is reported.
 *
 * @param  None.
 *
 * @retval #LOS_ERRNO_TSK_YIELD_IN_INT                  The task yield occurs during an interrupt.
 * @retval #LOS_ERRNO_TSK_YIELD_IN_LOCK                 The task yield occurs when the task is locked.
 * @retval #LOS_ERRNO_TSK_ID_INVALID                    The current task ID is invalid.
 * @retval #LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK         No tasks with the same priority is available for scheduling.
 * @retval #LOS_OK                                      The scheduling sequence of tasks with same priority is
 *                                                      successfully changed.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskYield(VOID);

/**
 * @ingroup  los_task
 * @brief Obtain a task priority.
 *
 * @par Description:
 * This API is used to obtain the priority of a specified task.
 *
 * @attention None.
 *
 * @param  taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 *
 * @retval #OS_INVALID      Fails to obtain the task priority.
 * @retval #UINT16          The task priority.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskPriSet
 * @since Huawei LiteOS V100R001C00
 */
extern UINT16 LOS_TaskPriGet(UINT32 taskId);

/**
 * @ingroup  los_task
 * @brief Obtain current running task ID.
 *
 * @par Description:
 * This API is used to obtain the ID of current running task.
 *
 * @attention
 * This interface should not be called before system initialized.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID    Can not get current running task.
 * @retval #UINT32                      Task ID.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_CurTaskIDGet(VOID);

/**
 * @ingroup  los_task
 * @brief Obtain a task information structure.
 *
 * @par Description:
 * This API is used to obtain a task information structure.
 *
 * @attention
 * One parameter of this interface is a pointer, it should be a correct value, otherwise, the system may be
 * abnormal.
 *
 * @param  taskId    [IN]  Type  #UINT32 Task ID. The task id value is obtained from task creation.
 * @param  taskInfo [OUT] Type  #TSK_INFO_S* Pointer to the task information structure to be obtained.
 *
 * @retval #LOS_ERRNO_TSK_PTR_NULL        Null parameter.
 * @retval #LOS_ERRNO_TSK_ID_INVALID      Invalid task ID.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED     The task is not created.
 * @retval #LOS_OK                        The task information structure is successfully obtained.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_TaskInfoGet(UINT32 taskId, TSK_INFO_S *taskInfo);

/**
 * @ingroup  los_task
 * @brief Set the affinity mask of the task scheduling cpu.
 *
 * @par Description:
 * This API is used to set the affinity mask of the task scheduling cpu.
 *
 * @attention
 * If any low #LOSCFG_KERNEL_CORE_NUM bit of the mask is not set or the system task is set, an error is reported.
 *
 * @param  taskId        [IN]  Type  #UINT32 Task ID. The task id value is obtained from task creation.
 * @param  usCpuAffiMask [IN]  Type  #UINT16 The scheduling cpu mask.The low to high bit of the mask corresponds to
 *                             the cpu number, the high bit that exceeding the CPU number is ignored.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                Invalid task ID.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED               The task is not created.
 * @retval #LOS_ERRNO_TSK_CPU_AFFINITY_MASK_ERR     The task cpu affinity mask is incorrect.
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK       The task is system task.
 * @retval #LOS_ERRNO_TSK_IS_ZOMBIE                 The task is in a "zombie" state.
 * @retval #LOS_OK                                  The task cpu affinity mask is successfully set.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskCpuAffiGet
 * @since Huawei LiteOS V200R003C00
 */
extern UINT32 LOS_TaskCpuAffiSet(UINT32 taskId, UINT16 usCpuAffiMask);

/**
 * @ingroup  los_task
 * @brief Get the affinity mask of the task scheduling cpu.
 *
 * @par Description:
 * This API is used to get the affinity mask of the task scheduling cpu.
 *
 * @attention None.
 *
 * @param  taskId       [IN]  Type  #UINT32 Task ID. The task id value is obtained from task creation.
 *
 * @retval 0            Fail to obtain the cpu affinity mask.
 * @retval #UINT16      The scheduling cpu mask. The low to high bit of the mask corresponds to the cpu number.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_TaskCpuAffiSet
 * @since Huawei LiteOS V200R003C00
 */
extern UINT16 LOS_TaskCpuAffiGet(UINT32 taskId);

/**
 * @ingroup  los_task
 * @brief Recycle task stack resource.
 *
 * @par Description:
 * This API is used to recycle task stack resource.
 *
 * @attention None.
 *
 * @param  None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R003C00
 */
extern VOID LOS_TaskResRecycle(VOID);

#ifdef LOSCFG_OBSOLETE_API
    #define LOS_TASK_PARAM_INIT_ARG_0(initParam, arg) \
            (initParam.auwArgs[0] = (UINTPTR)(arg))
    #define LOS_TASK_PARAM_INIT_ARG(initParam, arg) LOS_TASK_PARAM_INIT_ARG_0(initParam, arg)
#else
    #define LOS_TASK_PARAM_INIT_ARG(initParam, arg) \
            (initParam.pArgs = (VOID *)(arg))
#endif

/**
 * @ingroup los_task
 * @brief Define the lowpower framework process function type.
 *
 * @par Description:
 * This API is used to define the lowpower framework entry function type.
 *
 * @attention None.
 *
 * @param None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
typedef VOID (*LOWPOWERIDLEHOOK)(VOID);

/**
 * @ingroup  los_task
 * @brief Register a hook to enter lowpower framework process.
 *
 * @par Description:
 * This API is used to register lowpower framework entry function.
 *
 * @attention None.
 *
 * @param  hook [IN] The lowpower framework hook.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C10
 */
extern VOID LOS_LowpowerHookReg(LOWPOWERIDLEHOOK hook);

/**
 * @ingroup los_task
 * @brief Define the type of idle handler hook function.
 *
 * @par Description:
 * This API is used to define the type of idle handler hook function.
 * @attention None.
 *
 * @param None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R005C20
 */
typedef VOID (*IDLEHANDLERHOOK)(VOID);

/**
 * @ingroup  los_task
 * @brief Register the hook function for idle task.
 *
 * @par Description:
 * This API is used to register a hook function called when system idle.
 *
 * @attention The hook will be called when system idle.
 *
 * @param None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 * @since Huawei LiteOS V200R005C20
 */
extern VOID LOS_IdleHandlerHookReg(IDLEHANDLERHOOK hook);

#ifdef LOSCFG_TRUSTZONE
/**
 * @ingroup  los_task
 * @brief Alloc security stack.
 *
 * @par Description:
 * This API is used to apply for a security task stack.
 *
 * @attention
 * Do not call this API in interrupts.
 *
 * @param  taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 *         size   [IN] Type #UINT32 Security Stack. Size of the security task stack to be applied for.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                    Invalid Task ID.
 * @retval #LOS_ERRNO_TSK_ALLOC_SECURE_INT              Apply for the security task stack in interrupt.
 * @retval #LOS_ERRNO_TSK_SECURE_ALREADY_ALLOC          The security task stack is already allocated.
 * @retval #LOS_ERRNO_TSK_ALLOC_SECURE_FAILED           Failed to apply for the security task stack.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED                   The task is not created.
 * @retval #LOS_ERRNO_TSK_IS_ZOMBIE                     The task is in a "zombie" state.
 * @retval #LOS_OK                                      The security task stack is successfully allocated.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 LOS_TaskAllocSecureContext(UINT32 taskId, UINT32 size);

#endif

/**
 * @ingroup los_task
 * @brief Modify the task attributes to detach.
 *
 * @par Description:
 * This API is used to modify the attribute of the specified task to detach.
 * @attention None.
 *
 * @param taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                    Invalid Task ID.
 * @retval #LOS_ERRNO_TSK_NOT_ALLOW_IN_INT              Not allowed in interrupt context.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED                   The task is not created.
 * @retval #LOS_ERRNO_TSK_IS_DETACHED                   The task is already a detach task.
 * @retval #LOS_ERRNO_TSK_ALREADY_JOIN                  A task is waiting for this task.
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK           This task is a system task.
 * @retval #LOS_OK                                      Modify the task attributes to detach is successfully.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_TaskDetach(UINT32 taskId);

/**
 * @ingroup los_task
 * @brief Join with a terminated thread.
 *
 * @par Description:
 * This API is used to wait for the subtask to finish and reclaim the resource.
 * @attention None.
 *
 * @param taskId [IN] Type #UINT32 Task ID. The task id value is obtained from task creation.
 * @param retval [IN] Value returned when the task is complete.
 *
 * @retval #LOS_ERRNO_TSK_ID_INVALID                    Invalid Task ID.
 * @retval #LOS_ERRNO_TSK_NOT_ALLOW_IN_INT              Not allowed in interrupt context.
 * @retval #LOS_ERRNO_TSK_SCHED_LOCKED                  This task cannot be invoked when locked scheduled.
 * @retval #LOS_ERRNO_TSK_NOT_JOIN_SELF                 Tasks can't join himself.
 * @retval #LOS_ERRNO_TSK_OPERATE_SYSTEM_TASK           This task is a system task.
 * @retval #LOS_ERRNO_TSK_NOT_CREATED                   The task is not created.
 * @retval #LOS_ERRNO_TSK_IS_DETACHED                   Cannot join a detach task.
 * @retval #LOS_ERRNO_TSK_ALREADY_JOIN                  A task is waiting for this task.
 * @retval #LOS_OK                                      Modify the task attributes to detach is successfully.
 * @par Dependency:
 * <ul><li>los_task.h: the header file that contains the API declaration.</li></ul>
 */
extern UINT32 LOS_TaskJoin(UINT32 taskId, UINTPTR *retval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_TASK_H */
