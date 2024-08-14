/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V150 xip register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13， Create file. \n
 */
#ifndef HAL_XIP_V150_REGS_OP_H
#define HAL_XIP_V150_REGS_OP_H

#include <stdint.h>
#include "hal_xip_v150_regs_def.h"
#include "xip_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_xip_v150_regs_op XIP V150 Regs Operation
 * @ingroup  drivers_hal_xip
 * @{
 */

extern xip_regs_t *g_xip_regs;

/**
 * @brief  Init the xip which will set the base address of registers.
 */
void hal_xip_v150_regs_init(void);

/**
 * @brief  Deinit the hal_xip which will clear the base address of registers has been
 *         set by @ref hal_xip_v150_regs_init.
 */
void hal_xip_v150_regs_deinit(void);

/**
 * @brief  Set the value of @ref mem_div4_data_t.
 * @param  [in]  bus The bus of xip. @ref xip_bus_t.
 * @param  [in]  val The value of @ref mem_div4_data_t.
 */
void hal_xip_reg_mem_div4_set_qspi_div_num(xip_bus_t bus, uint32_t val);

/**
 * @brief  Set the value of @ref mem_clken_data_t.
 * @param  [in]  bus The bus of xip. @ref xip_bus_t.
 * @param  [in]  val The value of @ref mem_clken_data_t.
 */
void hal_xip_reg_mem_clken0_set_qspi_div_en(xip_bus_t bus, uint32_t val);

/**
 * @brief  get the value of @ref mem_div4_data_t.
 * @param  [in]  bus The bus of xip. @ref xip_bus_t.
 * @return The value of @ref mem_div4_data_t.
 */
uint32_t hal_xip_reg_mem_div4_get_qspi_div_num(xip_bus_t bus);

/**
 * @brief  Set the value of @ref mem_clken_data_t.qspi0_div_clken.
 * @param  [in]  val The value of @ref mem_clken_data_t.qspi0_div_clken.
 */
static inline void hal_xip_reg_mem_clken0_set_qspi0_clken(uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    mem_clken0.b.qspi0_div_clken = val;
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

/**
 * @brief  Set the value of @ref mem_clken_data_t.qspi0_div_en.
 * @param  [in]  val The value of @ref mem_clken_data_t.qspi0_div_en.
 */
static inline void hal_xip_reg_mem_clken0_set_qspi0_div_en(uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    mem_clken0.b.qspi0_div_en = val;
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

/**
 * @brief  Set the value of @ref mem_clken_data_t.qspi1_div_en.
 * @param  [in]  val The value of @ref mem_clken_data_t.qspi1_div_en.
 */
static inline void hal_xip_reg_mem_clken0_set_qspi1_div_en(uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    mem_clken0.b.qspi1_div_en = val;
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

/**
 * @brief  Set the value of @ref mem_clken_data_t.qspi3_div_en.
 * @param  [in]  val The value of @ref mem_clken_data_t.qspi3_div_en.
 */
static inline void hal_xip_reg_mem_clken0_set_qspi3_div_en(uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    mem_clken0.b.qspi3_div_en = val;
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

/**
 * @brief  Set the value of @ref mem_clken_data_t.qspi1_div_clken.
 * @param  [in]  val The value of @ref mem_clken_data_t.qspi1_div_clken.
 */
static inline void hal_xip_reg_mem_clken0_set_qspi1_clken(uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    mem_clken0.b.qspi1_div_clken = val;
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

/**
 * @brief  Set the value of @ref mem_clken_data_t.qspi3_div_clken.
 * @param  [in]  val The value of @ref mem_clken_data_t.qspi3_div_clken.
 */
static inline void hal_xip_reg_mem_clken0_set_qspi3_clken(uint32_t val)
{
    mem_clken_data_t mem_clken0;
    mem_clken0.d32 = g_xip_regs->mem_clken0;
    mem_clken0.b.qspi3_div_clken = val;
    g_xip_regs->mem_clken0 = mem_clken0.d32;
}

/**
 * @brief  Set the value of @ref mem_soft_rst_n_data_t.
 * @param  [in]  val The value of @ref mem_soft_rst_n_data_t.
 */
static inline void hal_xip_reg_mem_soft_rst_n_set_rst(uint32_t val)
{
    mem_soft_rst_n_data_t mem_soft_rst_n;
    mem_soft_rst_n.d32 = val;
    g_xip_regs->mem_soft_rst_n = mem_soft_rst_n.d32;
}

/**
 * @brief  Set the value of @ref mem_div4_data_t.qspi0_div_num.
 * @param  [in]  val The value of @ref mem_div4_data_t.qspi0_div_num.
 */
static inline void hal_xip_reg_mem_div4_set_qspi0_div_num(uint32_t val)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    mem_div4.b.qspi0_div_num = val;
    g_xip_regs->mem_div4 = mem_div4.d32;
}

/**
 * @brief  get the value of @ref mem_div4_data_t.qspi0_div_num.
 * @return The value of @ref mem_div4_data_t.qspi0_div_num.
 */
static inline uint32_t hal_xip_reg_mem_div4_get_qspi0_div_num(void)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    return mem_div4.b.qspi0_div_num;
}

/**
 * @brief  Set the value of @ref mem_div4_data_t.qspi0_div_num.
 * @param  [in]  val The value of @ref mem_div4_data_t.qspi0_div_num.
 */
static inline void hal_xip_reg_mem_div4_set_qspi1_div_num(uint32_t val)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    mem_div4.b.qspi1_div_num = val;
    g_xip_regs->mem_div4 = mem_div4.d32;
}

/**
 * @brief  get the value of @ref mem_div4_data_t.qspi1_div_num.
 * @return The value of @ref mem_div4_data_t.qspi1_div_num.
 */
static inline uint32_t hal_xip_reg_mem_div4_get_qspi1_div_num(void)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    return mem_div4.b.qspi1_div_num;
}

/**
 * @brief  Set the value of @ref mem_div4_data_t.qspi0_div_num.
 * @param  [in]  val The value of @ref mem_div4_data_t.qspi0_div_num.
 */
static inline void hal_xip_reg_mem_div4_set_qspi3_div_num(uint32_t val)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    mem_div4.b.qspi3_div_num = val;
    g_xip_regs->mem_div4 = mem_div4.d32;
}

/**
 * @brief  get the value of @ref mem_div4_data_t.qspi3_div_num.
 * @return The value of @ref mem_div4_data_t.qspi3_div_num.
 */
static inline uint32_t hal_xip_reg_mem_div4_get_qspi3_div_num(void)
{
    mem_div4_data_t mem_div4;
    mem_div4.d32 = g_xip_regs->mem_div4;
    return mem_div4.b.qspi3_div_num;
}

/**
 * @brief  Set the value of @ref xip_cache_en_data_t.cache_en.
 * @param  [in]  val The value of @ref xip_cache_en_data_t.cache_en.
 */
static inline void hal_xip_reg_xip_cache_en_set_cache_en(uint32_t val)
{
    xip_cache_en_data_t xip_cache_en;
    xip_cache_en.d32 = g_xip_regs->xip_cache_en;
    xip_cache_en.b.cache_en = val;
    g_xip_regs->xip_cache_en = xip_cache_en.d32;
}

/**
 * @brief  get the value of @ref xip_cache_en_data_t.cache_en.
 * @return The value of @ref xip_cache_en_data_t.cache_en.
 */
static inline uint32_t hal_xip_reg_xip_cache_en_get_cache_en(void)
{
    xip_cache_en_data_t xip_cache_en;
    xip_cache_en.d32 = g_xip_regs->xip_cache_en;
    return xip_cache_en.b.cache_en;
}

/**
 * @brief  Set the value of @ref xip_monitor_sel_data_t.monitor_sel.
 * @param  [in]  val The value of @ref xip_monitor_sel_data_t.monitor_sel.
 */
static inline void hal_xip_reg_xip_monitor_sel_set_monitor_sel(uint32_t val)
{
    xip_monitor_sel_data_t xip_monitor_sel;
    xip_monitor_sel.d32 = g_xip_regs->xip_monitor_sel;
    xip_monitor_sel.b.monitor_sel = val;
    g_xip_regs->xip_monitor_sel = xip_monitor_sel.d32;
}

/**
 * @brief  Set the value of @ref xip_icu_en_data_t.icu_en.
 * @param  [in]  val The value of @ref xip_icu_en_data_t.icu_en.
 */
static inline void hal_xip_reg_xip_icu_en_set_icu_en(uint32_t val)
{
    xip_icu_en_data_t xip_icu_en;
    xip_icu_en.d32 = g_xip_regs->xip_icu_en;
    xip_icu_en.b.icu_en = val;
    g_xip_regs->xip_icu_en = xip_icu_en.d32;
}

/**
 * @brief  Set the value of @ref cfg_cache2habm_over_time_data_t.data.
 * @param  [in]  val The value of @ref cfg_cache2habm_over_time_data_t.data.
 */
static inline void hal_xip_reg_cfg_cache2habm_over_time_h_t_set_data(uint32_t val)
{
    cfg_cache2habm_over_time_data_t cfg_cache2habm_over_time_h;
    cfg_cache2habm_over_time_h.d32 = g_xip_regs->cfg_cache2habm_over_time_h;
    cfg_cache2habm_over_time_h.b.data = val;
    g_xip_regs->cfg_cache2habm_over_time_h = cfg_cache2habm_over_time_h.d32;
}

/**
 * @brief  Set the value of @ref xip_cache_error_resp_mask_data_t.
 * @param  [in]  val The value of @ref xip_cache_error_resp_mask_data_t.
 */
static inline void hal_xip_reg_xip_cache_error_resp_mask_set_all(uint32_t val)
{
    xip_cache_error_resp_mask_data_t xip_cache_error_resp_mask;
    xip_cache_error_resp_mask.d32 = val;
    g_xip_regs->xip_cache_error_resp_mask = xip_cache_error_resp_mask.d32;
}

/**
 * @brief  Set the value of @ref cfg_calculate_en_data_t.en.
 * @param  [in]  val The value of @ref cfg_calculate_en_data_t.en.
 */
static inline void hal_xip_reg_cfg_calculate_en_set_en(uint32_t val)
{
    cfg_calculate_en_data_t cfg_calculate_en;
    cfg_calculate_en.d32 = g_xip_regs->cfg_calculate_en;
    cfg_calculate_en.b.en = val;
    g_xip_regs->cfg_calculate_en = cfg_calculate_en.d32;
}

/**
 * @brief  Set the value of @ref cache_miss_load_data_t.miss_load.
 * @param  [in]  val The value of @ref cache_miss_load_data_t.miss_load.
 */
static inline void hal_xip_reg_cache_miss_load_set_miss_load(uint32_t val)
{
    cache_miss_load_data_t cache_miss_load;
    cache_miss_load.d32 = g_xip_regs->cache_miss_load;
    cache_miss_load.b.miss_load = val;
    g_xip_regs->cache_miss_load = cache_miss_load.d32;
}

/**
 * @brief  Get the value of @ref cache_miss_load_data_t.miss_load.
 * @return The value of @ref cache_miss_load_data_t.miss_load.
 */
static inline uint32_t hal_xip_reg_cache_miss_load_get_miss_load(void)
{
    cache_miss_load_data_t cache_miss_load;
    cache_miss_load.d32 = g_xip_regs->cache_miss_load;
    return cache_miss_load.b.miss_load;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_total_h_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_total_h;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_total_m_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_total_m;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_total_l_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_total_l;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_miss_h_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_miss_h;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_miss_m_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_miss_m;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_miss_l_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_miss_l;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_hit_h_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_hit_h;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_hit_m_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_hit_m;
    return cache_data.b.data;
}

/**
 * @brief  Get the value of @ref cache_data_t.data.
 * @return  The value of @ref cache_data_t.data.
 */
static inline uint32_t hal_xip_reg_cache_hit_l_get_data(void)
{
    cache_data_t cache_data;
    cache_data.d32 = g_xip_regs->cache_hit_l;
    return cache_data.b.data;
}
/**
 * @brief  Set the value of @ref man_all_data_t.man_all_req.
 * @param  [in]  val The value of @ref man_all_data_t.man_all_req.
 */
static inline void hal_xip_reg_man_all_set_man_all_req(uint32_t val)
{
    man_all_data_t man_all;
    man_all.d32 = g_xip_regs->man_all;
    man_all.b.man_all_req = val;
    g_xip_regs->man_all = man_all.d32;
}

/**
 * @brief  Get the value of @ref man_all_data_t.man_all_done.
 * @return The value of @ref man_all_data_t.man_all_done.
 */
static inline uint32_t hal_xip_reg_man_all_get_man_all_done(void)
{
    man_all_data_t man_all;
    man_all.d32 = g_xip_regs->man_all;
    return man_all.b.man_all_done;
}

/**
 * @brief  Set the value of @ref xip_write_read_enable_data_t.write_read_opi_en.
 * @param  [in]  val The value of @ref xip_write_read_enable_data_t.write_read_opi_en.
 */
static inline void hal_xip_reg_xip_write_read_enable_set_write_read_opi_en(uint32_t val)
{
    xip_write_read_enable_data_t xip_write_read_enable;
    xip_write_read_enable.d32 = g_xip_regs->xip_write_read_enable;
    xip_write_read_enable.b.write_read_opi_en = val;
    g_xip_regs->xip_write_read_enable = xip_write_read_enable.d32;
}

/**
 * @brief  Set the value of @ref xip_write_read_sync_data_t.write_read_opi_sync.
 * @param  [in]  val The value of @ref xip_write_read_sync_data_t.write_read_opi_sync.
 */
static inline void hal_xip_reg_xip_write_read_sync_set_write_read_opi_en(uint32_t val)
{
    xip_write_read_sync_data_t xip_write_read_sync;
    xip_write_read_sync.d32 = g_xip_regs->xip_write_read_sync;
    xip_write_read_sync.b.write_read_opi_sync = val;
    g_xip_regs->xip_write_read_sync = xip_write_read_sync.d32;
}

/**
 * @brief  Set the value of @ref write_redundant_cnt_data_t.redundant_cnt.
 * @param  [in]  val The value of @ref write_redundant_cnt_data_t.redundant_cnt.
 */
static inline void hal_xip_reg_write_redundant_cnt_set_redundant_cnt(uint32_t val)
{
    write_redundant_cnt_data_t write_redundant_cnt;
    write_redundant_cnt.d32 = g_xip_regs->write_redundant_cnt;
    write_redundant_cnt.b.redundant_cnt = val;
    g_xip_regs->write_redundant_cnt = write_redundant_cnt.d32;
}

/**
 * @brief  Set the value of @ref xip_write_read_error_resp_mask_data_t.
 * @param  [in]  val The value of @ref xip_write_read_error_resp_mask_data_t.
 */
static inline void hal_xip_reg_xip_write_read_error_resp_mask_set_all(uint32_t val)
{
    xip_write_read_error_resp_mask_data_t xip_write_read_error_resp_mask;
    xip_write_read_error_resp_mask.d32 = val;
    g_xip_regs->xip_cache_error_resp_mask = xip_write_read_error_resp_mask.d32;
}

/**
 * @brief  Set the value of @ref xip_read_qspi_enable_data_t.read_qspi_enable.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref xip_read_qspi_enable_data_t.read_qspi_enable.
 */
static inline void hal_xip_reg_xip_read_qspi_enable_set_read_qspi_enable(uint32_t idx, uint32_t val)
{
    xip_read_qspi_enable_data_t xip_read_qspi_enable;
    xip_read_qspi_enable.d32 = g_xip_regs->xip_qspi[idx].xip_read_qspi_enable;
    xip_read_qspi_enable.b.read_qspi_enable = val;
    g_xip_regs->xip_qspi[idx].xip_read_qspi_enable = xip_read_qspi_enable.d32;
}

/**
 * @brief  Get the value of @ref xip_read_qspi_enable_data_t.read_qspi_enable.
 * @param  [in]  idx The idx of qspi.
 * @return The value of @ref xip_read_qspi_enable_data_t.read_qspi_enable.
 */
static inline uint32_t hal_xip_reg_xip_read_qspi_enable_get_read_qspi_enable(uint32_t idx)
{
    xip_read_qspi_enable_data_t xip_read_qspi_enable;
    xip_read_qspi_enable.d32 = g_xip_regs->xip_qspi[idx].xip_read_qspi_enable;
    return xip_read_qspi_enable.b.read_qspi_enable;
}

/**
 * @brief  Set the value of @ref xip_read_qspi_sync_data_t.read_qspi_sync.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref xip_read_qspi_sync_data_t.read_qspi_sync.
 */
static inline void hal_xip_reg_xip_read_qspi_sync_set_read_qspi_sync(uint32_t idx, uint32_t val)
{
    xip_read_qspi_sync_data_t xip_read_qspi_sync;
    xip_read_qspi_sync.d32 = g_xip_regs->xip_qspi[idx].xip_read_qspi_sync;
    xip_read_qspi_sync.b.read_qspi_sync = val;
    g_xip_regs->xip_qspi[idx].xip_read_qspi_sync = xip_read_qspi_sync.d32;
}

/**
 * @brief  Set the value of @ref cfg_wrap_operation_data_t.wrap_operation.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref cfg_wrap_operation_data_t.wrap_operation.
 */
static inline void hal_xip_reg_cfg_wrap_operation_set_wrap_operation(uint32_t idx, uint32_t val)
{
    cfg_wrap_operation_data_t cfg_wrap_operation;
    cfg_wrap_operation.d32 = g_xip_regs->xip_qspi[idx].cfg_wrap_operation;
    cfg_wrap_operation.b.wrap_operation = val;
    g_xip_regs->xip_qspi[idx].cfg_wrap_operation = cfg_wrap_operation.d32;
}

/**
 * @brief  Set the value of @ref cfg_addr_24_32_data_t.addr_24_32.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref cfg_addr_24_32_data_t.addr_24_32.
 */
static inline void hal_xip_reg_cfg_addr_24_32_set_addr_24_32(uint32_t idx, uint32_t val)
{
    cfg_addr_24_32_data_t cfg_addr_24_32;
    cfg_addr_24_32.d32 = g_xip_regs->xip_qspi[idx].cfg_addr_24_32;
    cfg_addr_24_32.b.addr_24_32 = val;
    g_xip_regs->xip_qspi[idx].cfg_addr_24_32 = cfg_addr_24_32.d32;
}

/**
 * @brief  Set the value of @ref cfg_flash_sel_data_t.flash_sel.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref cfg_flash_sel_data_t.flash_sel.
 */
static inline void hal_xip_reg_cfg_flash_sel_set_flash_sel(uint32_t idx, uint32_t val)
{
    cfg_flash_sel_data_t cfg_flash_sel;
    cfg_flash_sel.d32 = g_xip_regs->xip_qspi[idx].cfg_flash_sel;
    cfg_flash_sel.b.flash_sel = val;
    g_xip_regs->xip_qspi[idx].cfg_flash_sel = cfg_flash_sel.d32;
}

/**
 * @brief  Set the value of @ref xip_mode_code_data_t.read_mode_code.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref xip_mode_code_data_t.read_mode_code.
 */
static inline void hal_xip_reg_xip_mode_code_set_read_mode_code(uint32_t idx, uint32_t val)
{
    xip_mode_code_data_t xip_mode_code;
    xip_mode_code.d32 = g_xip_regs->xip_qspi[idx].xip_mode_code;
    xip_mode_code.b.read_mode_code = val;
    g_xip_regs->xip_qspi[idx].xip_mode_code = xip_mode_code.d32;
}

/**
 * @brief  Get the value of @ref xip_mode_code_data_t.read_mode_code.
 * @param  [in]  idx The idx of qspi.
 * @return The value of @ref xip_mode_code_data_t.read_mode_code.
 */
static inline uint32_t hal_xip_reg_xip_mode_code_get_read_mode_code(uint32_t idx)
{
    xip_mode_code_data_t xip_mode_code;
    xip_mode_code.d32 = g_xip_regs->xip_qspi[idx].xip_mode_code;
    return xip_mode_code.b.read_mode_code;
}

/**
 * @brief  Set the value of @ref flash_read_cmd_data_t.flash_read_cmd.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref flash_read_cmd_data_t.flash_read_cmd.
 */
static inline void hal_xip_reg_flash_read_cmd_set_flash_read_cmd(uint32_t idx, uint32_t val)
{
    flash_read_cmd_data_t flash_read_cmd;
    flash_read_cmd.d32 = g_xip_regs->xip_qspi[idx].flash_read_cmd;
    flash_read_cmd.b.flash_read_cmd = val;
    g_xip_regs->xip_qspi[idx].flash_read_cmd = flash_read_cmd.d32;
}

/**
 * @brief  Set the value of @ref cfg_xip_read_over_time_data_t.data.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref cfg_xip_read_over_time_data_t.data.
 */
static inline void hal_xip_reg_cfg_xip_read_over_time_l_set_data(uint32_t idx, uint32_t val)
{
    cfg_xip_read_over_time_data_t cfg_xip_read_over_time_l;
    cfg_xip_read_over_time_l.d32 = g_xip_regs->xip_qspi[idx].cfg_xip_read_over_time_l;
    cfg_xip_read_over_time_l.b.data = val;
    g_xip_regs->xip_qspi[idx].cfg_xip_read_over_time_l = cfg_xip_read_over_time_l.d32;
}

/**
 * @brief  Set the value of @ref cfg_xip_read_over_time_data_t.data.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref cfg_xip_read_over_time_data_t.data.
 */
static inline void hal_xip_reg_cfg_xip_read_over_time_h_set_data(uint32_t idx, uint32_t val)
{
    cfg_xip_read_over_time_data_t cfg_xip_read_over_time_h;
    cfg_xip_read_over_time_h.d32 = g_xip_regs->xip_qspi[idx].cfg_xip_read_over_time_h;
    cfg_xip_read_over_time_h.b.data = val;
    g_xip_regs->xip_qspi[idx].cfg_xip_read_over_time_h = cfg_xip_read_over_time_h.d32;
}

/**
 * @brief  Set the value of @ref xip_read_error_resp_mask_data_t.
 * @param  [in]  idx The idx of qspi.
 * @param  [in]  val The value of @ref xip_read_error_resp_mask_data_t.
 */
static inline void hal_xip_reg_xip_read_error_resp_mask_set_all(uint32_t idx, uint32_t val)
{
    xip_read_error_resp_mask_data_t xip_read_error_resp_mask;
    xip_read_error_resp_mask.d32 = val;
    g_xip_regs->xip_qspi[idx].xip_read_error_resp_mask = xip_read_error_resp_mask.d32;
}

/**
 * @brief  Set the value of @ref xip_sub_diag_en_data_t.cache_diag_en.
 * @param  [in]  val The value of @ref xip_sub_diag_en_data_t.cache_diag_en.
 */
static inline void hal_xip_reg_xip_sub_diag_en_set_cache_diag_en(uint32_t val)
{
    xip_sub_diag_en_data_t xip_sub_diag_en;
    xip_sub_diag_en.d32 = g_xip_regs->xip_sub_diag_en;
    xip_sub_diag_en.b.cache_diag_en = val;
    g_xip_regs->xip_sub_diag_en = xip_sub_diag_en.d32;
}

/**
 * @brief  Set the value of @ref xip_cfg_wait_cnt0_data_t.wait_cnt.
 * @param  [in]  val The value of @ref xip_cfg_wait_cnt0_data_t.wait_cnt.
 */
static inline void hal_xip_reg_xip_cfg_wait_cnt0_set_wait_cnt(uint32_t val)
{
    xip_cfg_wait_cnt0_data_t xip_cfg_wait_cnt0;
    xip_cfg_wait_cnt0.d32 = g_xip_regs->xip_cfg_wait_cnt0;
    xip_cfg_wait_cnt0.b.wait_cnt = val;
    g_xip_regs->xip_cfg_wait_cnt0 = xip_cfg_wait_cnt0.d32;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif