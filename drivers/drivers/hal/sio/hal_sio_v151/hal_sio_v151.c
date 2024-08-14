/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides v151 hal sio \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-07, Create file. \n
 */

#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "tcxo.h"
#include "sio_porting.h"
#include "hal_sio_v151.h"

#define SIO_FIFO_SIZE                   16
#define SIO_INTMASK                     0xfe
#define I2S_MCLK_DIV                    2
#define THRESHOLD                       7
#define EIGHT_BIT                       8

static hal_sio_rx_data_t g_sio_read_data[I2S_MAX_NUMBER] = { 0 };
static hal_sio_callback_t g_hal_sio_callbacks[I2S_MAX_NUMBER] = { 0 };
static hal_sio_config_t g_hal_sio_config[I2S_MAX_NUMBER] = { 0 };

static errcode_t hal_sio_v151_init(sio_bus_t bus)
{
    if (hal_sio_regs_init(bus) != ERRCODE_SUCC) {
        return ERRCODE_SIO_REG_ADDR_INVALID;
    }
    return ERRCODE_SUCC;
}

static void hal_sio_v151_deinit(sio_bus_t bus)
{
    hal_sio_regs_deinit(bus);
}

static void hal_sio_v151_set_intmask(sio_bus_t bus, uint32_t intmask)
{
    sios_v151_regs(bus)->intmask = intmask;
}

static void hal_sio_v151_clear_interrupt(sio_bus_t bus, uint32_t intclr)
{
    sios_v151_regs(bus)->intclr = intclr;
}

static uint32_t hal_sio_v151_get_intstatus(sio_bus_t bus)
{
    return sios_v151_regs(bus)->intstatus;
}

static void hal_sio_v151_crg_clock_enable(sio_bus_t bus, bool enable)
{
    hal_sio_v151_i2s_crg_set_crg_clken(bus, (uint32_t)enable);
}

static void hal_sio_v151_extend_receive_config(sio_bus_t bus, hal_sio_channel_number_t channel)
{
    hal_sio_v151_mode_set_ext_rec_en(bus, 1);
    hal_sio_v151_mode_set_chn_num(bus, channel);
}

static void hal_sio_v151_data_width_set(sio_bus_t bus, hal_sio_data_width_t data_width)
{
    hal_sio_v151_data_width_set_tx_mode(bus, data_width);
    hal_sio_v151_data_width_set_rx_mode(bus, data_width);
}

void hal_sio_v151_master_mode_sel(sio_bus_t bus, bool enable)
{
    if (enable) {
        hal_sio_v151_mode_set_cfg_i2s_ms_mode_sel(bus, 1);
    } else {
        hal_sio_v151_mode_set_cfg_i2s_ms_mode_sel(bus, 0);
    }
}

void hal_sio_v151_set_clk(sio_bus_t bus)
{
    hal_sio_v151_i2s_crg_set_crg_clken(bus, 1);
}

void hal_sio_v151_clr_clk(sio_bus_t bus)
{
    hal_sio_v151_i2s_crg_set_crg_clken(bus, 0);
}

static void hal_sio_v151_i2s_pos(sio_bus_t bus)
{
    sios_v151_regs(bus)->i2s_start_pos = 0;
    sios_v151_regs(bus)->i2s_pos_flag = 0;
}
static void hal_sio_v151_i2s_clk_sel(sio_bus_t bus)
{
    hal_sio_v151_i2s_crg_set_bclk_sel(bus, 0);
    hal_sio_v151_i2s_crg_set_fs_sel(bus, 0);
}

static void hal_sio_v151_i2s_set_fs(sio_bus_t bus, uint8_t data_width, uint32_t number_of_channels)
{
    hal_sio_v151_crg_clock_enable(bus, false);
    hal_sio_v151_fs_div_num_set_num(bus, (data_width * number_of_channels));
    hal_sio_v151_fs_div_ratio_num_set_num(bus, (data_width * number_of_channels) / I2S_DUTY_CYCLE);
}

void hal_i2s_set_bclk(sio_bus_t bus, uint8_t data_width, uint32_t ch)
{
    uint32_t bclk_div_num;
    bclk_div_num = sio_porting_get_bclk_div_num(data_width, ch);
    hal_sio_v151_crg_clock_enable(bus, false);
    hal_sio_v151_i2s_crg_set_bclk_div_en(bus, 0);
    hal_sio_v151_bclk_div_num_set_num(bus, bclk_div_num);
    hal_sio_v151_i2s_crg_set_bclk_div_en(bus, 1);
}

void hal_sio_v151_i2s_drive_mode(sio_bus_t bus, uint8_t drive_mode, uint8_t data_width, uint8_t number_of_channels)
{
    hal_sio_v151_i2s_clk_sel(bus);
    if (drive_mode == 0) {
        hal_sio_v151_master_mode_sel(bus, false);
        return;
    }
    hal_sio_v151_master_mode_sel(bus, true);
    hal_sio_v151_i2s_set_fs(bus, data_width, number_of_channels);
    hal_i2s_set_bclk(bus, data_width, number_of_channels);
}

static void hal_i2s_config(sio_bus_t bus, hal_sio_mode_t mode)
{
    hal_sio_v151_set_intmask(bus, SIO_INTMASK);
    hal_sio_v151_mode_set_mode(bus, mode);
    sio_porting_register_irq(bus);
    hal_sio_v151_i2s_drive_mode(bus, g_hal_sio_config[bus].drive_mode, g_hal_sio_config[bus].div_number,
                               g_hal_sio_config[bus].number_of_channels);
    hal_sio_v151_mode_set_clk_edge(bus, 1);
    hal_sio_v151_mode_set_pcm_mode(bus, 0);

    if (g_hal_sio_config[bus].transfer_mode == MULTICHANNEL_MODE) {
        hal_sio_v151_extend_receive_config(bus, g_hal_sio_config[bus].channels_num);
    }

    hal_sio_v151_data_width_set(bus, g_hal_sio_config[bus].data_width);
    hal_sio_v151_i2s_pos(bus);
    hal_sio_v151_ct_set_set_rst_n(bus, 1);
    hal_sio_v151_ct_set_set_intr_en(bus, 1);
    hal_sio_v151_fifo_threshold_set_rx_fifo_threshold(bus, THRESHOLD);
    hal_sio_v151_fifo_threshold_set_tx_fifo_threshold(bus, THRESHOLD);
}

static void hal_sio_v151_set_config(sio_bus_t bus, const hal_sio_config_t *config)
{
    (void)memcpy_s(&g_hal_sio_config[bus], sizeof(hal_sio_config_t), config, sizeof(hal_sio_config_t));
}

static errcode_t hal_sio_v151_get_config(sio_bus_t bus, hal_sio_config_t *config)
{
    hal_sio_config_t *config_temp = (hal_sio_config_t *)config;
    if (config_temp == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    (void)memcpy_s(config_temp, sizeof(hal_sio_config_t), &g_hal_sio_config[bus], sizeof(hal_sio_config_t));

    return ERRCODE_SUCC;
}

static void hal_sio_v151_rx_enable(sio_bus_t bus, bool en)
{
    hal_sio_v151_set_clk(bus);
    hal_sio_v151_ct_set_set_rx_enable(bus, (uint32_t)en);
}

static uint32_t hal_sio_read_data(sio_bus_t bus, hal_sio_voice_channel_t voice_channe)
{
    uint32_t data_temp = 0;
    if (voice_channe == SIO_LEFT) {
        if (hal_sio_v151_rx_sta_get_rx_left_depth(bus) > 0) {
            data_temp = hal_sio_v151_i2s_left_rd_get_rx_left_data(bus);
        }
    } else {
        if (hal_sio_v151_rx_sta_get_rx_right_depth(bus) > 0) {
            data_temp = hal_sio_v151_i2s_right_rd_get_rx_right_data(bus);
        }
    }
    return data_temp;
}

static void hal_sio_write_data(sio_bus_t bus, uint32_t data, hal_sio_voice_channel_t voice_channe)
{
    uint32_t data_temp = data;
    if (voice_channe == SIO_LEFT) {
        if (hal_sio_v151_tx_sta_get_tx_left_depth(bus) < SIO_FIFO_SIZE) {
            hal_sio_v151_i2s_left_xd_set_tx_left_data(bus, data_temp);
        }
    } else {
        if (hal_sio_v151_tx_sta_get_tx_right_depth(bus) < SIO_FIFO_SIZE) {
            hal_sio_v151_i2s_right_xd_set_tx_right_data(bus, data_temp);
        }
    }
}

static void hal_i2s_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_i2s_config(bus, mode);
    uint32_t *temp_tx_left_data = data->left_buff;
    uint32_t *temp_tx_right_data = data->right_buff;
    uint32_t trans_time = data->length;
    hal_sio_v151_ct_set_set_tx_enable(bus, 1);
    hal_sio_v151_set_clk(bus);
    for (uint32_t i = 0; i < trans_time; i++) {
        hal_sio_write_data(bus, temp_tx_left_data[i], SIO_LEFT);
        hal_sio_write_data(bus, temp_tx_right_data[i], SIO_RIGHT);
    }
    hal_sio_v151_ct_clr_set_tx_enable(bus, 1);
    hal_sio_v151_clr_clk(bus);
}

static hal_sio_write_t g_sio_write_func[NONE_SIO_MODE] = {
    hal_i2s_write,
};

static void hal_sio_v151_write(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    g_sio_write_func[mode](bus, data, mode);
}

static void hal_sio_v151_register(sio_bus_t bus, hal_sio_callback_t callback)
{
    g_hal_sio_callbacks[bus] = callback;
}

static void hal_sio_v151_unregister(sio_bus_t bus)
{
    g_hal_sio_callbacks[bus] = NULL;
}

static void hal_sio_v151_loop(sio_bus_t bus, bool en)
{
    hal_sio_v151_version_set_loop(bus, (uint32_t)en);
}

static void hal_sio_v151_get_data(sio_bus_t bus, hal_sio_rx_data_t *data)
{
    *data = g_sio_read_data[bus];
}

void hal_sio_v151_irq_handler(sio_bus_t bus)
{
    uint32_t int_status;
    int_status = hal_sio_v151_get_intstatus(bus);
    if ((bit(0) & int_status) != 0) {
        uint32_t trans_frames = hal_sio_v151_rx_sta_get_rx_right_depth(bus);
        while (trans_frames-- > 0) {
            g_sio_read_data[bus].left_buff[g_sio_read_data[bus].length] = hal_sio_read_data(bus, SIO_LEFT);
            g_sio_read_data[bus].right_buff[g_sio_read_data[bus].length] = hal_sio_read_data(bus, SIO_RIGHT);
            g_sio_read_data[bus].length++;
        }
        if (g_hal_sio_callbacks[bus]) {
            g_hal_sio_callbacks[bus](g_sio_read_data[bus].left_buff, g_sio_read_data[bus].right_buff,
                                     g_sio_read_data[bus].length);
            (void)memset_s(((void *)&g_sio_read_data), sizeof(hal_sio_rx_data_t), 0,
                           sizeof(hal_sio_rx_data_t));
            g_sio_read_data[bus].length = 0;
        }
    }

    int_status = hal_sio_v151_get_intstatus(bus);
    for (int j = 0; j < TX_LEFT_FIFO_UNDER; j++) {
        if ((bit(j) & int_status) != 0) {
            hal_sio_v151_clear_interrupt(bus, bit(j));
        }
    }
    hal_sio_v151_ct_clr_set_rst_n(bus, 1);
    hal_sio_v151_ct_clr_set_tx_enable(bus, 1);
    hal_sio_v151_ct_clr_set_rx_enable(bus, 1);
    hal_sio_v151_clr_clk(bus);
}

void hal_sio_v151_loop_trans(sio_bus_t bus, hal_sio_tx_data_t *data, hal_sio_mode_t mode)
{
    hal_i2s_config(bus, mode);
    uint32_t *write_left_data = data->left_buff;
    uint32_t *write_right_data = data->right_buff;
    uint32_t trans_time = data->length / 8;
    uint32_t index_i2s = 0;
    while (trans_time > 0) {
        hal_sio_v151_ct_set_set_rst_n(bus, 1);
        for (uint32_t i = 0; i < EIGHT_BIT; i++) {
            hal_sio_write_data(bus, write_left_data[index_i2s+i], SIO_LEFT);
            hal_sio_write_data(bus, write_right_data[index_i2s+i], SIO_RIGHT);
        }
        index_i2s += EIGHT_BIT;
        hal_sio_v151_ct_set_set_rx_enable(bus, 1);
        hal_sio_v151_ct_set_set_tx_enable(bus, 1);
        hal_sio_v151_set_clk(bus);
        trans_time--;
        uapi_tcxo_delay_us(4ULL);
    }
}

void hal_sio_v151_txrx_disable(sio_bus_t bus)
{
    sio_v151_ct_clr_data_t clr_set;
    clr_set.d32 = sios_v151_regs(bus)->ct_clr;
    clr_set.b.tx_enable = 1;
    clr_set.b.rx_enable = 1;
    clr_set.b.rst_n = 1;
    sios_v151_regs(bus)->ct_clr = clr_set.d32;
    hal_sio_v151_clr_clk(bus);
}

void hal_sio_v151_txrx_enable(sio_bus_t bus)
{
    sio_v151_ct_set_data_t ct_set;
    ct_set.d32 = sios_v151_regs(bus)->ct_set;
    ct_set.b.tx_enable = 1;
    ct_set.b.rx_enable = 1;
    ct_set.b.rst_n = 1;
    sios_v151_regs(bus)->ct_set = ct_set.d32;
    hal_sio_v151_set_clk(bus);
}

#if defined(CONFIG_I2S_SUPPORT_DMA)
#define SIO_INTMASK_TX_DISABLED 0x32
#define SIO_INTMASK_RX_DISABLED 0xd
static void hal_sio_v151_dma_cfg(sio_bus_t bus, const uintptr_t attr)
{
    hal_i2s_dma_attr_t *dma_attr = (hal_i2s_dma_attr_t *)attr;
    hal_i2s_config(bus, I2S_MODE);
    hal_sio_v151_pos_merge_set_en(bus, 1);
    if (dma_attr->tx_dma_enable) {
        hal_sio_fifo_threshold_set_tx_fifo_threshold(bus, dma_attr->tx_int_threshold);
        uint32_t mask = sios_v151_regs(bus)->intmask;
        mask = (mask | SIO_INTMASK_TX_DISABLED);
        hal_sio_set_intmask(bus, mask);
    }
    if (dma_attr->rx_dma_enable) {
        hal_sio_fifo_threshold_set_rx_fifo_threshold(bus, dma_attr->rx_int_threshold);
        uint32_t mask = sios_v151_regs(bus)->intmask;
        mask = (mask | SIO_INTMASK_RX_DISABLED);
        hal_sio_set_intmask(bus, mask);
    }
}
#endif

static hal_sio_funcs_t g_hal_sio_v151_funcs = {
    .init = hal_sio_v151_init,
    .deinit = hal_sio_v151_deinit,
    .set_config = hal_sio_v151_set_config,
    .get_config = hal_sio_v151_get_config,
    .rx_enable = hal_sio_v151_rx_enable,
    .write = hal_sio_v151_write,
    .get_data = hal_sio_v151_get_data,
    .registerfunc = hal_sio_v151_register,
    .unregisterfunc = hal_sio_v151_unregister,
    .loop = hal_sio_v151_loop,
    .loop_trans = hal_sio_v151_loop_trans,
#if defined(CONFIG_I2S_SUPPORT_DMA)
    .dma_cfg = hal_sio_v151_dma_cfg,
#endif
};

hal_sio_funcs_t *hal_sio_v151_funcs_get(void)
{
    return &g_hal_sio_v151_funcs;
}