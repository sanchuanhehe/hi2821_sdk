/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides systick porting template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-01， Create file. \n
 */
#include "hal_systick_v150.h"
#include "platform_core.h"
#include "systick_porting.h"

static uintptr_t g_systick_base_addr =  (uintptr_t)SYSTICK_BASE_ADDR;

uintptr_t systick_porting_base_addr_get(void)
{
    return g_systick_base_addr;
}

void systick_port_cali_xclk(void)
{
}