/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides adc port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef ADC_PORTING_H
#define ADC_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "platform_core.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_adc ADC
 * @ingroup  drivers_port
 * @{
 */

/**
 * @brief  device_bs21_port_adc ADC
 */

#define AFE_ENABLE_INTERRUPT 0
#define AIN_MAX_NUM 10

#define COMMON_DEFAULT_CONFIG                           \
{                                                       \
    .ana_div_cyc = 0x27,                                \
    .cfg_clk_div1.b.ana_div_th = 0x4,                   \
    .cfg_clk_div1.b.prechg_div_th = 0x19,               \
    .cfg_gadc_data0.b.data_num = 0x4,                   \
    .cfg_gadc_data0.b.wait_num = 0x4,                   \
    .cfg_gadc_data0.b.cont_mode = CONTIUNE_MODE,        \
    .cfg_gadc_data1.b.osr_len= 5,                       \
    .cfg_gadc_data1.b.osr_sel= 5,                       \
    .cfg_gadc_data1.b.osr_wait_num= 0x12,               \
    .cfg_gadc_data4.b.adc_cdac_fc_scale_div= 0x80,      \
    .cfg_gadc_data4.b.adc_cdac_scale_div_sel= 0x1,      \
    .cfg_freg_10.b.cfg_gadc_short_mode = 1,             \
    .cfg_freg_10.b.cfg_gadc_dly2inject = 1,             \
    .cfg_freg_10.b.cfg_gadc_dly2convstart = 3,          \
    .cfg_freg_10.b.cfg_gadc_dly2srwidth = 2,            \
    .cfg_freg_10.b.cfg_gadc_lsbrpts_en = 1,             \
    .cfg_freg_11.b.cfg_gadc_r_sel_rise = 3,             \
    .cfg_freg_11.b.cfg_gadc_os_range_sel = 1,           \
    .cfg_freg_11.b.reserved7_14 = 3,                    \
    .cfg_freg_11.b.cfg_dreg_iso_enb = 1,                \
}

#define GADC_DEFAULT_CONFIG                             \
{                                                       \
    .diag_node = ADC_ACCUMULATED_AVERAGE_OUTPUT,        \
    .os_cali = true,                                    \
    .cdac_cali = false,                                 \
}

#define AMIC_DEFAULT_CONFIG                             \
{                                                       \
    .diag_node = ADC_ACCUMULATED_AVERAGE_OUTPUT,        \
    .pga_gain = 0x8,                                    \
    .os_cali = true,                                    \
    .cdac_cali = true,                                  \
    .dcoc_cali = true,                                  \
}

/**
 * @brief  ADC channels definition.
 */
typedef enum adc_channel {
    GADC_CHANNEL_0,
    GADC_CHANNEL_1,
    GADC_CHANNEL_2,
    GADC_CHANNEL_3,
    GADC_CHANNEL_4,
    GADC_CHANNEL_5,
    GADC_CHANNEL_6,
    GADC_CHANNEL_7,
    AMIC_CHANNEL_0 = GADC_CHANNEL_4,
    AMIC_CHANNEL_1 = GADC_CHANNEL_5,
    ADC_CHANNEL_MAX_NUM = 8,
    ADC_CHANNEL_NONE = ADC_CHANNEL_MAX_NUM
} adc_channel_t;

/**
 * @if Eng
 * @brief  ADC auto scan frequency which is public used for all channel.
 * @else
 * @brief  adc自动扫描频率，用于所有通道。
 * @endif
 */
typedef enum {
    HAL_ADC_SCAN_FREQ_2HZ,
    HAL_ADC_SCAN_FREQ_4HZ,
    HAL_ADC_SCAN_FREQ_8HZ,
    HAL_ADC_SCAN_FREQ_16HZ,
    HAL_ADC_SCAN_FREQ_32HZ,
    HAL_ADC_SCAN_FREQ_64HZ,
    HAL_ADC_SCAN_FREQ_128HZ,
    HAL_ADC_SCAN_FREQ_256HZ,
    HAL_ADC_SCAN_FREQ_MAX,
    HAL_ADC_SCAN_FREQ_NONE = HAL_ADC_SCAN_FREQ_MAX
} port_adc_scan_freq_t;

/**
 * @brief  AFE interrupt source.
 */
typedef struct adc_irq {
    uint32_t done_irqn;
    uint32_t alarm_irqn;
} adc_irq_t;

#define AFE_INFO_RESERVED 51
// AFE config total 128 bytes
typedef struct {
    uint32_t    magic;                              // 4B  : const value 0x8AA855DDUL
    uint32_t    crc32;                              // 4B  : start : end of crc32, end : structure end.
    uint8_t     product_name;                       // 1B  : afe product name
    uint8_t     hadc_ia_gain;                       // 1B  : adc ia gain: cal 0x0F8
    uint8_t     hadc_pga_gain;                      // 1B  : adc pga gain: cal == 2 0x0F8

    uint8_t     gadc_os_code;                       // 1B  : gadc os cal  0x240
    uint32_t    gadc_wt_b14;                        // 4B  : gadc cap cal 0x0C0
    uint16_t    gadc_wt_b13;                        // 2B  : gadc cap cal 0x0C4
    uint16_t    gadc_wt_b12;                        // 2B  : gadc cap cal 0x0C8
    uint16_t    gadc_wt_b11;                        // 2B  : gadc cap cal 0x0CC
    uint16_t    gadc_wt_b10;                        // 2B  : gadc cap cal 0x0D0
    uint16_t    gadc_wt_b9;                         // 2B  : gadc cap cal 0x0D4
    uint16_t    gadc_wt_b8;                         // 2B  : gadc cap cal 0x0D8
    uint16_t    gadc_wt_b7;                         // 2B  : gadc cap cal 0x0DC
    uint16_t    gadc_wt_b6;                         // 2B  : gadc cap cal 0x0E0
    uint16_t    gadc_gain_coeff;                    // 2B  : gadc gain cal 0x06C, RB 0x070

    uint8_t     hadc_os_code;                       // 1B  : hadc os cal  0x244
    uint8_t     reserved0;                          // 1B  : put here to keep struct align
    uint32_t    hadc_wt_b22;                        // 4B  : hadc cap cal 0x1B0
    uint32_t    hadc_wt_b21;                        // 4B  : hadc cap cal 0x1B4
    uint32_t    hadc_wt_b20;                        // 4B  : hadc cap cal 0x1B8
    uint32_t    hadc_wt_b19;                        // 4B  : hadc cap cal 0x1BC
    uint16_t    hadc_wt_b18;                        // 2B  : hadc cap cal 0x1C0
    uint16_t    hadc_wt_b17;                        // 2B  : hadc cap cal 0x1C4
    uint16_t    hadc_wt_b16;                        // 2B  : hadc cap cal 0x1C8
    uint16_t    hadc_wt_b15;                        // 2B  : hadc cap cal 0x1CC
    uint16_t    hadc_wt_b14;                        // 2B  : hadc cap cal 0x1D0
    uint16_t    hadc_wt_b13;                        // 2B  : hadc cap cal 0x1D4
    uint16_t    hadc_wt_b12;                        // 2B  : hadc cap cal 0x1D8
    uint16_t    hadc_wt_b11;                        // 2B  : hadc cap cal 0x1DC
    uint16_t    hadc_wt_b10;                        // 2B  : hadc cap cal 0x1E0
    uint16_t    hadc_wt_b9;                         // 2B  : hadc cap cal 0x1E4
    uint16_t    hadc_gain_coeff;                    // 2B  : hadc gain cal 0x140, RB 0x144

    int16_t     dac_code1;                          // 2B  : dac Code 0x3D4, RB 0x3D8
    int16_t     dac_code2;                          // 2B  : dac Code backup
    uint8_t     reserved[AFE_INFO_RESERVED - 1];        // 51B : reserved data fill 0xFF
} afe_config_t;

/**
 * @brief  Get the base address of ADC.
 * @return uintptr_t The base address of ADC.
 */
uintptr_t adc_porting_base_addr_get(void);

/**
 * @brief  Get the analog base address of ADC.
 * @return uintptr_t The analog base address of ADC.
 */
uintptr_t adc_porting_ana_base_addr_get(void);

/**
 * @brief  Get the base address of PMU registers associated with ADC.
 * @return uintptr_t The base address of PMU registers associated with ADC.
 */
uintptr_t adc_pmu_base_addr_get(void);

/**
 * @brief  Get the base address of always-on registers associated with ADC.
 * @return uintptr_t The base address of always-on registers associated with ADC.
 */
uintptr_t adc_aon_cfg_addr_get(void);

/**
 * @brief  Get the base address of DIAG part0 registers associated with ADC.
 * @return uintptr_t The base address of DIAG part0 registers associated with ADC.
 */
uintptr_t adc_diag_cfg_addr0_get(void);

/**
 * @brief  Get the base address of DIAG part1 registers associated with ADC.
 * @return uintptr_t The base address of DIAG part1 registers associated with ADC.
 */
uintptr_t adc_diag_cfg_addr1_get(void);

/**
 * @brief  Register hal funcs objects into hal_adc module.
 */
void adc_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects from hal_adc module.
 */
void adc_port_unregister_hal_funcs(void);

/**
 * @brief  Set the divider number of the peripheral device clock.
 * @param [in] clock The clock which is used for adc sample, adc source clock is 2MHz.
 */
void adc_port_init_clock(adc_clock_t clock);

/**
 * @brief  Set the divider number of the peripheral device clock.
 * @param [in] on Enable or disable.
 */
void adc_port_clock_enable(bool on);

/**
 * @brief  Register the interrupt of adc.
 * @param [in] afe_scan_mode AFE mode to use.
 */
void adc_port_register_irq(afe_scan_mode_t afe_scan_mode);

/**
 * @brief  Unregister the interrupt of adc.
 * @param [in] afe_scan_mode AFE mode which is using.
 */
void adc_port_unregister_irq(afe_scan_mode_t afe_scan_mode);

/**
 * @brief  Power on or power off the peripheral device.
 * @param [in] on Power on or Power off.
 */
void adc_port_power_on(bool on);

/**
 * @brief  Lock of adc interrupt.
 * @param [in] channel The adc channel.
 */
void adc_irq_lock(uint8_t channel);

/**
 * @brief  Unlock of adc interrupt.
 * @param [in] channel The adc channel.
 */
void adc_irq_unlock(uint8_t channel);

/**
 * @brief Set adc calibration code into ADC_BASE_ADDR.
 * @param [in] afe_scan_mode AFE mode to set calibration code.
 * @param [in] afe_config ADC calibration code.
 * @return errcode_t Write result.
 */
errcode_t adc_set_cali_code(afe_scan_mode_t afe_scan_mode, afe_config_t *afe_config);

/**
 * @brief ADC calibration api.
 * @param [in]  afe_scan_mode AFE mode to use.
 * @param [in]  os_cali OS calibration enable.
 * @param [in]  cdac_cali CDAC calibration enable.
 * @param [in]  dcoc_cali DCOC calibration enable(Only enable for HADC).
 * @return errcode_t  Calibration result.
 */
errcode_t adc_calibration(afe_scan_mode_t afe_scan_mode, bool os_cali, bool cdac_cali, bool dcoc_cali);

/**
 * @brief  Restart cpu trace after ADC sample done.
 */
void hal_cpu_trace_restart(void);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif