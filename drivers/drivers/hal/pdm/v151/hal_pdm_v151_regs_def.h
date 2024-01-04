/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V151 register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-31, Create file. \n
 */
#ifndef HAL_PDM_V151_REGS_DEF_H
#define HAL_PDM_V151_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pdm_v151_regs_def PDM V151 Regs Definition
 * @ingroup  drivers_hal_pdm
 * @{
 */

#define PDM_V151_DMIC4 4
#define PDM_V151_DMIC5 5

/**
 * @brief  This union represents the bit fields in the CODEC_CLK_EN1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_clk_en0_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_11           :  12;
        uint32_t mic4_up_afifo_clken    :   1;  /*!< Control the UP FIFO clock of MIC4. */
        uint32_t mic5_up_afifo_clken    :   1;  /*!< Control the UP FIFO clock of MIC5. */
        uint32_t reserved14_24          :  11;
        uint32_t mic4_pga_clken         :   1;  /*!< Control the PGA clock of MIC4. */
        uint32_t mic5_pga_clken         :   1;  /*!< Control the PGA clock of MIC5. */
        uint32_t reserved27_31          :  11;
    } b;                                        /*!< Register bits. */
} pdm_v151_codec_clk_en0_data_t;

/**
 * @brief  This union represents the bit fields in the CODEC_CLK_EN1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_clk_en1_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t reserved0_7            :   8;
        uint32_t mic4_srcdn_clken       :   1;  /*!< Control the SRCDN clock of MIC4. */
        uint32_t mic5_srcdn_clken       :   1;  /*!< Control the SRCDN clock of MIC5. */
        uint32_t reserved10_27          :  18;
        uint32_t mic4_adc_clken         :   1;  /*!< Control the ADC clock of MIC4. */
        uint32_t mic5_adc_clken         :   1;  /*!< Control the ADC clock of MIC5. */
        uint32_t reserved30_31          :   2;
    } b;                                /*!< Register bits. */
} pdm_v151_codec_clk_en1_data_t;

/**
 * @brief  This union represents the bit fields in the CODEC_CLK_EN2. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_clk_en2_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_14           :  15;
        uint32_t dmic0_clken            :   1;  /*!< Control the clock of DMIC0. */
        uint32_t reserved16_19          :   8;
        uint32_t reserved20_24          :   5;
        uint32_t lp_cicdn_adc4_clken    :   1;  /*!< Control the clock of lp_cicdn_adc4. */
        uint32_t lp_cicdn_adc5_clken    :   1;  /*!< Control the clock of lp_cicdn_adc5. */
        uint32_t reserved27_31          :   5;
    } b;                                        /*!< Register bits. */
} pdm_v151_codec_clk_en2_data_t;

/**
 * @brief  This union represents the bit fields in the CODEC_SW_RST_N. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_sw_rst_n_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t codec_sw_rst_n         :   1;  /*!< Reset the codec, 0 is valid. */
        uint32_t reserved1_2            :   2;
        uint32_t rst_2dmic_access_irq   :   1;  /*!< Reset the 2 UP DMIC channeles. */
        uint32_t reserved4_31           :  28;
    } b;                                        /*!< Register bits. */
} pdm_v151_codec_sw_rst_n_data_t;

/**
 * @brief  This union represents the bit fields in the PGA_GAINOFFSET_CTRL2. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_pga_gainoffset_ctrl2_data {
    uint32_t d32;                                   /*!< Raw register data. */
    struct {
        uint32_t mic4_adc_pga_gainoffset    :   8;  /*!< MIC4 Gain offset, the range is 0-255. \n
                                                         The unit of gainoffset is related to fadeInTime. \n
                                                         Unit = max(2 ^ (fadeInTime -12), 1) \n
                                                         For example: \n
                                                         fadeInTime = 16, the valid gainoffset can be 0, 16, 32 ... */
        uint32_t mic5_adc_pga_gainoffset    :   8;  /*!< MIC5 Gain offset, the range is 0-255. \n
                                                         The unit of gainoffset is related to fadeInTime. \n
                                                         Unit = max(2 ^ {(fadeInTime -12)}, 1) \n
                                                         For example: \n
                                                         fadeInTime = 16, the valid gainoffset can be 0, 16, 32 ... */
        uint32_t reserved16_31              :  16;
    } b;                                            /*!< Register bits. */
} pdm_v151_pga_gainoffset_ctrl2_data_t;

/**
 * @brief  This union represents the bit fields in the MIC_PGA_CTRL. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_mic_pga_ctrl_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0              :   1;
        uint32_t pga_linear_sel         :   1;  /*!< Control the PGA linear selection. */
        uint32_t pga_zero_num           :   5;  /*!< Number of zero-crossing detections in PGA. \n
                                                     Default: 5'd20. */
        uint32_t pga_thre_id            :   2;  /*!< Select the PGA threshold, which is used to check
                                                     the number of zero-crossing detections.
                                                        - 0: Select the pga_thre_ctrl[23:0]
                                                        - 1：Select the pga_thre_ctrl[47:24]
                                                        - 2：Select the pga_thre_ctrl[71:48]
                                                        - 3：Select the pga_thre_ctrl[95:72] */
        uint32_t pga_noise_en           :   1;  /*!< Enable of disable the noise of PGA. \n
                                                        - 0: Disable noise, Process as 0 for the signal below threshold.
                                                        - 1: Enable noise, Do nothing for the signal below threshold. */
        uint32_t pga_bypass             :   1;  /*!< Bypass the PGA.
                                                        - 0: Dont bypass
                                                        - 1: Bypass */
        uint32_t pga_fade_out           :   5;  /*!< PGA fade-out time level setting. \n
                                                     @note The actual fade-out time is related to the level setting
                                                           and sampling rate. */
        uint32_t pga_fade_in            :   5;  /*!< PGA fade-in time level setting. \n
                                                     @note The actual fade-in time is related to the level setting
                                                           and sampling rate. */
        uint32_t pga_cfg_small_sig_en   :   1;  /*!< Enable of disable the small-signal of PGA. */
        uint32_t pga_cfg_anti_clip_en   :   1;  /*!< Enable of disable anti-clipping of PGA. */
        uint32_t pga_cfg_fade_en        :   1;  /*!< Enable of disable fade IN/OUT of PGA. */
        uint32_t pga_gain               :   8;  /*!< Gain of the PGA, the range is -120~60dB. \n
                                                     Bit[7] is the sign. \n
                                                     For Example:
                                                        - 0x88: -120dB
                                                        - 0x00: 0dB
                                                        - 0x3C: 60dB */
    } b;                                        /*!< Register bits. */
} pdm_v151_mic_pga_ctrl_data_t;

/**
 * @brief  This union represents the bit fields in the SRCDN_CTRL0. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_srcdn_ctrl0_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_15           :  16;
        uint32_t mic4_srcdn_src_mode    :   3;  /*!< SRC down-sampling decimation multiple configuration of MIC4.
                                                        - 3'b000: 3x
                                                        - 3'b001: Reserved
                                                        - 3'b010: 6x
                                                        - 3'b011: 2x
                                                        - 3'b100: 1.5x
                                                        - Others: Reserved */
        uint32_t mic4_srcdn_fifo_clr    :   1;  /*!< FIFO Clear Signal, high active of MIC4.
                                                        - 0: Dont clear FIFO
                                                        - 1: Clear FIFO. */
        uint32_t mic5_srcdn_src_mode    :   3;  /*!< SRC down-sampling decimation multiple configuration  of MIC5.
                                                        - 3'b000: 3x
                                                        - 3'b001: Reserved
                                                        - 3'b010: 6x
                                                        - 3'b011: 2x
                                                        - 3'b100: 1.5x
                                                        - Others: Reserved */
        uint32_t mic5_srcdn_fifo_clr    :   1;  /*!< FIFO Clear Signal, high active  of MIC5.
                                                        - 0: Dont clear FIFO
                                                        - 1: Clear FIFO. */
        uint32_t reserved24_31          :   8;
    } b;                                        /*!< Register bits. */
} pdm_v151_srcdn_ctrl0_data_t;

/**
 * @brief  This union represents the bit fields in the DMIC_CTRL. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_dmic_ctrl_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t dmic0_sw_dmic_mode     :   1;  /*!< Configuring the DMIC Channel. */
        uint32_t dmic0_reverse          :   1;  /*!< Configuring the input coding.
                                                        - 0: 0 -> +4, 1 -> -4
                                                        - 1: 0 -> -4, 1 -> +4 */
        uint32_t reserved2_31           :  30;
    } b;                                        /*!< Register bits. */
} pdm_v151_dmic_ctrl_data_t;

/**
 * @brief  This union represents the bit fields in the DMIC_DIV. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_dmic_div_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t fs_dmic0               :   1;  /*!< Configure of DMIC Clock
                                                        - 4'b0000: Close clock.
                                                        - 4'b0001: 2x, 3.072MHz
                                                        - 4'b0010: 3x, 2.048MHz
                                                        - 4'b0011: 4x, 1.536MHz
                                                        - 4'b0111: 8x, 768KHz
                                                        - 4'b1011: 12x, 512KHz
                                                        - Others: Close clock */
        uint32_t reserved4_31           :  28;
    } b;                                        /*!< Register bits. */
} pdm_v151_dmic_div_data_t;

/**
 * @brief  This union represents the bit fields in the MIC45_UP_AFIFO_CTRL. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_mic_up_afifo_ctrl_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_4            :   5;
        uint32_t mic4_upfifo_aempty_th  :   5;  /*!< The atemp to be EMPTY threshold of MIC4. */
        uint32_t mic4_upfifo_afull_th   :   5;  /*!< The atemp to be FULL threshold of MIC4. */
        uint32_t mic4_up_fifo_clr       :   1;  /*!< AFIFO Clear Signal, high active of MIC4..
                                                        - 0: Dont clear FIFO
                                                        - 1: Clear FIFO */
        uint32_t reserved16_20          :   5;
        uint32_t mic5_upfifo_aempty_th  :   5;  /*!< The atemp to be EMPTY threshold of MIC5. */
        uint32_t mic5_upfifo_afull_th   :   5;  /*!< The atemp to be FULL threshold of MIC5. */
        uint32_t mic5_up_fifo_clr       :   1;  /*!< AFIFO Clear Signal, high active of MIC5.
                                                        - 0: Dont clear FIFO
                                                        - 1: Clear FIFO */
    } b;                                        /*!< Register bits. */
} pdm_v151_mic_up_afifo_ctrl_data_t;

/**
 * @brief  This union represents the bit fields in the FS_CTRL0. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_fs_ctrl0_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_23           :  24;
        uint32_t fs_mic4_afifo          :   3;  /*!< Configuring the input sampling rate of MIC4.
                                                        - 3'b000: 16K
                                                        - 3'b001: 32K
                                                        - 3'b010: 48K
                                                        - 3'b011: 96K
                                                        - Others: 8K */
        uint32_t fs_mic5_afifo          :   3;  /*!< Configuring the input sampling rate of MIC5.
                                                        - 3'b000: 16K
                                                        - 3'b001: 32K
                                                        - 3'b010: 48K
                                                        - 3'b011: 96K
                                                        - Others: 8K */
        uint32_t reserved30_31          :   2;
    } b;                                        /*!< Register bits. */
} pdm_v151_fs_ctrl0_data_t;

/**
 * @brief  This union represents the bit fields in the FS_CTRL1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_fs_ctrl1_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_23           :  24;
        uint32_t fs_mic4_srcdn_dout     :   3;  /*!< Configuring the SRCDN output sampling rate of MIC4 ADC.
                                                        - 3'b000: 16K
                                                        - 3'b001: 32K
                                                        - 3'b010: 48K
                                                        - 3'b011: 96K
                                                        - Others: 8K */
        uint32_t fs_mic5_srcdn_dout     :   3;  /*!< Configuring the SRCDN output sampling rate of MIC5 ADC.
                                                        - 3'b000: 16K
                                                        - 3'b001: 32K
                                                        - 3'b010: 48K
                                                        - 3'b011: 96K
                                                        - Others: 8K */
        uint32_t reserved30_31          :   2;
    } b;                                        /*!< Register bits. */
} pdm_v151_fs_ctrl1_data_t;

/**
 * @brief  This union represents the bit fields in the CODEC_ADC_DC_OFFSET. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_adc_dc_offset_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t adc_dc_offset          :  24;  /*!< Configuring the ADC HPF DC offset. */
        uint32_t reserved24_31          :   8;
    } b;                                        /*!< Register bits. */
} pdm_v151_codec_adc_dc_offset_data_t;

/**
 * @brief  This union represents the bit fields in the CODEC_ADC_CIC_GAIN. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_adc_cic_gain_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t adc4_cic_gain          :   6;  /*!< Configuring the CIC gain of ADC4. */
        uint32_t reserved6_7            :   2;
        uint32_t adc5_cic_gain          :   6;  /*!< Configuring the CIC gain of ADC5. */
        uint32_t reserved14_31          :  16;
    } b;                                        /*!< Register bits. */
} pdm_v151_codec_adc_cic_gain_data_t;

/**
 * @brief  This union represents the bit fields in the LP_CIC_GAIN1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_lp_cic_gain1_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cicdn_adc4_gain        :   8;  /*!< Configuring the LP CIC gain of ADC4. */
        uint32_t cicdn_adc5_gain        :   8;  /*!< Configuring the LP CIC gain of ADC5. */
        uint32_t reserved14_31          :  16;
    } b;                                        /*!< Register bits. */
} pdm_v151_lp_cic_gain1_data_t;

/**
 * @brief  This union represents the bit fields in the PDM_EVENT. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
union pdm_v151_event_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t mic5_fifo_afull        :   1;  /*!< FIFO Atemp to full event of MIC5. */
        uint32_t mic4_fifo_afull        :   1;  /*!< FIFO Atemp to full event of MIC4. */
        uint32_t reserved2_3            :   2;
        uint32_t mic5_fifo_full         :   1;  /*!< FIFO Full event of MIC5. */
        uint32_t mic4_fifo_full         :   1;  /*!< FIFO Full event of MIC4. */
        uint32_t reserved6_31           :  26;
    } b;                                        /*!< Register bits. */
};

/**
 * @brief  This union represents the bit fields in the PDM_EVENT_CLR. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_event_data pdm_v151_event_clr_data_t;

/**
 * @brief  This union represents the bit fields in the PDM_EVENT_ST. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_event_data pdm_v151_event_st_data_t;

/**
 * @brief  This union represents the bit fields in the ADC_FILTER. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_adc_filter_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_15           :  16;
        uint32_t adc4_compd_bypass_en   :   1;  /*!< Bypass the COMPD of ADC4. */
        uint32_t adc4_fir2d_en          :   1;  /*!< Bypass the FIR2D of ADC4. */
        uint32_t adc4_com2d_en          :   1;  /*!< Bypass the COM2D of ADC4. */
        uint32_t adc4_hpf_en            :   1;  /*!< Bypass the HPF of ADC4. */
        uint32_t adc5_compd_bypass_en   :   1;  /*!< Bypass the COMPD of ADC5. */
        uint32_t adc5_fir2d_en          :   1;  /*!< Bypass the FIR2D of ADC5. */
        uint32_t adc5_com2d_en          :   1;  /*!< Bypass the COM2D of ADC5. */
        uint32_t adc5_hpf_en            :   1;  /*!< Bypass the HPF of ADC5. */
        uint32_t reserved24_31          :   8;
    } b;                                        /*!< Register bits. */
} pdm_v151_adc_filter_data_t;

/**
 * @brief  This union represents the bit fields in the CODEC_CLK_DIV0. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pdm_v151_codec_clk_div0_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0_7            :   8;
        uint32_t clk_6144k_div_ratio    :   7;  /*!< Configuring the 6144K Clock divide ratio. */
        uint32_t reserved15_31          :  17;
    } b;                                        /*!< Register bits. */
} pdm_v151_codec_clk_div0_data_t;

/**
 * @brief  Registers associated with PDM V151.
 */
typedef struct pdm_v151_regs {
    volatile uint32_t version;                  /*!< This register is the version of PDM V151. <i>Offset: 00h</i>. */
    volatile uint32_t codec_clk_en0;            /*!< This register control the clock of
                                                     Codec subsystem 0. <i>Offset: 04h</i>. */
    volatile uint32_t codec_clk_en1;            /*!< This register control the clock of
                                                     Codec subsystem 1. <i>Offset: 08h</i>. */
    volatile uint32_t codec_clk_en2;            /*!< This register control the clock of
                                                     Codec subsystem 2. <i>Offset: 0Ch</i>. */
    volatile uint32_t codec_sw_rst_n;           /*!< This register reset the codec. <i>Offset: 10h</i>. */
    volatile uint32_t rsvd_14_1c[3];            /*!< Reserved 3: 14h ~ 1Ch. */
    volatile uint32_t pga_thre_ctrl0;           /*!< This register control PGA/MIXER threshold 0. <i>Offset: 20h</i>. */
    volatile uint32_t pga_thre_ctrl1;           /*!< This register control PGA/MIXER threshold 1. <i>Offset: 24h</i>. */
    volatile uint32_t pga_thre_ctrl2;           /*!< This register control PGA/MIXER threshold 2. <i>Offset: 28h</i>. */
    volatile uint32_t rsvd_2c_30[2];            /*!< Reserved 2: 2Ch ~ 30h. */
    volatile uint32_t pga_gainoffset_ctrl2;     /*!< This register control the PGA GAINOFFSET. <i>Offset: 34h</i>. */
    volatile uint32_t rsvd_38_54[8];            /*!< Reserved 8: 38h ~ 54h. */
    volatile uint32_t mic4_pga_ctrl;            /*!< This register control the PGA of MIC4. <i>Offset: 58h</i>. */
    volatile uint32_t mic5_pga_ctrl;            /*!< This register control the PGA of MIC5. <i>Offset: 5Ch</i>. */
    volatile uint32_t rsvd_60_80[9];            /*!< Reserved 9: 60h ~ 80h. */
    volatile uint32_t srcdn_ctrl0;              /*!< This register control the SRCDN 0. <i>Offset: 84h</i>. */
    volatile uint32_t rsvd_88_90[3];            /*!< Reserved 3: 88h ~ 90h. */
    volatile uint32_t dmic_ctrl;                /*!< This register control the DMIC. <i>Offset: 94h</i>. */
    volatile uint32_t dmic_div;                 /*!< This register control the Devider of DMIC. <i>Offset: 98h</i>. */
    volatile uint32_t rsvd_9c_b4[7];            /*!< Reserved 7: 9Ch ~ B4h. */
    volatile uint32_t mic_up_afifo_ctrl;        /*!< This register control the AFIFO of MIC. <i>Offset: B8h</i>. */
    volatile uint32_t rsvd_bc_c4[3];            /*!< Reserved 3: BCh ~ C4h. */
    volatile uint32_t fs_ctrl0;                 /*!< This register control the
                                                     internal sample rate of Codec 0. <i>Offset: C8h</i>. */
    volatile uint32_t fs_ctrl1;                 /*!< This register control the
                                                     internal sample rate of Codec 1. <i>Offset: CCh</i>. */
    volatile uint32_t rsvd_d0_e8[7];            /*!< Reserved 7: D0h ~ E8h. */
    volatile uint32_t codec_adc4_dc_offset;     /*!< This register control the DC_OFFSET of ADC4. <i>Offset: ECh</i>. */
    volatile uint32_t codec_adc5_dc_offset;     /*!< This register control the DC_OFFSET of ADC5. <i>Offset: F0h</i>. */
    volatile uint32_t rsvd_f4_108[6];           /*!< Reserved 6: F4h ~ 108h. */
    volatile uint32_t codec_adc_cic_gain;       /*!< This register control the CIC gain of ADC. <i>Offset: 10Ch</i>. */
    volatile uint32_t rsvd_110_300[125];        /*!< Reserved 125: 110h ~ 300h. */
    volatile uint32_t lp_cic_gain1;             /*!< This register control the LPCICDN GAIN1. <i>Offset: 304h</i>. */
    volatile uint32_t rsvd_308_30c[2];          /*!< Reserved 2: 308h ~ 30Ch. */
    volatile uint32_t event_clr;                /*!< This register clear the Event of PDM. <i>Offset: 310h</i>. */
    volatile uint32_t rsvd_314_31c[3];          /*!< Reserved 3: 314h ~ 31Ch. */
    volatile uint32_t event_st;                 /*!< This register store the status
                                                     of PDM Event. <i>Offset: 320h</i>. */
    volatile uint32_t rsvd_324_4f0[116];        /*!< Reserved 116: 324h ~ 4F0h. */
    volatile uint32_t adc_filter;               /*!< This register enable of disable
                                                     the ADC Filter. <i>Offset: 4F4h</i>. */
    volatile uint32_t rsvd_4f8;                 /*!< Reserved 1: 4f8h. */
    volatile uint32_t codec_clk_div0;           /*!< This register control the Codec Clock Div0. <i>Offset: 4FCh</i>. */
} pdm_v151_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif