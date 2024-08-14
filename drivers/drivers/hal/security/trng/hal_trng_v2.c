/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng v2 hal drivers \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-05, Create file. \n
 */
#include "hal_trng_v2_regs_op.h"
#include "systick.h"
#include "hal_trng_v2.h"

#define TRNG_TIMEOUT_MS 3

static errcode_t hal_trng_v2_init(void)
{
    if (hal_trng_v2_regs_init() != 0) {
        return ERRCODE_TRNG_REG_ADDR_INVALID;
    }
    hal_set_trng_oscl_oscl_en(0x1);
    return ERRCODE_SUCC;
}

static void hal_trng_v2_deinit(void)
{
    hal_set_trng_oscl_oscl_en(0x0);
    hal_trng_v2_regs_deinit();
}

static void hal_trng_v2_start(hal_trng_attr_t trng_attr)
{
    hal_set_trng_config_sample_cycles(trng_attr.sample_cycles);
    hal_set_trng_config_read_timeout(trng_attr.read_timeout);
    hal_set_trng_config_sample_div(trng_attr.sample_div);
    hal_set_trng_config_noise_blocks(trng_attr.noise_blocks);

    hal_set_trng_control_drbg_en(0x1);
    hal_set_trng_control_data_blocks(trng_attr.data_blocks);
    hal_set_trng_control_enable_trng(0x1);
}

static bool hal_trng_v2_is_finish(void)
{
    return g_trng_v2_regs->trng_status.b.ready;
}

static errcode_t hal_trng_v2_get(trng_data_t *trng_data)
{
    uint64_t start_time = uapi_systick_get_ms();
    while (!hal_trng_v2_is_finish()) {
        if ((uapi_systick_get_ms() - start_time) >= TRNG_TIMEOUT_MS) {
            return ERRCODE_TRNG_TIMEOUT;
        }
    }
    trng_data->trng_data_0 = g_trng_v2_regs->trng_data_0;
    trng_data->trng_data_1 = g_trng_v2_regs->trng_data_1;
    trng_data->trng_data_2 = g_trng_v2_regs->trng_data_2;
    trng_data->trng_data_3 = g_trng_v2_regs->trng_data_3;
    return ERRCODE_SUCC;
}

static hal_trng_funcs_t g_hal_trng_v2_funcs = {
    .init = hal_trng_v2_init,
    .deinit = hal_trng_v2_deinit,
    .start = hal_trng_v2_start,
    .get_trng = hal_trng_v2_get
};

hal_trng_funcs_t *hal_trng_v2_get_funcs(void)
{
    return &g_hal_trng_v2_funcs;
}