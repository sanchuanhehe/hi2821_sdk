/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides xip porting template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13， Create file. \n
 */
#include "hal_xip.h"
#include "hal_xip_v150_regs_op.h"
#include "platform_core.h"
#include "debug_print.h"
#include "xip_porting.h"

uintptr_t g_xip_base_addr = (uintptr_t)XIP_CACHE_CTL_RB_BASE;

uint32_t xip_porting_get_index(void)
{
    return (uint32_t)USE_XIP_INDEX;
}

uintptr_t xip_porting_base_addr_get(void)
{
    return g_xip_base_addr;
}

void xip_error_interrupt_enable(void)
{
    PRINT("xip_error_interrupt_enable");
#if CORE == MASTER_BY_ALL
#if CHIP_FPGA

#else
    non_os_nmi_config(NMI_XIP_CTRL, true);
    non_os_nmi_config(NMI_XIP_CACHE, true);
#endif
#endif
}