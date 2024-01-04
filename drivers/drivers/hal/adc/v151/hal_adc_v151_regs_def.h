/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V151 adc register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef HAL_ADC_V151_REGS_DEF_H
#define HAL_ADC_V151_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define AUX_ADC_SCAN_TH_MAX_NUM 18

/**
 * @defgroup drivers_hal_adc_v151_regs_def ADC V151 Regs Definition
 * @ingroup  drivers_hal_adc
 * @{
 */

 /**
 * @brief  ADC scan mode.
 */
typedef enum {
    AUX_ADC_SCAN_MODE0 = 0,
    AUX_ADC_SCAN_MODE1,
    AUX_ADC_SCAN_MODE_MAX_NUM
} hal_adc_scan_mode_t;

 /**
 * @brief  ADC scan reg.
 */
typedef enum {
    AUX_ADC_SCAN_INT_EN = 0,
    AUX_ADC_SCAN_STAT,
    AUX_ADC_SCAN_INT_CLR,
    AUX_ADC_SCAN_EN_STAT,
    AUX_ADC_SCAN_REG_MAX_NUM
} hal_adc_scan_en_start_t;

 /**
 * @brief  ADC scan fifo ptr.
 */
typedef enum {
    AUX_ADC_SCAN_PTR0 = 0,
    AUX_ADC_SCAN_PTR1,
    AUX_ADC_SCAN_PTR2,
    AUX_ADC_SCAN_PTR3,
    AUX_ADC_SCAN_PTR4,
    AUX_ADC_SCAN_PTR_MAX_NUM
} hal_adc_scan_ptr_t;

 /**
 * @brief  ADC calibration reg.
 */
typedef enum {
    AUX_ADC_CALI_COEF0 = 0,
    AUX_ADC_CALI_COEF1,
    AUX_ADC_CALI_COEF2,
    AUX_ADC_CALI_COEF_MAX_NUM
} hal_adc_cali_t;

/**
 * @brief  This union represents the bit fields in the power on/off.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_power_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t en                         : 2;        /*!< input buffer enable. */
        uint32_t inbuf_vref                 : 2;        /*!< input buffer vref trim. */
        uint32_t inbuf_rt                   : 2;        /*!< input buffer rt trim. */
        uint32_t comp_vref                  : 2;        /*!< compare vref trim. */
    } b;                                                /*!< Register bits. */
} adc_power_data_t;

/**
 * @brief  This union represents the bit fields in the adc control.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_ctrl_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t iso18                     : 1;         /*!< If this bit is set to 1, set adc ISO No. */
        uint32_t clk_gt                    : 1;         /*!< If this bit is set to 1, set gatin in adc clock. */
        uint32_t ch_gt                     : 1;         /*!< If this bit is set to 1, set gatin in adc channel. */
        uint32_t pd                        : 1;         /*!< adc PD control. */
        uint32_t rstn                      : 1;         /*!< adc count value. */
    } b;                                                /*!< Register bits. */
} adc_ctrl_data_t;

/**
 * @brief  This union represents the bit fields in the adc stick.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_stick_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t stick                     : 12;        /*!< sample stick. */
        uint32_t reserved                  : 3;         /*!< reserved. */
        uint32_t flag                      : 1;         /*!< output Flag Signal. */
    } b;                                                /*!< Register bits. */
} adc_stick_data_t;

/**
 * @brief  This union represents the bit fields in the adc scan frequency.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_freq_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t freq                      : 3;         /*!< scan frequency. */
        uint32_t dis_sel0                  : 3;         /*!< discard num for 0_1/2_3. */
        uint32_t avg_sel0                  : 2;         /*!< average num for 0_1/2_3. */
        uint32_t dis_sel1                  : 3;         /*!< discard num for 0/2. */
        uint32_t avg_sel1                  : 2;         /*!< average num for 0/2. */
        uint32_t dis_sel2                  : 3;         /*!< discard num for 4/5/6/7/8. */
    } b;                                                /*!< Register bits. */
} adc_freq_data_t;

/**
 * @brief  This union represents the bit fields in the adc scan mode.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_trig_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t ch0_1                      : 1;        /*!< ch0_1 scan mode. */
        uint32_t ch2_3                      : 1;        /*!< ch2_3 scan mode. */
        uint32_t ch0                        : 1;        /*!< ch0 scan mode. */
        uint32_t ch2                        : 1;        /*!< ch2 scan mode. */
        uint32_t ch4                        : 1;        /*!< ch4 scan mode. */
        uint32_t ch5                        : 1;        /*!< ch5 scan mode. */
        uint32_t ch6                        : 1;        /*!< ch6 scan mode. */
        uint32_t ch7                        : 1;        /*!< ch7 scan mode. */
        uint32_t ch8                        : 1;        /*!< ch8 scan mode. */
        uint32_t reserved                   : 5;        /*!< reserved. */
        uint32_t avg_sel2                   : 2;        /*!< scan_en. */
    } b;                                                /*!< Register bits. */
} adc_trig_data_t;

/**
 * @brief  This union represents the bit fields in the adc scan enable.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_scan_en_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t ch0_1                      : 1;        /*!< ch0_1 scan mode. */
        uint32_t ch2_3                      : 1;        /*!< ch2_3 scan mode. */
        uint32_t ch0                        : 1;        /*!< ch0 scan mode. */
        uint32_t ch2                        : 1;        /*!< ch2 scan mode. */
        uint32_t ch4                        : 1;        /*!< ch4 scan mode. */
        uint32_t ch5                        : 1;        /*!< ch5 scan mode. */
        uint32_t ch6                        : 1;        /*!< ch6 scan mode. */
        uint32_t ch7                        : 1;        /*!< ch7 scan mode. */
        uint32_t ch8                        : 1;        /*!< ch8 scan mode. */
        uint32_t reserved                   : 3;        /*!< reserved. */
        uint32_t scan_en                    : 1;        /*!< scan_en. */
    } b;                                                /*!< Register bits. */
} adc_scan_en_data_t;

/**
 * @brief  This union represents the bit fields in the adc fifo ptr if threshold trig.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_ptr_data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t th01                       : 7;        /*!< channel 0_1/0/4/6/8. */
        uint32_t reserved                   : 1;        /*!< reserved. */
        uint32_t th23                       : 7;        /*!< channel 2_3/2/5/7. */
    } b;                                                /*!< Register bits. */
} adc_ptr_data_t;

/**
 * @brief  This union represents the bit fields in the adc calibration enable.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_cail_en_data {
    uint32_t d32;                                        /*!< Raw register data. */
    struct {
        uint32_t cali_en                    : 1;         /*!< ADC calibration enable. */
    } b;                                                 /*!< Register bits. */
} adc_cail_en_data_t;

/**
 * @brief  This union represents the bit fields in the ADC auto config.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union adc_auto_cfg__data {
    uint32_t d32;                                       /*!< Raw register data. */
    struct {
        uint32_t rb                         : 1;        /*!< Conversion signal. */
        uint32_t sel                        : 1;        /*!< Select conversion signal. */
        uint32_t man                        : 1;        /*!< Manual control signal. */
        uint32_t man_sel                    : 1;        /*!< Select manual control signal. */
        uint32_t cs                         : 1;        /*!< Conversion signal status. */
        uint32_t ch_sel                     : 4;        /*!< Channel select. */
    } b;                                                /*!< Register bits. */
} adc_auto_cfg_data_t;

/**
 * @brief  Registers associated with ADC.
 */

typedef struct cali_coef {
    volatile uint32_t gain;                              /*!< gain. */
    volatile uint32_t ofst;                              /*!< off set. */
} cali_coef_t;

/**
 * @brief  Registers associated with ADC.
 */

typedef struct adc_regs {
    volatile uint32_t aux_adc_cfg;                                       /*!< config.        <i>Offset: 00h</i>. */
    volatile uint32_t aux_adc_sel;                                       /*!< channel sel.   <i>Offset: 04h</i>. */
    volatile uint32_t aux_adc_eoc_flag;                                  /*!< eoc flag.      <i>Offset: 08h</i>. */
    volatile uint32_t aux_adc_dout;                                      /*!< adc dout.      <i>Offset: 0Ch</i>. */
    volatile uint32_t aux_adc_reg;                                       /*!< static reg.    <i>Offset: 10h</i>. */
    volatile uint32_t aux_adc_auto_cfg;                                  /*!< auto config.   <i>Offset: 14h</i>. */
    volatile uint32_t aux_adc_stick;                                     /*!< adc stick.     <i>Offset: 18h</i>. */
    volatile uint32_t aux_adc_stick_clr;                                 /*!< stick clear.   <i>Offset: 1Ch</i>. */
    volatile uint32_t aux_adc_cali_en;                                   /*!< cail enable.   <i>Offset: 20h</i>. */
    volatile uint32_t aux_adc_cali_cfg;                                  /*!< cail config.   <i>Offset: 24h</i>. */
    volatile uint32_t aux_adc_cali_period;                               /*!< cail period.   <i>Offset: 28h</i>. */
    volatile uint32_t reserved1;                                         /*!< reserved.      <i>Offset: 2Ch</i>. */
    volatile uint32_t aux_adc_cali_cfg_sync;                             /*!< config sync.   <i>Offset: 30h</i>. */
    volatile uint32_t aux_adc_cali_status;                               /*!< cali status.   <i>Offset: 34h</i>. */
    volatile uint32_t aux_adc_cali_out;                                  /*!< cali out.      <i>Offset: 38h</i>. */
    volatile uint32_t reserved2;                                         /*!< reserved.      <i>Offset: 3Ch</i>. */
    volatile cali_coef_t aux_adc_cali_coef[AUX_ADC_CALI_COEF_MAX_NUM];   /*!< cali coef.     <i>Offset: 40h</i>. */
    volatile uint32_t reserved3;                                         /*!< reserved.      <i>Offset: 58h</i>. */
    volatile uint32_t aux_adc_cali_bypass;                               /*!< cali bypass.   <i>Offset: 5Ch</i>. */
    volatile uint32_t aux_adc_scan_en;                                   /*!< scan enable.   <i>Offset: 60h</i>. */
    volatile uint32_t aux_adc_scan_freq;                                 /*!< scan freq.     <i>Offset: 64h</i>. */
    volatile uint32_t aux_adc_scan_mode[AUX_ADC_SCAN_MODE_MAX_NUM];      /*!< scan mode.     <i>Offset: 68h</i>. */
    volatile uint32_t aux_adc_scan_th[AUX_ADC_SCAN_TH_MAX_NUM];          /*!< scan threshold.<i>Offset: 70h</i>. */
    volatile uint64_t reserved4;                                         /*!< reserved.      <i>Offset: B8h</i>. */
    volatile uint32_t aux_adc_scan_ptr[AUX_ADC_SCAN_PTR_MAX_NUM];        /*!< scan_ptr.      <i>Offset: C0h</i>. */
    volatile uint32_t aux_adc_scan_reg[AUX_ADC_SCAN_REG_MAX_NUM];        /*!< scan reg.      <i>Offset: D4h</i>. */
} adc_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif