/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: LiteOS Mem Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-05-12
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

#include "los_memory_pri.h"
#include "los_memory_internal.h"

#include "securec.h"

#include "los_hwi.h"
#include "los_spinlock.h"
#include "los_trace.h"
#include "los_exc_pri.h"

LITE_OS_SEC_BSS  SPIN_LOCK_INIT(g_memSpin);

UINT8 *m_aucSysMem0 = (UINT8 *)NULL;
UINT8 *m_aucSysMem1 = (UINT8 *)NULL;
MALLOC_HOOK g_MALLOC_HOOK = NULL;

LITE_OS_SEC_TEXT_INIT UINT32 LOS_MemPoolInit(VOID *pool, UINT32 size, UINT32 attr)
{
    UINT32 intSave;

    if ((pool == NULL) || (size < OS_MEM_MIN_POOL_SIZE)) {
        return LOS_NOK;
    }

    if (!IS_ALIGNED(size, OS_MEM_ALIGN_SIZE) || !IS_ALIGNED(pool, OS_MEM_ALIGN_SIZE)) {
        PRINT_WARN("pool size 0x%x should be aligned with OS_MEM_ALIGN_SIZE\n", size);
        size = OS_MEM_ALIGN(size, OS_MEM_ALIGN_SIZE) - OS_MEM_ALIGN_SIZE;
    }

    MEM_LOCK(intSave);
    if (OsMemMulPoolInit(pool, size) != LOS_OK) {
        MEM_UNLOCK(intSave);
        return LOS_NOK;
    }

    if (OsHeapInit(pool, size) == FALSE) {
        (VOID)OsMemMulPoolDeinit(pool);
        MEM_UNLOCK(intSave);
        return LOS_NOK;
    }

    OsSlabMemProcInitFlag(pool, size, attr & LOS_MEM_INIT_ATTR_SLAB_MSK);
    MEM_UNLOCK(intSave);

    LOS_TRACE(MEM_INFO_REQ, pool);
    return LOS_OK;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_MemInit(VOID *pool, UINT32 size)
{
    return LOS_MemPoolInit(pool, size, LOS_MEM_INIT_ATTR_SLAB_MSK);
}

#ifdef LOSCFG_EXC_INTERACTION
LITE_OS_SEC_TEXT_INIT UINT32 OsMemExcInteractionInit(UINTPTR memStart)
{
    UINT32 ret;
    m_aucSysMem0 = (UINT8 *)memStart;
    g_excInteractMemSize = EXC_INTERACT_MEM_SIZE;
    ret = LOS_MemInit(m_aucSysMem0, g_excInteractMemSize);
    PRINT_INFO("LiteOS kernel exc interaction memory address:%p,size:0x%x\n", m_aucSysMem0, g_excInteractMemSize);
    return ret;
}
#endif

/*
 * Description : Initialize Dynamic Memory pool
 * Return      : LOS_OK on success or error code on failure
 */
LITE_OS_SEC_TEXT_INIT UINT32 OsMemSystemInit(UINTPTR memStart)
{
    UINT32 ret;
    UINT32 memSize;

    m_aucSysMem1 = (UINT8 *)memStart;
    memSize = OS_SYS_MEM_SIZE;
    ret = LOS_MemInit((VOID *)m_aucSysMem1, memSize);
#ifndef LOSCFG_EXC_INTERACTION
    m_aucSysMem0 = m_aucSysMem1;
#endif
    return ret;
}

/*
 * Description : print heap information
 * Input       : pool --- Pointer to the manager, to distinguish heap
 */
VOID OsMemInfoPrint(const VOID *pool)
{
    struct LosHeapManager *heapMan = (struct LosHeapManager *)pool;
    LosHeapStatus status = {0};

    if (OsHeapStatisticsGet(heapMan, &status) == LOS_NOK) {
        return;
    }

    PRINT_INFO("pool addr    pool size    used size    free size    max free    alloc Count    free Count\n");
    PRINT_INFO("0x%-8x   0x%-8x   0x%-8x    0x%-8x   0x%-8x   0x%-8x     0x%-8x\n",
               pool, heapMan->size, status.totalUsedSize, status.totalFreeSize, status.maxFreeNodeSize,
               status.usedNodeNum, status.freeNodeNum);
}

LITE_OS_SEC_TEXT VOID *LOS_MemAlloc(VOID *pool, UINT32 size)
{
    VOID *ptr = NULL;
    UINT32 intSave;
    MALLOC_HOOK mallocHook;

    if ((pool == NULL) || (size == 0)) {
        return ptr;
    }

    mallocHook = g_MALLOC_HOOK;
    if (mallocHook != NULL) {
        mallocHook();
    }

    MEM_LOCK(intSave);

    ptr = OsSlabMemAlloc(pool, size);
    if (ptr == NULL) {
        ptr = OsHeapAlloc(pool, size);
    }

    MEM_UNLOCK(intSave);

    LOS_TRACE(MEM_ALLOC, pool, (UINTPTR)ptr, size);
    return ptr;
}

LITE_OS_SEC_TEXT VOID *LOS_MemAllocAlign(VOID *pool, UINT32 size, UINT32 boundary)
{
    VOID *ptr = NULL;
    UINT32 intSave;

    MEM_LOCK(intSave);
    ptr = OsHeapAllocAlign(pool, size, boundary);
    MEM_UNLOCK(intSave);

    LOS_TRACE(MEM_ALLOC_ALIGN, pool, (UINTPTR)ptr, size, boundary);
    return ptr;
}

VOID *LOS_MemRealloc(VOID *pool, VOID *ptr, UINT32 size)
{
    VOID *retPtr = NULL;
    VOID *freePtr = NULL;
    UINT32 intSave;
    struct LosHeapNode *node = NULL;
    UINT32 cpySize;
    UINT32 gapSize;
    errno_t rc;

    if (pool == NULL) {
        return NULL;
    }

    /* Zero-size requests are treated as free. */
    if ((ptr != NULL) && (size == 0)) {
        if (LOS_MemFree(pool, ptr) != LOS_OK) {
            PRINT_ERR("LOS_MemFree error, pool[%p], pPtr[%p]\n", pool, ptr);
        }
    } else if (ptr == NULL) { // Requests with NULL pointers are treated as malloc.
        retPtr = LOS_MemAlloc(pool, size);
    } else {
        MEM_LOCK(intSave);

        UINT32 oldSize = OsSlabMemCheck(pool, ptr);
        if (oldSize != (UINT32)(-1)) {
            cpySize = (size > oldSize) ? oldSize : size;
        } else {
            /* find the real ptr through gap size */
            gapSize = *((UINTPTR *)((UINTPTR)ptr - sizeof(UINTPTR)));
            if (OS_MEM_GET_ALIGN_FLAG(gapSize)) {
                MEM_UNLOCK(intSave);
                return NULL;
            }

            node = ((struct LosHeapNode *)ptr) - 1;
            cpySize = (size > (node->size)) ? (node->size) : size;
        }

        MEM_UNLOCK(intSave);

        retPtr = LOS_MemAlloc(pool, size);
        if (retPtr != NULL) {
            rc = memcpy_s(retPtr, size, ptr, cpySize);
            if (rc == EOK) {
                freePtr = ptr;
            } else {
                freePtr = retPtr;
                retPtr = NULL;
            }

            if (LOS_MemFree(pool, freePtr) != LOS_OK) {
                PRINT_ERR("LOS_MemFree error, pool[%p], ptr[%p]\n", pool, freePtr);
            }
        }
    }

    LOS_TRACE(MEM_REALLOC, pool, (UINTPTR)ptr, size);
    return retPtr;
}

LITE_OS_SEC_TEXT UINT32 LOS_MemFree(VOID *pool, VOID *mem)
{
    BOOL ret = FALSE;
    UINT32 intSave;

    if ((pool == NULL) || (mem == NULL)) {
        return LOS_NOK;
    }

    MEM_LOCK(intSave);

    ret = OsSlabMemFree(pool, mem);
    if (ret != TRUE) {
        ret = OsHeapFree(pool, mem);
    }

    MEM_UNLOCK(intSave);

    LOS_TRACE(MEM_FREE, pool, (UINTPTR)mem);
    return (ret == TRUE ? LOS_OK : LOS_NOK);
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_MemInfoGet(VOID *pool, LOS_MEM_POOL_STATUS *status)
{
    LosHeapStatus heapStatus;
    UINT32 err;
    UINT32 intSave;

    if ((pool == NULL) || (status == NULL)) {
        return LOS_NOK;
    }

    MEM_LOCK(intSave);

    err = OsHeapStatisticsGet(pool, &heapStatus);
    if (err != LOS_OK) {
        MEM_UNLOCK(intSave);
        return LOS_NOK;
    }

    status->uwTotalUsedSize   = heapStatus.totalUsedSize;
    status->uwTotalFreeSize   = heapStatus.totalFreeSize;
    status->uwMaxFreeNodeSize = heapStatus.maxFreeNodeSize;
    status->uwUsedNodeNum  = heapStatus.usedNodeNum;
    status->uwFreeNodeNum  = heapStatus.freeNodeNum;

#ifdef LOSCFG_MEM_TASK_STAT
    status->uwUsageWaterLine = heapStatus.usageWaterLine;
#endif

    MEM_UNLOCK(intSave);
    return LOS_OK;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_MemTotalUsedGet(VOID *pool)
{
    LosHeapStatus heapStatus;
    UINT32 err;
    UINT32 intSave;

    if (pool == NULL) {
        return OS_NULL_INT;
    }

    MEM_LOCK(intSave);
    err = OsHeapStatisticsGet(pool, &heapStatus);
    MEM_UNLOCK(intSave);

    if (err != LOS_OK) {
        return OS_NULL_INT;
    }

    return heapStatus.totalUsedSize;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_MemPoolSizeGet(const VOID *pool)
{
    struct LosHeapManager *heapManager = NULL;

    if (pool == NULL) {
        return OS_NULL_INT;
    }

    heapManager = (struct LosHeapManager *)pool;
    return heapManager->size;
}

LITE_OS_SEC_TEXT_MINOR UINT32 LOS_MemIntegrityCheck(const VOID *pool)
{
    UINT32 intSave;
    UINT32 ret;
    struct LosHeapManager *heapMan = (struct LosHeapManager *)pool;
    if (pool == NULL) {
        return OS_NULL_INT;
    }
    MEM_LOCK(intSave);
    ret = OsHeapIntegrityCheck(heapMan);
    MEM_UNLOCK(intSave);

    return ret;
}

VOID OsMemIntegrityMultiCheck(VOID)
{
    if (LOS_MemIntegrityCheck(m_aucSysMem1) == LOS_OK) {
        EXCINFO_PRINTK("system memcheck over, all passed!\n");
    }
#ifdef LOSCFG_EXC_INTERACTION
    if (LOS_MemIntegrityCheck(m_aucSysMem0) == LOS_OK) {
        EXCINFO_PRINTK("exc interaction memcheck over, all passed!\n");
    }
#endif
}

LITE_OS_SEC_TEXT_MINOR UINTPTR LOS_MemLastUsedGet(VOID *pool)
{
    return OsHeapLastUsedGet(pool);
}

/* bestfit_little do not support get used block count */
UINT32 LOS_MemUsedBlksGet(VOID *pool)
{
    (VOID)pool;
    return OS_INVALID;
}

/* bestfit_little do not support get owner's task id */
UINT32 LOS_MemTaskIdGet(const VOID *ptr)
{
    (VOID)ptr;
    return OS_INVALID;
}
