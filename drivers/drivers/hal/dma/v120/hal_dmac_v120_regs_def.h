/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V120 dma register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-12-16， Create file. \n
 */
#ifndef HAL_DMAC_V120_REGS_DEF_H
#define HAL_DMAC_V120_REGS_DEF_H

#include <stdint.h>
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v120_regs_def DMA V120 Regs Definition
 * @ingroup  drivers_hal_dma
 * @{
 */

/**
 * @brief  This union represents the bit fields in the DMA CFG_L configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_ctrl_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t transfersize : 12;         /*!< Transfer size. */
        uint32_t sbsize       : 3;          /*!< Source burst size. */
        uint32_t dbsize       : 3;          /*!< Destination burst size. */
        uint32_t swsize       : 3;          /*!< Source transfer width. */
        uint32_t dwsize       : 3;          /*!< Destination transfer width. */
        uint32_t src_ms_sel   : 1;          /*!< Source AHB master select. */
        uint32_t dest_ms_sel  : 1;          /*!< Destination AHB master select. */
        uint32_t src_inc      : 1;          /*!< Source increment. */
        uint32_t dest_inc     : 1;          /*!< Destination increment. */
        uint32_t protection   : 3;          /*!< Protection. */
        uint32_t tc_int_en    : 1;          /*!< Terminal count interrupt enable bit. */
    } b;                                    /*!< Register bits. */
} dma_ctrl_data_t;

/**
 * @brief  This union represents the bit fields in the DMA configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_cfg_data {
    uint32_t d32;                           /*!< Raw register data. */
    struct {
        uint32_t ch_enable      : 1;        /*!< Channel enable. */
        uint32_t src_per        : 4;        /*!< Source peripheral. */
        uint32_t src_reserved   : 1;        /*!< reserved bits. */
        uint32_t dest_per       : 4;        /*!< Destination peripheral. */
        uint32_t dest_reserved  : 1;        /*!< reserved bits. */
        uint32_t fc_tt          : 3;        /*!< Flow control and transfer type. */
        uint32_t int_err_mask   : 1;        /*!< Interrupt error mask. */
        uint32_t tc_int_mask    : 1;        /*!< Terminal count interrupt mask. */
        uint32_t lock           : 1;        /*!< Lock. When set, this bit enables locked transfers. */
        uint32_t active         : 1;        /*!< You can use this value with the halt and
                                                 channel enable bits to cleanly disable a DMA channel. */
        uint32_t halt           : 1;        /*!< You can use this value with the active and
                                                 channel enable bits to cleanly disable a DMA channel. */
        uint32_t reserved       : 13;       /*!< reserved bit. */
    } b;                                    /*!< Register bits. */
} dma_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the DMA configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_v120_cfg_reg_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t dma_enable : 1;          /*!< DMA enable bit. */
        uint32_t master1    : 1;          /*!< AHB master 1 endianness configuration bit. */
        uint32_t master2    : 1;          /*!< AHB master 2 endianness configuration bit. */
        uint32_t reserved   : 1;          /*!< reserved bit. */
    } b;                                  /*!< Register bits. */
} dma_v120_cfg_reg_data_t;

/**
 * @brief  DMA Channel configuration.
 */
typedef struct dma_v120_ch_cfg {
    volatile uint32_t src;                  /*!< Channel source address registers.  <i>Offset: 100H</i>. */
    volatile uint32_t dest;                 /*!< Channel destination address registers.  <i>Offset: 104H</i>. */
    volatile uint32_t lli;                  /*!< Channel linked list item registers.  <i>Offset: 108H</i>. */
    volatile uint32_t ctrl;                 /*!< Channel control registers.  <i>Offset: 10CH</i>. */
    volatile uint32_t cfg;                  /*!< Channel configuration registers.  <i>Offset: 110H</i>. */
    volatile uint32_t ch_reserved[3];       /*!< reserved.  <i>Offset: 114H</i>. */
} dma_v120_ch_cfg_t;

/**
 * @brief  Registers associated with DMA.
 */
typedef struct dma_v120_regs {
    volatile uint32_t int_st;               /*!< Interrupt status register.  <i>Offset: 000H</i>. */
    volatile uint32_t int_tc_st;            /*!< Interrupt terminal count status register.  <i>Offset: 004H</i>. */
    volatile uint32_t int_tc_clr;           /*!< Interrupt terminal count clear register.  <i>Offset: 008H</i>. */
    volatile uint32_t int_err_st;           /*!< Interrupt error status register.  <i>Offset: 00CH</i>. */
    volatile uint32_t int_err_clr;          /*!< Interrupt error clear register.  <i>Offset: 010H</i>. */
    volatile uint32_t raw_int_tc_st;        /*!< Raw interrupt terminal count status register.  <i>Offset: 014H</i>. */
    volatile uint32_t raw_int_err_st;       /*!< Raw error interrupt status register.  <i>Offset: 018H</i>. */
    volatile uint32_t en_bld_ch;            /*!< Enabled channel register.  <i>Offset: 01CH</i>. */
    volatile uint32_t soft_b_req;           /*!< Software burst request register.  <i>Offset: 020H</i>. */
    volatile uint32_t soft_s_req;           /*!< Software single request register.  <i>Offset: 024H</i>. */
    volatile uint32_t soft_lb_req;          /*!< Software last burst request register.  <i>Offset: 028H</i>. */
    volatile uint32_t soft_ls_req;          /*!< Software last single request register.  <i>Offset: 02CH</i>. */
    volatile uint32_t configuration;        /*!< Configuration register.  <i>Offset: 030H</i>. */
    volatile uint32_t sync;                 /*!< Synchronization register.  <i>Offset: 034H</i>. */
    volatile uint32_t reserved[50];         /*!< reserved.  <i>Offset: 038H</i>. */
    volatile dma_v120_ch_cfg_t ch_config[B_DMA_CHANNEL_MAX_NUM];
                                            /*!< DMA channel configuration.  <i>Offset: 100H</i>. */
} dma_v120_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif