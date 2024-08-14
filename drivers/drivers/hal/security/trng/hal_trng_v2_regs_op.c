/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng v2 regs operation \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-05, Create file. \n
 */
#include <stdio.h>
#include "trng_porting.h"
#include "hal_trng_v2_regs_op.h"

trng_regs_v2_t *g_trng_v2_regs = NULL;

int32_t hal_trng_v2_regs_init(void)
{
    if (trng_get_base_addr() == 0) {
        return -1;
    }
    g_trng_v2_regs = (trng_regs_v2_t*)trng_get_base_addr();
    return 0;
}

void hal_trng_v2_regs_deinit(void)
{
    g_trng_v2_regs = NULL;
}