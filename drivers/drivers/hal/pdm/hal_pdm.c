/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides hal pdm \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-29, Create file. \n
 */
#include "common_def.h"
#include "pdm_porting.h"
#include "hal_pdm.h"

uintptr_t g_hal_pdm_regs = 0;
static hal_pdm_funcs_t *g_hal_pdm_funcs = NULL;

errcode_t hal_pdm_regs_init(void)
{
    if (pdm_porting_base_addr_get() == 0) {
        return ERRCODE_PDM_REG_ADDR_INVALID;
    }
    g_hal_pdm_regs = pdm_porting_base_addr_get();
    return ERRCODE_SUCC;
}

void hal_pdm_regs_deinit(void)
{
    g_hal_pdm_regs = 0;
}

errcode_t hal_pdm_register_funcs(hal_pdm_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_pdm_funcs = funcs;
    return ERRCODE_SUCC;
}

void hal_pdm_unregister_funcs(void)
{
    g_hal_pdm_funcs = NULL;
}

hal_pdm_funcs_t *hal_pdm_get_funcs(void)
{
    return g_hal_pdm_funcs;
}