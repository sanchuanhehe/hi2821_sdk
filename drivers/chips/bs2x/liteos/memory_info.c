/*
 * Copyright (c) @CompanyNameMagicTag 202-2023. All rights reserved.
 * Description:  LiteOs Heap and Stack info
 * Author: @CompanyNameTag
 * Create: 2022-09-27
 */

#include <stdint.h>
#include "log_uart.h"
#include "log_common.h"
#include "log_printf.h"
#include "log_def.h"
#include "los_task_pri.h"
#include "los_memory.h"
#include "chip_io.h"
#include "memory_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void print_stack_waterline_riscv(void)
{
    if (readl(MEMORY_INFO_CRTL_REG) == MEMORY_INFO_CLOSE) { return; }
    TSK_INFO_S taskinfo;
    uint32_t ret = 0;
    for (uint32_t loop = 0; loop < g_taskMaxNum + 1; loop++) {
        ret = LOS_TaskInfoGet(loop, &taskinfo);
        if (ret != LOS_OK) {
            continue;
        }
        if ((taskinfo.usTaskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
        oml_pf_log_print_alter(LOG_BCORE_PLT_INFO_STACK, LOG_NUM_INFO_STACK, LOG_LEVEL_INFO, \
            "[STACK] id:%d, top:0x%x, size:0x%x, task usage peak:%x, sp:0x%x", FIVE_ARG, \
            (uint32_t)(taskinfo.uwTaskID), (uint32_t)(taskinfo.uwTopOfStack), \
            (uint32_t)(taskinfo.uwStackSize), (uint32_t)(taskinfo.uwPeakUsed), (uint32_t)((uintptr_t)(taskinfo.uwSP)));
    }
}

void print_heap_statistics_riscv(void)
{
    if (readl(MEMORY_INFO_CRTL_REG) == MEMORY_INFO_CLOSE) { return; }
    LOS_MEM_POOL_STATUS status;

    LOS_MemInfoGet(OS_SYS_MEM_ADDR, &status);
    oml_pf_log_print4(LOG_BCORE_PLT_INFO_HEAP, LOG_NUM_INFO_HEAP, LOG_LEVEL_INFO,
                      "[HEAP_STAT1] total:0x%x, used:0x%x, free:0x%x, usage waterline:0x%x",
                      (uint32_t)(status.uwTotalFreeSize + status.uwTotalUsedSize),
                      (uint32_t)(status.uwTotalUsedSize), (uint32_t)(status.uwTotalFreeSize),
                      (uint32_t)(status.uwUsageWaterLine));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */