/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL xip source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13, Create file. \n
 */
#include <stdint.h>
#include "soc_osal.h"
#include "common_def.h"
#include "xip_porting.h"
#include "hal_xip.h"

#define FLASH_QRD_CMD 0xEB

static xip_mode_t g_xip_mode[XIP_MAX] = { XIP_MODE_DISABLE, XIP_MODE_DISABLE };
static xip_cache_count_t g_xip_cache_count;
void osal_dsb(void);
void osal_isb(void);

void hal_xip_auto_cg(bool en) __attribute__((weak, alias("hal_xip_auto")));

void hal_xip_auto(bool en)
{
    unused(en);
}

void hal_xip_init(void)
{
    hal_xip_v150_regs_init();
}

void hal_xip_deinit(void)
{
    hal_xip_v150_regs_deinit();
}

void hal_xip_clear_tag(void)
{
    hal_xip_reg_man_all_set_man_all_req(1);
    while (hal_xip_reg_man_all_get_man_all_done() != 1) { }
}

void hal_xip_enable(xip_id_t xip_index, uint8_t xip_mode_code, bool enable_addr_32bit, bool enable_wrap, bool cmd_mode)
{
    uint32_t irqs = osal_irq_lock();
    osal_dsb();
    osal_isb();
    /* XIP CACHE */
    hal_xip_reg_xip_icu_en_set_icu_en(1);
    hal_xip_reg_xip_monitor_sel_set_monitor_sel(0);
    hal_xip_reg_xip_cache_en_set_cache_en(1);

    /* invalid all cache */
    hal_xip_auto_cg(true);
    hal_xip_clear_tag();
    hal_xip_auto_cg(false);
    /* configure XIP_QSPI_READ_CTL */
    hal_xip_reg_xip_read_qspi_enable_set_read_qspi_enable(xip_index, 0);
    if (enable_wrap) {
        hal_xip_reg_cfg_wrap_operation_set_wrap_operation(xip_index, 1);
    }
    if (enable_addr_32bit) {
        hal_xip_reg_cfg_addr_24_32_set_addr_24_32(xip_index, 1);
    }

    hal_xip_reg_xip_mode_code_set_read_mode_code(xip_index, xip_mode_code);
    while (xip_mode_code != hal_xip_reg_xip_mode_code_get_read_mode_code(xip_index)) { }

    if (cmd_mode) {
        hal_xip_reg_cfg_flash_sel_set_flash_sel(xip_index, 1);
        hal_xip_reg_flash_read_cmd_set_flash_read_cmd(xip_index, FLASH_QRD_CMD);
    } else {
        hal_xip_reg_cfg_flash_sel_set_flash_sel(xip_index, 0);
    }
    hal_xip_reg_xip_read_error_resp_mask_set_all(xip_index, 0);
    /* chip_libra */
#if defined(CONFIG_XIP_ENABLE_READ_OVERTIME)
    hal_xip_reg_cfg_xip_read_over_time_l_set_data(xip_index, 0xFFFF);
    hal_xip_reg_cfg_xip_read_over_time_h_set_data(xip_index, 0xFFFF);
    hal_xip_reg_xip_cfg_wait_cnt0_set_wait_cnt(0xFF);
#endif  /* CONFIG_XIP_ENABLE_READ_OVERTIME */
    hal_xip_reg_xip_read_qspi_sync_set_read_qspi_sync(xip_index, 1);
    hal_xip_reg_xip_sub_diag_en_set_cache_diag_en(1);
    hal_xip_reg_xip_read_qspi_enable_set_read_qspi_enable(xip_index, 1);

    hal_xip_reg_cfg_cache2habm_over_time_h_t_set_data(0x1000);
    osal_dsb();
    osal_isb();
    osal_irq_restore(irqs);
}

void hal_xip_disable(xip_id_t xip_index)
{
    uint32_t irqs = osal_irq_lock();
    hal_xip_reg_xip_read_qspi_enable_set_read_qspi_enable(xip_index, 0);
    while (hal_xip_reg_xip_read_qspi_enable_get_read_qspi_enable(xip_index) != 0) { }
    if ((hal_xip_reg_xip_read_qspi_enable_get_read_qspi_enable(0) ||
        hal_xip_reg_xip_read_qspi_enable_get_read_qspi_enable(1)) == 0) {
        hal_xip_reg_xip_cache_en_set_cache_en(0);
    }
    osal_irq_restore(irqs);
}

bool hal_xip_is_enable(xip_id_t xip_index)
{
    return ((hal_xip_reg_xip_cache_en_get_cache_en() == 1) &&
            hal_xip_reg_xip_read_qspi_enable_get_read_qspi_enable(xip_index) == 1);
}

void hal_xip_soft_rst(void)
{
    uint32_t irqs = osal_irq_lock();
    /* Soft reset XIP SUB^M */
    hal_xip_reg_mem_soft_rst_n_set_rst(0x0);
    hal_xip_reg_mem_soft_rst_n_set_rst(0xFFFF);
    osal_irq_restore(irqs);
}


void hal_xip_cache_miss_en(void)
{
    uint32_t irqs = osal_irq_lock();
    hal_xip_reg_cfg_calculate_en_set_en(1);
    osal_irq_restore(irqs);
}

void hal_xip_cache_miss_load_lock(void)
{
    hal_xip_reg_cache_miss_load_set_miss_load(1);
}

uint64_t hal_xip_get_cache_total_count(void)
{
    uint64_t total_count;
    total_count = ((uint64_t)hal_xip_reg_cache_total_h_get_data() << 32U);
    total_count |= ((uint64_t)hal_xip_reg_cache_total_m_get_data() << 16U);
    total_count |= ((uint64_t)hal_xip_reg_cache_total_l_get_data());
    return total_count;
}

uint64_t hal_xip_get_cache_hit_count(void)
{
    uint64_t hit_count;
    hit_count = ((uint64_t)hal_xip_reg_cache_hit_h_get_data() << 32U);
    hit_count |= ((uint64_t)hal_xip_reg_cache_hit_m_get_data() << 16U);
    hit_count |= ((uint64_t)hal_xip_reg_cache_hit_l_get_data());
    return hit_count;
}

uint64_t hal_xip_get_cache_miss_count(void)
{
    uint64_t miss_count;
    miss_count = ((uint64_t)hal_xip_reg_cache_miss_h_get_data() << 32U);
    miss_count |= ((uint64_t)hal_xip_reg_cache_miss_m_get_data() << 16U);
    miss_count |= ((uint64_t)hal_xip_reg_cache_miss_l_get_data());
    return miss_count;
}

void hal_xip_get_cache_count(xip_cache_count_t *cache_count)
{
    uint32_t irqs = osal_irq_lock();
    uint64_t total_count, hit_count, miss_count;
    hal_xip_cache_miss_load_lock();
    total_count = hal_xip_get_cache_total_count();
    hit_count = hal_xip_get_cache_hit_count();
    miss_count = hal_xip_get_cache_miss_count();
    /* calculate singer period count */
    cache_count->total_count = total_count - g_xip_cache_count.total_count;
    cache_count->hit_count = hit_count - g_xip_cache_count.hit_count;
    cache_count->miss_count = miss_count - g_xip_cache_count.miss_count;
    /* update record */
    g_xip_cache_count.total_count = total_count;
    g_xip_cache_count.hit_count = hit_count;
    g_xip_cache_count.miss_count = miss_count;
    osal_irq_restore(irqs);
}

void hal_xip_opi_enable(uint8_t dummy_cycle)
{
    /* disable xip opi */
    hal_xip_reg_xip_write_read_enable_set_write_read_opi_en(0);
    /* config redundant ccount amd sync before enable xip opi */
    hal_xip_reg_write_redundant_cnt_set_redundant_cnt(dummy_cycle);
    hal_xip_reg_xip_write_read_sync_set_write_read_opi_en(1);
    hal_xip_reg_xip_write_read_enable_set_write_read_opi_en(1);
}

void hal_xip_opi_disable(void)
{
    /* disable xip opi */
    hal_xip_reg_xip_write_read_enable_set_write_read_opi_en(0);
}

void hal_xip_mask_ctrl_error_resp(xip_id_t xip_index, bool mask)
{
    if (mask) {
        /* xip opi error resp mask */
        hal_xip_reg_xip_write_read_error_resp_mask_set_all(0xF);
        /* xip qspi0 error resp mask */
        hal_xip_reg_xip_read_error_resp_mask_set_all(xip_index, 0xF);
    } else {
        /* xip opi error resp unmask */
        hal_xip_reg_xip_write_read_error_resp_mask_set_all(0);
        /* xip qspi0 error resp unmask */
        hal_xip_reg_xip_read_error_resp_mask_set_all(xip_index, 0);
    }
}

void hal_xip_mask_cache_error_resp(bool mask)
{
    if (mask) {
        /* xip cache error resp mask */
        hal_xip_reg_xip_cache_error_resp_mask_set_all(0xF);
    } else {
        /* xip cache error resp unmask */
        hal_xip_reg_xip_cache_error_resp_mask_set_all(0);
    }
}

void hal_xip_set_div(xip_bus_t bus, xip_div_t div)
{
    if (div >= XIP_DIV_MAX) {
        return;
    }
    hal_xip_reg_mem_clken0_set_qspi_div_en(bus, 0);
    hal_xip_reg_mem_div4_set_qspi_div_num(bus, (uint32_t)div);
    hal_xip_reg_mem_clken0_set_qspi_div_en(bus, 1);
}

uint8_t hal_xip_get_div(xip_bus_t bus)
{
    return (uint8_t)hal_xip_reg_mem_div4_get_qspi_div_num(bus);
}

void hal_xip_set_cur_mode(xip_id_t index, xip_mode_t mode)
{
    if (index >= XIP_MAX) { return; }
    unsigned int irq_sts = osal_irq_lock();
    g_xip_mode[index] = mode;
    osal_irq_restore(irq_sts);
}

xip_mode_t hal_xip_get_cur_mode(xip_id_t index)
{
    return g_xip_mode[index];
}

void hal_xip_config_interrupt_type(xip_id_t xip_index, xip_intr_types_t types)
{
#if (XIP_INT_BY_NMI == YES)
    if (types == XIP_INTR_NMI) {
        non_os_nmi_config(NMI_XIP_CTRL, true);
        non_os_nmi_config(NMI_XIP_CACHE, true);
        hal_xip_mask_ctrl_error_resp(xip_index, true);
        hal_xip_mask_cache_error_resp(true);
    } else {
        non_os_nmi_config(NMI_XIP_CTRL, false);
        non_os_nmi_config(NMI_XIP_CACHE, false);
        hal_xip_mask_ctrl_error_resp(xip_index, false);
        hal_xip_mask_cache_error_resp(false);
    }
#endif
    unused(types);
}
