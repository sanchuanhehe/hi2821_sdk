/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Misc Functions
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
#include "los_misc_pri.h"
#include "los_memory_pri.h"
#include "los_exc_pri.h"

#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif

#ifdef LOSCFG_KERNEL_LMS
#include "los_lms_pri.h"
#endif

#ifdef LOSCFG_LIB_CONFIGURABLE
LITE_OS_SEC_BSS UINT32 g_osSysClock;
LITE_OS_SEC_BSS UINT32 g_semLimit;
LITE_OS_SEC_BSS UINT32 g_rwsemLimit;
LITE_OS_SEC_BSS UINT32 g_muxLimit;
LITE_OS_SEC_BSS UINT32 g_queueLimit;

LITE_OS_SEC_BSS UINT32 g_swtmrLimit;
LITE_OS_SEC_BSS UINT32 g_taskLimit;
LITE_OS_SEC_BSS UINT32 g_minusOneTickPerSecond;
LITE_OS_SEC_BSS UINT32 g_taskMinStkSize;
LITE_OS_SEC_BSS UINT32 g_taskIdleStkSize;
LITE_OS_SEC_BSS UINT32 g_taskSwtmrStkSize;
LITE_OS_SEC_BSS UINT32 g_taskDfltStkSize;
LITE_OS_SEC_BSS UINT32 g_timeSliceTimeOut;

LITE_OS_SEC_DATA BOOL g_nxEnabled = FALSE;
LITE_OS_SEC_BSS UINTPTR g_dlNxHeapBase;
LITE_OS_SEC_BSS UINT32 g_dlNxHeapSize;
#endif

#ifdef LOSCFG_KERNEL_TRACE
TRACE_EVENT_HOOK g_traceEventHook = NULL;
TRACE_DUMP_HOOK g_traceDumpHook = NULL;
#endif

#ifdef LOSCFG_KERNEL_LMS
LmsHook* g_lms = NULL;
#endif

LITE_OS_SEC_TEXT UINTPTR LOS_Align(UINTPTR addr, UINT32 boundary)
{
    return (addr + boundary - 1) & ~((UINTPTR)(boundary - 1));
}

LITE_OS_SEC_TEXT_MINOR VOID LOS_Msleep(UINT32 msecs)
{
    UINT32 interval;

    if (msecs == 0) {
        interval = 0; /* The value 0 indicates direct scheduling. */
    } else {
        interval = LOS_MS2Tick(msecs);
        if (interval < UINT32_MAX) {
            interval += 1; /* Add a tick to compensate for the inaccurate tick count. */
        }
    }

    (VOID)LOS_TaskDelay(interval);
}

/*
 * Some targets use vendor's print function, which doesn't support special format characters like '-' or '.'.
 * And then, we need to redefine it in targets codes.
 * If you modify these format strings, please repair copyies in targets codes at the same time, otherwise,
 * maybe there exists error printf info.
 */
#ifndef MISC_MEM_FMT
#define MISC_MEM_FMT "%0+2x"
#endif

__attribute__((noinline)) STATIC VOID DumpByte(CHAR **addr, size_t length, size_t splitLen)
{
    size_t index;
    if (length == 0) {
        return;
    }

    if (IS_ALIGNED(splitLen, sizeof(CHAR *))) {
        EXCINFO_PRINTK("\n %p :", (CHAR *)(*addr));
    }

    for (index = length; index > 0; index--) {
        EXCINFO_PRINTK(MISC_MEM_FMT, (*addr)[index - 1]);
    }
    EXCINFO_PRINTK(" ");

    *addr += length;
}

VOID OsDumpMemByte(size_t length, UINTPTR addr)
{
    size_t countInteger = length / sizeof(UINTPTR);
    size_t index;

    if ((length == 0) || (addr == 0)) {
        return;
    }

    for (index = 0; index < countInteger; index++) {
        DumpByte((CHAR **)&addr, sizeof(UINTPTR), index);
    }

    DumpByte((CHAR **)&addr, (length % sizeof(UINTPTR)), index);
    EXCINFO_PRINTK("\n");

    return;
}

#if defined(LOSCFG_DEBUG_SEMAPHORE) || defined(LOSCFG_DEBUG_MUTEX) || defined(LOSCFG_DEBUG_QUEUE)
VOID OsArraySort(UINT32 *sortArray, UINT32 start, UINT32 end,
                 const SortParam *sortParam, OsCompareFunc compareFunc)
{
    UINT32 left = start;
    UINT32 right = end;
    UINT32 idx = start;
    UINT32 pivot = sortArray[start];

    while (left < right) {
        while ((left < right) && (sortArray[right] < sortParam->ctrlBlockCnt) && (pivot < sortParam->ctrlBlockCnt) &&
               compareFunc(sortParam, sortArray[right], pivot)) {
            right--;
        }

        if (left < right) {
            sortArray[left] = sortArray[right];
            idx = right;
            left++;
        }

        while ((left < right) && (sortArray[left] < sortParam->ctrlBlockCnt) && (pivot < sortParam->ctrlBlockCnt) &&
               compareFunc(sortParam, pivot, sortArray[left])) {
            left++;
        }

        if (left < right) {
            sortArray[right] = sortArray[left];
            idx = left;
            right--;
        }
    }

    sortArray[idx] = pivot;

    if (start < idx) {
        OsArraySort(sortArray, start, idx - 1, sortParam, compareFunc);
    }
    if (idx < end) {
        OsArraySort(sortArray, idx + 1, end, sortParam, compareFunc);
    }
}
#endif
