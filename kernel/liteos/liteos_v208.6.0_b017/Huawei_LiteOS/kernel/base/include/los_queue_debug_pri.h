/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Queue Debug Pri HeadFile
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

#ifndef _LOS_QUEUE_DEBUG_PRI_H
#define _LOS_QUEUE_DEBUG_PRI_H

#include "los_queue_pri.h"
#include "los_config.h"
#include "los_task_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* queue debug initialization interface */
extern UINT32 OsQueueDbgInit(VOID);
STATIC INLINE UINT32 OsQueueDbgInitHook(VOID)
{
#ifdef LOSCFG_DEBUG_QUEUE
    return OsQueueDbgInit();
#else
    return LOS_OK;
#endif
}

/* Update the last time the queue was executed */
extern VOID OsQueueDbgTimeUpdate(UINT32 queueId);
STATIC INLINE VOID OsQueueDbgTimeUpdateHook(UINT32 queueId)
{
#ifdef LOSCFG_DEBUG_QUEUE
    OsQueueDbgTimeUpdate(queueId);
#else
    (VOID)queueId;
#endif
}

/* Update the task  entry of  the queue debug info when created or deleted */
extern VOID OsQueueDbgUpdate(UINT32 queueId, TSK_ENTRY_FUNC entry);
STATIC INLINE VOID OsQueueDbgUpdateHook(UINT32 queueId, TSK_ENTRY_FUNC entry)
{
#ifdef LOSCFG_DEBUG_QUEUE
    OsQueueDbgUpdate(queueId, entry);
#else
    (VOID)queueId;
    (VOID)entry;
#endif
}

/* check the leak of queue */
extern VOID OsQueueCheck(VOID);
STATIC INLINE VOID OsQueueCheckHook(VOID)
{
#ifdef LOSCFG_DEBUG_QUEUE
    OsQueueCheck();
#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_QUEUE_DEBUG_PRI_H */
