/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS task schedule module implementation and task context initialization.
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
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
 * ---------------------------------------------------------------------------- */

#include "los_hwi_pri.h"
#include "los_task_pri.h"

#define STACK_INIT_VAL    0x02020202U
#define STACK_INIT_ACC    0x01010101U

/* Save the TaskTcb of the new task */
VOID *g_newTask = NULL;
LITE_OS_SEC_TEXT_MINOR VOID ArchTaskExit(VOID)
{
    (VOID)LOS_IntLock();
    for (; ;) {
    }
}

LITE_OS_SEC_TEXT_INIT VOID *ArchTaskStackInit(UINT32 taskId, UINT32 stackSize, VOID *topStack)
{
    UINT32 index;
    TaskContext  *context = NULL;

    OsStackInit(topStack, stackSize);
    context = (TaskContext *)(((UINTPTR)topStack + stackSize) - sizeof(TaskContext));

    /* mstatus and mepc need to assign a value separately, so 2 is the beginning index. */
    ((UINT32 *)context)[TP_STACK_INDEX] = STACK_INIT_VAL;
    for (index = SP_STACK_INDEX; index < (sizeof(TaskContext) / sizeof(UINT32)); index++) {
        ((UINT32 *)context)[index] = ((UINT32 *)context)[index - 1] + STACK_INIT_ACC;
    }
#ifdef LOSCFG_ARCH_FPU_ENABLE
    context->fcsr    = 0x0; /* the bits's value of fflags and frm is 0. */
#endif
    context->mstatus = LOSCFG_MSTATUS_M;
    context->mepc = (UINT32)(UINTPTR)OsTaskEntry;
    context->a0 = taskId;
    context->ra = (UINT32)(UINTPTR)ArchTaskExit;

    return (VOID *)context;
}
