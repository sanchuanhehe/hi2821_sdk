/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V152 adc register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-30， Create file. \n
 */

#ifndef HAL_ADC_V152_REGS_DEF_H
#define HAL_ADC_V152_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_v152_regs_def ADC V152 Regs Definition
 * @ingroup  drivers_hal_adc
 * @{
 */

#define HAL_ADC_V152_CFG_DELAY_5    5
#define HAL_ADC_V152_CFG_DELAY_10   10
#define HAL_ADC_V152_CFG_DELAY_30   30
#define HAL_ADC_V152_CFG_DELAY_50   50
#define HAL_ADC_V152_CFG_DELAY_150  150
#define HAL_ADC_V152_CFG_DELAY_1000 1000
#define HAL_ADC_V152_CFG_DELAY_2000 2000

/**
 * @brief ADLDO vset, 50mV/step
 */
typedef enum adc_v152_adldo_voltage {
    VOLTAGE_2_4,
    VOLTAGE_2_45,
    VOLTAGE_2_5,
    VOLTAGE_2_55,
    VOLTAGE_2_6,
    VOLTAGE_2_65,
    VOLTAGE_2_7,
    VOLTAGE_2_75,
    VOLTAGE_2_85,
    VOLTAGE_2_9,
    VOLTAGE_2_95,
    VOLTAGE_3_0,
    VOLTAGE_3_05,
    VOLTAGE_3_1,
    VOLTAGE_3_2,
    VOLTAGE_3_3
} adc_v152_adldo_voltage_t;

typedef enum adc_v152_data_mode {
    COUNT_MODE,
    CONTIUNE_MODE
} adc_v152_data_mode_t;

typedef enum adc_v152_gadc_channel_sel {
    SINGLE_END_AINP1  = 0,
    SINGLE_END_AINN1  = 1,
    SINGLE_END_AINP2  = 2,
    SINGLE_END_AINN2  = 3,
    SINGLE_END_AINP3  = 4,
    SINGLE_END_AINN3  = 5,
    SINGLE_END_AINP4  = 6,
    SINGLE_END_AINN4  = 7,
    DIFFERENTIAL_AIN1 = 0,
    DIFFERENTIAL_AIN2 = 1,
    DIFFERENTIAL_AIN3 = 2,
    DIFFERENTIAL_AIN4 = 3
} adc_v152_gadc_channel_sel_t;

typedef enum adc_v152_pga_gain {
    PGA_GAIN_ONE,
    PGA_GAIN_TWO,
    PGA_GAIN_FOUR,
    PGA_GAIN_EIGHT,
    PGA_GAIN_SIXTEEN,
    PGA_GAIN_THIRTY_TWO,
    PGA_GAIN_STOP
} adc_v152_pga_gain_t;

typedef enum adc_v152_ia_gain {
    IA_GAIN_ONE,
    IA_GAIN_TWO,
    IA_GAIN_FOUR,
    IA_GAIN_EIGHT,
    IA_GAIN_SIXTEEN,
    IA_GAIN_STOP
} adc_v152_ia_gain_t;

typedef enum adc_v152_diag_node {
    GADC_RAW_DATA_INPUT,
    GADC_WEIGHTED_OUTPUT,
    GADC_ACCUMULATED_AVERAGE_OUTPUT,
    GADC_COMPARATOR_OS_CORRECTION_VALUE,
    HADC_RAW_DATA_INPUT,
    HADC_WEIGHTED_OUTPUT,
    HADC_ACCUMULATIVE_AVERAGE_OUTPUT,
    HADC_COMPARATOR_OS_CORRECTION_VALUE,
    HADC_DCOC_CORRECTION_VALUE,
    HADC_SPD_CNT_VALUE,
    MAINTENANCE_CNT
} adc_v152_diag_node_t;

typedef enum adc_v152_compensation_mode {
    DIRECT_COMPENSATION,
    LUT_COMPENSATION,
    STATIC_CONFIGURATION
} adc_v152_compensation_mode_t;

/**
 * @brief  This union represents the bit fields in the AFE_DIG_PWR_EN. \n
 *         Read the register into the <i>d16</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union afe_dig_pwr_data {
    uint32_t d16;                               /*!< Raw register data. */
    struct {
        uint32_t afe_pwr_en            :   1;
        uint32_t afe_iso_en            :   1;
        uint32_t afe_pwr_ack           :   1;
    } b;                                        /*!< Register bits. */
} afe_dig_pwr_data_t;

/**
 * @brief  This union represents the bit fields in the ADLDO1_CFG/ADLDO2_CFG. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union afe_adlao_cfg_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t adldo_vset            :   4;
        uint32_t adldo_ictr            :   3;
        uint32_t reserved              :   1;
        uint32_t adldo_sel             :   2;
    } b;                                        /*!< Register bits. */
} afe_adlao_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the 0x000. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_rstn_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_rstn_tst             :   1;
        uint32_t reserved1_3              :   3;
        uint32_t cfg_hadc_rstn_bc         :   1;
        uint32_t reserved5_7              :   3;
        uint32_t cfg_hadc_rstn_fc         :   1;
        uint32_t reserved9_11             :   3;
        uint32_t cfg_gadc_rstn_fc         :   1;
        uint32_t reserved13_15            :   3;
        uint32_t cfg_hadc_rstn_data       :   1;
        uint32_t reserved17_19            :   3;
        uint32_t cfg_gadc_rstn_data       :   1;
        uint32_t reserved21_23            :   3;
        uint32_t cfg_hadc_rstn_ana        :   1;
        uint32_t reserved25_27            :   3;
        uint32_t cfg_gadc_rstn_ana        :   1;
        uint32_t reserved29_31            :   3;
    } b;                                        /*!< Register bits. */
} cfg_rstn_data_t;

/**
 * @brief  This union represents the bit fields in the 0x004. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_clken_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_clken_tst            :   1;
        uint32_t reserved1_3              :   3;
        uint32_t cfg_hadc_clken_bc        :   1;
        uint32_t reserved5_7              :   3;
        uint32_t cfg_hadc_clken_fc        :   1;
        uint32_t reserved9_11             :   3;
        uint32_t cfg_gadc_clken_fc        :   1;
        uint32_t reserved13_15            :   3;
        uint32_t cfg_hadc_clken_byp       :   1;
        uint32_t reserved17_19            :   3;
        uint32_t cfg_gadc_clken_byp       :   1;
        uint32_t reserved21_23            :   3;
        uint32_t cfg_hadc_clken_ctrl      :   1;
        uint32_t reserved25_27            :   3;
        uint32_t cfg_gadc_clken_ctrl      :   1;
        uint32_t reserved29_31            :   3;
    } b;                                        /*!< Register bits. */
} cfg_clken_data_t;

/**
 * @brief  This union represents the bit fields in the 0x018. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_iso_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t hadc_iso_en            :   1;
        uint32_t reserved1_7            :   7;
        uint32_t gadc_iso_en            :   1;
    } b;                                        /*!< Register bits. */
} cfg_iso_data_t;

/**
 * @brief  This union represents the bit fields in the 0x008/0x00C. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_clk_div_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_adc_ana_div_th        :   8;
        uint32_t reserved8_15              :   8;
        uint32_t cfg_adc_ana_div_cyc       :   8;
    } b;                                        /*!< Register bits. */
} cfg_clk_div_data_t;

/**
 * @brief  This union represents the bit fields in the 0x050. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_adc_data0 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data_num                 :   2;
        uint32_t reserved2_3              :   2;
        uint32_t wait_num                 :   3;
        uint32_t reserved7                :   1;
        adc_v152_data_mode_t cont_mode    :   1;
    } b;                                        /*!< Register bits. */
} cfg_gadc_data0_t;

/**
 * @brief  This union represents the bit fields in the 0x054. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_gadc_data1 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t osr_len                  :   2;
        uint32_t reserved2_3              :   2;
        uint32_t osr_sel                  :   2;
    } b;                                        /*!< Register bits. */
} cfg_gadc_data1_t;

/**
 * @brief  This union represents the bit fields in the 0x120. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_hadc_data0 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t data_num                 :   3;
        uint32_t reserved3                :   1;
        uint32_t wait_num                 :   3;
        uint32_t reserved7                :   1;
        adc_v152_data_mode_t cont_mode    :   1;
    } b;                                        /*!< Register bits. */
} cfg_hadc_data0_t;

/**
 * @brief  This union represents the bit fields in the 0x124. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_hadc_data1 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t osr_len                  :   3;
        uint32_t reserved3                :   1;
        uint32_t osr_sel                  :   3;
    } b;                                        /*!< Register bits. */
} cfg_hadc_data1_t;

/**
 * @brief  This union represents the bit fields in the 0x06C/0x140. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_adc_data4 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t adc_cdac_fc_scale_div    :   9;
        uint32_t reserved9_15             :   7;
        uint32_t adc_cdac_scale_div_sel   :   1;
    } b;                                        /*!< Register bits. */
} cfg_adc_data4_t;

/**
 * @brief  This union represents the bit fields in the 0x0F8. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_hadc_ctrl2 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        adc_v152_pga_gain_t cfg_hadc_pga_gain     :   3;
        uint32_t reserved3_7                      :   5;
        adc_v152_ia_gain_t cfg_hadc_ia_gain       :   3;
    } b;                                        /*!< Register bits. */
} cfg_hadc_ctrl2_t;

/**
 * @brief  This union represents the bit fields in the 0x238/0x23c. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cmp_os_code {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t adc_manual_cmp_os_code            :   7;
        uint32_t reserved7                         :   1;
        uint32_t adc_manual_cmp_os_update_en       :   1;
    } b;                                        /*!< Register bits. */
} cfg_cmp_os_code_t;

/**
 * @brief  This union represents the bit fields in the 0x254. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cdac_fc0_data1 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_hadc_cdac_fc_cap_start        :   5;
        uint32_t reserved5_7                       :   3;
        uint32_t cfg_gadc_cdac_fc_cap_start        :   4;
    } b;                                        /*!< Register bits. */
} cfg_cdac_fc0_data1_t;

/**
 * @brief  This union represents the bit fields in the 0x3D4. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_dcoc_cal_13_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t hadc_manual_dcoc_cal_code            :   10;
        uint32_t reserved10_15                        :   6;
        uint32_t hadc_manual_dcoc_cal_update_en       :   1;
    } b;                                        /*!< Register bits. */
} cfg_dcoc_cal_13_data_t;

/**
 * @brief  This union represents the bit fields in the 0x500. \n
 *         Read the register into the <i>d16</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_dreg_data0 {
    uint16_t d16;                               /*!< Raw register data. */
    struct {
        uint32_t channel_sel            :   3;
        uint32_t diff_mode              :   1;
        uint32_t sel_gnd                :   1;
        uint32_t hafe_tst_en            :   1;
        uint32_t local_mux_enb          :   1;
    } b;                                        /*!< Register bits. */
} cfg_dreg_data0_t;

/**
 * @brief  This union represents the bit fields in the 0x502. \n
 *         Read the register into the <i>d16</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_dreg_data1 {
    uint16_t d16;                               /*!< Raw register data. */
    struct {
        uint32_t nonovlp_dly            :   2;
        uint32_t chop_clk_dly           :   2;
        uint32_t cap_tuning             :   3;
        uint32_t res_tuning             :   3;
        uint32_t drv_sel                :   1;
        uint32_t reserved11_12          :   2;
        uint32_t en_chop                :   1;
        uint32_t en_buf_local           :   1;
    } b;                                        /*!< Register bits. */
} cfg_dreg_data1_t;

/**
 * @brief  This union represents the bit fields in the 0x504. \n
 *         Read the register into the <i>d16</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_dreg_data2 {
    uint16_t d16;                               /*!< Raw register data. */
    struct {
        uint32_t sr_width             :   2;
        uint32_t trk2conv             :   2;
        uint32_t done_dly             :   2;
        uint32_t cnv_rst_width        :   2;
        uint32_t sr2trk_dly           :   2;
        uint32_t st2trk_dly           :   2;
        uint32_t vcm_mode             :   1;
        uint32_t short_mode           :   1;
        uint32_t en_chop              :   1;
        uint32_t chop_freq            :   1;
    } b;                                        /*!< Register bits. */
} cfg_dreg_data2_t;

/**
 * @brief  This union represents the bit fields in the 0x506. \n
 *         Read the register into the <i>d16</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_dreg_data3 {
    uint16_t d16;                               /*!< Raw register data. */
    struct {
        uint32_t timer_disch_r        :   3;
        uint32_t timer_c_sel          :   2;
        uint32_t timer_ch_r           :   2;
        uint32_t cmp_dly              :   2;
        uint32_t os_cdac_sel          :   1;
        uint32_t meta_dect_en         :   1;
        uint32_t meta_dly_tune        :   2;
        uint32_t en_os_range          :   1;
        uint32_t en_vcm_sink          :   1;
        uint32_t enb_sw               :   1;
    } b;                                        /*!< Register bits. */
} cfg_dreg_data3_t;

/**
 * @brief  This union represents the bit fields in the ADLDO1_EN/ADLDO2_EN. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adldo_en_cfg_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t en_adldo                  :   1;
        uint32_t en_adldo_delay            :   1;
        uint32_t adldo_ocp_set             :   1;
        uint32_t adldo_ocp_bypass          :   1;
        uint32_t en_adldo_hiz              :   1;
        uint32_t en_adldo_sw               :   1;
    } b;                                        /*!< Register bits. */
} adldo_en_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the AFE_ADC_LDO_CFG. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union afe_ldo_cfg_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t s2d_afeldo3_en_dly            :   1;
        uint32_t s2d_afeldo3_en                :   1;
        uint32_t s2d_afeldo2_en_dly            :   1;
        uint32_t s2d_afeldo2_en                :   1;
        uint32_t s2d_afeldo1_en_dly            :   1;
        uint32_t s2d_afeldo1_en                :   1;
        uint32_t s2d_afeldo_iso_en_n           :   1;
    } b;                                        /*!< Register bits. */
} afe_ldo_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the AFE_GADC_CFG. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union afe_gadc_cfg_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t s2d_gadc_en                :   1;
        uint32_t s2d_gadc_mux_en            :   1;
        uint32_t s2d_gadc_iso_en            :   1;
        uint32_t d2s_gadc_alarm             :   1;
        uint32_t d2s_gadc_done              :   1;
    } b;                                        /*!< Register bits. */
} afe_gadc_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the AFE_HADC_CFG. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union afe_hadc_cfg_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t s2d_hadc_en                :   1;
        uint32_t s2d_hadc_pga_en            :   1;
        uint32_t s2d_hadc_dcoc_dac_en       :   1;
        uint32_t s2d_hadc_ia_en_dly         :   1;
        uint32_t s2d_hadc_ia_en             :   1;
        uint32_t s2d_hadc_sensor_en         :   1;
        uint32_t s2d_hadc_iso_en            :   1;
        uint32_t d2s_hadc_alarm             :   1;
        uint32_t d2s_hadc_done              :   1;
    } b;                                        /*!< Register bits. */
} afe_hadc_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the 0x2C/0x104. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_adc_cali_ctrl {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_adc_monitor_start     :   1;
        uint32_t reserved1_3               :   3;
        uint32_t cfg_adc_cal_en            :   1;
    } b;                                        /*!< Register bits. */
} cfg_adc_cali_ctrl_t;

/**
 * @brief  This union represents the bit fields in the 0x398. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_dcoc_cal_data2 {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_hadc_dcoc_cal_iter_cyc         :   11;
        uint32_t reserved11_15                      :   5;
        uint32_t cfg_hadc_dcoc_cal_iter_polar       :   1;
    } b;                                        /*!< Register bits. */
} cfg_dcoc_cal_data2_t;

/**
 * @brief  This union represents the bit fields in the 0x200. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cmp_os_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_hadc_cmp_os_start      :   1;
        uint32_t reserved1_3                :   3;
        uint32_t cfg_gadc_cmp_os_start      :   1;
    } b;                                        /*!< Register bits. */
} cfg_cmp_os_data_t;

/**
 * @brief  This union represents the bit fields in the 0x250. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cfg_cdac_fc0_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_hadc_cdac_fc_start      :   1;
        uint32_t reserved1_3                 :   3;
        uint32_t cfg_gadc_cdac_fc_start      :   1;
    } b;                                        /*!< Register bits. */
} cfg_cdac_fc0_data_t;

/**
 * @brief  This union represents the bit fields in the CFG_MCU_DIAG_SAMPLE_MODE. \n
 *         Read the register into the <i>d16</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union diag_sample_mode_data {
    uint32_t d16;                               /*!< Raw register data. */
    struct {
        uint32_t cfg_mcu_diag_sample_mode       :   2;
        uint32_t reserved2_7                    :   6;
        uint32_t cfg_mcu_diag_sample_en         :   1;
        uint32_t cfg_mcu_diag_sample_sync       :   1;
    } b;                                        /*!< Register bits. */
} diag_sample_mode_data_t;

/**
 * @brief  Registers associated with ADC.
 */

/**
 * @brief  AFE config registers.
 */
typedef struct adc_regs {
        volatile uint32_t cfg_rstn;               /*!< CFG_RSTN.         <i>Offset: 00h</i>. */
        volatile uint32_t cfg_clken;              /*!< CFG_CLKEN.        <i>Offset: 04h</i>. */
        volatile uint32_t cfg_clk_div_0;          /*!< CFG_CLK_DIV_0.    <i>Offset: 08h</i>. */
        volatile uint32_t cfg_clk_div_1;          /*!< CFG_CLK_DIV_1.    <i>Offset: 0ch</i>. */
        volatile uint32_t cfg_manual_clk_0;       /*!< CFG_MANUAL_CLK_0. <i>Offset: 10h</i>. */
        volatile uint32_t cfg_manual_clk_1;       /*!< CFG_MANUAL_CLK_1. <i>Offset: 14h</i>. */
        volatile uint32_t cfg_iso;                /*!< CFG_ISO.          <i>Offset: 18h</i>. */
        volatile uint32_t reserved0;              /*!< Reserved.         <i>Offset: 1ch</i>. */
        volatile uint32_t cfg_gadc_ctrl_0;        /*!< CFG_GADC_CTRL_0.  <i>Offset: 20h</i>. */
        volatile uint32_t cfg_gadc_ctrl_1;        /*!< CFG_GADC_CTRL_1.  <i>Offset: 24h</i>. */
        volatile uint32_t cfg_gadc_ctrl_2;        /*!< CFG_GADC_CTRL_2.  <i>Offset: 28h</i>. */
        volatile uint32_t cfg_gadc_ctrl_3;        /*!< CFG_GADC_CTRL_3.  <i>Offset: 2Ch</i>. */
        volatile uint32_t rpt_gadc_ctrl_0;        /*!< RPT_GADC_CTRL_0.  <i>Offset: 30h</i>. */
        volatile uint32_t cfg_gadc_ctrl_4;        /*!< CFG_GADC_CTRL_4.  <i>Offset: 34h</i>. */
        volatile uint32_t cfg_gadc_ctrl_5;        /*!< CFG_GADC_CTRL_5.  <i>Offset: 38h</i>. */
        volatile uint32_t rpt_gadc_ctrl_1;        /*!< RPT_GADC_CTRL_1.  <i>Offset: 3Ch</i>. */
        volatile uint32_t cfg_gadc_ctrl_6;        /*!< CFG_GADC_CTRL_6.  <i>Offset: 40h</i>. */
        volatile uint32_t rpt_gadc_ctrl_2;        /*!< RPT_GADC_CTRL_2.  <i>Offset: 44h</i>. */
        volatile uint32_t reserved1[2];           /*!< Reserved.         <i>Offset: 48h</i>. */
        volatile uint32_t cfg_gadc_data_0;        /*!< CFG_GADC_DATA_0.  <i>Offset: 50h</i>. */
        volatile uint32_t cfg_gadc_data_1;        /*!< CFG_GADC_DATA_1.  <i>Offset: 54h</i>. */
        volatile uint32_t rpt_gadc_data_0;        /*!< RPT_GADC_DATA_0.  <i>Offset: 58h</i>. */
        volatile uint32_t rpt_gadc_data_1;        /*!< RPT_GADC_DATA_1.  <i>Offset: 5Ch</i>. */
        volatile uint32_t rpt_gadc_data_2;        /*!< RPT_GADC_DATA_2.  <i>Offset: 60h</i>. */
        volatile uint32_t rpt_gadc_data_3;        /*!< RPT_GADC_DATA_3.  <i>Offset: 64h</i>. */
        volatile uint32_t cfg_gadc_data_3;        /*!< CFG_GADC_DATA_3.  <i>Offset: 68h</i>. */
        volatile uint32_t cfg_gadc_data_4;        /*!< CFG_GADC_DATA_4.  <i>Offset: 6Ch</i>. */
        volatile uint32_t rpt_gadc_data_4;        /*!< RPT_GADC_DATA_4.  <i>Offset: 70h</i>. */
        volatile uint32_t cfg_gadc_data_5;        /*!< CFG_GADC_DATA_5.  <i>Offset: 74h</i>. */
        volatile uint32_t reserved2[2];           /*!< Reserved.         <i>Offset: 78h</i>. */
        volatile uint32_t cfg_gadc_data_7;        /*!< CFG_GADC_DATA_7.  <i>Offset: 80h</i>. */
        volatile uint32_t cfg_gadc_data_8;        /*!< CFG_GADC_DATA_8.  <i>Offset: 84h</i>. */
        volatile uint32_t cfg_gadc_data_9;        /*!< CFG_GADC_DATA_9.  <i>Offset: 88h</i>. */
        volatile uint32_t cfg_gadc_data_10;       /*!< CFG_GADC_DATA_10. <i>Offset: 8Ch</i>. */
        volatile uint32_t cfg_gadc_data_11;       /*!< CFG_GADC_DATA_11. <i>Offset: 90h</i>. */
        volatile uint32_t cfg_gadc_data_12;       /*!< CFG_GADC_DATA_12. <i>Offset: 94h</i>. */
        volatile uint32_t cfg_gadc_data_13;       /*!< CFG_GADC_DATA_13. <i>Offset: 98h</i>. */
        volatile uint32_t cfg_gadc_data_14;       /*!< CFG_GADC_DATA_14. <i>Offset: 9Ch</i>. */
        volatile uint32_t cfg_gadc_data_15;       /*!< CFG_GADC_DATA_15. <i>Offset: A0h</i>. */
        volatile uint32_t cfg_gadc_data_16;       /*!< CFG_GADC_DATA_16. <i>Offset: A4h</i>. */
        volatile uint32_t cfg_gadc_data_17;       /*!< CFG_GADC_DATA_17. <i>Offset: A8h</i>. */
        volatile uint32_t cfg_gadc_data_18;       /*!< CFG_GADC_DATA_18. <i>Offset: ACh</i>. */
        volatile uint32_t cfg_gadc_data_19;       /*!< CFG_GADC_DATA_19. <i>Offset: B0h</i>. */
        volatile uint32_t cfg_gadc_data_20;       /*!< CFG_GADC_DATA_20. <i>Offset: B4h</i>. */
        volatile uint32_t cfg_gadc_data_21;       /*!< CFG_GADC_DATA_21. <i>Offset: B8h</i>. */
        volatile uint32_t cfg_gadc_data_22;       /*!< CFG_GADC_DATA_22. <i>Offset: BCh</i>. */
        volatile uint32_t rpt_gadc_data_5;        /*!< RPT_GADC_DATA_5.  <i>Offset: C0h</i>. */
        volatile uint32_t rpt_gadc_data_6;        /*!< RPT_GADC_DATA_6.  <i>Offset: C4h</i>. */
        volatile uint32_t rpt_gadc_data_7;        /*!< RPT_GADC_DATA_7.  <i>Offset: C8h</i>. */
        volatile uint32_t rpt_gadc_data_8;        /*!< RPT_GADC_DATA_8.  <i>Offset: CCh</i>. */
        volatile uint32_t rpt_gadc_data_9;        /*!< RPT_GADC_DATA_9.  <i>Offset: D0h</i>. */
        volatile uint32_t rpt_gadc_data_10;       /*!< RPT_GADC_DATA_10. <i>Offset: D4h</i>. */
        volatile uint32_t rpt_gadc_data_11;       /*!< RPT_GADC_DATA_11. <i>Offset: D8h</i>. */
        volatile uint32_t rpt_gadc_data_12;       /*!< RPT_GADC_DATA_12. <i>Offset: DCh</i>. */
        volatile uint32_t rpt_gadc_data_13;       /*!< RPT_GADC_DATA_13. <i>Offset: E0h</i>. */
        volatile uint32_t reserved3[3];           /*!< Reserved.         <i>Offset: E4h</i>. */
        volatile uint32_t cfg_hadc_ctrl_0;        /*!< CFG_HADC_CTRL_0.  <i>Offset: F0h</i>. */
        volatile uint32_t cfg_hadc_ctrl_1;        /*!< CFG_HADC_CTRL_1.  <i>Offset: F4h</i>. */
        volatile uint32_t cfg_hadc_ctrl_2;        /*!< CFG_HADC_CTRL_2.  <i>Offset: F8h</i>. */
        volatile uint32_t cfg_hadc_ctrl_3;        /*!< CFG_HADC_CTRL_3.  <i>Offset: FCh</i>. */
        volatile uint32_t cfg_hadc_ctrl_4;        /*!< CFG_HADC_CTRL_4.  <i>Offset: 100h</i>. */
        volatile uint32_t cfg_hadc_ctrl_5;        /*!< CFG_HADC_CTRL_5.  <i>Offset: 104h</i>. */
        volatile uint32_t rpt_hadc_ctrl_0;        /*!< RPT_HADC_CTRL_0.  <i>Offset: 108h</i>. */
        volatile uint32_t cfg_hadc_ctrl_6;        /*!< CFG_HADC_CTRL_6.  <i>Offset: 10Ch</i>. */
        volatile uint32_t cfg_hadc_ctrl_7;        /*!< CFG_HADC_CTRL_7.  <i>Offset: 110h</i>. */
        volatile uint32_t rpt_hadc_ctrl_1;        /*!< RPT_HADC_CTRL_1.  <i>Offset: 114h</i>. */
        volatile uint32_t cfg_hadc_ctrl_8;        /*!< CFG_HADC_CTRL_8.  <i>Offset: 118h</i>. */
        volatile uint32_t rpt_hadc_ctrl_2;        /*!< RPT_HADC_CTRL_2.  <i>Offset: 11Ch</i>. */
        volatile uint32_t cfg_hadc_data_0;        /*!< CFG_HADC_DATA_0.  <i>Offset: 120h</i>. */
        volatile uint32_t cfg_hadc_data_1;        /*!< CFG_HADC_DATA_1.  <i>Offset: 124h</i>. */
        volatile uint32_t cfg_hadc_data_2;        /*!< CFG_HADC_DATA_2.  <i>Offset: 128h</i>. */
        volatile uint32_t rpt_hadc_data_0;        /*!< RPT_HADC_DATA_0.  <i>Offset: 12Ch</i>. */
        volatile uint32_t rpt_hadc_data_1;        /*!< RPT_HADC_DATA_1.  <i>Offset: 130h</i>. */
        volatile uint32_t rpt_hadc_data_2;        /*!< RPT_HADC_DATA_2.  <i>Offset: 134h</i>. */
        volatile uint32_t rpt_hadc_data_3;        /*!< RPT_HADC_DATA_3.  <i>Offset: 138h</i>. */
        volatile uint32_t cfg_hadc_data_3;        /*!< CFG_HADC_DATA_3.  <i>Offset: 13Ch</i>. */
        volatile uint32_t cfg_hadc_data_4;        /*!< CFG_HADC_DATA_4.  <i>Offset: 140h</i>. */
        volatile uint32_t rpt_hadc_data_4;        /*!< RPT_HADC_DATA_4.  <i>Offset: 144h</i>. */
        volatile uint32_t cfg_hadc_data_5;        /*!< CFG_HADC_DATA_5.  <i>Offset: 148h</i>. */
        volatile uint32_t reserved4;              /*!< Reserved.         <i>Offset: 14ch</i>. */
        volatile uint32_t cfg_hadc_data_7;        /*!< CFG_HADC_DATA_7.  <i>Offset: 150h</i>. */
        volatile uint32_t cfg_hadc_data_8;        /*!< CFG_HADC_DATA_8.  <i>Offset: 154h</i>. */
        volatile uint32_t cfg_hadc_data_9;        /*!< CFG_HADC_DATA_9.  <i>Offset: 158h</i>. */
        volatile uint32_t cfg_hadc_data_10;       /*!< CFG_HADC_DATA_10. <i>Offset: 15Ch</i>. */
        volatile uint32_t cfg_hadc_data_11;       /*!< CFG_HADC_DATA_11. <i>Offset: 160h</i>. */
        volatile uint32_t cfg_hadc_data_12;       /*!< CFG_HADC_DATA_12. <i>Offset: 164h</i>. */
        volatile uint32_t cfg_hadc_data_13;       /*!< CFG_HADC_DATA_13. <i>Offset: 168h</i>. */
        volatile uint32_t cfg_hadc_data_14;       /*!< CFG_HADC_DATA_14. <i>Offset: 16Ch</i>. */
        volatile uint32_t cfg_hadc_data_15;       /*!< CFG_HADC_DATA_15. <i>Offset: 170h</i>. */
        volatile uint32_t cfg_hadc_data_16;       /*!< CFG_HADC_DATA_16. <i>Offset: 174h</i>. */
        volatile uint32_t cfg_hadc_data_17;       /*!< CFG_HADC_DATA_17. <i>Offset: 178h</i>. */
        volatile uint32_t cfg_hadc_data_18;       /*!< CFG_HADC_DATA_18. <i>Offset: 17Ch</i>. */
        volatile uint32_t cfg_hadc_data_19;       /*!< CFG_HADC_DATA_19. <i>Offset: 180h</i>. */
        volatile uint32_t cfg_hadc_data_20;       /*!< CFG_HADC_DATA_20. <i>Offset: 184h</i>. */
        volatile uint32_t cfg_hadc_data_21;       /*!< CFG_HADC_DATA_21. <i>Offset: 188h</i>. */
        volatile uint32_t cfg_hadc_data_22;       /*!< CFG_HADC_DATA_22. <i>Offset: 18Ch</i>. */
        volatile uint32_t cfg_hadc_data_23;       /*!< CFG_HADC_DATA_23. <i>Offset: 190h</i>. */
        volatile uint32_t cfg_hadc_data_24;       /*!< CFG_HADC_DATA_24. <i>Offset: 194h</i>. */
        volatile uint32_t cfg_hadc_data_25;       /*!< CFG_HADC_DATA_25. <i>Offset: 198h</i>. */
        volatile uint32_t cfg_hadc_data_26;       /*!< CFG_HADC_DATA_26. <i>Offset: 19Ch</i>. */
        volatile uint32_t cfg_hadc_data_27;       /*!< CFG_HADC_DATA_27. <i>Offset: 1A0h</i>. */
        volatile uint32_t cfg_hadc_data_28;       /*!< CFG_HADC_DATA_28. <i>Offset: 1A4h</i>. */
        volatile uint32_t cfg_hadc_data_29;       /*!< CFG_HADC_DATA_29. <i>Offset: 1A8h</i>. */
        volatile uint32_t cfg_hadc_data_30;       /*!< CFG_HADC_DATA_30. <i>Offset: 1ACh</i>. */
        volatile uint32_t rpt_hadc_data_5;        /*!< RPT_HADC_DATA_5.  <i>Offset: 1B0h</i>. */
        volatile uint32_t rpt_hadc_data_6;        /*!< RPT_HADC_DATA_6.  <i>Offset: 1B4h</i>. */
        volatile uint32_t rpt_hadc_data_7;        /*!< RPT_HADC_DATA_7.  <i>Offset: 1B8h</i>. */
        volatile uint32_t rpt_hadc_data_8;        /*!< RPT_HADC_DATA_8.  <i>Offset: 1BCh</i>. */
        volatile uint32_t rpt_hadc_data_9;        /*!< RPT_HADC_DATA_9.  <i>Offset: 1C0h</i>. */
        volatile uint32_t rpt_hadc_data_10;       /*!< RPT_HADC_DATA_10. <i>Offset: 1C4h</i>. */
        volatile uint32_t rpt_hadc_data_11;       /*!< RPT_HADC_DATA_11. <i>Offset: 1C8h</i>. */
        volatile uint32_t rpt_hadc_data_12;       /*!< RPT_HADC_DATA_12. <i>Offset: 1CCh</i>. */
        volatile uint32_t rpt_hadc_data_13;       /*!< RPT_HADC_DATA_13. <i>Offset: 1D0h</i>. */
        volatile uint32_t rpt_hadc_data_14;       /*!< RPT_HADC_DATA_14. <i>Offset: 1D4h</i>. */
        volatile uint32_t rpt_hadc_data_15;       /*!< RPT_HADC_DATA_15. <i>Offset: 1D8h</i>. */
        volatile uint32_t rpt_hadc_data_16;       /*!< RPT_HADC_DATA_16. <i>Offset: 1DCh</i>. */
        volatile uint32_t rpt_hadc_data_17;       /*!< RPT_HADC_DATA_17. <i>Offset: 1E0h</i>. */
        volatile uint32_t rpt_hadc_data_18;       /*!< RPT_HADC_DATA_18. <i>Offset: 1E4h</i>. */
        volatile uint32_t reserved5[6];           /*!< Reserved.         <i>Offset: 1E8h</i>. */
        volatile uint32_t cfg_cmp_os_0;           /*!< CFG_CMP_OS_0.     <i>Offset: 200h</i>. */
        volatile uint32_t cfg_cmp_os_1;           /*!< CFG_CMP_OS_1.     <i>Offset: 204h</i>. */
        volatile uint32_t cfg_cmp_os_2;           /*!< CFG_CMP_OS_2.     <i>Offset: 208h</i>. */
        volatile uint32_t cfg_cmp_os_3;           /*!< CFG_CMP_OS_3.     <i>Offset: 20Ch</i>. */
        volatile uint32_t cfg_cmp_os_4;           /*!< CFG_CMP_OS_4.     <i>Offset: 210h</i>. */
        volatile uint32_t cfg_cmp_os_5;           /*!< CFG_CMP_OS_5.     <i>Offset: 214h</i>. */
        volatile uint32_t cfg_cmp_os_6;           /*!< CFG_CMP_OS_6.     <i>Offset: 218h</i>. */
        volatile uint32_t cfg_cmp_os_7;           /*!< CFG_CMP_OS_7.     <i>Offset: 21Ch</i>. */
        volatile uint32_t cfg_cmp_os_8;           /*!< CFG_CMP_OS_8.     <i>Offset: 220h</i>. */
        volatile uint32_t cfg_cmp_os_9;           /*!< CFG_CMP_OS_9.     <i>Offset: 224h</i>. */
        volatile uint32_t cfg_cmp_os_10;          /*!< CFG_CMP_OS_10.    <i>Offset: 228h</i>. */
        volatile uint32_t rpt_cmp_os_0;           /*!< RPT_CMP_OS_0.     <i>Offset: 22Ch</i>. */
        volatile uint32_t rpt_cmp_os_1;           /*!< RPT_CMP_OS_1.     <i>Offset: 230h</i>. */
        volatile uint32_t cfg_cmp_os_11;          /*!< CFG_CMP_OS_11.    <i>Offset: 234h</i>. */
        volatile uint32_t cfg_cmp_os_12;          /*!< CFG_CMP_OS_12.    <i>Offset: 238h</i>. */
        volatile uint32_t cfg_cmp_os_13;          /*!< CFG_CMP_OS_13.    <i>Offset: 23Ch</i>. */
        volatile uint32_t rpt_cmp_os_2;           /*!< RPT_CMP_OS_2.     <i>Offset: 240h</i>. */
        volatile uint32_t rpt_cmp_os_3;           /*!< RPT_CMP_OS_3.     <i>Offset: 244h</i>. */
        volatile uint32_t reserved6[2];           /*!< Reserved.         <i>Offset: 248h</i>. */
        volatile uint32_t cfg_cdac_fc0_0;         /*!< CFG_CDAC_FC0_0.   <i>Offset: 250h</i>. */
        volatile uint32_t cfg_cdac_fc0_1;         /*!< CFG_CDAC_FC0_1.   <i>Offset: 254h</i>. */
        volatile uint32_t cfg_cdac_fc0_2;         /*!< CFG_CDAC_FC0_2.   <i>Offset: 258h</i>. */
        volatile uint32_t cfg_cdac_fc0_3;         /*!< CFG_CDAC_FC0_3.   <i>Offset: 25Ch</i>. */
        volatile uint32_t cfg_cdac_fc0_4;         /*!< CFG_CDAC_FC0_4.   <i>Offset: 260h</i>. */
        volatile uint32_t cfg_cdac_fc0_5;         /*!< CFG_CDAC_FC0_5.   <i>Offset: 264h</i>. */
        volatile uint32_t cfg_cdac_fc0_6;         /*!< CFG_CDAC_FC0_6.   <i>Offset: 268h</i>. */
        volatile uint32_t cfg_cdac_fc0_7;         /*!< CFG_CDAC_FC0_7.   <i>Offset: 26Ch</i>. */
        volatile uint32_t rpt_cdac_fc0_0;         /*!< RPT_CDAC_FC0_0.   <i>Offset: 270h</i>. */
        volatile uint32_t cfg_cdac_fc0_8;         /*!< CFG_CDAC_FC0_8.   <i>Offset: 274h</i>. */
        volatile uint32_t cfg_cdac_fc0_9;         /*!< CFG_CDAC_FC0_9.   <i>Offset: 278h</i>. */
        volatile uint32_t cfg_cdac_fc0_10;        /*!< CFG_CDAC_FC0_10.  <i>Offset: 27Ch</i>. */
        volatile uint32_t cfg_cdac_fc0_11;        /*!< CFG_CDAC_FC0_11.  <i>Offset: 280h</i>. */
        volatile uint32_t rpt_cdac_fc0_1;         /*!< RPT_CDAC_FC0_1.   <i>Offset: 284h</i>. */
        volatile uint32_t reserved7[2];           /*!< Reserved.         <i>Offset: 288h</i>. */
        volatile uint32_t cfg_cdac_fc1_0;         /*!< CFG_CDAC_FC1_0.   <i>Offset: 290h</i>. */
        volatile uint32_t cfg_cdac_fc1_1;         /*!< CFG_CDAC_FC1_1.   <i>Offset: 294h</i>. */
        volatile uint32_t cfg_cdac_fc1_2;         /*!< CFG_CDAC_FC1_2.   <i>Offset: 298h</i>. */
        volatile uint32_t cfg_cdac_fc1_3;         /*!< CFG_CDAC_FC1_3.   <i>Offset: 29Ch</i>. */
        volatile uint32_t cfg_cdac_fc1_4;         /*!< CFG_CDAC_FC1_4.   <i>Offset: 2A0h</i>. */
        volatile uint32_t rpt_cdac_fc1_0;         /*!< RPT_CDAC_FC1_0.   <i>Offset: 2A4h</i>. */
        volatile uint32_t rpt_cdac_fc1_1;         /*!< RPT_CDAC_FC1_1.   <i>Offset: 2A8h</i>. */
        volatile uint32_t rpt_cdac_fc1_2;         /*!< RPT_CDAC_FC1_2.   <i>Offset: 2ACh</i>. */
        volatile uint32_t rpt_cdac_fc1_3;         /*!< RPT_CDAC_FC1_3.   <i>Offset: 2B0h</i>. */
        volatile uint32_t reserved8[3];           /*!< Reserved.         <i>Offset: 2B4h</i>. */
        volatile uint32_t rpt_cdac_fc3[18];       /*!< RPT_CDAC_FC3.     <i>Offset: 2C0h</i>. */
        volatile uint32_t reserved9[2];           /*!< Reserved.         <i>Offset: 308h</i>. */
        volatile uint32_t rpt_cdac_fc4[28];       /*!< RPT_CDAC_FC4.     <i>Offset: 310h</i>. */
        volatile uint32_t reserved10[4];          /*!< Reserved.         <i>Offset: 380h</i>. */
        volatile uint32_t cfg_dcoc_cal[12];       /*!< CFG_DCOC_CAL.     <i>Offset: 390h</i>. */
        volatile uint32_t rpt_dcoc_cal_0;         /*!< RPT_DCOC_CAL_0.   <i>Offset: 3C0h</i>. */
        volatile uint32_t reserved11[3];          /*!< Reserved.         <i>Offset: 3C4h</i>. */
        volatile uint32_t cfg_dcoc_cal_12;        /*!< CFG_DCOC_CAL_12.  <i>Offset: 3D0h</i>. */
        volatile uint32_t cfg_dcoc_cal_13;        /*!< CFG_DCOC_CAL_13.  <i>Offset: 3D4h</i>. */
        volatile uint32_t rpt_dcoc_cal_1;         /*!< RPT_DCOC_CAL_1.   <i>Offset: 3D8h</i>. */
        volatile uint32_t cfg_dcoc_cal_14;        /*!< CFG_DCOC_CAL_14.  <i>Offset: 3DCh</i>. */
        volatile uint32_t rpt_dcoc_cal_2;         /*!< RPT_DCOC_CAL_2.   <i>Offset: 3E0h</i>. */
        volatile uint32_t reserved12[3];          /*!< Reserved.         <i>Offset: 3E4h</i>. */
        volatile uint32_t cfg_sar_spd_0;          /*!< CFG_SAR_SPD_0.    <i>Offset: 3F0h</i>. */
        volatile uint32_t cfg_sar_spd_1;          /*!< CFG_SAR_SPD_1.    <i>Offset: 3F4h</i>. */
        volatile uint32_t cfg_sar_spd_2;          /*!< CFG_SAR_SPD_2.    <i>Offset: 3F8h</i>. */
        volatile uint32_t cfg_sar_spd_3;          /*!< CFG_SAR_SPD_3.    <i>Offset: 3FCh</i>. */
        volatile uint32_t cfg_sar_spd_4;          /*!< CFG_SAR_SPD_4.    <i>Offset: 400h</i>. */
        volatile uint32_t rpt_sar_spd_0;          /*!< RPT_SAR_SPD_0.    <i>Offset: 404h</i>. */
        volatile uint32_t rpt_sar_spd_1;          /*!< RPT_SAR_SPD_1.    <i>Offset: 408h</i>. */
        volatile uint32_t reserved13;             /*!< Reserved.         <i>Offset: 40Ch</i>. */
        volatile uint32_t cfg_sar_spd_6;          /*!< CFG_SAR_SPD_6.    <i>Offset: 410h</i>. */
        volatile uint32_t rpt_sar_spd_2;          /*!< RPT_SAR_SPD_2.    <i>Offset: 414h</i>. */
        volatile uint32_t reserved14[2];          /*!< Reserved.         <i>Offset: 418h</i>. */
        volatile uint32_t diag_cfg_tst[2];        /*!< DIAG cfg tst.     <i>Offset: 420h</i>. */
        volatile uint32_t reserved15[54];         /*!< Reserved.         <i>Offset: 42Ch</i>. */
        volatile uint16_t cfg_dreg[14];           /*!< CFG_DREG.         <i>Offset: 500h</i>. */
} adc_regs_t;

/**
 * @brief  PMU registers associated with AFE.
 */
typedef struct adc_pmu_regs {
        volatile uint32_t afe_adc_rst_n;          /*!< AFE_ADC_RST_N.    <i>Offset: 00h</i>. */
        volatile uint32_t afe_adc_ldo_cfg;        /*!< AFE_ADC_LDO_CFG.  <i>Offset: 04h</i>. */
        volatile uint32_t afe_gadc_cfg;           /*!< AFE_GADC_CFG.     <i>Offset: 08h</i>. */
        volatile uint32_t afe_gadc_data_l;        /*!< AFE_GADC_DATA_L.  <i>Offset: 0Ch</i>. */
        volatile uint32_t afe_gadc_data_h;        /*!< AFE_GADC_DATA_H.  <i>Offset: 10h</i>. */
        volatile uint32_t afe_hadc_cfg;           /*!< AFE_HADC_CFG.     <i>Offset: 14h</i>. */
        volatile uint32_t afe_hadc_data_l;        /*!< AFE_HADC_DATA_L.  <i>Offset: 18h</i>. */
        volatile uint32_t afe_hadc_data_h;        /*!< AFE_HADC_DATA_H.  <i>Offset: 1Ch</i>. */
        volatile uint32_t afe_dig_pwr_en;         /*!< AFE_DIG_PWR_EN.   <i>Offset: 20h</i>. */
        volatile uint32_t afe_soft_rst;           /*!< AFE_SOFT_RST.     <i>Offset: 24h</i>. */
        volatile uint32_t afe_clk_en;             /*!< AFE_CLK_EN.       <i>Offset: 28h</i>. */
} adc_pmu_regs_t;

/**
 * @brief  Always-On registers associated with AFE.
 */
typedef struct adc_aon_regs {
        volatile uint32_t reserved[2];            /*!< Reserved.         <i>Offset: 00h</i>. */
        volatile uint32_t adldo1_cfg;             /*!< ADLDO1_CFG.       <i>Offset: 08h</i>. */
        volatile uint32_t adldo1_en;              /*!< ADLDO1_EN.        <i>Offset: 0Ch</i>. */
        volatile uint32_t adldo2_cfg;             /*!< ADLDO2_CFG.       <i>Offset: 10h</i>. */
        volatile uint32_t adldo2_en;              /*!< ADLDO2_EN.        <i>Offset: 14h</i>. */
        volatile uint32_t auxldo1_trim;           /*!< AUXLDO1_TRIM.     <i>Offset: 18h</i>. */
        volatile uint32_t adldo1_trim;            /*!< ADLDO1_TRIM.      <i>Offset: 1Ch</i>. */
        volatile uint32_t adldo2_trim;            /*!< ADLDO2_TRIM.      <i>Offset: 20h</i>. */
        volatile uint32_t phyldo_trim;            /*!< PHYLDO_TRIM.      <i>Offset: 24h</i>. */
        volatile uint32_t simo_ref_trim;          /*!< SIMO_REF_TRIM.    <i>Offset: 28h</i>. */
} adc_aon_regs_t;

/**
 * @brief  DIAG registers associated with AFE.
 */
typedef struct adc_diag_regs {
        volatile uint32_t cfg_monitor_sel;                  /*!< CFG_MONITOR_SEL.                <i>Offset: 00h</i>. */
        volatile uint32_t cfg_mcu_diag_trace_save_sel;      /*!< CFG_MCU_DIAG_TRACE_SAVE_SEL.    <i>Offset: 04h</i>. */
        volatile uint32_t cfg_mcu_diag_cpu_trace;           /*!< CFG_MCU_DIAG_CPU_TRACE.         <i>Offset: 08h</i>. */
        volatile uint32_t cfg_mcu_diag_monitor_clock;       /*!< CFG_MCU_DIAG_MONITOR_CLOCK.     <i>Offset: 0ch</i>. */
        volatile uint32_t reserved[60];                     /*!< Reserved.                       <i>Offset: 10h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_sel;          /*!< MCU_DIAG_SAMPLE_SEL.            <i>Offset: 100h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_mode;         /*!< MCU_DIAG_SAMPLE_MODE.           <i>Offset: 104h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_length_l;     /*!< MCU_DIAG_SAMPLE_LENGTH_L.       <i>Offset: 108h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_length_h;     /*!< MCU_DIAG_SAMPLE_LENGTH_H.       <i>Offset: 10ch</i>. */
        volatile uint32_t cfg_mcu_diag_sample_start_addr_l; /*!< MCU_DIAG_SAMPLE_START_ADDR_L.   <i>Offset: 110h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_start_addr_h; /*!< MCU_DIAG_SAMPLE_START_ADDR_H.   <i>Offset: 114h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_end_addr_l;   /*!< MCU_DIAG_SAMPLE_END_ADDR_L.     <i>Offset: 118h</i>. */
        volatile uint32_t cfg_mcu_diag_sample_end_addr_h;   /*!< MCU_DIAG_SAMPLE_END_ADDR_H.     <i>Offset: 11ch</i>. */
        volatile uint32_t mcu_diag_sample_done_addr_l;      /*!< MCU_DIAG_SAMPLE_DONE_ADDR_L.    <i>Offset: 120h</i>. */
        volatile uint32_t mcu_diag_sample_done_addr_h;      /*!< MCU_DIAG_SAMPLE_DONE_ADDR_H.    <i>Offset: 124h</i>. */
        volatile uint32_t mcu_diag_sample_done;             /*!< MCU_DIAG_SAMPLE_DONE.           <i>Offset: 128h</i>. */
        volatile uint32_t cfg_aux_adc_sample_period;        /*!< CFG_AUX_ADC_SAMPLE_PERIOD.      <i>Offset: 12ch</i>. */
        volatile uint32_t cfg_adc_dig_pin;                  /*!< CFG_ADC_DIG_PIN.                <i>Offset: 130h</i>. */
} adc_diag_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif