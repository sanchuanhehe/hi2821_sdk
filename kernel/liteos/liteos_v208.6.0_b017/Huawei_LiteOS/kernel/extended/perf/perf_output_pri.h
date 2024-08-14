/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: LiteOS Perf Output Implementation Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2020-07-29
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

#ifndef _PERF_OUTPUT_PRI_H
#define _PERF_OUTPUT_PRI_H

#include "los_perf_pri.h"
#include "los_ringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    Ringbuf ringbuf;           /* ring buffer */
    UINT32 waterMark;          /* notify water mark */
} PerfOutputCB;

extern UINT32 OsPerfOutPutInit(VOID *buf, UINT32 size);
extern VOID OsPerfOutPutDeinit(VOID);
extern UINT32 OsPerfOutPutRead(CHAR *dest, UINT32 size);
extern UINT32 OsPerfOutPutWrite(CHAR *data, UINT32 size);
extern VOID OsPerfOutPutFlush(VOID);
extern VOID OsPerfNotifyHookReg(const PERF_BUF_NOTIFY_HOOK func);
extern VOID OsPerfFlushHookReg(const PERF_BUF_FLUSH_HOOK func);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PERF_OUTPUT_PRI_H */
