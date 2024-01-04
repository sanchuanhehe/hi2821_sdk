/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V150 xip register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13, Create file. \n
 */
#include <stdint.h>
#include "common_def.h"
#include "xip_porting.h"
#include "hal_xip_v150_regs_op.h"

xip_regs_t *g_xip_regs = NULL;

void hal_xip_v150_regs_init(void)
{
    g_xip_regs = (xip_regs_t *)xip_porting_base_addr_get();
}

void hal_xip_v150_regs_deinit(void)
{
    g_xip_regs = NULL;
}

void hal_xip_reg_mem_div4_set_qspi_div_num(xip_bus_t bus, uint32_t val)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    switch (bus) {
        case BUS_QSPI0:
            mem_div4.b.qspi0_div_num = val;
            break;
        case BUS_QSPI1:
            mem_div4.b.qspi1_div_num = val;
            break;
        case BUS_QSPI3:
            mem_div4.b.qspi3_div_num = val;
            break;
        default:
            break;
    }
    g_xip_regs->mem_div4 = mem_div4.d32;
}

void hal_xip_reg_mem_clken0_set_qspi_div_en(xip_bus_t bus, uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    switch (bus) {
        case BUS_QSPI0:
            mem_clken0.b.qspi0_div_en = val;
            break;
        case BUS_QSPI1:
            mem_clken0.b.qspi1_div_en = val;
            break;
        case BUS_QSPI3:
            mem_clken0.b.qspi3_div_en = val;
            break;
        default:
            break;
    }
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

uint32_t hal_xip_reg_mem_div4_get_qspi_div_num(xip_bus_t bus)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    switch (bus) {
        case BUS_QSPI0:
            return mem_div4.b.qspi0_div_num;
        case BUS_QSPI1:
            return mem_div4.b.qspi1_div_num;
        case BUS_QSPI3:
            return mem_div4.b.qspi3_div_num;
        default:
            return 0;
    }
}