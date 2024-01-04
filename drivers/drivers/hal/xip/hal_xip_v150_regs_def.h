/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V150 xip register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-01, Create file. \n
 */
#ifndef HAL_XIP_V150_REGS_DEF_H
#define HAL_XIP_V150_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_xip_v150_regs_def XIP V150 Regs Definition
 * @ingroup  drivers_hal_xip
 * @{
 */

/**
 * @brief  This union represents the bit fields in the gp_reg register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gp_reg_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t func_bist_clk_sel      : 1;    /*!< If this bit is set to 1, set xip clk_rxds.
                                                     If this bit is set to 0, set xip func bist. */
        uint32_t reserved1_15           : 15;   /*!< Reserved */
    } b;                                        /*!< Register bits. */
} gp_reg_data_data_t;

/**
 * @brief  This union represents the bit fields in the mem_clken register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union mem_clken_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t qspi0_div_en               : 1;    /*!< If this bit is set to 1. qspi0 div enable */
        uint32_t qspi1_div_en               : 1;    /*!< If this bit is set to 1. qspi1 div enable */
        uint32_t qspi3_div_en               : 1;    /*!< If this bit is set to 1. qspi3 div enable */
        uint32_t reserved3                  : 1;    /*!< Reserved */
        uint32_t qspi0_div_clken            : 1;    /*!< If this bit is set to 1. qspi0 div clock open */
        uint32_t qspi1_div_clken            : 1;    /*!< If this bit is set to 1. qspi1 div clock open */
        uint32_t qspi3_div_clken            : 1;    /*!< If this bit is set to 1. qspi3 div clock open */
        uint32_t reserved7                  : 1;    /*!< Reserved */
        uint32_t qspi0_clken                : 1;    /*!< If this bit is set to 1. qspi0 clock open */
        uint32_t qspi1_clken                : 1;    /*!< If this bit is set to 1. qspi1 clock open */
        uint32_t qspi3_clken                : 1;    /*!< If this bit is set to 1. qspi3 clock open */
        uint32_t opi_clken                  : 1;    /*!< If this bit is set to 1. opi clock open */
        uint32_t qspi0_xip_clken            : 1;    /*!< If this bit is set to 1. qspi0 xip clock open */
        uint32_t qspi1_xip_clken            : 1;    /*!< If this bit is set to 1. qspi1 xip clock open */
        uint32_t qspi3_xip_clken            : 1;    /*!< If this bit is set to 1. qspi3 xip clock open */
        uint32_t opi_xip_clken              : 1;    /*!< If this bit is set to 1. opi xip clock open */
    } b;                                            /*!< Register bits. */
} mem_clken_data_t;

/**
 * @brief  This union represents the bit fields in the mem_soft_rst_n register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union mem_soft_rst_n_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t soft_rst_qspi0_xip_n       : 1;    /*!< If this bit is set to 0. qspi0 reset */
        uint32_t soft_rst_qspi1_xip_n       : 1;    /*!< If this bit is set to 0. qspi1 reset */
        uint32_t soft_rst_qspi3_xip_n       : 1;    /*!< If this bit is set to 0. qspi0 reset */
        uint32_t soft_rst_opi_xip_n         : 1;    /*!< If this bit is set to 0. opi reset */
        uint32_t soft_rst_xip_cache_n       : 1;    /*!< If this bit is set to 0. cache reset */
        uint32_t soft_rst_xip_sub_diag_n    : 1;    /*!< If this bit is set to 0. sub diag reset */
        uint32_t soft_rst_pinmux_ctl_n      : 1;    /*!< If this bit is set to 0. pinmux ctl reset */
        uint32_t reserved7_15               : 9;    /*!< Reserved */
    } b;                                            /*!< Register bits. */
} mem_soft_rst_n_data_t;

/**
 * @brief  This union represents the bit fields in the mem_div4 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union mem_div4_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t qspi0_div_num              : 3;    /*!< qspi0 clock div */
        uint32_t reserved3                  : 1;    /*!< Reserved */
        uint32_t qspi1_div_num              : 3;    /*!< qspi1 clock div */
        uint32_t reserved7                  : 1;    /*!< Reserved */
        uint32_t qspi3_div_num              : 3;    /*!< qspi3 clock div */
        uint32_t reserved11_15              : 5;    /*!< Reserved */
    } b;                                            /*!< Register bits. */
} mem_div4_data_t;

/**
 * @brief  This union represents the bit fields in the xip_cache_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_cache_en_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t cache_en                   : 1;    /*!< If this bit is set to 1. xip cache enable */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_cache_en_data_t;

/**
 * @brief  This union represents the bit fields in the xip_monitor_sel register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_monitor_sel_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t monitor_sel                : 1;    /*!< If this bit is set to 1. xip monitor enable */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_monitor_sel_data_t;

/**
 * @brief  This union represents the bit fields in the xip_icu_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_icu_en_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t icu_en                     : 1;    /*!< If this bit is set to 1. xip cache enable */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_icu_en_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_cache2habm_over_time register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cache2habm_over_time_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t data                       : 16;   /*!< If this bit is set to 1. xip cacahe enable */
    } b;                                            /*!< Register bits. */
} cfg_cache2habm_over_time_data_t;

/**
 * @brief  This union represents the bit fields in the xip_cache_error_resp_mask register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_cache_error_resp_mask_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t cache_disable_ahb          : 1;    /*!< If this bit is set to 1. masking cache_disable_ahb */
        uint32_t cache_enable_write         : 1;    /*!< If this bit is set to 1. masking cache_enable_write */
        uint32_t cache2ahbm_error           : 1;    /*!< If this bit is set to 1. masking cache2ahbm_error */
        uint32_t cache2ahbm_overtime        : 1;    /*!< If this bit is set to 1. masking cache2ahbm_overtime */
        uint32_t reserved4_15               : 12;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_cache_error_resp_mask_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_calculate_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_calculate_en_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t clr                        : 1;    /*!< If this bit is set to 1. xip calculate enable */
        uint32_t en                         : 1;    /*!< If this bit is set to 1. xip calculate empty */
        uint32_t reserved2_15               : 14;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} cfg_calculate_en_data_t;

/**
 * @brief  This union represents the bit fields in the cache_miss_load register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cache_miss_load_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t miss_load                  : 1;    /*!< If this bit is set to 1. xip calculate latching */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} cache_miss_load_data_t;

/**
 * @brief  This union represents the bit fields in the cache_data register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cache_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t data                       : 16;   /*!< cache data */
    } b;                                            /*!< Register bits. */
} cache_data_t;

/**
 * @brief  This union represents the bit fields in the man_all register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union man_all_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t man_all_req                : 1;    /*!< If this bit is set to 1. cache invalid complete */
        uint32_t man_all_done               : 1;    /*!< If this bit is set to 1. cache invalid */
        uint32_t reserved2_15               : 14;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} man_all_data_t;

/**
 * @brief  This union represents the bit fields in the xip_write_read_enable register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_write_read_enable_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t write_read_opi_en          : 1;    /*!< If this bit is set to 1. xip write read ctl enable */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_write_read_enable_data_t;

/**
 * @brief  This union represents the bit fields in the xip_write_read_sync register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_write_read_sync_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t write_read_opi_sync        : 1;    /*!< If this bit is set to 1. xip write read ctl sync */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_write_read_sync_data_t;

/**
 * @brief  This union represents the bit fields in the write_redundant_cnt register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union write_redundant_cnt_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t redundant_cnt              : 7;    /*!< psram write dummy cycle */
        uint32_t psram_type                 : 1;    /*!< If this bit is set to 1/0. winbond psram/AP psram */
        uint32_t reserved9_15               : 8;    /*!< Reserved */
    } b;                                            /*!< Register bits. */
} write_redundant_cnt_data_t;

/**
 * @brief  This union represents the bit fields in the xip_write_read_error_resp_mask register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_write_read_error_resp_mask_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t disable_ahb_resp_mask      : 1;    /*!< If this bit is set to 1. masking disable_ahb_resp_mask */
        uint32_t wrong_burst_size_resp_mask : 1;    /*!< If this bit is set to 1. masking wrong_burst_size_resp_mask */
        uint32_t write_wrap_resp_mask       : 1;    /*!< If this bit is set to 1. masking write_wrap_resp_mask */
        uint32_t overtime_resp_mask         : 1;    /*!< If this bit is set to 1. masking overtime_resp_mask */
        uint32_t reserved4_15               : 12;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_write_read_error_resp_mask_data_t;

/**
 * @brief  This union represents the bit fields in the xip_read_qspi_enable register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_read_qspi_enable_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t read_qspi_enable           : 1;    /*!< If this bit is set to 1. xip read qspi enable */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_read_qspi_enable_data_t;

/**
 * @brief  This union represents the bit fields in the xip_read_qspi_sync register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_read_qspi_sync_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t read_qspi_sync             : 1;    /*!< If this bit is set to 1. xip read qspi sync */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_read_qspi_sync_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_wrap_operation register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_wrap_operation_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t wrap_operation             : 1;    /*!< If this bit is set to 1. flash support wrap */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} cfg_wrap_operation_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_addr_24_32 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_addr_24_32_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t addr_24_32                 : 1;    /*!< If this bit is set to 1/0. flash choose 32/24 addr */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} cfg_addr_24_32_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_flash_sel register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_flash_sel_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t flash_sel                  : 1;    /*!< If this bit is set to 1. flash support xip */
        uint32_t reserved1_15               : 15;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} cfg_flash_sel_data_t;

/**
 * @brief  This union represents the bit fields in the xip_mode_code register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_mode_code_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t read_mode_code             : 8;    /*!< flash mode code */
        uint32_t reserved8_15               : 8;    /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_mode_code_data_t;

/**
 * @brief  This union represents the bit fields in the flash_read_cmd register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union flash_read_cmd_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t flash_read_cmd             : 8;    /*!< flash read cmd */
        uint32_t reserved8_15               : 8;    /*!< Reserved */
    } b;                                            /*!< Register bits. */
} flash_read_cmd_data_t;

/**
 * @brief  This union represents the bit fields in the cfg_xip_read_over_time register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_xip_read_over_time_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t data                       : 16;   /*!< qspi read ctl overtime protection */
    } b;                                            /*!< Register bits. */
} cfg_xip_read_over_time_data_t;

/**
 * @brief  This union represents the bit fields in the xip_read_error_resp_mask register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_read_error_resp_mask_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t disable_ahb_resp_mask      : 1;    /*!< If this bit is set to 1. masking disable_ahb_resp_mask */
        uint32_t write_wrap_resp_mask       : 1;    /*!< If this bit is set to 1. masking write_wrap_resp_mask */
        uint32_t wrong_burst_size_resp_mask : 1;    /*!< If this bit is set to 1. masking wrong_burst_size_resp_mask */
        uint32_t overtime_resp_mask         : 1;    /*!< If this bit is set to 1. masking overtime_resp_mask */
        uint32_t reserved4_15               : 12;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_read_error_resp_mask_data_t;

/**
 * @brief  This union represents the bit fields in the xip_sub_diag_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_sub_diag_en_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t cache_diag_en              : 1;    /*!< If this bit is set to 1. cache diag enable */
        uint32_t opi_diag_en                : 1;    /*!< If this bit is set to 1. opi diag enable */
        uint32_t qspi_diag_en               : 1;    /*!< If this bit is set to 1. qspi diag enable */
        uint32_t cache2ahbm_diag_en         : 1;    /*!< If this bit is set to 1. cache2ahbm diag enable */
        uint32_t first_32k_data_en          : 1;    /*!< If this bit is set to 1/0. front/rear 8k data */
        uint32_t write_read_en              : 1;    /*!< If this bit is set to 1/0. write/read data */
        uint32_t reserved4_15               : 12;   /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_sub_diag_en_data_t;

/**
 * @brief  This union represents the bit fields in the xip_cfg_wait_cnt register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xip_cfg_wait_cnt0_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t wait_cnt                   : 8;    /*!< the wait time when the PCLK of QSPI0 is twice or
                                                         more than the SSICLK. */
        uint32_t reserved8_15               : 8;    /*!< Reserved */
    } b;                                            /*!< Register bits. */
} xip_cfg_wait_cnt0_data_t;

/**
 * @brief  Registers associated with Xip_qspi.
 */
typedef struct xip_qspi {
    volatile uint32_t xip_read_qspi_enable;                    /*!< xip_read_qspi_enable_0.  <i>Offset: 300h</i>. */
    volatile uint32_t xip_read_qspi_sync;                      /*!< xip_read_qspi_sync_0.  <i>Offset: 304h</i>. */
    volatile uint32_t cfg_wrap_operation;                      /*!< cfg_wrap_operation_0.  <i>Offset: 308h</i>. */
    volatile uint32_t cfg_addr_24_32;                          /*!< cfg_addr_24_32_0.  <i>Offset: 30Ch</i>. */
    volatile uint32_t cfg_flash_sel;                           /*!< cfg_flash_sel_0.  <i>Offset: 310h</i>. */
    volatile uint32_t xip_mode_code;                           /*!< xip_mode_code_0.  <i>Offset: 31h</i>. */
    volatile uint32_t flash_read_cmd;                          /*!< flash_read_cmd_0.  <i>Offset: 318h</i>. */
    volatile uint32_t cfg_xip_read_over_time_l;                /*!< cfg_xip_read_over_time_l_0.  <i>Offset: 31Ch</i>. */
    volatile uint32_t cfg_xip_read_over_time_h;                /*!< cfg_xip_read_over_time_h_0.  <i>Offset: 320h</i>. */
    volatile uint32_t xip_read_error_resp_mask;                /*!< xip_read_error_resp_mask_0.  <i>Offset: 324h</i>. */
    volatile uint32_t res0[6];                                 /*!< Reserved.  <i>Offset: 328h</i>. */
    volatile uint32_t xip_read_disable_ahb_addr_l;             /*!< xip_read_disable_ahb_addr_l_0.
                                                                   <i>Offset: 340h</i>. */
    volatile uint32_t xip_read_disable_ahb_addr_h;             /*!< xip_read_disable_ahb_addr_h_0.
                                                                   <i>Offset: 344h</i>. */
    volatile uint32_t xip_read_write_addr_l;                   /*!< xip_read_write_addr_l_0.  <i>Offset: 348h</i>. */
    volatile uint32_t xip_read_write_addr_h;                   /*!< xip_read_write_addr_h_0.  <i>Offset: 34Ch</i>. */
    volatile uint32_t xip_read_write_data_l;                   /*!< xip_read_write_data_l_0.  <i>Offset: 350h</i>. */
    volatile uint32_t xip_read_write_data_h;                   /*!< xip_read_write_data_h_0.  <i>Offset: 354h</i>. */
    volatile uint32_t xip_read_wrong_burst_size_addr_l;        /*!< xip_read_wrong_burst_size_addr_l_0.
                                                                   <i>Offset: 358h</i>. */
    volatile uint32_t xip_read_wrong_burst_size_addr_h;        /*!< xip_read_wrong_burst_size_addr_h_0.
                                                                   <i>Offset: 35Ch</i>. */
    volatile uint32_t xip_read_qspi_apb_addr_l;                /*!< xip_read_qspi_apb_addr_l_0.  <i>Offset: 360h</i>. */
    volatile uint32_t xip_read_qspi_apb_addr_h;                /*!< xip_read_qspi_apb_addr_h_0.  <i>Offset: 364h</i>. */
    volatile uint32_t xip_read_cur_sts;                        /*!< xip_read_cur_sts_0.  <i>Offset: 368h</i>. */
    volatile uint32_t res1[37];                               /*!< xip_read_cur_sts_0.  <i>Offset: 36Ch</i>. */
} xip_qspi_t;

/**
 * @brief  Registers associated with Xip.
 * @note   reserved not occupy memory, because directly point to the register.
 */
typedef struct xip_regs {
    volatile uint32_t cache_ctl_id;                            /*!< cache_ctl_id.     <i>Offset: 0h</i>. */
    volatile uint32_t res0[3];                                 /*!< Reserved.  <i>Offset: 04h</i>. */
    volatile uint32_t gp_reg0;                                 /*!< gp_reg0.  <i>Offset: 10h</i>. */
    volatile uint32_t gp_reg1;                                 /*!< gp_reg1.  <i>Offset: 14h</i>. */
    volatile uint32_t gp_reg2;                                 /*!< gp_reg2.  <i>Offset: 18h</i>. */
    volatile uint32_t gp_reg3;                                 /*!< gp_reg3.  <i>Offset: 1Ch</i>. */
    volatile uint32_t mem_clken0;                              /*!< mem_clken0.  <i>Offset: 20h</i>. */
    volatile uint32_t mem_clken1;                              /*!< mem_clken1.  <i>Offset: 24h</i>. */
    volatile uint32_t res1[2];                                 /*!< Reserved.  <i>Offset: 28h</i>. */
    volatile uint32_t mem_soft_rst_n;                          /*!< value3.  <i>Offset: 30h</i>. */
    volatile uint32_t res2[7];                                  /*!< Reserved.  <i>Offset: 34h</i>. */
    volatile uint32_t mem_div4;                                /*!< value3.  <i>Offset: 50h</i>. */
    volatile uint32_t res3[43];                                /*!< Reserved.  <i>Offset: 54h</i>. */

    volatile uint32_t xip_cache_en;                            /*!< xip_cache_en.  <i>Offset: 100h</i>. */
    volatile uint32_t xip_monitor_sel;                         /*!< xip_monitor_sel.  <i>Offset: 104h</i>. */
    volatile uint32_t xip_icu_en;                              /*!< xip_icu_en.  <i>Offset: 108h</i>. */
    volatile uint32_t cfg_cache2habm_over_time_l;              /*!< cfg_cache2habm_over_time_l.  <i>Offset: 10Ch</i>. */
    volatile uint32_t cfg_cache2habm_over_time_h;              /*!< cfg_cache2habm_over_time_h.  <i>Offset: 110h</i>. */
    volatile uint32_t xip_cache_error_resp_mask;               /*!< xip_cache_error_resp_mask.  <i>Offset: 114h</i>. */
    volatile uint32_t res4[2];                                 /*!< Reserved.  <i>Offset: 118h</i>. */

    volatile uint32_t xip_cache_intr_sts;                      /*!< xip_cache_intr_sts.  <i>Offset: 120h</i>. */
    volatile uint32_t xip_cache_intr_mask_sts;                 /*!< xip_cache_intr_mask_sts.  <i>Offset: 124h</i>. */
    volatile uint32_t xip_cache_intr_mask;                     /*!< xip_cache_intr_mask.  <i>Offset: 128h</i>. */
    volatile uint32_t xip_cache_intr_clr;                      /*!< xip_cache_intr_clr.  <i>Offset: 12Ch</i>. */
    volatile uint32_t res5[4];                                  /*!< Reserved.  <i>Offset: 130h</i>. */

    volatile uint32_t cfg_calculate_en;                        /*!< cfg_calculate_en.  <i>Offset: 140h</i>. */
    volatile uint32_t cache_miss_load;                         /*!< cache_miss_load.  <i>Offset: 144h</i>. */
    volatile uint32_t cache_total_h;                           /*!< cache_total_h.  <i>Offset: 148h</i>. */
    volatile uint32_t cache_total_m;                           /*!< cache_total_m.  <i>Offset: 14Ch</i>. */
    volatile uint32_t cache_total_l;                           /*!< cache_total_l.  <i>Offset: 150h</i>. */
    volatile uint32_t cache_hit_h;                             /*!< cache_hit_h.  <i>Offset: 154h</i>. */
    volatile uint32_t cache_hit_m;                             /*!< cache_hit_m.  <i>Offset: 158h</i>. */
    volatile uint32_t cache_hit_l;                             /*!< cache_hit_l.  <i>Offset: 15Ch</i>. */
    volatile uint32_t cache_miss_h;                            /*!< cache_miss_h.  <i>Offset: 160h</i>. */
    volatile uint32_t cache_miss_m;                            /*!< cache_miss_m.  <i>Offset: 164h</i>. */
    volatile uint32_t cache_miss_l;                            /*!< cache_miss_l.  <i>Offset: 168h</i>. */
    volatile uint32_t res6;                                    /*!< Reserved.  <i>Offset: 16Ch</i>. */
    volatile uint32_t man_single;                              /*!< man_single.  <i>Offset: 170h</i>. */
    volatile uint32_t man_single_addr_h;                       /*!< man_single_addr_h.  <i>Offset: 174h</i>. */
    volatile uint32_t man_single_addr_l;                       /*!< man_single_addr_l.  <i>Offset: 178h</i>. */
    volatile uint32_t man_all;                                 /*!< man_all.  <i>Offset: 17Ch</i>. */
    volatile uint32_t res7[4];                                 /*!< Reserved.  <i>Offset: 180h</i>. */

    volatile uint32_t diag_read_addr_l;                        /*!< diag_read_addr_l.  <i>Offset: 190h</i>. */
    volatile uint32_t diag_read_addr_h;                        /*!< diag_read_addr_h.  <i>Offset: 194h</i>. */
    volatile uint32_t diag_write_addr_l;                       /*!< diag_write_addr_l.  <i>Offset: 198h</i>. */
    volatile uint32_t diag_write_addr_h;                       /*!< diag_write_addr_h.  <i>Offset: 19Ch</i>. */
    volatile uint32_t diag_write_data_l;                       /*!< diag_write_data_l.  <i>Offset: 1A0h</i>. */
    volatile uint32_t diag_write_data_h;                       /*!< diag_write_data_h.  <i>Offset: 1A4h</i>. */
    volatile uint32_t cache2ahbm_cur_sts;                      /*!< cache2ahbm_cur_sts.  <i>Offset: 1A8h</i>. */
    volatile uint32_t res8[21];                                /*!< Reserved.  <i>Offset: 1ACh</i>. */

    volatile uint32_t xip_write_read_enable;                   /*!< xip_write_read_enable.  <i>Offset: 200h</i>. */
    volatile uint32_t xip_write_read_sync;                     /*!< xip_write_read_sync.  <i>Offset: 204h</i>. */
    volatile uint32_t write_tcph_period;                       /*!< write_tcph_period.  <i>Offset: 208h</i>. */
    volatile uint32_t write_redundant_cnt;                     /*!< write_redundant_cnt.  <i>Offset: 20Ch</i>. */
    volatile uint32_t write_fifo_threshold;                    /*!< write_fifo_threshold.  <i>Offset: 210h</i>. */
    volatile uint32_t write_fifo_soft_reset;                   /*!< write_fifo_soft_reset.  <i>Offset: 214h</i>. */
    volatile uint32_t write_fifo_sts_clr;                      /*!< write_fifo_sts_clr.  <i>Offset: 218h</i>. */
    volatile uint32_t cfg_xip_opi_read_over_time_l;            /*!< cfg_xip_opi_read_over_time_l.
                                                                    <i>Offset: 21Ch</i>. */
    volatile uint32_t cfg_xip_opi_read_over_time_h;            /*!< cfg_xip_opi_read_over_time_h.
                                                                    <i>Offset: 220h</i>. */
    volatile uint32_t cfg_xip_write_over_time_l;               /*!< cfg_xip_write_over_time_l.  <i>Offset: 224h</i>. */
    volatile uint32_t cfg_xip_write_over_time_h;               /*!< cfg_xip_write_over_time_h.  <i>Offset: 228h</i>. */
    volatile uint32_t xip_write_read_error_resp_mask;          /*!< xip_write_read_error_resp_mask.
                                                                    <i>Offset: 22Ch</i>. */
    volatile uint32_t cfg_xip_write_psram_cmd_l;               /*!< cfg_xip_write_psram_cmd_l.  <i>Offset: 230h</i>. */
    volatile uint32_t cfg_xip_write_psram_cmd_h;               /*!< cfg_xip_write_psram_cmd_h.  <i>Offset: 234h</i>. */
    volatile uint32_t cfg_xip_read_psram_cmd_l;                /*!< cfg_xip_read_psram_cmd_l.  <i>Offset: 238h</i>. */
    volatile uint32_t cfg_xip_read_psram_cmd_h;                /*!< cfg_xip_read_psram_cmd_h.  <i>Offset: 23Ch</i>. */
    volatile uint32_t cfg_clk_bus_low_freq;                    /*!< cfg_clk_bus_low_freq.  <i>Offset: 240h</i>. */
    volatile uint32_t res9[15];                                 /*!< Reserved.  <i>Offset: 244h</i>. */

    volatile uint32_t write_fifo_sts;                          /*!< write_fifo_sts.  <i>Offset: 280h</i>. */
    volatile uint32_t res10[3];                                 /*!< Reserved.  <i>Offset: 284h</i>. */

    volatile uint32_t xip_write_read_disable_ahb_add_l;        /*!< xip_write_read_disable_ahb_add_l.
                                                                    <i>Offset: 290h</i>. */
    volatile uint32_t xip_write_read_disable_ahb_add_h;        /*!< xip_write_read_disable_ahb_add_h.
                                                                    <i>Offset: 294h</i>. */
    volatile uint32_t xip_write_read_wrong_burst_size_addr_l;  /*!< xip_write_read_wrong_burst_size_addr_l.
                                                                    <i>Offset: 298h</i>. */
    volatile uint32_t xip_write_read_wrong_burst_size_addr_h;  /*!< xip_write_read_wrong_burst_size_addr_h.
                                                                    <i>Offset: 29Ch</i>. */
    volatile uint32_t xip_write_read_write_wrap_addr_l;        /*!< valuxip_write_read_write_wrap_addr_le3.
                                                                    <i>Offset: 2A0h</i>. */
    volatile uint32_t xip_write_read_write_wrap_addr_h;        /*!< xip_write_read_write_wrap_addr_h.
                                                                    <i>Offset: 2A4h</i>. */
    volatile uint32_t xip_write_read_opi_apb_addr_l;           /*!< xip_write_read_opi_apb_addr_l.
                                                                    <i>Offset: 2A8h</i>. */
    volatile uint32_t xip_write_read_opi_apb_addr_h;           /*!< xip_write_read_opi_apb_addr_h.
                                                                    <i>Offset: 2ACh</i>. */
    volatile uint32_t xip_write_read_cur_sts_write;            /*!< xip_write_read_cur_sts_write.
                                                                    <i>Offset: 2B0h</i>. */
    volatile uint32_t xip_write_read_cur_sts_read;             /*!< xip_write_read_cur_sts_read.
                                                                    <i>Offset: 2B4h</i>. */
    volatile uint32_t res11[18];                                 /*!< Reserved.  <i>Offset: 2B8h</i>. */

    volatile xip_qspi_t xip_qspi[2];                           /*!< xip qspi0 or qspi1.  <i>Offset: 300h</i>. */

    volatile uint32_t xip_ctl_intr_sts;                        /*!< xip_ctl_intr_sts.  <i>Offset: 500h</i>. */
    volatile uint32_t xip_ctl_intr_mask_sts;                   /*!< xip_ctl_intr_mask_sts.  <i>Offset: 504h</i>. */
    volatile uint32_t xip_ctl_intr_mask;                       /*!< xip_ctl_intr_mask.  <i>Offset: 508h</i>. */
    volatile uint32_t xip_ctl_intr_clr;                        /*!< xip_ctl_intr_clr.  <i>Offset: 50Ch</i>. */
    volatile uint32_t res12[60];                               /*!< Reserved.  <i>Offset: 510h</i>. */

    volatile uint32_t cache_ahb_s1_icm_priority;               /*!< cache_ahb_s1_icm_priority.  <i>Offset: 600h</i>. */
    volatile uint32_t cache_ahb_s2_icm_priority;               /*!< cache_ahb_s2_icm_priority.  <i>Offset: 604h</i>. */
    volatile uint32_t cache_ahb_s3_icm_priority;               /*!< cache_ahb_s3_icm_priority.  <i>Offset: 608h</i>. */
    volatile uint32_t cache_ahb_s4_icm_priority;               /*!< cache_ahb_s4_icm_priority.  <i>Offset: 60Ch</i>. */
    volatile uint32_t res13[60];                               /*!< Reserved.  <i>Offset: 610h</i>. */

    volatile uint32_t mem_sub_gating_configure;                /*!< mem_sub_gating_configure.  <i>Offset: 700h</i>. */
    volatile uint32_t mem_sub_gating_sts;                      /*!< mem_sub_gating_sts.  <i>Offset: 704h</i>. */
    volatile uint32_t res14[2];                               /*!< Reserved.  <i>Offset: 708h</i>. */

    volatile uint32_t xip_sub_diag_en;                         /*!< xip_sub_diag_en.  <i>Offset: 710h</i>. */
    volatile uint32_t xip_sub_diag_info;                       /*!< xip_sub_diag_info.  <i>Offset: 714h</i>. */
    volatile uint32_t xip_sub_diag_clken;                      /*!< xip_sub_diag_clken.  <i>Offset: 718h</i>. */
    volatile uint32_t res15;                                   /*!< Reserved.  <i>Offset: 71ch</i>. */
    volatile uint32_t rxds_sel;                                /*!< rxds_sel.  <i>Offset: 720h</i>. */
    volatile uint32_t rxds_high_sel;                           /*!< rxds_high_sel.  <i>Offset: 724h</i>. */
    volatile uint32_t res16[250];                              /*!< Reserved.  <i>Offset: 728h</i>. */

    volatile uint32_t xip_cfg_wait_cnt0;                       /*!< xip_cfg_wait_cnt0.  <i>Offset: B10h</i>. */
} xip_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif