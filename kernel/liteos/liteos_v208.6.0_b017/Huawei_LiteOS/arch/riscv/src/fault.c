/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS exception module implementation.
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

#include "arch/exception.h"
#include "los_task_pri.h"
#include "los_typedef.h"
#include "los_printf_pri.h"
#include "los_init.h"
#if defined(LOSCFG_SHELL_EXCINFO_DUMP) || defined(LOSCFG_EXC_INTERACTION)
#include "los_exc_pri.h"
#include "los_hwi_pri.h"
#endif
#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif

#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
#include "console_pri.h"
#endif

#ifdef LOSCFG_LIB_CONFIGURABLE
    UINTPTR g_nmiStackTop = (UINTPTR)(&__nmi_stack_top);
    UINTPTR g_excStackTop = (UINTPTR)(&__exc_stack_top);
    UINTPTR g_irqStackTop = (UINTPTR)(&__irq_stack_top);
#endif

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr);
LITE_OS_SEC_BSS ExcInfo g_excInfo;
STATIC EXC_PROC_FUNC g_excHook = OsExcHook;

UINT32 ArchSetExcHook(EXC_PROC_FUNC excHook)
{
    UINT32 intSave;

    intSave = ArchIntLock();
    g_excHook = excHook;
    ArchIntRestore(intSave);
    return 0;
}

EXC_PROC_FUNC ArchGetExcHook(VOID)
{
    return g_excHook;
}

#ifdef LOSCFG_BACKTRACE
#define RA_OFFSET         4
#define FP_OFFSET         8
#define OS_MAX_BACKTRACE  100
#define FP_ALIGN(value)   (((UINT32)(value) & (UINT32)(LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) == 0)

/* this function is used to validate fp or validate the checking range start and end. */
STATIC INLINE BOOL IsValidFP(UINTPTR regFP, UINTPTR start, UINTPTR end)
{
    return (regFP > start) && (regFP <= end) && FP_ALIGN(regFP);
}

STATIC INLINE BOOL FindSuitableStack(UINTPTR regFP, UINTPTR *start, UINTPTR *end)
{
    UINT32 index;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    BOOL found = FALSE;
    LosTaskCB *taskCB = NULL;

    /* Search in the task stacks */
    for (index = 0; index < g_taskMaxNum; index++) {
        taskCB = OS_TCB_FROM_TID(index);
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }

        stackStart = taskCB->topOfStack;
        stackEnd = taskCB->topOfStack + (UINTPTR)taskCB->stackSize;
        if (IsValidFP(regFP, stackStart, stackEnd)) {
            found = TRUE;
            goto FOUND;
        }
    }

    /* Search in the exc stacks */
    stackStart = (UINTPTR)&__int_stack_start;
    stackEnd = (UINTPTR)&__int_stack_end;
    if (IsValidFP(regFP, stackStart, stackEnd)) {
        found = TRUE;
    }

FOUND:
    if (found == TRUE) {
        *start = stackStart;
        *end = stackEnd;
    }

    return found;
}

STATIC VOID BackTraceWithFp(UINTPTR fp)
{
    PRINT_EXC("*******backtrace begin*******\n");
    (VOID)ArchBackTraceGet(fp, NULL, OS_MAX_BACKTRACE, 0);
    PRINT_EXC("*******backtrace end*******\n");
}
#endif

UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR tmpFP;
    UINTPTR backRa;
    UINTPTR backFP = fp;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    UINT32 count;

    if (FindSuitableStack(fp, &stackStart, &stackEnd) == FALSE) {
        PRINT_EXC("fp error, backtrace failed!\n");
        return 0;
    }

    /*
     * Check whether it is the leaf function.
     * Generally, the frame pointer minus RA_OFFSET points to the address of lr, while in the leaf function,
     * there's no function call, and compiler will not store the link register, but the frame pointer
     * will still be stored and updated. In that case we needs to find the right position of frame pointer.
     */
    tmpFP = *((UINTPTR *)(UINTPTR)(fp - RA_OFFSET));
    if (IsValidFP(tmpFP, stackStart, stackEnd)) {
        backFP = tmpFP;
        if (callChain == NULL) {
            PRINT_EXC("traceback fp fixed, trace using   fp = 0x%x\n", backFP);
        }
    }

    for (count = 0; count < maxDepth; count++) {
        if (!IsValidFP(backFP, stackStart, stackEnd)) {
            break;
        }
        tmpFP = backFP;
        backRa = *((UINTPTR *)(UINTPTR)(tmpFP - RA_OFFSET));
        backFP = *((UINTPTR *)(UINTPTR)(tmpFP - FP_OFFSET));

        if (count < ignrDepth) {
            continue;
        }

        if (callChain == NULL) {
            PRINT_EXC("traceback %u -- ra = 0x%x    fp = 0x%x\n", count - ignrDepth, backRa, backFP);
        } else {
            callChain[count - ignrDepth] = backRa;
        }

        if (backFP == tmpFP) { /* no need to check ra */
            break;
        }
    }

    return count;
#else
    (VOID)fp;
    (VOID)callChain;
    (VOID)maxDepth;
    (VOID)ignrDepth;
    return 0;
#endif
}

LITE_OS_SEC_TEXT VOID ArchBackTraceWithSp(const VOID *stackPointer)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR fp = ArchGetTaskFp(stackPointer);
    PRINT_EXC("fp:0x%08x\n", fp);
    BackTraceWithFp(fp);
#else
    (VOID)stackPointer;
#endif
}

LITE_OS_SEC_TEXT VOID ArchBackTrace(VOID)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR fp = ArchGetFp();
    PRINT_EXC("fp:0x%08x\n", fp);
    BackTraceWithFp(fp);
#endif
}

#ifndef LOSCFG_EXC_SIMPLE_INFO
STATIC const CHAR *g_mcauseInfo[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Reserved",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Reserved",
    "Store page fault",
    "Hard fault",         /* Reserved exception code */
    "Lock up"             /* Reserved exception code */
};

#ifdef LOSCFG_ARCH_LINX_M
#define REG_CCAUSE_CAUSE_MASK        0x1F
STATIC const CHAR *g_ccauseInfo[] = {
    "Not available",
    "Memory map region access fault",
    "MMEM BUS error response",
    "PMEM BUS error response",
    "Crossing PMP entries",
    "System register access fault",
    "No PMP entry matched",
    "PMP access fault",
    "Reserved",
    "CSR access fault",
    "Reserved",
    "Reserved",
    "I/D-cache parity check error",
    "Dcache evict BUS error response",
    "Asynchronous Dcache parity check error",
    "Reserved",
    "Dcache 2bit ECC error",
    "Reserved",
    "Asynchronous Dcache 2bit ECC error",
    "Device access fault",
    "Atomic access fault",
    "Uopcnt out of range",
#ifndef LOSCFG_ARCH_RISCV_TES
    "TCM error response"
#else
    "TCM error response",
    "Trust related access violation",
    "Untrusted TESVEC table",
    "Illegal TES transition",
    "Marker expected",
    "Stack pointer limit exceeded"
#endif
};
#else
#define REG_CCAUSE_CAUSE_MASK        0xF
STATIC const CHAR *g_ccauseInfo[] = {
    "Not available",
    "Memory map region access fault",
    "AXIM error response",
    "AHBM error response",
    "Crossing PMP entries",
    "Reserved",
    "No PMP entry matched",
    "PMP access fault",
    "CMO access fault",
    "CSR access fault",
    "LDM/STMIA instruction fault",
    "ITCM write access fault"
};
#endif

STATIC const CHAR *g_excOccurStage[] = {
    "Init",
    "Task",
    "Irq",
    "Exc"
};
#else
/*
 * refer to TaskContext defined in task.h
 * g_xRegsMap start from X4(instead of X0) to X31
 * the index of array maps subscript of x-regs plus 4, e.g. index0 -> X4, index1 -> X5, index2 -> X6 ...
 * the value of elements means offset of member in TaskContext, e.g. X4 -> the 2nd member of TaskContext
 */
STATIC const UINT8 g_xRegsMap[] = {
/*  X4  X5  X6  X7  X8  X9  X10 */
    2,  30, 29, 28, 15, 14, 27,
/*  X11 X12 X13 X14 X15 X16 X17 */
    26, 25, 24, 23, 22, 21, 20,
/*  X18 X19 X20 X21 X22 X23 X24 */
    13, 12, 11, 10, 9,  8,  7,
/*  X25 X26 X27 X28 X29 X30 X31 */
    6,  5,  4,  19, 18, 17, 16
};
#endif

UINT32 ArchExcInit(VOID)
{
    return LOS_OK;
}
LOS_SYS_INIT(ArchExcInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);

LITE_OS_SEC_TEXT STATIC VOID OsExcInfoDisplayContext(const ExcInfo *exc)
{
    const ExcContext *excContext = exc->context;
    const TaskContext *taskContext = &(excContext->taskContext);

    PRINT_EXC("ccause:0x%x\n""mcause:0x%x\n""mtval:0x%x\n"
        "gp:0x%x\n""mstatus:0x%x\n""mepc:0x%x\n"
        "ra:0x%x\n""sp:0x%x\n",
        excContext->ccause, excContext->mcause, excContext->mtval,
        excContext->gp, taskContext->mstatus, taskContext->mepc,
        taskContext->ra, taskContext->sp);

#ifndef LOSCFG_EXC_SIMPLE_INFO
    PRINT_EXC("tp:0x%x\n""t0:0x%x\n""t1:0x%x\n""t2:0x%x\n"
        "s0:0x%x\n""s1:0x%x\n""a0:0x%x\n""a1:0x%x\n",
        taskContext->tp, taskContext->t0, taskContext->t1, taskContext->t2,
        taskContext->s0, taskContext->s1, taskContext->a0, taskContext->a1);
    PRINT_EXC("a2:0x%x\n""a3:0x%x\n""a4:0x%x\n""a5:0x%x\n"
        "a6:0x%x\n""a7:0x%x\n""s2:0x%x\n""s3:0x%x\n",
        taskContext->a2, taskContext->a3, taskContext->a4, taskContext->a5,
        taskContext->a6, taskContext->a7, taskContext->s2, taskContext->s3);
    PRINT_EXC("s4:0x%x\n""s5:0x%x\n""s6:0x%x\n""s7:0x%x\n"
        "s8:0x%x\n""s9:0x%x\n""s10:0x%x\n""s11:0x%x\n",
        taskContext->s4, taskContext->s5, taskContext->s6, taskContext->s7,
        taskContext->s8, taskContext->s9, taskContext->s10, taskContext->s11);
    PRINT_EXC("t3:0x%x\n""t4:0x%x\n""t5:0x%x\n""t6:0x%x\n",
        taskContext->t3, taskContext->t4, taskContext->t5, taskContext->t6);
#else
    UINT32 i;
    const UINT32 *contextRegs = (const UINT32 *)taskContext;

    /* X0 is always zero, X1 - X3 is displayed before, so start from X4 */
    for (i = 0; i < sizeof(g_xRegsMap) / sizeof(g_xRegsMap[0]); i++) {
        PRINT_EXC("X%-02u:0x%x\n", i + 4, contextRegs[g_xRegsMap[i]]); /* 4: X4 reg */
    }
#endif
}

VOID OsExcInfoDisplay(UINT32 excType, const ExcContext *excBufAddr)
{
    if (g_excInfo.nestCnt > 1) { /* Exception nesting level is 1 */
        g_excInfo.phase = OS_EXC_STAGE_EXC;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else if (!OS_SCHEDULER_ACTIVE) {
        g_excInfo.phase = OS_EXC_STAGE_INIT;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else if (OS_INT_ACTIVE) {
        g_excInfo.phase = OS_EXC_STAGE_IRQ;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else {
        g_excInfo.phase = OS_EXC_STAGE_TASK;
        g_excInfo.thrdPid = LOS_CurTaskIDGet();
    }
    g_excInfo.faultAddr = excBufAddr->mtval;
    g_excInfo.type = excType;
    g_excInfo.context = (ExcContext *)excBufAddr;

#ifdef LOSCFG_EXC_SIMPLE_INFO
    PRINT_EXC("task:%s\n""thrdPid:0x%x\n""type:0x%x\n""nestCnt:%u\n""phase:%u\n",
        OsCurTaskNameGet(), g_excInfo.thrdPid, g_excInfo.type, g_excInfo.nestCnt, g_excInfo.phase);
#else
    PRINT_EXC("task:%s\n""thrdPid:0x%x\n""type:0x%x\n""nestCnt:%u\n""phase:%s\n",
        OsCurTaskNameGet(), g_excInfo.thrdPid, g_excInfo.type, g_excInfo.nestCnt, g_excOccurStage[g_excInfo.phase]);
#endif

    OsExcInfoDisplayContext(&g_excInfo);
}

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr)
{
#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
    OsConsoleDisable();
#endif

    OsExcInfoDisplay(excType, excBufAddr);

#ifdef LOSCFG_BACKTRACE
    BackTraceWithFp(excBufAddr->taskContext.s0);
#endif
#ifndef LOSCFG_EXC_SIMPLE_INFO
    (VOID)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    OsExcStackInfo();
#endif
}

VOID OsExcHandleEntry(UINT32 excType, const ExcContext *excBufAddr)
{
#ifndef LOSCFG_EXC_SIMPLE_INFO
    UINT32 cause = excBufAddr->ccause & REG_CCAUSE_CAUSE_MASK;

    if ((excType < sizeof(g_mcauseInfo) / sizeof(g_mcauseInfo[0])) &&
        (cause < sizeof(g_ccauseInfo) / sizeof(g_ccauseInfo[0]))) {
        PRINT_EXC("%s\n""%s\n",  g_mcauseInfo[excType], g_ccauseInfo[cause]);
    } else {
        PRINT_EXC("Unknown exc:%u, ccause:%u.\n", excType, cause);
    }
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    LogReadWriteFunc func = OsGetExcInfoRW();
#endif

    if (g_excInfo.nestCnt == 1) { /* 1: nest cnt, first exc */
#ifdef LOSCFG_SHELL_EXCINFO_DUMP
        if (func != NULL) {
            OsSetExcInfoOffset(0);
        }
#endif
        if (g_excHook != NULL) {
            g_excHook(excType, excBufAddr);
        }
    } else {
        OsExcInfoDisplay(excType, excBufAddr);
    }

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    if (func != NULL) {
        PRINT_EXC("Be sure your space bigger than OsGetExcInfoOffset():0x%x\n", OsGetExcInfoOffset());
        OsIrqNestingCntSet(0);     /* 0: int nest count */
        func(OsGetExcInfoDumpAddr(), OsGetExcInfoLen(), 0, OsGetExcInfoBuf());
        OsIrqNestingCntSet(1);     /* 1: int nest count */
    }
#endif

#ifdef LOSCFG_KERNEL_TRACE
    if (g_traceDumpHook != NULL) {
        g_traceDumpHook(FALSE);
    }
#endif

    for (;;) {
    }
}
#ifdef LOSCFG_ARCH_LINXCORE_131
VOID OsTriggerNMI(VOID)
{
    WRITE_UINT32(0x1, NMI_REG_BASE);
}

VOID OsClearNMI(VOID)
{
    WRITE_UINT32(0x0, NMI_REG_BASE);
}
#endif
LITE_OS_SEC_TEXT VOID OsNMIHandler(const ExcContext *excBufAddr)
{
    UINT32 excType = excBufAddr->mcause;
    PRINT_EXC("Oops:NMI\n");
#ifdef LOSCFG_ARCH_LINXCORE_131
    OsClearNMI();
#endif
    OsExcInfoDisplay(excType, excBufAddr);
    for (;;) {
    }
}

NMI_PROC_FUNC g_osNmiHook = OsNMIHandler;
UINT32 ArchSetNMIHook(NMI_PROC_FUNC nmiHook)
{
    UINT32 intSave;

    intSave = ArchIntLock();
    g_osNmiHook = nmiHook;
    ArchIntRestore(intSave);
    return 0;
}

NMI_PROC_FUNC ArchGetNMIHook(VOID)
{
    return g_osNmiHook;
}

