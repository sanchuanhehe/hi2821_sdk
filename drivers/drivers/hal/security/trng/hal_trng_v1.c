/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng v1 hal drivers \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-05, Create file. \n
 */
#include "common_def.h"
#include "hal_trng_v1_regs_op.h"
#include "systick.h"
#include "hal_trng_v1.h"

#define TRNG_TIMEOUT_MS 3ULL

static errcode_t hal_trng_v1_init(void)
{
    if (hal_trng_v1_regs_init() != 0) {
        return ERRCODE_TRNG_REG_ADDR_INVALID;
    }
    return ERRCODE_SUCC;
}

static void hal_trng_v1_deinit(void)
{
    hal_trng_v1_regs_deinit();
}

static void hal_trng_v1_start(hal_trng_attr_t trng_attr)
{
    unused(trng_attr);
    hal_set_trng_ring_en_ro_en(0x1);
    hal_set_trng_ring_en_tero_en(0x1);
    hal_set_trng_ring_en_fro_en(0x1);
}

static bool hal_trng_v1_is_finish(void)
{
    if ((g_trng_v1_regs->trng_fifo_ready.b.trng_ready != 0) &&
        (g_trng_v1_regs->trng_fifo_ready.b.trng_done != 0)) {
        return true;
    }
    return false;
}

static errcode_t hal_trng_v1_get(trng_data_t *trng_data)
{
    uint64_t start_time = uapi_systick_get_ms();
    while (!hal_trng_v1_is_finish()) {
        if ((uapi_systick_get_ms() - start_time) > TRNG_TIMEOUT_MS) {
            return ERRCODE_TRNG_TIMEOUT;
        }
    }
    // trng fifo data读完会刷新
    trng_data->trng_data_0 = g_trng_v1_regs->trng_fifo_data;
    trng_data->trng_data_1 = g_trng_v1_regs->trng_fifo_data;
    trng_data->trng_data_2 = g_trng_v1_regs->trng_fifo_data;
    trng_data->trng_data_3 = g_trng_v1_regs->trng_fifo_data;
    return ERRCODE_SUCC;
}

static hal_trng_funcs_t g_hal_trng_v1_funcs = {
    .init = hal_trng_v1_init,
    .deinit = hal_trng_v1_deinit,
    .start = hal_trng_v1_start,
    .get_trng = hal_trng_v1_get
};

hal_trng_funcs_t *hal_trng_v1_get_funcs(void)
{
    return &g_hal_trng_v1_funcs;
}