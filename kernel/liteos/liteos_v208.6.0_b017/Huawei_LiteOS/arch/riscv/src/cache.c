/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS cache module implementation.
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
 * ---------------------------------------------------------------------------- */

#include "arch/regs.h"
#include "arch/cache.h"
#include "arch/barrier.h"
#include "los_base.h"

LITE_OS_SEC_TEXT UINT32 ArchICacheEnable(CacheSize icclSize)
{
#ifdef LOSCFG_ARCH_LINX_M
    /* linxCore m-profile cache size: a read-only field */
    (VOID)icclSize;

    WRITE_CUSTOM_CSR_VAL(ICCTL, ICCTL_ENABLE);
#else
    if ((icclSize != CACHE_4KB) && (icclSize != CACHE_8KB) &&
        (icclSize != CACHE_16KB) && (icclSize != CACHE_32KB)) {
        return LOS_NOK;
    }

    WRITE_CUSTOM_CSR_VAL(ICCTL, (UINT32)((UINT32)icclSize | ICCTL_ENABLE));
#endif
    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 ArchDCacheEnable(CacheSize dcclSize)
{
#ifdef LOSCFG_ARCH_LINX_M
    /* linxCore m-profile cache size: a read-only field */
    (VOID)dcclSize;

    WRITE_CUSTOM_CSR_VAL(DCCTL, DCCTL_ENABLE);
#else
    if ((dcclSize != CACHE_4KB) && (dcclSize != CACHE_8KB) &&
        (dcclSize != CACHE_16KB) && (dcclSize != CACHE_32KB)) {
        return LOS_NOK;
    }

    WRITE_CUSTOM_CSR_VAL(DCCTL, (UINT32)((UINT32)dcclSize | DCCTL_ENABLE));
#endif
    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 ArchICachePrefetchEnable(CachePrefLines iclValue)
{
    if (iclValue >= CACHE_PREF_LINES_MAX) {
        return LOS_NOK;
    }

    WRITE_CUSTOM_CSR_VAL(APREFI, (UINT32)(((UINT32)iclValue << 1) | IAPEN)); /* 1: ICL control bits */
    return LOS_OK;
}

LITE_OS_SEC_TEXT UINT32 ArchDCachePrefetchEnable(CachePrefLines iclValue, CachePrefLines sclValue)
{
    UINT32 value;

    if ((iclValue >= CACHE_PREF_LINES_MAX) || (sclValue >= CACHE_PREF_LINES_MAX)) {
        return LOS_NOK;
    }

    value = ((UINT32)iclValue << 1) | IAPEN;  /* 1: ICL control bits */
    value |= ((UINT32)sclValue << 5) | SAPEN; /* 5: SCL control bits; */
    WRITE_CUSTOM_CSR_VAL(APREFD, value);
    return LOS_OK;
}

LITE_OS_SEC_TEXT VOID ArchICacheFlush(VOID)
{
    WRITE_CUSTOM_CSR_VAL(ICMAINT, ICACHE_BY_ALL);
    mb();
}

LITE_OS_SEC_TEXT VOID ArchDCacheFlush(VOID)
{
    WRITE_CUSTOM_CSR_VAL(DCMAINT, DCACHE_FLUSH_ALL);
    mb();
}

LITE_OS_SEC_TEXT VOID ArchICacheFlushByVa(UINTPTR baseAddr, UINT32 size)
{
    UINT32 flushNum;
    UINT32 count;
    UINTPTR endAddr;

    baseAddr = baseAddr & ~(CACHE_LINE_SIZE - 1);
    endAddr = baseAddr + size;

    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (UINT32)(endAddr - baseAddr);

    flushNum = size / CACHE_LINE_SIZE;
    for (count = 0; count < flushNum; count++) {
        WRITE_CUSTOM_CSR_VAL(ICINCVA, baseAddr);  /* write cmo's va */
        WRITE_CUSTOM_CSR_VAL(ICMAINT, ICACHE_BY_VA);  /* enable cmo(clean and invalidate) by va */
        baseAddr += CACHE_LINE_SIZE;
    }
}

STATIC VOID DCacheOperate(UINTPTR baseAddr, UINT32 size, UINT32 config)
{
    UINT32 flushNum;
    UINT32 count;
    UINTPTR endAddr;

    baseAddr = baseAddr & ~(CACHE_LINE_SIZE - 1);
    endAddr = baseAddr + size;

    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (UINT32)(endAddr - baseAddr);

    flushNum = size / CACHE_LINE_SIZE;
    for (count = 0; count < flushNum; count++) {
        WRITE_CUSTOM_CSR_VAL(DCINCVA, baseAddr);  /* write cmo's va */
        WRITE_CUSTOM_CSR_VAL(DCMAINT, config);
        baseAddr += CACHE_LINE_SIZE;
    }
}

LITE_OS_SEC_TEXT VOID ArchDCacheFlushByVa(UINTPTR baseAddr, UINT32 size)
{
    DCacheOperate(baseAddr, size, DCACHE_FLUSH_BY_VA);
}

LITE_OS_SEC_TEXT VOID ArchDCacheInvByVa(UINTPTR baseAddr, UINT32 size)
{
    UINTPTR OpStartAddr = baseAddr & ~(CACHE_LINE_SIZE - 1);
    UINTPTR OpEndAddr = (baseAddr + size) & ~(CACHE_LINE_SIZE - 1);

    if (OpStartAddr != baseAddr) {
        DCacheOperate(OpStartAddr, CACHE_LINE_SIZE, DCACHE_CLEAN_BY_VA);
    }

    if (OpEndAddr != (baseAddr + size)) {
        DCacheOperate(OpEndAddr, CACHE_LINE_SIZE, DCACHE_CLEAN_BY_VA);
    }
    DCacheOperate(baseAddr, size, DCACHE_INV_BY_VA);
}

LITE_OS_SEC_TEXT VOID ArchDCacheCleanByVa(UINTPTR baseAddr, UINT32 size)
{
    DCacheOperate(baseAddr, size, DCACHE_CLEAN_BY_VA);
}

LITE_OS_SEC_TEXT VOID ArchICacheFlushByAddr(UINTPTR startAddr, UINTPTR endAddr)
{
    UINT32 size;

    startAddr = (UINT32)startAddr & ~(CACHE_LINE_SIZE - 1);
    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (UINT32)(endAddr - startAddr);

    ArchICacheFlushByVa(startAddr, size);
}

LITE_OS_SEC_TEXT VOID ArchDCacheFlushByAddr(UINTPTR startAddr, UINTPTR endAddr)
{
    UINT32 size;

    startAddr = (UINT32)startAddr & ~(CACHE_LINE_SIZE - 1);
    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (UINT32)(endAddr - startAddr);

    ArchDCacheFlushByVa(startAddr, size);
}

LITE_OS_SEC_TEXT VOID ArchDCacheCleanByAddr(UINTPTR startAddr, UINTPTR endAddr)
{
    UINT32 size;

    startAddr = (UINT32)startAddr & ~(CACHE_LINE_SIZE - 1);
    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (UINT32)(endAddr - startAddr);

    ArchDCacheCleanByVa(startAddr, size);
}

LITE_OS_SEC_TEXT VOID ArchDCacheInvByAddr(UINTPTR startAddr, UINTPTR endAddr)
{
    UINT32 size;

    startAddr = (UINT32)startAddr & ~(CACHE_LINE_SIZE - 1);
    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (UINT32)(endAddr - startAddr);

    ArchDCacheInvByVa(startAddr, size);
}

LITE_OS_SEC_TEXT VOID ArchICacheInvalid(VOID)
{
    WRITE_CUSTOM_CSR_VAL(ICMAINT, ICACHE_BY_ALL);
    mb();
}

LITE_OS_SEC_TEXT VOID ArchDCacheInvalid(VOID)
{
    WRITE_CUSTOM_CSR_VAL(DCMAINT, DCACHE_INV_BY_ALL);
    mb();
}
