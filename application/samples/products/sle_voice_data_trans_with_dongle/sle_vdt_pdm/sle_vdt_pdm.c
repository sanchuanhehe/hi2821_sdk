/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone PDM Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#include "osal_debug.h"
#include "pinctrl.h"
#include "hal_dma.h"
#include "pdm.h"
#include "hal_pdm.h"
#include "sle_vdt_pdm.h"

#define MICROPHONE_PDM_DMA_PRIORITY 3

#define MICROPHONE_PDM_LOG "[sle_vdt pdm]"

int32_t sle_vdt_pdm_init(void)
{
    errcode_t ret;
    uint8_t mic = HAL_PDM_DMIC_DUAL;
#if defined(CONFIG_SLE_PDM_DMIC_4)
    mic = HAL_PDM_DMIC_4;
#elif defined(CONFIG_SLE_PDM_DMIC_5)
    mic = HAL_PDM_DMIC_5;
#elif defined(CONFIG_SLE_PDM_DMIC_DUAL)
    mic = HAL_PDM_DMIC_DUAL;
#endif

#if defined(CONFIG_SLE_PDM_SAMPLE_RATE_8K)
    uint8_t fs_ctrl_freq = PDM_MIC_FRE_8K;
#elif defined(CONFIG_SLE_PDM_SAMPLE_RATE_16K)
    uint8_t fs_ctrl_freq = PDM_MIC_FRE_16K;
#elif defined(CONFIG_SLE_PDM_SAMPLE_RATE_32K)
    uint8_t fs_ctrl_freq = PDM_MIC_FRE_32K;
#elif defined(CONFIG_SLE_PDM_SAMPLE_RATE_48K)
    uint8_t fs_ctrl_freq = PDM_MIC_FRE_48K;
#elif defined(CONFIG_SLE_PDM_SAMPLE_RATE_96K)
    uint8_t fs_ctrl_freq = PDM_MIC_FRE_96K;
#endif

    uapi_pin_set_mode(S_MGPIO30, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO31, PIN_MODE_1);

    if (uapi_pdm_init() != ERRCODE_SUCC) {
        return 1;
    }

    pdm_config_t config = { 0 };
    config.fs_ctrl_freq = fs_ctrl_freq;
    config.linear_select = 0;
    config.zero_num = 0x14;
    config.threshold_id = 0;
    config.noise_enable = 0;
    config.pga_bypass_enable = 0;
    config.fade_out_time = 0;
    config.fade_in_time = 0;
    config.little_signal = 0;
    config.anti_clip = 0;
    config.fade_in_out = 0;
    config.pga_gain = 0x28;
    config.srcdn_src_mode = DOUBLE_EXTRACT;
    ret = uapi_pdm_set_attr(mic, &config);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the PDM fail. %x\r\n", MICROPHONE_PDM_LOG, ret);
        return 1;
    }

    osal_printk("%s Start the PDM transfer success.\r\n", MICROPHONE_PDM_LOG);

    return 0;
}

int32_t sle_vdt_pdm_start_dma_transfer(uint32_t *pcm_buffer, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;
    uint8_t channel = 0;

    unused(transfer_config);

    transfer_config.src = uapi_pdm_get_fifo_addr();
    transfer_config.dest = (uint32_t)(uintptr_t)pcm_buffer;
    transfer_config.transfer_num = CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA;
    transfer_config.src_handshaking = 0x6;  /* MIC45_UPLINK_REQ: pdm两路mic的fifo握手通道 */
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = MICROPHONE_PDM_DMA_PRIORITY;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 1;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    errcode_t ret = uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel,
                                                                  trans_done, (uintptr_t)NULL);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the DMA fail. %x\r\n", MICROPHONE_PDM_LOG, ret);
        return 1;
    }

    ret = uapi_dma_start_transfer(channel);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Start the DMA fail. %x\r\n", MICROPHONE_PDM_LOG, ret);
        return 1;
    }

    return 0;
}

uint32_t sle_vdt_pdm_get_fifo_deepth(void)
{
    return uapi_pdm_get_fifo_deepth();
}