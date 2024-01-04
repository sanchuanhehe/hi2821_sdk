/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Sem Debug Private HeadFile
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

#ifndef _LOS_SEM_DEBUG_PRI_H
#define _LOS_SEM_DEBUG_PRI_H

#include "los_config.h"
#include "los_sem_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* semaphore debug initialization interface */
extern UINT32 OsSemDbgInit(VOID);
STATIC INLINE UINT32 OsSemDbgInitHook(VOID)
{
#ifdef LOSCFG_DEBUG_SEMAPHORE
    return OsSemDbgInit();
#else
    return LOS_OK;
#endif
}

/* Update the last time the semaphore was executed */
extern VOID OsSemDbgTimeUpdate(UINT32 semId);
STATIC INLINE VOID OsSemDbgTimeUpdateHook(UINT32 semId)
{
#ifdef LOSCFG_DEBUG_SEMAPHORE
    OsSemDbgTimeUpdate(semId);
#else
    (VOID)semId;
#endif
}

/* Update the SEM_DEBUG_CB of the semaphore when created or deleted */
extern VOID OsSemDbgUpdate(UINT32 semID, TSK_ENTRY_FUNC creator, UINT16 count);
STATIC INLINE VOID OsSemDbgUpdateHook(UINT32 semId, TSK_ENTRY_FUNC creator, UINT16 count)
{
#ifdef LOSCFG_DEBUG_SEMAPHORE
    OsSemDbgUpdate(semId, creator, count);
#else
    (VOID)semId;
    (VOID)creator;
    (VOID)count;
#endif
}

/* get the full data of SEM_DFX_CB */
extern UINT32 OsSemInfoGetFullData(VOID);
STATIC INLINE VOID OsSemInfoGetFullDataHook(VOID)
{
#ifdef LOSCFG_DEBUG_SEMAPHORE
    (VOID)OsSemInfoGetFullData();
#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_SEM_DEBUG_PRI_H */
