/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Mmu Pagetable Management Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-06-09
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

#include "arch/mmu.h"
#include "arch/mmu_pri.h"
#include "pagetable_manager.h"
#include "los_hwi.h"

#define TABLE_ADDR_ALIGNED __attribute__((aligned(LN_TABLE_ENTRY_NUM * sizeof(UINTPTR))))

STATIC TABLE_BSS_SECTION TABLE_ADDR_ALIGNED UINTPTR g_pageTables[LOSCFG_ARCH_MMU_PAGE_TBL_NUM * LN_TABLE_ENTRY_NUM];
STATIC TABLE_BSS_SECTION INT32 g_pteUsedCnt[LOSCFG_ARCH_MMU_PAGE_TBL_NUM] = {0};

UINT32 PageTableGetIdx(UINTPTR *pte)
{
    UINT32 tblIdx = ((UINTPTR)pte - (UINTPTR)g_pageTables) / LOSCFG_ARCH_ARM64_PAGE_SIZE;

    return tblIdx;
}

/* Modifying page table entry used count, which is no more than LN_TABLE_ENTRY_NUM. */
INT32 PageTableModifyEntryUsedCnt(UINT32 tblIdx, INT32 offset)
{
    if (offset != 0) {
        g_pteUsedCnt[tblIdx] += offset;
    }
    return g_pteUsedCnt[tblIdx];
}

STATIC INLINE VOID SetPageTableFree(UINT32 tblIdx)
{
    g_pteUsedCnt[tblIdx] = PAGE_TABLE_FREE;
}

STATIC INLINE VOID SetPageTableReady(UINT32 tblIdx)
{
    g_pteUsedCnt[tblIdx] = PAGE_TABLE_READY;
}

UINTPTR *PageTableAlloc(VOID)
{
    UINT32 i;
    UINTPTR *table = NULL;

    for (i = 0; i < LOSCFG_ARCH_MMU_PAGE_TBL_NUM; i++) {
        if (PageTableIsFree(i)) {
            table = &g_pageTables[i * LN_TABLE_ENTRY_NUM];
            MMU_LOG(LOS_INFO_LEVEL, "Allocating new page table index[%u], table addr:0x%lx\n", i, (UINTPTR)table);
            SetPageTableReady(i);
            return table;
        }
    }

    MMU_LOG(LOS_ERR_LEVEL, "No enough page tables space left.\n");
    return NULL;
}

VOID PageTableFree(UINTPTR *table)
{
    UINT32 i = PageTableGetIdx(table);

    MMU_LOG(LOS_INFO_LEVEL, "Recycling page table index[%u], pte addr:0x%lx, table addr:0x%lx\n", i,
        (UINTPTR)table, ALIGN_DOWN((UINTPTR)table, LOSCFG_ARCH_ARM64_PAGE_SIZE));
    SetPageTableFree(i);
}

VOID PageTableInitManager(VOID)
{
    UINT32 i;
    PageTableMemInit((UINTPTR)g_pageTables, sizeof(g_pageTables));
    for (i = 0; i < LOSCFG_ARCH_MMU_PAGE_TBL_NUM; i++) {
        SetPageTableFree(i);
    }
    MMU_LOG(LOS_INFO_LEVEL, "Page tables overview: total tables num %d, from 0x%lx to 0x%lx\n",
        LOSCFG_ARCH_MMU_PAGE_TBL_NUM, g_pageTables, (UINTPTR)g_pageTables + sizeof(g_pageTables));
}
