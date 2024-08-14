/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pmp porting template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-26， Create file. \n
 */

#include "hal_pmp_riscv31.h"
#include "pmp_porting.h"

void pmp_port_register_hal_funcs(void)
{
    hal_pmp_register_funcs(hal_pmp_riscv31_funcs_get());
}

void pmp_port_unregister_hal_funcs(void)
{
    hal_pmp_unregister_funcs();
}

/* first use pmp api, make sure reset cfg before use. */
void uapi_pmp_reset_cfg(void)
{
    /* set pmp reg to default value */
    write_csr(pmpcfg0, 0);
    write_csr(pmpcfg1, 0);
    write_csr(pmpcfg2, 0);
    write_csr(pmpcfg3, 0);
    write_custom_csr_val(MEMATTRL, 0);
}