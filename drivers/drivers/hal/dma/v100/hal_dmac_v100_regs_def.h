/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 dma register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-10-16， Create file. \n
 */

#ifndef HAL_DMAC_V100_REGS_DEF_H
#define HAL_DMAC_V100_REGS_DEF_H

#include <stdint.h>
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v100_regs_def DMA V100 Regs Definition
 * @ingroup  drivers_hal_dma
 * @{
 */

 /**
 * @brief  DMA interrupt reg.
 */
typedef enum {
    DMA_INT_REG_RAW,
    DMA_INT_REG_STATUS,
    DMA_INT_REG_MASK,
    DMA_INT_REG_CLEAR,
    DMA_INT_REG_TYPE_MAX
} hal_dma_int_reg_type_t;

/**
 * @brief  This union represents the bit fields in the DMA LLP configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_llp_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t lms : 2;                 /*!< List master select. Identifies the AHB layer/interface
                                               where the memory device that stores the next linked list item resides. */
        uint32_t loc : 30;                /*!< Starting address in memory of next LLI
                                               if block chaining is enabled. */
    } b;                                  /*!< Register bits. */
} dma_llp_data_t;

/**
 * @brief  This union represents the bit fields in the DMA CTL_L configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_ctrl_l_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t int_en : 1;              /*!< Interrupt enable bit. If set,
                                               then all interrupt-generating sources are enabled. */
        uint32_t dst_tr_width : 3;        /*!< Destination transfer width. */
        uint32_t src_tr_width : 3;        /*!< Source transfer width. */
        uint32_t dinc : 2;                /*!< Destination address increment. */
        uint32_t sinc : 2;                /*!< Source address incremen.t */
        uint32_t dest_msize : 3;          /*!< Destination burst transaction length. */
        uint32_t src_msize : 3;           /*!< Source burst transaction length. */
        uint32_t src_gather_en : 1;       /*!< Source gather enable bit:
                                               0 = Gather disabled
                                               1 = Gather enabled
                                               Gather on the source side is applicable only when the CTLx.SINC bit
                                               indicates an incrementing or decrementing address control. */
        uint32_t dst_scatter_en : 1;      /*!< Destination scatter enable bit:
                                               0 = Scatter disabled
                                               1 = Scatter enabled
                                               Scatter on the destination side is applicable only when the
                                               CTLx.DINC bit indicates an incrementing or
                                               decrementing address control. */
        uint32_t reserved0 : 1;           /*!< Reserved */
        uint32_t tt_fc : 3;               /*!< Transfer Type and Flow Control.
                                               The following transfer types are supported.
                                               • Memory to Memory
                                               • Memory to Peripheral
                                               • Peripheral to Memory
                                               • Peripheral to Peripheral */
        uint32_t dms : 2;                 /*!< Destination master select. */
        uint32_t sms : 2;                 /*!< Source master select. */
        uint32_t llp_dst_en : 1;          /*!< Block chaining is enabled on the destination side only if the
                                               LLP_DST_EN field is high and LLPx.LOC is non-zero. */
        uint32_t llp_src_en : 1;          /*!< Block chaining is enabled on the source side only if the
                                               LLP_SRC_EN field is high and LLPx.LOC is non-zero. */
    } b;                                  /*!< Register bits. */
} dma_ctrl_l_data_t;

/**
 * @brief  This union represents the bit fields in the DMA CTL_H configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_ctrl_h_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t block_ts : 12;           /*!< Block transfer size.  */
        uint32_t done : 1;                /*!< Done bit. */
    } b;                                  /*!< Register bits. */
} dma_ctrl_h_data_t;

/**
 * @brief  This union represents the bit fields in the DMA CFG_L configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_cfg_l_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t reserved0 : 5;           /*!< Reserved. */
        uint32_t ch_prior : 3;            /*!< Channel priority. */
        uint32_t ch_susp : 1;             /*!< Channel suspend. */
        uint32_t fifo_empty : 1;          /*!< Indicates if there is data left in the channel FIFO. */
        uint32_t hs_sel_dst : 1;          /*!< Destination software or hardware handshaking select. */
        uint32_t hs_sel_src : 1;          /*!< Source software or hardware handshaking select. */
        uint32_t lock_ch_l : 2;           /*!< Channel lock level. */
        uint32_t lock_b_l : 2;            /*!< Bus lock level. */
        uint32_t lock_ch : 1;             /*!< Channel lock bit. */
        uint32_t lock_b : 1;              /*!< Bus lock bit. */
        uint32_t dst_hs_pol : 1;          /*!< Destination handshaking interface polarity. */
        uint32_t src_hs_pol : 1;          /*!< Source handshaking interface polarity. */
        uint32_t max_abrst : 10;          /*!< Maximum AMBA burst length. */
        uint32_t reload_src : 1;          /*!< Automatic source reload. */
        uint32_t reload_dst : 1;          /*!< Automatic destination reload. */
    } b;                                  /*!< Register bits. */
} dma_cfg_l_data_t;

/**
 * @brief  This union represents the bit fields in the DMA CFG_H configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_cfg_h_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t fcmode : 1;              /*!< Flow control mode. */
        uint32_t fifo_mode : 1;           /*!< FIFO mode select. */
        uint32_t protctl : 3;             /*!< Protection control. */
        uint32_t ds_upd_en : 1;           /*!< Destination status update enable. */
        uint32_t ss_upd_en : 1;           /*!< Source status update enable. */
        uint32_t src_per : 4;             /*!< Assigns a hardware handshaking interface(destination). */
        uint32_t dest_per : 4;            /*!< Assigns a hardware handshaking interface(source). */
        uint32_t reserved : 17;           /*!< reserved. */
    } b;                                  /*!< Register bits. */
} dma_cfg_h_data_t;

/**
 * @brief  This union represents the bit fields in the DMA SGR configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_sgr_cfg_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t sgi : 20;                /* Source gather interval. */
        uint32_t sgc : 12;                /* Source gather count. Source contiguous transfer count between
                                             successive gather boundaries. */
    } b;                                  /*!< Register bits. */
} dma_sgr_cfg_data_t;


/**
 * @brief  This union represents the bit fields in the DMA DSR configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_dsr_cfg_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t dsi : 20;                /*!< Destination scatter interval. */
        uint32_t dsc : 12;                /*!< Destination scatter count. Destination contiguous transfer count
                                               between successive scatter boundaries. */
    } b;                                  /*!< Register bits. */
} dma_dsr_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the DMA interrupt status.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_int_status_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t tfr : 1;                 /*!< OR of the contents of StatusTfr register. */
        uint32_t block : 1;               /*!< OR of the contents of StatusBlock register. */
        uint32_t srct : 1;                /*!< OR of the contents of StatusSrcTran register. */
        uint32_t dstt : 1;                /*!< OR of the contents of StatusDst register. */
        uint32_t err : 1;                 /*!< OR of the contents of StatusErr register. */
    } b;                                  /*!< Register bits. */
} dma_int_status_data_t;

/**
 * @brief  This union represents the bit fields in the DMA configuration.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dma_cfg_reg_data {
    uint32_t d32;                         /*!< Raw register data. */
    struct {
        uint32_t dma_enable : 1;          /*!< DMA enable bit. */
    } b;                                  /*!< Register bits. */
} dma_cfg_reg_data_t;

/**
 * @brief  DMA Channel configuration.
 */
typedef struct dma_ch_cfg {
    volatile uint32_t src;                              /*!< Source Address Register. */
    volatile uint32_t src_reserved;                     /*!< Source Address Register upper 32bit. */
    volatile uint32_t dst;                              /*!< Destination Address Register. */
    volatile uint32_t dst_reserved;                     /*!< Destination Address Register 32bit. */
    volatile uint32_t llp;                              /*!< Linked List Pointer Register. */
    volatile uint32_t llp_reserved;                     /*!< Linked List Pointer Register upper 32 bit. */
    volatile uint32_t ctl_l;                            /*!< Control Register lower 32 bit. */
    volatile uint32_t ctl_h;                            /*!< Control Register upper 32 bit. */
    volatile uint32_t sstat;                            /*!< Source Status Register. */
    volatile uint32_t sstat_reserved;                   /*!< Source Status Register upper 32 bit. */
    volatile uint32_t dstat;                            /*!< Destination Status Register. */
    volatile uint32_t dstat_reserved;                   /*!< Destination Status Register upper 32 bit. */
    volatile uint32_t sstatar;                          /*!< Source Status Address Register. */
    volatile uint32_t sstatar_reserved;                 /*!< Source Status Address Register upper 32 bit. */
    volatile uint32_t dstatar;                          /*!< Destination Status Address Register. */
    volatile uint32_t dstatar_reserved;                 /*!< Destination Status Address Register upper 32 bit. */
    volatile uint32_t cfg_l;                            /*!< Configuration Register. */
    volatile uint32_t cfg_h;                            /*!< Configuration Register upper 32 bit. */
    volatile uint32_t sgr;                              /*!< Source Gather Register. */
    volatile uint32_t sgr_reserved;                     /*!< Source Gather Register upper 32 bit. */
    volatile uint32_t dsr;                              /*!< Destination Scatter Register. */
    volatile uint32_t dsr_reserved;                     /*!< Destination Scatter Register upper 32 bit. */
} dma_ch_cfg_t;

/**
 * @brief  DMA interrupt type register.
 */
typedef struct dma_int_type {
    volatile uint32_t tfr;                              /*!< DMA transfer complete interrupt. */
    volatile uint32_t tfr_reserved;                     /*!< DMA transfer complete interrupt upper 32 bit. */
    volatile uint32_t block;                            /*!< Block transfer complete interrupt. */
    volatile uint32_t block_reserved;                   /*!< Block transfer complete interrupt upper 32 bit. */
    volatile uint32_t src_tran;                         /*!< Source transaction complete interrupt. */
    volatile uint32_t src_tran_reserved;                /*!< Source transaction complete interrupt 32 bit. */
    volatile uint32_t dst_tran;                         /*!< Destination transaction complete interrupt. */
    volatile uint32_t dst_tran_reserved;                /*!< Destination transaction complete interrupt 32 bit. */
    volatile uint32_t err;                              /*!< Error interrupt. */
    volatile uint32_t err_reserved;                     /*!< Error interrupt 32 bit. */
} dma_int_type_t;

/**
 * @brief  Registers associated with DMA.
 */
typedef struct dma_regs {
    volatile dma_ch_cfg_t dma_ch_config[B_DMA_CHANNEL_MAX_NUM];       /*!< DMA channel configuration. */
    volatile dma_int_type_t dma_interrupt_type[DMA_INT_REG_TYPE_MAX]; /*!< DMA interrupt type register. */
    volatile uint32_t dma_sts_int;                                    /*!< Combined interrupt status. */
    volatile uint32_t dma_sts_int_reserved;                           /*!< reserved. */
    volatile uint32_t dma_hs_src;                                     /*!< Source software transaction request. */
    volatile uint32_t dma_hs_src_reserved;                            /*!< reserved. */
    volatile uint32_t dma_hs_dst;                                     /*!< Destination software transaction request. */
    volatile uint32_t dma_hs_dst_reserved;                            /*!< reserved. */
    volatile uint32_t dma_hs_sgl_src;                                 /*!< Single source transaction request. */
    volatile uint32_t dma_hs_sgl_src_reserved;                        /*!< reserved. */
    volatile uint32_t dma_hs_sgl_dst;                                 /*!< Single destination transaction request. */
    volatile uint32_t dma_hs_sgl_dst_reserved;                        /*!< reserved. */
    volatile uint32_t dma_hs_lst_src;                                 /*!< Last source transaction request. */
    volatile uint32_t dma_hs_lst_src_reserved;                        /*!< reserved. */
    volatile uint32_t dma_hs_lst_dst;                                 /*!< Last destination transaction request. */
    volatile uint32_t dma_hs_lst_dst_reserved;                        /*!< reserved. */
    volatile uint32_t dma_cfg_reg;                                    /*!< DMA configuration. */
    volatile uint32_t reserved;                                       /*!< reserved. */
    volatile uint32_t dma_ch_en;                                      /*!< DMA channel enable. */
} dma_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif