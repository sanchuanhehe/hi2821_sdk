/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: System time Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2020-07-28
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

#ifndef _LOS_MISC_PRI_H
#define _LOS_MISC_PRI_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

VOID OsDumpMemByte(size_t length, UINTPTR addr);

#if defined(LOSCFG_DEBUG_SEMAPHORE) || defined(LOSCFG_DEBUG_MUTEX) || defined(LOSCFG_DEBUG_QUEUE)
typedef struct {
    CHAR *buf;             /* Control block array total buffer */
    size_t ctrlBlockSize;  /* Single control block size */
    size_t ctrlBlockCnt;   /* Number of control blocks */
    UINT32 sortElemOff;    /* The offset of the member to be compared in the control block */
} SortParam;

/* Compare the size of the last access time */
typedef BOOL (*OsCompareFunc)(const SortParam *sortParam, UINT32 left, UINT32 right);

/* Get the address of the comparison member variable */
#define SORT_ELEM_ADDR(sortParam, index) \
    ((sortParam)->buf + ((index) * (sortParam)->ctrlBlockSize) + (sortParam)->sortElemOff)

/* Sort this index array. */
extern VOID OsArraySort(UINT32 *sortArray, UINT32 start, UINT32 end, const SortParam *sortParam,
                        OsCompareFunc compareFunc);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_MISC_PRI_H */
