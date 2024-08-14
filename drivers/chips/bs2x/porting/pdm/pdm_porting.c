/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides PDM port for SUSONG \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-31, Create file. \n
 */
#include "hal_pdm.h"
#include "hal_pdm_v150.h"
#include "pdm_porting.h"

#define PDM_BASE_ADDR 0x5208E000

uintptr_t pdm_porting_base_addr_get(void)
{
    return (uintptr_t)PDM_BASE_ADDR;
}

void pdm_port_clock_enable(bool en)
{
    if (en) {
        uapi_reg_write32(0x52000548, 0x3ee);
        uapi_reg_write32(0x5200004c, 0x0);
        uapi_reg_write32(0x52000548, 0x7ee);
    } else {
        uapi_reg_write32(0x52000548, 0x3ee);
    }
}

void pdm_port_register_hal_funcs(void)
{
    hal_pdm_register_funcs(hal_pdm_v150_funcs_get());
}

void pdm_port_unregister_hal_funcs(void)
{
    hal_pdm_unregister_funcs();
}