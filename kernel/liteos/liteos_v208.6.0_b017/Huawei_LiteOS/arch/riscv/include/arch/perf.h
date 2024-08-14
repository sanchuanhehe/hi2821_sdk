/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS Arch Perf HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
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

#ifndef _ARCH_PERF_H
#define _ARCH_PERF_H

#include "los_typedef.h"
#include "arch/exception.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EVENT_PERIOD_LOWER_BOUND    0x000004FF
#define EVENT_PERIOD_UPPER_BOUND    0xFFFFFFFF
#define PERIOD_EVENT_CALC(p)        (p)
#define VALID_EVENT_PERIOD(p)       ((p > EVENT_PERIOD_LOWER_BOUND) \
                                    && (p < EVENT_PERIOD_UPPER_BOUND))

#define CYCLE_PERIOD_LOWER_BOUND    0x000004FF
#define CYCLE_PERIOD_UPPER_BOUND    0xFFFFFFFF
#define PERIOD_CYCLE_CALC(p)        PERIOD_EVENT_CALC(p)
#define VALID_CYCLE_PERIOD(p)       VALID_EVENT_PERIOD(p)

/* Get Caller's pc and fp in non-irq context */
#define OsPerfArchFetchCallerRegs(regs) \
    do { \
        (regs)->pc = (UINTPTR)__builtin_return_address(0); \
        (regs)->fp = (UINTPTR)__builtin_frame_address(0);  \
    } while (0)

/* Get Caller's pc and fp in irq context */
#define OsPerfArchFetchIrqRegs(regs, tcb) \
    do { \
        (regs)->pc = (tcb)->pc; \
        (regs)->fp = (tcb)->fp; \
    } while (0)

STATIC INLINE UINT32 ArchPerfBackTraceGet(UINTPTR fp, UINTPTR pc, UINTPTR *callChain, UINT32 maxDepth)
{
    (void)pc;
    return ArchBackTraceGet(fp, callChain, maxDepth, 0);
}

extern UINT32 OsGetPmuInstretCounter(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_PERF_H */
