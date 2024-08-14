/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-09, Create file. \n
 */
#include "common_def.h"
#include "securec.h"
#include "non_os.h"
#include "oal_interface.h"
#if defined(CONFIG_PM_SUPPORT_LPC) && (CONFIG_PM_SUPPORT_LPC == YES)
#include "pm_lpc.h"
#endif
#include "pal_sec_config_port.h"
#include "trng_porting.h"
#include "hal_trng.h"
#include "drv_trng.h"

typedef hal_trng_attr_t trng_attr_t;

static void trng_pre_process(void)
{
    trng_port_set_soft_reset(0x1);
    trng_port_set_clk_en(0x1);
    trng_port_set_clk_sample_en(0x1);
}

static void trng_post_process(void)
{
    trng_port_set_soft_reset(0x0);
    trng_port_set_clk_en(0x0);
    trng_port_set_clk_sample_en(0x0);
}

static errcode_t trng_init(void)
{
#if defined(CONFIG_PM_SUPPORT_LPC) && (CONFIG_PM_SUPPORT_LPC == YES)
    pm_lpc_mclken_sec_enable(LPC_MCLKEN_SEC_USED_TRNG, true);
#endif
    trng_pre_process();
    trng_port_register_hal_funcs();
    hal_trng_funcs_t *hal_funcs = trng_port_get_funcs();
    if (hal_funcs == NULL) {
        return ERRCODE_TRNG_INVALID_PARAMETER;
    }
    return hal_funcs->init();
}

static void trng_deinit(const hal_trng_funcs_t *hal_funcs)
{
    hal_funcs->deinit();
    trng_port_unregister_hal_funcs();
    trng_post_process();
#if defined(CONFIG_PM_SUPPORT_LPC) && (CONFIG_PM_SUPPORT_LPC == YES)
    pm_lpc_mclken_sec_enable(LPC_MCLKEN_SEC_USED_TRNG, false);
#endif
}

static void trng_start(const hal_trng_funcs_t *hal_funcs)
{
    trng_attr_t trng_attr;
    trng_attr.sample_cycles = TRNG_SAMPLE_CYCLES;
    trng_attr.read_timeout = TRNG_READ_TIMEOUT;
    trng_attr.sample_div = TRNG_SAMPLE_DIV;
    trng_attr.noise_blocks = TRNG_NOISE_BLOCKS;
    trng_attr.data_blocks = TRNG_DATA_BLOCKS;
    hal_funcs->start(trng_attr);
}

static errcode_t get_trng(const hal_trng_funcs_t *hal_funcs, trng_data_t *trng_data)
{
    return hal_funcs->get_trng(trng_data);
}

errcode_t drv_cipher_trng_get_random(uint8_t *buffer, uint32_t buffer_len)
{
    uint8_t *buf = buffer;
    uint32_t buf_len = buffer_len;

    if ((buf == NULL) || (buf_len == 0)) {
        return ERRCODE_TRNG_INVALID_PARAMETER;
    }

    errcode_t ret = trng_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_trng_funcs_t *hal_funcs = trng_port_get_funcs();
    trng_start(hal_funcs);

    trng_data_t trng_data;
    while (buf_len > sizeof(trng_data)) {
        ret = get_trng(hal_funcs, &trng_data);
        if (ret != ERRCODE_SUCC) {
            goto exit;
        }

        if (memcpy_s(buf, buf_len, &trng_data, sizeof(trng_data)) != EOK) {
            ret = ERRCODE_MEMCPY;
            goto exit;
        }

        buf += sizeof(trng_data);
        buf_len -= (size_t)sizeof(trng_data);
    }

    ret = get_trng(hal_funcs, &trng_data);
    if (ret != ERRCODE_SUCC) {
        goto exit;
    }

    trng_deinit(hal_funcs);

    if (memcpy_s(buf, buf_len, &trng_data, buf_len) != EOK) {
        return ERRCODE_MEMCPY;
    }
    return ERRCODE_SUCC;

exit:
    trng_deinit(hal_funcs);
    return ret;
}
