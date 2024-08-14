/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V153 HAL adc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-31， Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "adc_porting.h"
#include "hal_adc_v153.h"

hal_common_sample_info_t g_common_sample = COMMON_DEFAULT_CONFIG;
hal_gafe_sample_info_t g_gafe_sample = GADC_DEFAULT_CONFIG;
hal_amic_sample_info_t g_amic_sample = AMIC_DEFAULT_CONFIG;
static volatile int32_t g_sample_data[CONFIG_AFE_SAMPLE_TIMES];
static bool g_enable_flag = false;

static errcode_t hal_adc_v153_init(void)
{
    if (hal_adc_v153_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_ADC_REG_ADDR_INVALID;
    }
    hal_afe_release_xo32m();
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_30);
    hal_afe_mtcmos_en();
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_50);
    hal_afe_iso_release();
    hal_afe_ana_rstn_release();
    hal_afe_dig_clk_release();
    hal_afe_dig_apb_rstn_release();
    hal_afe_dig_clr();
    hal_afe_dig_start();
    return ERRCODE_SUCC;
}

static errcode_t hal_adc_v153_deinit(void)
{
    hal_adc_v153_regs_deinit();
    return ERRCODE_SUCC;
}

#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
static errcode_t hal_adc_v153_differential_channel_set(adc_channel_t postive_ch, adc_channel_t negative_ch, bool on)
{
    if (unlikely((adc_v153_gadc_channel_sel_t)postive_ch > VICMREF) ||
        unlikely((adc_v153_gadc_channel_sel_t)negative_ch > VICMREF)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (!on) {
        hal_gafe_channel_close();
    } else {
        g_gafe_sample.cfg_amux_1.b.amuxp_devide_disable = 1;
        g_gafe_sample.cfg_amux_1.b.amuxn_devide_disable = 1;
        g_adc_regs->cfg_amux_1 = g_gafe_sample.cfg_amux_1.d32;
        g_gafe_sample.cfg_amux_1.b.amuxp_sensor_ch_sel = BIT(postive_ch);
        g_gafe_sample.cfg_amux_1.b.amuxn_sensor_ch_sel = BIT(negative_ch);
        g_adc_regs->cfg_amux_2 = 0;
    }
    return ERRCODE_SUCC;
}
#endif

static errcode_t hal_adc_v153_channel_set(adc_channel_t channel, bool on)
{
    if (unlikely((adc_v153_gadc_channel_sel_t)channel > VICMREF)) {
        return ERRCODE_ADC_INVALID_PARAMETER;
    }
    if (!on) {
        hal_gafe_channel_close();
    } else {
        g_gafe_sample.cfg_amux_1.b.amuxp_devide_disable = 1;
        g_gafe_sample.cfg_amux_1.b.amuxn_devide_disable = 1;
        g_adc_regs->cfg_amux_1 = g_gafe_sample.cfg_amux_1.d32;
        g_gafe_sample.cfg_amux_1.b.amuxp_sensor_ch_sel = BIT(channel);
        g_gafe_sample.cfg_amux_1.b.amuxn_sensor_ch_sel = BIT(VSSAFE1);
        g_adc_regs->cfg_amux_2 = 0;
    }
    return ERRCODE_SUCC;
}

static void hal_adc_v153_power_on(void)
{
    if (g_enable_flag == true) { return; }
    hal_afe_afeldo_rpoly_trim_set();
    hal_afe_vrefldo_rpoly_trim_set();
    hal_afe_adcldo_rpoly_trim_set();
    hal_afe_afeldo_open();
    hal_afe_adcldo_open();
    hal_afe_vrefldo_open();
    g_enable_flag = true;
}

errcode_t hal_adc_v153_cali(afe_scan_mode_t afe_scan_mode, bool os_cali, bool cdac_cali, bool dcoc_cali)
{
    hal_adc_v153_spd_cali();
    if (os_cali) {
        hal_adc_v153_os_cali();
    }
    if (cdac_cali) {
        hal_adc_v153_cdac_cali();
    }
    if (dcoc_cali == true) {
        hal_adc_v153_dcoc_cali();
    }
    UNUSED(afe_scan_mode);
    return ERRCODE_SUCC;
}

static void hal_adc_v153_enable(afe_scan_mode_t afe_scan_mode)
{
    hal_adc_common_enable(g_common_sample);
    if (afe_scan_mode == AFE_GADC_MODE) {
        hal_adc_gadc_enable(g_gafe_sample);
    } else if (afe_scan_mode == AFE_AMIC_MODE) {
        hal_adc_amic_enable(g_amic_sample);
    }
    hal_gafe_enable();
}

static void hal_adc_v153_power_off(afe_scan_mode_t afe_scan_mode)
{
    if (afe_scan_mode == AFE_GADC_MODE) {
        hal_gafe_power_off();
    }  else if (afe_scan_mode == AFE_AMIC_MODE) {
        hal_amic_power_off();
    }
}

static void hal_adc_v153_power_en(afe_scan_mode_t afe_scan_mode, bool on)
{
    if (on) {
        hal_adc_v153_power_on();
        hal_adc_v153_enable(afe_scan_mode);
    } else {
        hal_adc_v153_power_off(afe_scan_mode);
        g_enable_flag = false;
    }
}

__attribute__((weak)) void hal_cpu_trace_restart(void)
{
}

static int32_t hal_adc_v153_manual(adc_channel_t channel)
{
    UNUSED(channel);
    return 0;
}

static void hal_gadc_sample_with_lpc(void)
{
    hal_afe_diag_set();
    hal_afe_diag_send2aix();
    hal_afe_diag_clk_enable();
    hal_afe_diag_source_sel();
    hal_afe_diag_set_full_done_mode();
    hal_afe_diag_sample_len((uint32_t)(sizeof(g_sample_data) / sizeof(uint32_t)));
    hal_afe_diag_sample_start_addr((uint32_t)(uintptr_t)g_sample_data);
    hal_afe_diag_sample_end_addr((uint32_t)(uintptr_t)g_sample_data + (uint32_t)(sizeof(g_sample_data)));

    g_adc_diag_regs1->cfg_mcu_diag_sample_mode = 0;
    hal_afe_diag_sample_sync();
    hal_afe_diag_sample_en();
    while (!hal_afe_diag_get_sts()) {}
    hal_afe_diag_sample_off();
    hal_cpu_trace_restart();
}

static int32_t hal_adc_v153_sample(adc_channel_t channel)
{
    if (channel >= ADC_CHANNEL_MAX_NUM) { return ERRCODE_ADC_INVALID_SAMPLE_VALUE; }
    int32_t sample_value = 0;
    hal_gafe_channel_sel(g_gafe_sample.cfg_amux_1);
    hal_gadc_sample_with_lpc();
    int8_t count = 0;
    int32_t temp = 0;
    uint8_t index0 = 0;
    uint8_t index1 = 0;
    for (uint32_t i = 0; i < CONFIG_AFE_SAMPLE_TIMES; i++) {
        if ((g_sample_data[i] & BIT(GAFE_SAMPLE_VALUE_SIGN_BIT)) != 0) {
            temp = g_sample_data[i] - GAFE_SAMPLE_MINUS_VALUE;
            sample_value += temp;
            count++;
            index0 = i;
        } else {
            sample_value += g_sample_data[i];
            count--;
            index1 = i;
        }
    }
    if (count == CONFIG_AFE_SAMPLE_TIMES || count == -CONFIG_AFE_SAMPLE_TIMES) {
        sample_value /= CONFIG_AFE_SAMPLE_TIMES;
    } else if (count == 1) {
        sample_value = ((g_sample_data[index0] & BIT(GAFE_SAMPLE_VALUE_SIGN_BIT)) == 0) ?
                       g_sample_data[index0] : g_sample_data[index0] - GAFE_SAMPLE_MINUS_VALUE;
    } else {
        sample_value = ((g_sample_data[index1] & BIT(GAFE_SAMPLE_VALUE_SIGN_BIT)) == 0) ?
                       g_sample_data[index1] : g_sample_data[index1] - GAFE_SAMPLE_MINUS_VALUE;
    }
    return sample_value;
}

static hal_adc_funcs_t g_hal_adc_v153_funcs = {
    .init = hal_adc_v153_init,
    .deinit = hal_adc_v153_deinit,
    .ch_set = hal_adc_v153_channel_set,
#if defined(CONFIG_ADC_SUPPORT_DIFFERENTIAL)
    .diff_ch_set = hal_adc_v153_differential_channel_set,
#endif
    .power_en = hal_adc_v153_power_en,
    .manual = hal_adc_v153_manual,
    .auto_sample = hal_adc_v153_sample
};

void hal_adc_done_irq_handler(afe_scan_mode_t afe_scan_mode)
{
    unused(afe_scan_mode);
}

void hal_adc_alarm_irq_handler(afe_scan_mode_t afe_scan_mode)
{
    unused(afe_scan_mode);
}

hal_adc_funcs_t *hal_adc_v153_funcs_get(void)
{
    return &g_hal_adc_v153_funcs;
}
