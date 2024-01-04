/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "chip_io.h"
#include "hal_trng_v1.h"
#include "trng_porting.h"

#define M_CTL_BASE_ADDR              0x52000000
#define TRNG_BASE_ADDR               0x52009000
#define TRNG_SOFT_CTRL_BASE_ADDR     0x52009020
#define TRNG_SAMPLE_DIV_ADDR         0x52000FA0
#define TRNG_CLK_EN_OFFSET           0x40
#define TRNG_RING_EN_OFFSET          0xd80
#define TRNG_SOFT_RST_OFFSET         0x150
#define TRNG_SOFT_RST_BIT            2

#define trng_reg16(register)                (*(volatile uint16_t*)(uintptr_t)(register))
#define trng_reg16_setbit(register, pos)    (trng_reg16(register) |= ((uint16_t)((1U) << (uint16_t)(pos))))
#define trng_reg16_clrbit(register, pos)    (trng_reg16(register) &= ~((uint16_t)((1U) << (uint16_t)(pos))))

uintptr_t g_trng_v1_base_addr = (uintptr_t)TRNG_BASE_ADDR;

static void trng_set_clk_en(uint32_t clken)
{
    if (clken) {
        reg_setbit(M_CTL_BASE_ADDR, TRNG_CLK_EN_OFFSET, 0);
        reg_setbit(M_CTL_BASE_ADDR, TRNG_RING_EN_OFFSET, 0);
    } else {
        reg_clrbit(M_CTL_BASE_ADDR, TRNG_CLK_EN_OFFSET, 0);
        reg_clrbit(M_CTL_BASE_ADDR, TRNG_RING_EN_OFFSET, 0);
    }
}

static void trng_set_soft_reset(uint32_t soft_reset)
{
    UNUSED(soft_reset);
    reg_setbit(M_CTL_BASE_ADDR, TRNG_SOFT_RST_OFFSET, TRNG_SOFT_RST_BIT);
}

void trng_port_register_hal_funcs(void)
{
    hal_trng_register_funcs(hal_trng_v1_get_funcs());
}

void trng_port_unregister_hal_funcs(void)
{
    hal_trng_unregister_funcs();
}

hal_trng_funcs_t *trng_port_get_funcs(void)
{
    return hal_trng_get_funcs();
}

uintptr_t trng_get_base_addr(void)
{
    return g_trng_v1_base_addr;
}

void trng_port_set_clk_en(uint32_t clken)
{
    trng_set_clk_en(clken);
}

void trng_port_set_soft_reset(uint32_t soft_reset)
{
    trng_set_soft_reset(soft_reset);
}

void trng_port_set_clk_sample_en(uint32_t clk_en)
{
    if (clk_en == 0x1) {
        trng_reg16_setbit(TRNG_SAMPLE_DIV_ADDR, 0x5);
    } else {
        trng_reg16_clrbit(TRNG_SAMPLE_DIV_ADDR, 0x5);
    }
}