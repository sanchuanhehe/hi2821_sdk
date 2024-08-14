/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: RunStop HeadFile
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

#ifndef _LOS_RUNSTOP_PRI_H
#define _LOS_RUNSTOP_PRI_H

#include "los_runstop.h"
#include "los_memory_pri.h"
#include "los_deepsleep_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum {
    OS_NO_STORE_SYSTEM, /* Do not store the system. */
    OS_STORE_SYSTEM,    /* Start the storage system. */
};

/* The following four variables and two functions are defined in the assembly file */
extern CHAR __wow_end;
extern CHAR __ram_vectors_vma;

extern BOOL IsImageResume(VOID);

extern VOID OsCarryLeftScatter(VOID);
extern UINT32 OsWaitImagingDone(UINTPTR wowFlashAddr, size_t *wowImgSize);
extern VOID OsSystemWakeup(VOID);
extern VOID OsStoreSystemInfoBeforeSuspend(VOID);
extern VOID OsWriteWow2Flash(VOID);
extern UINT32 OsWowWriteFlashTaskCreate(VOID);
extern size_t OsWowImageSizeGet(VOID);
extern VOID OsWowOtherCoreResume(UINT32 cpuid);
extern UINT32 OsWowSysDoneFlagGet(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_RUNSTOP_PRI_H */
