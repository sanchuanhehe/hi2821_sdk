/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description:  Exception Implementation
 * Author: Huawei LiteOS Team
 * Create: 2020-06-24
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

#include "los_exc.h"
#include "los_printf_pri.h"
#include "los_task_pri.h"
#ifdef LOSCFG_SHELL_EXCINFO_DUMP
#include "time.h"
#endif
#ifdef LOSCFG_EXC_INTERACTION
#include "soc/uart.h"
#include "los_swtmr_pri.h"
#include "los_hwi_pri.h"
#endif
#ifdef LOSCFG_SHELL
#include "shcmd.h"
#include "shell_pri.h"
#endif
#include "los_exc_pri.h"

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
typedef struct {
    CHAR *buf;                  /* pointer to the buffer for storing the exception information */
    UINT32 offset;              /* the offset of the buffer for storing the exception information */
    UINT32 len;                 /* the size of storing the exception information */
    UINTPTR dumpAddr;           /* the address of storing the exception information */
} ExcInfoDumpFormat;

STATIC ExcInfoDumpFormat g_excInfoPool = {0};
/* the hook of read-writing exception information */
STATIC __attribute__((section(".data"))) LogReadWriteFunc g_dumpHook = NULL;

VOID LOS_ExcInfoRegHook(UINTPTR startAddr, UINT32 len, CHAR *buf, LogReadWriteFunc hook)
{
    if ((hook == NULL) || (buf == NULL)) {
        PRINT_ERR("Buf or hook is null.\n");
        return;
    }

    g_excInfoPool.dumpAddr = startAddr;
    g_excInfoPool.len = len;
    /*
     * offset was initialized to MAX, and then was assigned to 0 in exc handler when it happens,
     * which make sure that PrintExcInfo only be called in exc handler.
     */
    g_excInfoPool.offset = 0xFFFFFFFF;
    g_excInfoPool.buf = buf;
    g_dumpHook = hook;
}

VOID OsSetExcInfoRW(LogReadWriteFunc func)
{
    g_dumpHook = func;
}

LogReadWriteFunc OsGetExcInfoRW(VOID)
{
    return g_dumpHook;
}

VOID OsSetExcInfoBuf(CHAR *buf)
{
    g_excInfoPool.buf = buf;
}

CHAR *OsGetExcInfoBuf(VOID)
{
    return g_excInfoPool.buf;
}

VOID OsSetExcInfoOffset(UINT32 Offset)
{
    g_excInfoPool.offset = Offset;
}

UINT32 OsGetExcInfoOffset(VOID)
{
    return g_excInfoPool.offset;
}

VOID OsSetExcInfoDumpAddr(UINTPTR addr)
{
    g_excInfoPool.dumpAddr = addr;
}

UINTPTR OsGetExcInfoDumpAddr(VOID)
{
    return g_excInfoPool.dumpAddr;
}

VOID OsSetExcInfoLen(UINT32 len)
{
    g_excInfoPool.len = len;
}

UINT32 OsGetExcInfoLen(VOID)
{
    return g_excInfoPool.len;
}

WEAK VOID WriteExcBufVa(const CHAR *format, va_list arglist)
{
    errno_t ret;
    CHAR *startAddr;

    if (g_excInfoPool.len > g_excInfoPool.offset) {
        startAddr = g_excInfoPool.buf + g_excInfoPool.offset;
        ret = vsnprintf_s(startAddr, (g_excInfoPool.len - g_excInfoPool.offset),
                          (g_excInfoPool.len - g_excInfoPool.offset - 1), format, arglist);
        if (ret == -1) {
            PRINT_ERR("exc info buffer is not enough or vsnprintf_s is error.\n");
            return;
        }

        ArchDCacheCleanByAddr((UINTPTR)startAddr, (UINTPTR)(startAddr + ret));

        g_excInfoPool.offset += ret;
    }
}

VOID WriteExcInfoToBuf(const CHAR *format, ...)
{
    va_list arglist;

    va_start(arglist, format);
    WriteExcBufVa(format, arglist);
    va_end(arglist);
}

#ifdef LOSCFG_SHELL
INT32 OsShellCmdReadExcInfo(INT32 argc, const CHAR **argv)
{
    LogReadWriteFunc hook = NULL;
    UINT32 recordSpace = OsGetExcInfoLen();

    (VOID)argc;
    (VOID)argv;

    CHAR *buf = (CHAR*)LOS_MemAlloc((void *)OS_SYS_MEM_ADDR, recordSpace + 1);
    if (buf == NULL) {
        return LOS_NOK;
    }
    (VOID)memset(buf, 0, recordSpace + 1);

    hook = OsGetExcInfoRW();
    if (hook != NULL) {
        hook(OsGetExcInfoDumpAddr(), recordSpace, 1, buf);
    }
    PRINTK("%s\n", buf);
    (VOID)LOS_MemFree((void *)OS_SYS_MEM_ADDR, buf);

    return LOS_OK;
}

SHELLCMD_ENTRY(readExcInfo_shellcmd, CMD_TYPE_EX, "excInfo", 0, (CmdCallBackFunc)OsShellCmdReadExcInfo);
#endif
#endif

#ifdef LOSCFG_EXC_INTERACTION
UINT32 OsCheckExcInteractionTask(const TSK_INIT_PARAM_S *initParam)
{
    if ((initParam->pfnTaskEntry == (TSK_ENTRY_FUNC)ShellTask) ||
        (initParam->pfnTaskEntry == (TSK_ENTRY_FUNC)ShellEntry) ||
        (initParam->pfnTaskEntry == (TSK_ENTRY_FUNC)OsIdleTask)) {
        return LOS_OK;
    }
    return LOS_NOK;
}

VOID OsKeepExcInteractionTask(VOID)
{
    LosTaskCB *taskCB = NULL;
    UINT32 taskId;
    UINT32 curIrqNum;

    OsIrqNestingCntSet(0);
    for (taskId = 0; taskId < g_taskMaxNum; taskId++) {
        taskCB = OS_TCB_FROM_TID(taskId);
        if ((IsShellTask(taskId) == TRUE)) {
            if (taskCB->taskStatus & OS_TASK_STATUS_RUNNING) {
                PRINT_EXC("Task %u is running, exception interaction may be abnormal\n", taskId);
            }
            continue;
        }

        if ((taskId == OsCurrTaskGet()->taskId) || (IsIdleTask(taskId) == TRUE)) {
            continue;
        }

        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }
        if (IsSwtmrTask(taskId) == TRUE) {
            taskCB->taskFlags &= (~OS_TASK_FLAG_SYSTEM);
        }
        (VOID)LOS_TaskDelete(taskId);
    }
    (VOID)OsHwiInit();
    /* if isr cause exception, clear latest interrupt to recover gic status */
    curIrqNum = OsIntNumGet();
    (VOID)LOS_HwiClear(curIrqNum);
    (VOID)LOS_HwiEnable(NUM_HAL_INTERRUPT_UART);
    (VOID)LOS_TaskDelete(OsCurrTaskGet()->taskId);
    /* unreachable */
}

#endif

VOID LOS_Panic(const CHAR *fmt, ...)
{
#ifdef LOSCFG_KERNEL_PRINTF
    va_list ap;
    va_start(ap, fmt);
    UartVprintf(fmt, ap);
    va_end(ap);
#else
    (VOID)fmt;
#endif
    ArchHaltCpu();
}

VOID LOS_BackTrace(VOID)
{
#ifdef LOSCFG_BACKTRACE
    LosTaskCB *runTask = OsCurrTaskGet();
    PRINT_EXC("runTask->taskName = %s\n""runTask->taskId = %u\n",
        runTask->taskName, runTask->taskId);
    ArchBackTrace();
#endif
}

VOID LOS_TaskBackTrace(UINT32 taskID)
{
#ifdef LOSCFG_BACKTRACE
    LosTaskCB *taskCB = NULL;

    if (taskID >= g_taskMaxNum) {
        PRINT_ERR("\r\nTask PID is invalid!\n");
        return;
    }
    taskCB = OS_TCB_FROM_TID(taskID);
    if ((taskCB->taskStatus & OS_TASK_STATUS_UNUSED) ||
        (taskCB->taskEntry == NULL) ||
        (taskCB->taskName == NULL)) {
        PRINT_ERR("\r\nThe task is not created!\n");
        return;
    }
    PRINTK("TaskName = %s\n""TaskId = 0x%x\n",
        taskCB->taskName, taskCB->taskId);
    ArchBackTraceWithSp(taskCB->stackPointer);
#else
    (VOID)taskID;
#endif
}

#ifdef __GNUC__
/* stack protector */
WEAK VOID __stack_chk_fail(VOID)
{
    /* __builtin_return_address is a builtin function, building in gcc */
    LOS_PANIC("stack-protector: Kernel stack is corrupted in: %p\n",
              __builtin_return_address(0));
}
#endif
