/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides V151 i2c register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15, Create file. \n
 */
#include <stdint.h>
#include "common_def.h"
#include "i2c_porting.h"
#include "hal_i2c_v151_regs_op.h"

i2c_v151_regs_t *g_i2c_v151_regs[I2C_BUS_MAX_NUM] = { 0 };

static inline uintptr_t hal_i2c_base_addrs_get(i2c_bus_t bus)
{
    return g_i2c_base_addrs[bus];
}

void hal_i2c_v151_regs_init(void)
{
    for (uint32_t bus = 0; bus < I2C_BUS_MAX_NUM; bus++) {
        g_i2c_v151_regs[bus] = (i2c_v151_regs_t *)hal_i2c_base_addrs_get(bus);
    }
}

void hal_i2c_v151_regs_deinit(void)
{
    for (uint32_t bus = 0; bus < I2C_BUS_MAX_NUM; bus++) {
        g_i2c_v151_regs[bus] = NULL;
    }
}

static volatile uint32_t *hal_i2c_v151_get_int_addr(i2c_bus_t bus, i2c_v151_int_reg_t reg)
{
    volatile uint32_t *reg_addr = NULL;

    switch (reg) {
        case I2C_V151_INTR_STAT:
            reg_addr = &g_i2c_v151_regs[bus]->intr_stat;
            break;
        case I2C_V151_INTR_MASK:
            reg_addr = &g_i2c_v151_regs[bus]->intr_mask;
            break;
        default:
            reg_addr = &g_i2c_v151_regs[bus]->raw_intr_stat;
            break;
    }
    return reg_addr;
}

uint32_t hal_i2c_v151_get_int(i2c_bus_t bus, i2c_v151_int_reg_t reg, i2c_v151_int_t int_id)
{
    volatile uint32_t *reg_addr = NULL;
    reg_addr = hal_i2c_v151_get_int_addr(bus, reg);

    return ((*reg_addr) >> ((uint32_t)int_id)) & 1;
}

void hal_i2c_v151_unmask_int(i2c_bus_t bus, i2c_v151_int_t int_id)
{
    ic_v151_intr_mask_data_t int_mask;
    int_mask.d32 = g_i2c_v151_regs[bus]->intr_mask;
    int_mask.d32 = int_mask.d32 | (1U << (uint32_t)int_id);
    g_i2c_v151_regs[bus]->intr_mask = int_mask.d32;
}

void hal_i2c_v151_mask_int(i2c_bus_t bus, i2c_v151_int_t int_id)
{
    ic_v151_intr_mask_data_t int_mask;
    int_mask.d32 = g_i2c_v151_regs[bus]->intr_mask;
    int_mask.d32 = int_mask.d32 & (~(1U << (uint32_t)int_id));
    g_i2c_v151_regs[bus]->intr_mask = int_mask.d32;
}

void hal_i2c_v151_mask_all_int(i2c_bus_t bus)
{
    g_i2c_v151_regs[bus]->intr_mask = 0;
}

static uint32_t hal_i2c_get_data_cmd(uint32_t first_flag, uint32_t restart_flag, uint32_t last_flag, uint32_t stop_flag)
{
    ic_v151_data_cmd_data_t data_cmd;
    data_cmd.d32 = 0;

    if ((first_flag == true) && (restart_flag == true)) {
        data_cmd.b.restart = true;
    }

    if ((last_flag == true) && (stop_flag == true)) {
        data_cmd.b.stop = true;
    }

    return data_cmd.d32;
}

uint32_t hal_i2c_v151_get_data_read_cmd(uint32_t first_flag, uint32_t restart_flag,
                                        uint32_t last_flag, uint32_t stop_flag)
{
    ic_v151_data_cmd_data_t data_cmd;

    data_cmd.d32 = hal_i2c_get_data_cmd(first_flag, restart_flag, last_flag, stop_flag);
    data_cmd.b.cmd = 1;

    return data_cmd.d32;
}

uint32_t hal_i2c_v151_get_data_write_cmd(uint32_t first_flag, uint32_t restart_flag,
                                         uint32_t last_flag, uint32_t stop_flag)
{
    ic_v151_data_cmd_data_t data_cmd;

    data_cmd.d32 = hal_i2c_get_data_cmd(first_flag, restart_flag, last_flag, stop_flag);
    data_cmd.b.cmd = 0;

    return data_cmd.d32;
}

void hal_i2c_v151_set_transmit_data(i2c_bus_t bus, uint32_t cmd_type, uint32_t val)
{
    uint32_t data = cmd_type | val;
    g_i2c_v151_regs[bus]->data_cmd = data;
}

void hal_i2c_v151_set_gc_mode(i2c_bus_t bus)
{
    ic_v151_tar_data_t data_cmd;

    data_cmd.d32 = g_i2c_v151_regs[bus]->tar;
    data_cmd.b.gc_or_start = 0;
    data_cmd.b.special = 1;

    g_i2c_v151_regs[bus]->tar = data_cmd.d32;
}

void hal_i2c_v151_set_normal_mode(i2c_bus_t bus)
{
    ic_v151_tar_data_t data_cmd;

    data_cmd.d32 = g_i2c_v151_regs[bus]->tar;
    data_cmd.b.gc_or_start = 0;
    data_cmd.b.special = 0;

    g_i2c_v151_regs[bus]->tar = data_cmd.d32;
}
