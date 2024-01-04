/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides hal DMA \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-10-16， Create file. \n
 */
#ifndef HAL_DMA_H
#define HAL_DMA_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
/**
 * @defgroup drivers_hal_dma_api DMA
 * @ingroup  drivers_hal_dma
 * @{
 */

/**
 * @if Eng
 * @brief  DMA channel transfer done/error callback function pointer type of user for hal.
 * @param  [in]  intr DMA interrupt type.
 * @else
 * @brief  hal层DMA通道传输完成/错误所触发的回调函数指针
 * @param  [in]  intr DMA中断类型
 * @endif
 */
typedef void (*hal_dma_transfer_cb_t)(uint8_t intr, uint8_t channel, uintptr_t arg);

/**
 * @if Eng
 * @brief  DMA transfer data width.
 * @else
 * @brief  DMA传输数据宽度
 * @endif
 */
typedef enum hal_dma_data_width {
    HAL_DMA_TRANSFER_WIDTH_8,
    HAL_DMA_TRANSFER_WIDTH_16,
    HAL_DMA_TRANSFER_WIDTH_32,
    HAL_DMA_TRANSFER_WIDTH_64,
    HAL_DMA_TRANSFER_WIDTH_128,
    HAL_DMA_TRANSFER_WIDTH_256
} hal_dma_data_width_t;

/**
 * @if Eng
 * @brief  DMA transfer burst length.
 * @else
 * @brief  DMA传输burst长度
 * @endif
 */
typedef enum hal_dma_burst_transaction_length {
    HAL_DMA_BURST_TRANSACTION_LENGTH_1,
    HAL_DMA_BURST_TRANSACTION_LENGTH_4,
    HAL_DMA_BURST_TRANSACTION_LENGTH_8,
    HAL_DMA_BURST_TRANSACTION_LENGTH_16,
    HAL_DMA_BURST_TRANSACTION_LENGTH_32,
    HAL_DMA_BURST_TRANSACTION_LENGTH_64,
    HAL_DMA_BURST_TRANSACTION_LENGTH_128,
    HAL_DMA_BURST_TRANSACTION_LENGTH_256
} hal_dma_burst_transaction_length_t;

/**
 * @if Eng
 * @brief  DMA src/dst address increment.
 * @else
 * @brief  DMA源/目的端地址增量模式长度
 * @endif
 */
typedef enum hal_dma_address_inc {
    HAL_DMA_ADDRESS_INC_INCREMENT,
    HAL_DMA_ADDRESS_INC_DECREMENT,
    HAL_DMA_ADDRESS_INC_NO_CHANGE,
    HAL_DMA_ADDRESS_INC_TYPES
} hal_dma_address_inc_t;

/**
 * @if Eng
 * @brief  DMA Channel priority.
 * @else
 * @brief  DMA传输优先级
 * @endif
 */
typedef enum hal_dma_ch_priority {
    HAL_DMA_CH_PRIORITY_0,
    HAL_DMA_CH_PRIORITY_1,
    HAL_DMA_CH_PRIORITY_2,
    HAL_DMA_CH_PRIORITY_3
} hal_dma_ch_priority_t;

/**
 * @if Eng
 * @brief  DMA transfer type.
 * @else
 * @brief  DMA传输类型
 * @endif
 */
typedef enum hal_dma_trans_type {
    HAL_DMA_TRANS_MEMORY_TO_MEMORY_DMA,
    HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA,
    HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA,
    HAL_DMA_TRANS_PERIPHERAL_TO_PERIPHERAL_DMA,
    HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_PERIPHERAL,
    HAL_DMA_TRANS_PERIPHERAL_TO_PERIPHERAL_SRC,
    HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_PERIPHERAL,
    HAL_DMA_TRANS_PERIPHERAL_TO_PERIPHERAL_DST
} hal_dma_trans_type_t;

/**
 * @if Eng
 * @brief  DMA protection control bits used to drive the AHB HPROT[3:1] bus.
 * @else
 * @brief  DMA保护控制位，用于驱动AHB HPRO[3:1]总线
 * @endif
 */
typedef enum hal_dma_protection_control {
    /** @if Eng  The access is in user mode, and can not be buffered or cached.
     *  @else     访问处于用户模式，不能缓存或者cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_NONE,
    /** @if Eng  The access is in privileged mode, and can not be buffered or cached.
     *  @else     访问处于特权模式，不能缓存或者cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_PRIVILEGED,
    /** @if Eng  The access is in user mode, and can be buffered but can not be cached.
     *  @else     访问处于用户模式，能缓存但是不能cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_BUFFERABLE,
    /** @if Eng  The access is in privileged mode, and can be buffered but can not be cached.
     *  @else     访问处于特权模式，能缓存但是不能cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_PRIVILEGED_BUFFERABLE,
    /** @if Eng  The access is in user mode, and can not be buffered but can be cached.
     *  @else     访问处于用户模式，不能缓存但是能cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_CACHEABLE,
    /** @if Eng  The access is in privileged mode, and can not be buffered but can be cached.
     *  @else     访问处于特权模式，不能缓存但是能cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_PRIVILEGED_CACHEABLE,
    /** @if Eng  The access is in user mode, and can be buffered or cached.
     *  @else     访问处于用户模式，能缓存或者cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_BUFFERABLE_CACHEABLE,
    /** @if Eng  The access is in privileged mode, and can be buffered or cached.
     *  @else     访问处于特权模式，能缓存或者cache。
     *  @endif */
    HAL_DMA_PROTECTION_CONTROL_ALL
} hal_dma_protection_control_t;

/**
 * @if Eng
 * @brief  DMA handshaking status.
 * @else
 * @brief  DMA外设握手状态
 * @endif
 */
typedef enum hal_dma_mux_handshaking_status {
    HAL_DMA_MUX_HANDSHAKING_IDLE,
    HAL_DMA_MUX_HANDSHAKING_USING
} hal_dma_mux_handshaking_status_t;

/**
 * @if Eng
 * @brief  DMA transfer direction.
 * @else
 * @brief  DMA传输方向
 * @endif
 */
typedef enum dma_ch_transfer_dir {
    HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL,
    HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM,
    HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_PERIPHERAL
} dma_ch_transfer_dir_t;

/**
 * @if Eng
 * @brief  DMA Handshaking Select
 * @else
 * @brief  DMA握手选择
 * @endif
 */
typedef enum hal_dma_handshaking_select {
    HAL_DMA_HARDWARE_HANDSHAKING = 0x0,
    HAL_DMA_SOFTWARE_HANDSHAKING = 0x1
} hal_dma_handshaking_select_t;

/**
 * @if Eng
 * @brief  DMA Interrupt type.
 * @else
 * @brief  DMA中断类型
 * @endif
 */
typedef enum hal_dma_interrupt {
    HAL_DMA_INTERRUPT_TFR,
    HAL_DMA_INTERRUPT_BLOCK,
    HAL_DMA_INTERRUPT_SRCTRAN,
    HAL_DMA_INTERRUPT_DSTTRAN,
    HAL_DMA_INTERRUPT_ERR
} hal_dma_interrupt_t;

/**
 * @if Eng
 * @brief  DMA Master Select.
 * @else
 * @brief  DMA主机选择
 * @endif
 */
typedef enum hal_dma_master_select {
    HAL_DMA_MASTER_SELECT_DEFAULT = 0x0,
    HAL_DMA_MASTER_SELECT_0 = 0x0,
    HAL_DMA_MASTER_SELECT_1 = 0x1,
    HAL_DMA_MASTER_SELECT_2 = 0x2,
    HAL_DMA_MASTER_SELECT_3 = 0x3,
    HAL_DMA_MASTER_SELECT_HIGHEST = 0x3,
    HAL_DMA_MASTER_SELECT_ERROR = 0x4
} hal_dma_master_select_t;

/**
 * @if Eng
 * @brief  DMA channel working state.
 * @else
 * @brief  DMA通道工作状态
 * @endif
 */
typedef enum hal_dma_ch_state {
    /** @if Eng  The DMA channel is closed.
     *  @else    DMA通道关闭中
     *  @endif */
    HAL_DMA_CH_STATE_CLOSED,
    /** @if Eng  The DMA channel has been opend but its logic configuration queue has no configurations to be sent.
     *  @else    DMA通道已打开，但其配置队列中没有要发送的数据
     *  @endif */
    HAL_DMA_CH_STATE_IDLE,
    /** @if Eng  The DMA channel is transferring data or has been done the data transferring.
     *  @else    DMA通道正在传输数据或已完成数据传输
     *  @endif */
    HAL_DMA_CH_STATE_ACTIVE,
    /** @if Eng  The DMA channel has had error while transferring data.
     *  @else    DMA通道在传输数据时出错
     *  @endif */
    HAL_DMA_CH_STATE_ERROR
} hal_dma_ch_state_t;

/**
 * @if Eng
 * @brief  DMA transfer base config.
 * @else
 * @brief  DMA传输基本配置
 * @endif
 */
typedef struct hal_dma_transfer_base_config {
    uint32_t src;
    uint32_t dest;
    uint16_t transfer_num;
    hal_dma_ch_priority_t priority;
    hal_dma_data_width_t src_width;
    hal_dma_data_width_t dest_width;
    hal_dma_address_inc_t src_inc;
    hal_dma_address_inc_t dest_inc;
    hal_dma_burst_transaction_length_t src_burst_trans_length;
    hal_dma_burst_transaction_length_t dest_burst_trans_length;
    hal_dma_transfer_cb_t callback;
    uintptr_t priv_arg;
} hal_dma_transfer_base_config_t;

/**
 * @if Eng
 * @brief  DMA transfer peripheral config.
 * @else
 * @brief  DMA传输外设配置
 * @endif
 */
typedef struct hal_dma_transfer_peri_config {
    hal_dma_trans_type_t trans_type;
    dma_ch_transfer_dir_t trans_dir;
    hal_dma_handshaking_source_t hs_source;
    hal_dma_handshaking_source_t hs_dest;
    hal_dma_protection_control_t protection;
} hal_dma_transfer_peri_config_t;

/**
 * @if Eng
 * @brief  Distinguish DMA type.
 * @else
 * @brief  区分DMA类型
 * @endif
 */
typedef enum hal_dma_type {
    /** @if Eng  Big dma, used for spi, opi, qspi0, qpsi1.
     *  @else    大dma，用于spi、opi、qspi0、qpsi1
     *  @endif */
    HAL_DMA_TYPE_BDMA,
    /** @if Eng  small dma, used for i2c, uart L0, uart H0, uart H1, IR.
     *  @else    小dma，用于i2c、uart L0、uart H0、uart H1、IR
     *  @endif */
    HAL_DMA_TYPE_SDMA
} hal_dma_type_t;

/**
 * @if Eng
 * @brief  Initialize device for hal DMA.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层DMA的初始化接口
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_dma_init_t)(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal DMA.
 * @else
 * @brief  HAL层DMA的去初始化接口
 * @endif
 */
typedef void (*hal_dma_deinit_t)(void);

 /**
 * @if Eng
 * @brief  Open interface for hal DMA.
 * @else
 * @brief  HAL层DMA开启接口
 * @endif
 */
typedef void (*hal_dma_open_t)(void);

 /**
 * @if Eng
 * @brief  Close interface for hal DMA.
 * @else
 * @brief  HAL层DMA关闭接口
 * @endif
 */
typedef void (*hal_dma_close_t)(void);

/**
 * @if Eng
 * @brief  Channel enable/disable interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  en flag of enable or disable.
 * @else
 * @brief  HAL层通道使能/去使能接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  en 使能/去使能标志
 * @endif
 */
typedef void (*hal_dma_ch_enable_t)(dma_channel_t ch, bool en);

/**
 * @if Eng
 * @brief  Get the number of data transferred by the DMA for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @retval  The number of data transferred by the DMA.
 * @else
 * @brief  HAL层获取DMA传输的数据量接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @retval  DMA传输的数据数
 * @endif
 */
typedef uint32_t (*hal_dma_get_block_t)(dma_channel_t ch);

/**
 * @if Eng
 * @brief  Interrupt clear interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  int_type Interrupt type. For details, see @ref hal_dma_interrupt_t
 * @else
 * @brief  HAL层清除中断接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  int_type 中断类型 参考 @ref hal_dma_interrupt_t
 * @endif
 */
typedef void (*hal_dma_clear_t)(dma_channel_t ch, hal_dma_interrupt_t int_type);

/**
 * @if Eng
 * @brief  Get the idle channel interface for hal DMA.
 * @param  [in]  source DMA handshaking source select. For details, see @ref hal_dma_handshaking_source_t
 * @param  [in]  burst_length DMA transfer burst length. For details, see @ref hal_dma_burst_transaction_length_t
 * @retval The DMA channel. For details, see @ref dma_channel_t
 * @else
 * @brief  HAL层获取闲置通道接口
 * @param  [in]  source DMA握手来源选择
 * @param  [in]  burst_length DMA传输burst长度
 * @retval DMA通道 参考 @ref dma_channel_t
 * @endif
 */
typedef dma_channel_t (*hal_dma_get_idle_t)(hal_dma_handshaking_source_t source,
                                            hal_dma_burst_transaction_length_t burst_length);

/**
 * @if Eng
 * @brief  Configure a Single-block Transfer interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  base_cfg Base config for this transfer. For details, see @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg Peripheral config for this transfer. For details, see @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层配置单块传输
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  base_cfg 传输的基本配置 参考 @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg 传输的外设配置 参考 @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_dma_cfg_sgl_t)(dma_channel_t ch,
                                       const hal_dma_transfer_base_config_t *base_cfg,
                                       hal_dma_transfer_peri_config_t *periph_cfg);

/**
 * @if Eng
 * @brief  Dma lli transfer config interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  base_cfg Base config for this transfer. For details, see @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg Peripheral config for this transfer. For details, see @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层配置链表传输
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @param  [in]  base_cfg 传输的基本配置 参考 @ref hal_dma_transfer_base_config_t
 * @param  [in]  periph_cfg 传输的外设配置 参考 @ref hal_dma_transfer_peri_config_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_dma_add_lli_t)(dma_channel_t ch,
                                       const hal_dma_transfer_base_config_t *base_cfg,
                                       hal_dma_transfer_peri_config_t *periph_cfg);

/**
 * @if Eng
 * @brief  Check whether the DMA channel is enabled interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @retval true   enabled.
 * @retval false  disable.
 * @else
 * @brief  HAL层检查DMA通道是否使能接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @retval true   使能
 * @retval false  未使能
 * @endif
 */
typedef bool (*hal_dma_is_enabled_t)(dma_channel_t ch);

/**
 * @if Eng
 * @brief  Enable dma lli transfer interface for hal DMA.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @else
 * @brief  HAL层使能链表传输接口
 * @param  [in]  ch DMA通道 参考 @ref dma_channel_t
 * @endif
 */
typedef void (*hal_dma_enable_lli_t)(dma_channel_t ch, hal_dma_transfer_cb_t callback, uintptr_t arg);

/**
 * @if Eng
 * @brief  Interface between DMA driver and DMA hal.
 * @else
 * @brief  Driver层dma和HAL层DMA的接口
 * @endif
 */
typedef struct hal_dma_funcs {
    hal_dma_init_t        init;             /*!< @if Eng Init device interface.
                                                 @else   HAL层DMA的初始化接口 @endif */
    hal_dma_deinit_t      deinit;           /*!< @if Eng Deinit device interface.
                                                 @else   HAL层DMA去初始化接口 @endif */
    hal_dma_open_t        open;             /*!< @if Eng Open interface.
                                                 @else   HAL层DMA开启接口 @endif */
    hal_dma_close_t       close;            /*!< @if Eng Close interface.
                                                 @else   HAL层DMA关闭接口 @endif */
    hal_dma_ch_enable_t   ch_enable;        /*!< @if Eng Channel enable/disable interface.
                                                 @else   HAL层DMA通道使能/去使能接口 @endif */
    hal_dma_get_block_t   get_block;        /*!< @if Eng Get the number of data transferred by the DMA interface.
                                                 @else   HAL层DMA获取DMA传输的数据量接口 @endif */
    hal_dma_clear_t       clear;            /*!< @if Eng Interrupt clear interface
                                                 @else   HAL层DMA清除中断接口 @endif */
    hal_dma_get_idle_t    get_idle;         /*!< @if Eng Get the idle channel interface.
                                                 @else   HAL层DMA获取闲置通道接口 @endif */
    hal_dma_cfg_sgl_t     cfg_single;       /*!< @if Eng Configure a Single-block Transfer interface.
                                                 @else   HAL层DMA配置单块传输接口 @endif */
    hal_dma_add_lli_t     add_lli;          /*!< @if Eng Dma lli transfer config interface.
                                                 @else   HAL层DMA配置链表传输接口 @endif */
    hal_dma_is_enabled_t  is_enabled;       /*!< @if Eng Check whether the DMA channel is enabled interface.
                                                 @else   HAL层检查DMA通道是否使能接口 @endif */
    hal_dma_enable_lli_t  enable_lli;       /*!< @if Eng Enable dma lli transfer interface.
                                                 @else   HAL层DMA使能链表传输接口 @endif */
} hal_dma_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_dma_funcs_t into the g_hal_dmas_funcs.
 * @param  [out] funcs Interface between DMA driver and DMA hal.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_dma_funcs_t 到 g_hal_dmas_funcs
 * @param  [out] funcs Driver层dma和HAL层DMA的接口实例
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t hal_dma_register_funcs(hal_dma_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_dma_funcs_t from the g_hal_dmas_funcs.
 * @else
 * @brief  从g_hal_dmas_funcs注销 @ref hal_dma_funcs_t
 * @endif
 */
void hal_dma_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between DMA driver and DMA hal, see @ref hal_dma_funcs_t.
 * @return Interface between DMA driver and DMA hal, see @ref hal_dma_funcs_t.
 * @else
 * @brief  获取Driver层dma和HAL层DMA的接口实例，参考 @ref hal_dma_funcs_t.
 * @return Driver层dma和HAL层DMA的接口实例，参考 @ref hal_dma_funcs_t.
 * @endif
 */
hal_dma_funcs_t *hal_dma_get_funcs(void);

/**
 * @if Eng
 * @brief  Set the handshaking channel mux.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [out] per_cfg DMA channel peripheral configuration. For details, see @ref hal_dma_transfer_peri_config_t
 * @return ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置硬件握手号复用
 * @param  [in]  ch DMA通道，参考 @ref dma_channel_t
 * @param  [out] per_cfg DMA外设配置，参考 @ref hal_dma_transfer_peri_config_t
 * @return ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t dma_port_set_mux_channel(dma_channel_t ch, hal_dma_transfer_peri_config_t *per_cfg);

/**
 * @if Eng
 * @brief  Set the channel select of handshaking.
 * @param  [in]  ch The handshaking channel. For details, see @ref hal_dma_hardshaking_channel_t
 * @param  [in]  source The handshaking select. For details, see @ref hal_dma_handshaking_source_t
 * @else
 * @brief  设置硬件握手选择.
 * @param  [in]  ch 硬件握手通道号，参考 @ref hal_dma_hardshaking_channel_t
 * @param  [in]  source 硬件握手选择，参考 @ref hal_dma_handshaking_source_t
 * @endif
 */
void dma_port_set_channel_handshaking_source(hal_dma_hardshaking_channel_t ch, hal_dma_handshaking_source_t source);

/**
 * @if Eng
 * @brief  Get the master select.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t
 * @param  [in]  addr Source/destination address.
 * @retval The master select. For details, see @ref hal_dma_master_select_t
 * @else
 * @brief  获取master选择.
 * @param  [in]  ch DMA通道，参考 @ref dma_channel_t
 * @param  [in]  addr 源或目的地址
 * @retval master选择，参考 @ref hal_dma_master_select_t
 * @endif
 */
hal_dma_master_select_t dma_port_get_master_select(dma_channel_t ch, uint32_t addr);

/**
 * @if Eng
 * @brief  Init the dma which will set the base address of registers.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化DMA，设置寄存器的基地址。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_dma_regs_init(void);

/**
 * @if Eng
 * @brief  Deinit the hal_drv_dma which will clear the base address of registers has been
 *         set by @ref hal_dma_regs_init.
 * @else
 * @brief  去初始化，然后清除在 @ref hal_dma_regs_init 中设置的寄存器地址。
 * @endif
 */
void hal_dma_regs_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif