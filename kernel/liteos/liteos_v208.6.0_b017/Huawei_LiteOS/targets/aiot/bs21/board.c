/* ----------------------------------------------------------------------------
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: board adapter \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-10, Create file. \n
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
#include "los_tick_pri.h"
#include "los_init_pri.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void *g_sysMemAddr;
uint32_t g_sysMemSize;

void dma_cache_clean(unsigned int start, unsigned int end)
{
    ArchDCacheCleanByAddr(start, end);
}

void dma_cache_inv(unsigned int start, unsigned int end)
{
    ArchDCacheFlushByAddr(start, end);
}

VOID BoardConfig(VOID)
{
#ifdef LOSCFG_LIB_CONFIGURABLE
    g_sysMemAddr = (void *)&g_intheap_begin;
    g_sysMemSize = (unsigned)&g_ram_begin + (unsigned)&g_ram_size - (unsigned)&g_intheap_begin;
#endif
    /* config start memory before startup usb_init */

    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
}

UINT32 oal_get_sleep_ticks(VOID)
{
    UINT32 intSave;
    intSave = LOS_IntLock();
    UINT32 taskTimeout = OsSortLinkGetNextExpireTime(&OsPercpuGet()->taskSortLink);
#ifdef LOSCFG_BASE_CORE_SWTMR
    UINT32 swtmrTimeout = OsSortLinkGetNextExpireTime(&OsPercpuGet()->swtmrSortLink);
    if (swtmrTimeout < taskTimeout) {
        taskTimeout = swtmrTimeout;
    }
#endif
    LOS_IntRestore(intSave);
    return taskTimeout;
}

VOID oal_ticks_restore(UINT32 ticks)
{
    UINT32 intSave;
    intSave = LOS_IntLock();
    g_tickCount[ArchCurrCpuid()] += ticks;
    OsSortLinkUpdateExpireTime(ticks, &OsPercpuGet()->taskSortLink);
#ifdef LOSCFG_BASE_CORE_SWTMR
    OsSortLinkUpdateExpireTime(ticks, &OsPercpuGet()->swtmrSortLink);
#endif
    LOS_IntRestore(intSave);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
