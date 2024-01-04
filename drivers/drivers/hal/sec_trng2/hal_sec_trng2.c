/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: HAL SEC TRNG2
 * Author: @CompanyNameTag
 * Create: 2020-01-20
 */

#include "hal_sec_trng2.h"
#include "hal_sec_common.h"
#include "platform_core.h"
#include "chip_io.h"
#include "std_def.h"

#define TRNG_OUTPUT_0_REG (TRNG_RB_BASE + 0x0)
#define TRNG_OUTPUT_1_REG (TRNG_RB_BASE + 0x4)
#define TRNG_OUTPUT_2_REG (TRNG_RB_BASE + 0x8)
#define TRNG_OUTPUT_3_REG (TRNG_RB_BASE + 0xc)
#define TRNG_STATUS_REG (TRNG_RB_BASE + 0x10)
#define TRNG_CONTROL_REG (TRNG_RB_BASE + 0x14)
#define TRNG_CONFIG_REG (TRNG_RB_BASE + 0x18)
#define TRNG_OSCL_EN_REG (TRNG_RB_BASE + 0xcc)
#define TRNG_READY_OFFSET 0
#define READY_MASK_OFFSET 0
#define ENABLE_TRNG_OFFSET 10
#define DRBG_EN_OFFSET 12
#define REQUEST_DATA_OFFSET 16
#define DATA_BLOCKS_OFFSET 4
#define REDATA_BLOCKS_OFFSET 0
#define DATA_BLOCKS_LENGTH 12
#define DATA_BLOCKS_MASK 0xfff
#define DATA_LENGTH 16
#define OSCL_EN_OFFSET 2

#if TRNG_WITH_SEC_COMMON == NO
#define TRNG_CLKEN_REG      (B_CTL_RB_BASE + 0x800)
#define TRNG_SOFT_RST_N_REG (B_CTL_RB_BASE + 0x80C)

#define TRNG_CLKEN_OFFSET        8
#define SOFT_RST_B_TRNG_N_OFFSET 4
#endif

void hal_sec_trng2_enable(void)
{
#if TRNG_WITH_SEC_COMMON == NO
    reg32_setbit(TRNG_SOFT_RST_N_REG, SOFT_RST_B_TRNG_N_OFFSET);
    reg32_setbit(TRNG_CLKEN_REG, TRNG_CLKEN_OFFSET);
#else
    hal_sec_comm_enable(SEC_TRNG2);
#endif
    reg32_setbit(TRNG_OSCL_EN_REG, OSCL_EN_OFFSET);
}

void hal_sec_trng2_disable(void)
{
    reg32_clrbit(TRNG_OSCL_EN_REG, OSCL_EN_OFFSET);
#if TRNG_WITH_SEC_COMMON == NO
    reg32_clrbit(TRNG_CLKEN_REG, TRNG_CLKEN_OFFSET);
#else
    hal_sec_comm_disable(SEC_TRNG2);
#endif
}

void hal_sec_trng2_start(trng2_cfg_reg_t trng2_cfg_para, uint32_t data_blocks)
{
    uint32_t result = ((data_blocks & DATA_BLOCKS_MASK) << DATA_BLOCKS_OFFSET) | BIT(REDATA_BLOCKS_OFFSET);
    writel(TRNG_CONFIG_REG, trng2_cfg_para.trng2_cfg);
    UNUSED(trng2_cfg_para);
    reg32_setbit(TRNG_CONTROL_REG, DRBG_EN_OFFSET);
    reg32_setbits(TRNG_CONTROL_REG, REQUEST_DATA_OFFSET, DATA_LENGTH, result);
    reg32_setbit(TRNG_CONTROL_REG, ENABLE_TRNG_OFFSET);
}

bool hal_sec_trng2_is_ready(void)
{
    return reg32_getbit(TRNG_STATUS_REG, TRNG_READY_OFFSET);
}

void hal_sec_trng2_clear_ready_ack(void)
{
    reg32_setbit(TRNG_STATUS_REG, TRNG_READY_OFFSET);
    reg32_clrbit(TRNG_STATUS_REG, TRNG_READY_OFFSET);
}

trng2_output_data_t hal_sec_trng2_output(void)
{
    trng2_output_data_t output_data;
    output_data.trng2_output_0 = reg32(TRNG_OUTPUT_0_REG);
    output_data.trng2_output_1 = reg32(TRNG_OUTPUT_1_REG);
    output_data.trng2_output_2 = reg32(TRNG_OUTPUT_2_REG);
    output_data.trng2_output_3 = reg32(TRNG_OUTPUT_3_REG);
    return output_data;
}
