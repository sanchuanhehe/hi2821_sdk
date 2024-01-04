/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides flash driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-16, Create file. \n
 */

#include <stdint.h>
#include "common_def.h"
#include "hal_xip.h"
#include "securec.h"
#if defined(CONFIG_SPI_USING_V151) && (CONFIG_SPI_USING_V151 == 1)
#include "hal_spi_v151_regs_op.h"
#else
#include "hal_spi_v100_regs_op.h"
#endif
#include "flash.h"

#define FLASH_CMD_INDEX 0
#define FLASH_CMD_OFFSET_INDEX 1
#define FLASH_CMD_VALUE_INDEX 2

#define FLASH_CMD_SHIFT_BIT 24
#define FLASH_BUFF_BYTES 1
#define FLASH_WORD_ALIGN 4
#define FLASH_XIP_8_BYTE 8

#ifndef SPI_RX_FIFO_DEPTH
#define SPI_RX_FIFO_DEPTH 16
#endif

#ifndef SPI_TX_FIFO_DEPTH
#define SPI_TX_FIFO_DEPTH 16
#endif

#define SHIFT_WORD_23_16_BITS_TO_BYTE         16
#define SHIFT_WORD_15_8_BITS_TO_BYTE          8

#define SPI_MINUMUM_CLK_DIV         2
#define SPI_MAXIMUM_CLK_DIV         65534
#define SPI_MHZ_TO_HZ               1000000

#define flash_trans_bytes_to_word(x)   ((((uint32_t)(*(x)))) + (((uint32_t)(*((x) + 1))) << 8) + \
                                        (((uint32_t)(*((x) + 2))) << 16) + ((uint32_t)(*((x) + 3)) << 24))

flash_cfg_t g_flash_config[FLASH_MAX] = { 0 };

#if !defined(CONFIG_FLASH_SUPPORT_XIP)
static bool flash_is_power_on(flash_id_t id)
{
    uint8_t status = 0;

    if (uapi_flash_read_status(id, &status) != ERRCODE_SUCC) {
        return false;
    }

    if (((uint32_t)status & FLASH_WIP) == 0) {
        return true;
    }
    return false;
}
#endif

static bool flash_wait_for_power_on(flash_id_t id)
{
    uint8_t status = 0;
    uint32_t timeout = CONFIG_FLASH_POWER_ON_TIMEOUT;
    do {
        if (uapi_flash_read_status(id, &status) != ERRCODE_SUCC) {
            return false;
        }

        if ((--timeout) == 0) {
            flash_print("flash power on fail\r\n");
            return false;
        }
    } while (((uint32_t)status & FLASH_WIP) == 1);

    return true;
}

static void flash_spi_init(flash_id_t id)
{
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 0);
    hal_spi_ser_set_ser(g_flash_config[id].bus, 1);
    hal_spi_txftlr_set_tft(g_flash_config[id].bus, CONFIG_SPI_TX_FIFO_THRESHOLD);
    hal_spi_rxftlr_set_rft(g_flash_config[id].bus, CONFIG_SPI_RX_FIFO_THRESHOLD);
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 1);
}

errcode_t uapi_flash_init(flash_id_t id)
{
    if (g_flash_config[id].isinit == 1) {
        return ERRCODE_SUCC;
    }
#if defined(CONFIG_FLASH_SUPPORT_LPC)
    flash_port_power_on(true);
    flash_port_clock_enable(true);
#endif

    flash_porting_get_config(id, g_flash_config);
#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
    /* Xip is enable means flash is power on. */
    if (hal_xip_is_enable((xip_id_t)id)) {
        hal_xip_set_cur_mode((xip_id_t)id, XIP_MODE_NORMAL);
        xip_error_interrupt_enable();
        g_flash_config[id].mode = HAL_SPI_FRAME_FORMAT_QUAD;
        if (uapi_spi_init(g_flash_config[id].bus, &g_flash_config[id].attr, &g_flash_config[id].extra_attr) !=
            ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_INIT_FAIL;
        };
    } else {
        if (uapi_spi_init(g_flash_config[id].bus, &g_flash_config[id].attr, &g_flash_config[id].extra_attr) !=
            ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_INIT_FAIL;
        };
        flash_spi_init(id);
        flash_porting_power_on(id);

        /* Wait flash power on. */
        if (!flash_wait_for_power_on(id)) {
            return ERRCODE_FLASH_INIT_FAIL;
        }
    }
#else
    if (uapi_spi_init(g_flash_config[id].bus, &g_flash_config[id].attr, &g_flash_config[id].extra_attr) !=
        ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_INIT_FAIL;
    };

    /* Flash is not power on. */
    if (!flash_is_power_on(id)) {
        flash_spi_init(id);
        flash_porting_power_on(id);
        if (flash_wait_for_power_on(id)) {
            return ERRCODE_FLASH_INIT_FAIL;
        }
    }
#endif
    flash_porting_spi_lock_create(id);
    g_flash_config[id].isinit = 1;
    return ERRCODE_SUCC;
}

errcode_t uapi_flash_deinit(flash_id_t id)
{
    if (g_flash_config[id].isinit == 0) {
        return ERRCODE_SUCC;
    }

    if (uapi_spi_deinit(g_flash_config[id].bus) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_INIT_FAIL;
    };

    flash_porting_spi_lock_delete(id);

#if defined(CONFIG_FLASH_SUPPORT_LPC)
    flash_port_clock_enable(false);
    flash_port_power_on(false);
#endif
    g_flash_config[id].isinit = 0;
    g_flash_config[id].qspi_isinit = 0;
    return ERRCODE_SUCC;
}

static flash_qspi_xip_config_t *flash_get_xip_config(flash_id_t id)
{
    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return NULL;
    }
    return g_flash_device_parameter[g_flash_config[id].flash_manufacturer].enter_xip_mode_config;
}

static bool flash_parameter_check(flash_id_t id, uint32_t addr, const uint8_t *dst, uint32_t length)
{
    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return false;
    }

    if (dst == NULL) {
        return false;
    }

    if (length == 0) {
        return false;
    }

    if (addr >= g_flash_device_parameter[g_flash_config[id].flash_manufacturer].flash_size) {
        return false;
    }

    return true;
}

static void trans_complete_wait_timeout(spi_bus_t bus, uint32_t timeout)
{
    uint32_t ul_trans_timeout = 0;
    uint32_t ul_spi_state;
    uint32_t ul_spi_trans_fifo_num;

    do {
        ul_spi_trans_fifo_num = hal_spi_txflr_get_txtfl(bus);
        ul_spi_state = hal_spi_sr_get_busy(bus);
        ul_trans_timeout++;

        if (ul_trans_timeout >= timeout) {
            return;
        }
    } while (ul_spi_state == 1 || ul_spi_trans_fifo_num > 0);
    return;
}

static void flash_exit_xip_mode(flash_id_t id, uint32_t xip_enabled)
{
    if (!g_flash_config[id].is_xip || xip_enabled == 0) {
        return;
    }
#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
    uapi_flash_exit_from_xip_mode(id);
#endif
}

static void flash_enter_xip_mode(flash_id_t id, uint32_t xip_enabled)
{
    if (!g_flash_config[id].is_xip || xip_enabled == 0) {
        return;
    }
#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
    uapi_flash_switch_to_xip_mode(id);
#endif
}

static uint32_t flash_get_xip_enabled(flash_id_t id)
{
    uint32_t xip_enabled = 0;
#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
    if (g_flash_config[id].is_xip) {
        xip_enabled = hal_xip_is_enable((xip_id_t)id) ? 1 : 0;
    }
#else
    unused(id);
#endif
#if !defined(BUILD_APPLICATION_SSB)
    xip_enabled = 0;
#endif

    return xip_enabled;
}

static bool flash_read_status_config(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    if (mode_config == NULL) {
        return false;
    }

    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    attr->ndf = FLASH_BUFF_BYTES;

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
            extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
            extra_attr->qspi_param.trans_type = HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q;
            extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
            extra_attr->qspi_param.wait_cycles = 0;
        }
    }

    return true;
}

errcode_t uapi_flash_read_status(flash_id_t id, uint8_t *status)
{
    if (!flash_read_status_config(id)) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_xfer_data_t xfer_data = { 0 };

    uint8_t cmd = FLASH_RDSR1_CMD;
    switch (g_flash_config[id].mode) {
        case HAL_SPI_FRAME_FORMAT_QUAD:
            if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
                xfer_data.cmd = cmd;
                break;
            } else {
                xfer_data.tx_buff = &cmd;
                xfer_data.tx_bytes = FLASH_BUFF_BYTES;
                break;
            }
        case HAL_SPI_FRAME_FORMAT_STANDARD:
            xfer_data.tx_buff = &cmd;
            xfer_data.tx_bytes = FLASH_BUFF_BYTES;
            break;
        default:
            return ERRCODE_FLASH_FALSE_MODE;
    }
    xfer_data.rx_buff = status;
    xfer_data.rx_bytes = FLASH_BUFF_BYTES;

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_enter_xip_mode(id, xip_enabled);
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD && mode_config->enter_xip_before_trans_type !=
        HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr);
    }

    if (uapi_spi_master_writeread(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_enter_xip_mode(id, xip_enabled);
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);
    return ERRCODE_SUCC;
}

bool uapi_flash_is_processing(flash_id_t id)
{
    uint8_t status = 0;

    if (uapi_flash_read_status(id, &status) != ERRCODE_SUCC) {
        return true;
    }

    if (((uint32_t)status & FLASH_WIP) == 1) {
        return true;
    }
    return false;
}

static uint32_t flash_get_write_once_trans_length(uint32_t addr, uint32_t length)
{
    /* Get trans len, addr must align FLASH_WRITE_MAX_TRANS_CNT, len is not greater than FLASH_WRITE_MAX_TRANS_CNT. */
    if ((uint32_t)((addr & (FLASH_WRITE_MAX_TRANS_CNT - 1)) + length) >= FLASH_WRITE_MAX_TRANS_CNT) {
        return FLASH_WRITE_MAX_TRANS_CNT - ((uint32_t)(addr & (FLASH_WRITE_MAX_TRANS_CNT - 1)));
    }
    return length;
}

static void flash_wait_for_wen_work_config(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    attr->ndf = FLASH_BUFF_BYTES;
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
            extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
            extra_attr->qspi_param.trans_type = HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q;
            extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
            extra_attr->qspi_param.wait_cycles = 0;
        }
    }
}

static errcode_t flash_wait_for_wen_work(flash_id_t id)
{
    uint32_t recv_data = 0;
    uint32_t timeout = CONFIG_SPI_WAIT_READ_TIMEOUT;
    uint8_t cmd = FLASH_RDSR1_CMD;
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_xfer_data_t xfer_data = { 0 };

    flash_wait_for_wen_work_config(id);
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            xfer_data.cmd = cmd;
        } else {
            xfer_data.tx_buff = &cmd;
            xfer_data.tx_bytes = FLASH_BUFF_BYTES;
        }
    } else {
        xfer_data.tx_buff = &cmd;
        xfer_data.tx_bytes = FLASH_BUFF_BYTES;
    }

    xfer_data.rx_buff = (uint8_t *)&recv_data;
    xfer_data.rx_bytes = FLASH_BUFF_BYTES;

    if ((g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) && mode_config->enter_xip_before_trans_type !=
        HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }
    do {
        /* Send read cmd. */
        if (uapi_spi_master_writeread(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) !=
            ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_TRANS_FAIL;
        }
        if ((--timeout) == 0) {
            flash_print("wait for wen timeout\r\n");
            return ERRCODE_FLASH_TIMEOUT;
        }
    } while ((recv_data & FLASH_WEL) == 0);

    return ERRCODE_SUCC;
}

static errcode_t flash_write_enable(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_xfer_data_t xfer_data = { 0 };
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    uint8_t cmd = FLASH_WREN_CMD;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    switch (g_flash_config[id].mode) {
        case HAL_SPI_FRAME_FORMAT_QUAD:
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
            extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
            extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
            extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
            extra_attr->qspi_param.wait_cycles = 0;
            xfer_data.cmd = cmd;
            break;
        case HAL_SPI_FRAME_FORMAT_STANDARD:
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
            xfer_data.tx_buff = &cmd;
            xfer_data.tx_bytes = FLASH_BUFF_BYTES;
            break;
        default:
            return ERRCODE_FLASH_FALSE_MODE;
    }

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    /* Write enable. */
    if (uapi_spi_master_write(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    return flash_wait_for_wen_work(id);
}

static bool flash_write_cfg(flash_id_t id, uint32_t dest_addr)
{
    if (flash_write_enable(id) != ERRCODE_SUCC) {
        return false;
    }
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    attr->ndf = 0;
    attr->frame_size = HAL_SPI_FRAME_SIZE_32;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_24;
            if (mode_config->enter_xip_after_enable_32bit_addr) {
                extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_32;
            }
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
            extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
            extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
            extra_attr->qspi_param.wait_cycles = 0;
        }
    }

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return false;
    }
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return false;
        }
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_PP_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, dest_addr);
        } else {
            hal_spi_dr_set_dr(g_flash_config[id].bus, (FLASH_PP_CMD << FLASH_CMD_SHIFT_BIT) | dest_addr);
        }
    } else {
        hal_spi_dr_set_dr(g_flash_config[id].bus, (FLASH_PP_CMD << FLASH_CMD_SHIFT_BIT) | dest_addr);
    }

    return true;
}

static bool flash_write_cfg_without_align(flash_id_t id, uint32_t dest_addr)
{
    if (flash_write_enable(id) != ERRCODE_SUCC) {
        return false;
    }
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    attr->ndf = 0;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_24;
            if (mode_config->enter_xip_after_enable_32bit_addr) {
                extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_32;
            }
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
            extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
            extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
            extra_attr->qspi_param.wait_cycles = 0;
        }
    }

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return false;
    }
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return false;
        }
    }
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_PP_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, dest_addr);
        } else {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_PP_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, dest_addr);
        }
    } else {
        hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_PP_CMD);
        hal_spi_dr_set_dr(g_flash_config[id].bus, dest_addr);
    }

    return true;
}

static uint32_t flash_write_get_first_word(uint32_t addr, const uint8_t **src, uint32_t *data_len, uint32_t *word,
                                           bool *send_first_data)
{
    uint32_t dest_addr;
    uint8_t cover_count;
    uint8_t first_data[FLASH_WORD_ALIGN] = { 0xff };

    if (((addr & 0x3) != 0) || (*data_len < FLASH_WORD_ALIGN)) {
        *send_first_data = true;
        memset_s(first_data, sizeof(first_data), 0xff, sizeof(first_data));

        dest_addr = (addr & 0xFFFFFFFC);

        cover_count = (uint8_t)(FLASH_WORD_ALIGN - (addr & 0x3));
        while ((cover_count > 0) && (*data_len > 0)) {
            first_data[FLASH_WORD_ALIGN - cover_count] = **src;
            cover_count--;
            (*data_len)--;
            (*src)++;
        }
    } else {
        dest_addr = addr;
    }
    *word = 0;
    *word = *((uint32_t *)first_data);
    return dest_addr;
}

static void flash_write_get_last_word(const uint8_t *src, uint32_t *data_len, uint32_t *word, bool *send_last_data)
{
    uint8_t cover_count;
    uint8_t last_data[FLASH_WORD_ALIGN] = { 0xff };

    if ((*data_len & 0x3) != 0) {
        *send_last_data = true;
        memset_s(last_data, sizeof(last_data), 0xff, sizeof(last_data));

        cover_count = *data_len & 0x3;
        *data_len -= cover_count;

        while (cover_count > 0) {
            cover_count--;
            last_data[cover_count] = src[*data_len + cover_count];
        }
    }
    *word = 0;
    *word = *((uint32_t *)last_data);
}

static uint32_t flash_get_src_index(const uint8_t *addr)
{
    uint32_t src_index = 0;

    if ((bool)addr_unalign_4b((uint32_t)(uintptr_t)addr)) {
        src_index = flash_trans_bytes_to_word(addr);
    } else {
        src_index = *(uint32_t *)addr;
    }

    return src_index;
}

static void flash_wait_fifo_empty(flash_id_t id)
{
    while (hal_spi_txflr_get_txtfl(g_flash_config[id].bus) >= SPI_TX_FIFO_DEPTH) {
    }
}

static void flash_write_word_by_once(flash_id_t id, uint32_t word, bool *send_flag)
{
    if (*send_flag) {
        flash_wait_fifo_empty(id);
        hal_spi_dr_set_dr(g_flash_config[id].bus, word);
        *send_flag = false;
    }
}

uint32_t uapi_flash_write_data(flash_id_t id, uint32_t addr, const uint8_t *src, uint32_t length)
{
    if (!flash_parameter_check(id, addr, src, length)) {
        return 0;
    }

    uint32_t write_len = length;
    uint32_t write_addr = addr;
    const uint8_t *write_src = src;
    uint32_t first_word = 0;
    uint32_t last_word = 0;
    uint32_t send_data_len = 0;
    bool send_first_data = false;
    bool send_last_data = false;

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);

    do {
        uint32_t data_len = flash_get_write_once_trans_length(write_addr, write_len);
        write_len -= data_len;
        uint32_t trasn_len = data_len;
        uint32_t dest_addr = flash_write_get_first_word(write_addr, &write_src, &data_len,
                                                        &first_word, &send_first_data);
        flash_write_get_last_word(write_src, &data_len, &last_word, &send_last_data);

        if (!flash_write_cfg(id, dest_addr)) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return 0;
        }

        /* Write the first word. */
        flash_write_word_by_once(id, first_word, &send_first_data);

        /* Write the middle word. */
        while (data_len > 0) {
            flash_wait_fifo_empty(id);
            hal_spi_dr_set_dr(g_flash_config[id].bus, flash_get_src_index(write_src));
            data_len -= FLASH_WORD_ALIGN;
            if ((data_len > 0) || (write_len > 0)) {
                write_src += FLASH_WORD_ALIGN;
            }
        }

        /* Write the last word. */
        flash_write_word_by_once(id, last_word, &send_last_data);

        trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

        while (uapi_flash_is_processing(id)) {
        }

        write_addr += trasn_len;
        send_data_len += trasn_len;
    } while (write_len > 0);

    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);
    return send_data_len;
}

uint32_t uapi_flash_write_data_without_align(flash_id_t id, uint32_t addr, const uint8_t *src, uint32_t length)
{
    if (!flash_parameter_check(id, addr, src, length)) {
        return 0;
    }

    uint32_t write_len = length;
    uint32_t write_addr = addr;
    const uint8_t *write_src = src;
    uint32_t send_data_len = 0;

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);

    do {
        uint32_t data_len = flash_get_write_once_trans_length(write_addr, write_len);
        write_len -= data_len;
        uint32_t trasn_len = data_len;

        if (!flash_write_cfg_without_align(id, write_addr)) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return 0;
        }

        /* Write the middle word. */
        while (data_len > 0) {
            flash_wait_fifo_empty(id);
            hal_spi_dr_set_dr(g_flash_config[id].bus, *write_src);
            data_len -= 1;
            if ((data_len > 0) || (write_len > 0)) {
                write_src += 1;
            }
        }

        trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

        while (uapi_flash_is_processing(id)) {
        }

        write_addr += trasn_len;
        send_data_len += trasn_len;
    } while (write_len > 0);

    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);
    return send_data_len;
}

static uint32_t flash_read_data_align(const uint32_t *addr, uint32_t *length, uint8_t *first_cover_count,
                                      uint8_t *last_cover_count)
{
    uint32_t dest_addr;
    /* Check if the address is 4 bytes aligned. */
    if (((*addr) & 0x3) != 0) {
        /* The actual send address requires 4 byte alignment. */
        dest_addr = ((*addr) & 0xFFFFFFFC);

        /* Fill the first 4 bytes of data. */
        *first_cover_count = (*addr) & 0x3;
        (*length) += (*first_cover_count);
    } else {
        dest_addr = (*addr);
    }
    /* Check if the last 4 bytes are aligned. */
    if (((*length) & 0x3) != 0) {
        /* Fill the first 4 bytes of data. */
        *last_cover_count = FLASH_WORD_ALIGN - (uint8_t)((*length) & 0x3);
        (*length) += (*last_cover_count);
    }
    /* Length >> 2 to Calculate how many 4 bytes total. */
    (*length) >>= 2;
    return dest_addr;
}

static bool flash_read_cfg(flash_id_t id, uint32_t *one_process_len, uint32_t *length, uint32_t dest_addr)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->frame_size = HAL_SPI_FRAME_SIZE_32;
    *one_process_len = *length > SPI_RX_FIFO_DEPTH ? SPI_RX_FIFO_DEPTH : *length;
    *length -= *one_process_len;
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
        extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
        extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_32;
        extra_attr->qspi_param.trans_type = mode_config->enter_xip_after_trans_type;
        extra_attr->qspi_param.wait_cycles = g_flash_device_parameter
                                             [g_flash_config[id].flash_manufacturer].read_dummy_clk;
    } else {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    }
    attr->ndf = *one_process_len;

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return false;
    }
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return false;
        }
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_after_enable_32bit_addr) {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_QRD_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, dest_addr);
        } else {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_QRD_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, (dest_addr << SHIFT_WORD_15_8_BITS_TO_BYTE) |
                              g_flash_device_parameter[g_flash_config[id].flash_manufacturer].exit_xip);
        }
    } else {
        hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_RD_CMD << FLASH_CMD_SHIFT_BIT | dest_addr);
    }

    return true;
}

static bool flash_read_cfg_without_align(flash_id_t id, uint32_t *one_process_len, uint32_t *length, uint32_t dest_addr)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    *one_process_len = *length > SPI_RX_FIFO_DEPTH ? SPI_RX_FIFO_DEPTH : *length;
    *length -= *one_process_len;
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
        extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
        extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_32;
        extra_attr->qspi_param.trans_type = mode_config->enter_xip_after_trans_type;
        extra_attr->qspi_param.wait_cycles = g_flash_device_parameter
                                                               [g_flash_config[id].flash_manufacturer].read_dummy_clk;
    } else {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    }
    attr->ndf = *one_process_len;

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return false;
    }
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return false;
        }
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_after_enable_32bit_addr) {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_QRD_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, dest_addr);
        } else {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_QRD_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, (dest_addr << SHIFT_WORD_15_8_BITS_TO_BYTE) |
                              g_flash_device_parameter[g_flash_config[id].flash_manufacturer].exit_xip);
        }
    } else {
        hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_RD_CMD << FLASH_CMD_SHIFT_BIT | dest_addr);
    }

    return true;
}

static uint8_t flash_read_first_word(flash_id_t id, uint8_t *dst, uint8_t *first_cover_count)
{
    uint8_t first_read[FLASH_WORD_ALIGN];
    *((uint32_t *)first_read) = hal_spi_dr_get_dr(g_flash_config[id].bus);
    uint8_t *recv_data = dst;
    uint8_t count = 0;

    while ((*first_cover_count) < FLASH_WORD_ALIGN) {
        *recv_data++ = first_read[*first_cover_count];
        (*first_cover_count)++;
        count++;
    }
    *first_cover_count = 0;
    return count;
}

static uint8_t flash_read_last_word(flash_id_t id, uint8_t *dst, uint8_t *last_cover_count)
{
    uint8_t last_read[FLASH_WORD_ALIGN] = { 0 };
    *((uint32_t *)last_read) = hal_spi_dr_get_dr(g_flash_config[id].bus);
    uint8_t *recv_data = dst;
    uint8_t count = 0;

    while ((*last_cover_count) < FLASH_WORD_ALIGN) {
        *recv_data++ = *(last_read + count);
        (*last_cover_count)++;
        count++;
    }
    *last_cover_count = 0;
    return count;
}

static uint8_t flash_read_process_word(flash_id_t id, uint8_t *dst, uint8_t *first_cover_count,
                                       uint8_t *last_cover_count, bool left_len)
{
    uint8_t count = 0;
    // Check if the first 4 bytes need to be filled
    if (*first_cover_count > 0) {
        count = flash_read_first_word(id, dst, first_cover_count);
    // Check if the last 4 bytes need to be filled
    } else if (left_len && (*last_cover_count > 0)) {
        count = flash_read_last_word(id, dst, last_cover_count);
    } else {
        *((uint32_t *)dst) = hal_spi_dr_get_dr(g_flash_config[id].bus);
        count = FLASH_WORD_ALIGN;
    }
    return count;
}

uint32_t uapi_flash_read_data(flash_id_t id, uint32_t addr, uint8_t *dst, uint32_t length)
{
    if (!flash_parameter_check(id, addr, dst, length)) {
        return 0;
    }
    uint32_t total_length = length;
    uint8_t *read_dst = dst;

    uint32_t one_process_len = 0;
    uint8_t first_cover_count = 0;
    uint8_t last_cover_count = 0;
    uint32_t dest_addr = flash_read_data_align(&addr, &length, &first_cover_count, &last_cover_count);

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);
    do {
        if (!flash_read_cfg(id, &one_process_len, &length, dest_addr)) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return 0;
        }

        dest_addr += FLASH_WORD_ALIGN * one_process_len;

        /* Save read data. */
        uint32_t ul_recv_timeout = 0;
        uint32_t ul_recv_num;
        while (one_process_len > 0) {
            ul_recv_num = hal_spi_rxflr_get_rxtfl(g_flash_config[id].bus);
            while ((ul_recv_num > 0) && (one_process_len > 0)) {
                read_dst += flash_read_process_word(id, read_dst, &first_cover_count, &last_cover_count,
                                                    (length == 0) && (one_process_len == 1));
                one_process_len--;
                ul_recv_num--;
                ul_recv_timeout = 0;
            }

            ul_recv_timeout++;
            if (ul_recv_timeout >= CONFIG_SPI_TRAN_MAX_TIMEOUT) {
                flash_enter_xip_mode(id, xip_enabled);
                flash_porting_spi_unlock(id, irqs);
                return 0;
            }
        }
    } while (length > 0);
    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);
    return total_length;
}

uint32_t uapi_flash_read_data_without_align(flash_id_t id, uint32_t addr, uint8_t *dst, uint32_t length)
{
    if (!flash_parameter_check(id, addr, dst, length)) {
        return 0;
    }
    uint32_t total_length = length;
    uint8_t *read_dst = dst;

    uint32_t one_process_len = 0;
    uint32_t dest_addr = addr;

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);
    do {
        if (!flash_read_cfg_without_align(id, &one_process_len, &length, dest_addr)) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return 0;
        }

        dest_addr += one_process_len;

        /* Save read data. */
        uint32_t ul_recv_timeout = 0;
        uint32_t ul_recv_num;
        while (one_process_len > 0) {
            ul_recv_num = hal_spi_rxflr_get_rxtfl(g_flash_config[id].bus);
            while ((ul_recv_num > 0) && (one_process_len > 0)) {
                *read_dst  = (uint8_t)hal_spi_dr_get_dr(g_flash_config[id].bus);
                read_dst++;
                one_process_len--;
                ul_recv_num--;
                ul_recv_timeout = 0;
            }

            ul_recv_timeout++;
            if (ul_recv_timeout >= CONFIG_SPI_TRAN_MAX_TIMEOUT) {
                flash_enter_xip_mode(id, xip_enabled);
                flash_porting_spi_unlock(id, irqs);
                return 0;
            }
        }
    } while (length > 0);
    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);
    return total_length;
}

static void flash_spi_write_config(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    attr->ndf = 0;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    attr->frame_size = HAL_SPI_FRAME_SIZE_32;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (!mode_config->enter_xip_after_enable_32bit_addr) {
            attr->frame_size = HAL_SPI_FRAME_SIZE_24;
        }
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
            extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
            extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
            extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
            extra_attr->qspi_param.wait_cycles = 0;
        } else {
            attr->frame_size = HAL_SPI_FRAME_SIZE_32;
        }
    }
}

errcode_t uapi_flash_chip_erase(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    if (mode_config == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    uint32_t irqs = flash_porting_spi_lock(id);

    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);

    if (flash_write_enable(id) != ERRCODE_SUCC) {
        flash_enter_xip_mode(id, xip_enabled);
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    flash_spi_write_config(id);

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD && mode_config->enter_xip_before_trans_type !=
        HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_enter_xip_mode(id, xip_enabled);
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    /* Write chip erase cmd. */
    hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_CE_CMD);

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

    while (uapi_flash_is_processing(id)) {
    }

    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

static void flash_get_max_erase_step(uint32_t current_addr, uint32_t max_addr, uint32_t *max_step, uint32_t *cmd_id)
{
    uint32_t current_offset;

    current_offset = current_addr % FLASH_BLOCK_64K_SIZE;
    if ((current_offset != FLASH_BLOCK_32K_SIZE) && (current_offset != 0)) {
        *max_step = FLASH_PAGE_SIZE;
        *cmd_id = FLASH_SE_CMD;
    } else if ((current_offset == FLASH_BLOCK_32K_SIZE) && ((current_addr + FLASH_BLOCK_32K_SIZE) <= max_addr)) {
        *max_step = FLASH_BLOCK_32K_SIZE;
        *cmd_id = FLASH_BE32K_CMD;
    } else {
        if ((current_addr + FLASH_BLOCK_64K_SIZE) <= max_addr) {
            *max_step = FLASH_BLOCK_64K_SIZE;
            *cmd_id = FLASH_BE_CMD;
        } else if ((current_addr + FLASH_BLOCK_32K_SIZE) <= max_addr) {
            *max_step = FLASH_BLOCK_32K_SIZE;
            *cmd_id = FLASH_BE32K_CMD;
        } else {
            *max_step = FLASH_PAGE_SIZE;
            *cmd_id = FLASH_SE_CMD;
        }
    }
}

static errcode_t check_erase_param(flash_id_t id, uint32_t addr, const flash_qspi_xip_config_t *mode_config)
{
    if (mode_config == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }
    if (addr > g_flash_device_parameter[g_flash_config[id].flash_manufacturer].flash_size) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_flash_block_erase(flash_id_t id, uint32_t addr, uint32_t length, bool is_wait)
{
    uint32_t current_addr = addr;
    uint32_t end_addr = addr + length;
    uint32_t max_step;
    uint32_t cmd_id;
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    if (check_erase_param(id, addr, mode_config) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);

    while (current_addr < end_addr) {
        /* Get max erase step and erase cmd. */
        flash_get_max_erase_step(current_addr, end_addr, &max_step, &cmd_id);
        if (flash_write_enable(id) != ERRCODE_SUCC) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return ERRCODE_FLASH_SPI_TRANS_FAIL;
        }
        flash_spi_write_config(id);

        if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }

        if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD && mode_config->enter_xip_before_trans_type !=
            HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr);
        }

        /* Send erase cmd. */
        if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
            if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
                hal_spi_dr_set_dr(g_flash_config[id].bus, cmd_id);
                hal_spi_dr_set_dr(g_flash_config[id].bus, current_addr);
            } else {
                hal_spi_dr_set_dr(g_flash_config[id].bus, (cmd_id << FLASH_CMD_SHIFT_BIT) | current_addr);
            }
        } else {
            hal_spi_dr_set_dr(g_flash_config[id].bus, (cmd_id << FLASH_CMD_SHIFT_BIT) | current_addr);
        }

        trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

        if (is_wait) {
            while (uapi_flash_is_processing(id)) { }
        }
        current_addr += max_step;
    }

    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_sector_erase(flash_id_t id, uint32_t addr, bool is_wait)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    if (check_erase_param(id, addr, mode_config) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    uint32_t irqs = flash_porting_spi_lock(id);
    uint32_t xip_enabled = flash_get_xip_enabled(id);
    flash_exit_xip_mode(id, xip_enabled);

    if (flash_write_enable(id) != ERRCODE_SUCC) {
        flash_enter_xip_mode(id, xip_enabled);
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    flash_spi_write_config(id);

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_enter_xip_mode(id, xip_enabled);
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD && mode_config->enter_xip_before_trans_type !=
        HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            flash_enter_xip_mode(id, xip_enabled);
            flash_porting_spi_unlock(id, irqs);
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    /* Send erase cmd. */
    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
            hal_spi_dr_set_dr(g_flash_config[id].bus, FLASH_SE_CMD);
            hal_spi_dr_set_dr(g_flash_config[id].bus, addr);
        } else {
            hal_spi_dr_set_dr(g_flash_config[id].bus, (FLASH_SE_CMD << FLASH_CMD_SHIFT_BIT | addr));
        }
    } else {
        hal_spi_dr_set_dr(g_flash_config[id].bus, (FLASH_SE_CMD << FLASH_CMD_SHIFT_BIT | addr));
    }

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

    if (is_wait) {
        while (uapi_flash_is_processing(id)) {
        }
    }
    flash_enter_xip_mode(id, xip_enabled);
    flash_porting_spi_unlock(id, irqs);
    return ERRCODE_SUCC;
}

errcode_t uapi_flash_reset(flash_id_t id)
{
    spi_xfer_data_t xfer_data = { 0 };
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    attr->ndf = 0;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
    extra_attr->qspi_param.trans_type = HAL_SPI_TRANS_TYPE_INST_Q_ADDR_Q;
    extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
    extra_attr->qspi_param.wait_cycles = 0;

    uint32_t irqs = flash_porting_spi_lock(id);
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    xfer_data.cmd = FLASH_RSTEN_CMD;

    /* Send reset enable cmd. */
    if (uapi_spi_master_write(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

    xfer_data.cmd = FLASH_RST_CMD;

    /* Send reset cmd. */
    if (uapi_spi_master_write(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

static errcode_t flash_excute_cmd(flash_id_t id, uint8_t cmd)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    uint32_t irqs = flash_porting_spi_lock(id);
    if (flash_write_enable(id) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    flash_spi_write_config(id);

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (g_flash_config[id].mode == HAL_SPI_FRAME_FORMAT_QUAD && mode_config->enter_xip_before_trans_type !=
        HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            flash_porting_spi_unlock(id, irqs);
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    hal_spi_dr_set_dr(g_flash_config[id].bus, cmd);

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_suspend(flash_id_t id)
{
    if (id >= FLASH_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    return flash_excute_cmd(id, g_flash_device_parameter[g_flash_config[id].flash_manufacturer].suspend_cmd);
}

errcode_t uapi_flash_resume(flash_id_t id)
{
    if (id >= FLASH_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    return flash_excute_cmd(id, g_flash_device_parameter[g_flash_config[id].flash_manufacturer].resume_cmd);
}

static errcode_t flash_switch_deeppower(flash_id_t id, bool en)
{
    spi_xfer_data_t xfer_data = { 0 };
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    attr->ndf = 0;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
    extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
    extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
    extra_attr->qspi_param.wait_cycles = 0;
    xfer_data.cmd = en ? FLASH_DP_CMD : FLASH_RDB_CMD;

    uint32_t irqs = flash_porting_spi_lock(id);
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (uapi_spi_master_write(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_switch_to_deeppower(flash_id_t id)
{
    if (id >= FLASH_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    return flash_switch_deeppower(id, true);
}

errcode_t uapi_flash_resume_from_deeppower(flash_id_t id)
{
    if (id >= FLASH_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    return flash_switch_deeppower(id, false);
}

errcode_t uapi_flash_read_security_status(flash_id_t id, uint8_t *read_data)
{
    spi_xfer_data_t xfer_data = { 0 };
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    if (mode_config == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    uint8_t cmd = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].security_reg_cmd;
    attr->ndf = FLASH_BUFF_BYTES;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
        extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
        extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
        extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
        extra_attr->qspi_param.wait_cycles = 0;
        xfer_data.cmd = cmd;
    } else {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
        xfer_data.tx_buff = &cmd;
        xfer_data.tx_bytes = FLASH_BUFF_BYTES;
    }
    xfer_data.rx_buff = read_data;
    xfer_data.rx_bytes = FLASH_BUFF_BYTES;

    uint32_t irqs = flash_porting_spi_lock(id);
    /* spi config */
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (mode_config->enter_xip_before_trans_type != HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            flash_porting_spi_unlock(id, irqs);
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    /* save read data */
    if (uapi_spi_master_writeread(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    flash_porting_spi_unlock(id, irqs);
    return ERRCODE_SUCC;
}

static errcode_t flash_send_cmd_func(flash_id_t id, hal_spi_frame_format_t mode, uint8_t len, uint8_t *cmd)
{
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    uint8_t cmd_len = len;
    uint8_t *send_cmd = cmd;
    attr->ndf = 0;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_TX;
    if (mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
        extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
        extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
        extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
        extra_attr->qspi_param.wait_cycles = 0;
    } else {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    }

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    hal_spi_ser_set_ser(g_flash_config[id].bus, 0);
    while (cmd_len > 0) {
        cmd_len--;
        if (send_cmd != NULL) {
            hal_spi_dr_set_dr(g_flash_config[id].bus, *send_cmd++);
        }
    }
    hal_spi_ser_set_ser(g_flash_config[id].bus, 1);

    trans_complete_wait_timeout(g_flash_config[id].bus, CONFIG_SPI_WAIT_FIFO_LONG_TIMEOUT);

    return ERRCODE_SUCC;
}

static errcode_t flash_status_check_func(flash_id_t id, hal_spi_frame_format_t mode, uint8_t cmd,
                                         uint8_t offset, uint8_t value)
{
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    uint8_t status = 0;
    uint32_t timeout = CONFIG_SPI_WAIT_READ_TIMEOUT;
    spi_xfer_data_t xfer_data = { 0 };
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    attr->ndf = FLASH_BUFF_BYTES;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    if (mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
        extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;
        extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_8;
        extra_attr->qspi_param.addr_len = HAL_SPI_ADDR_LEN_0;
        extra_attr->qspi_param.wait_cycles = 0;
        xfer_data.cmd = cmd;
    } else {
        xfer_data.tx_buff = &cmd;
        xfer_data.tx_bytes = FLASH_BUFF_BYTES;
    }
    xfer_data.rx_buff = &status;
    xfer_data.rx_bytes = FLASH_BUFF_BYTES;

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (mode == HAL_SPI_FRAME_FORMAT_QUAD) {
        if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_CONFIG_FAIL;
        }
    }

    do {
        /* Send status check cmd. */
        if (uapi_spi_master_writeread(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) !=
            ERRCODE_SUCC) {
            return ERRCODE_FLASH_SPI_TRANS_FAIL;
        }

        if ((--timeout) == 0) {
            flash_print("status check timeout\r\n");
            return ERRCODE_FLASH_TIMEOUT;
        }
    } while ((status & bit(offset)) != value);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_send_cmd_exe(flash_id_t id, flash_cmd_exe_t *cmd_exe)
{
    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    flash_cmd_exe_t *flash_cmd = cmd_exe;
    while (flash_cmd != NULL) {
        switch (flash_cmd->cmd_type) {
            case FLASH_CMD_TYPE_CMD:
                if (flash_send_cmd_func(id, (flash_cmd->spi_frf_mode), (flash_cmd->cmd_len),
                                        (flash_cmd->cmd)) != ERRCODE_SUCC) {
                    return ERRCODE_FLASH_SPI_TRANS_FAIL;
                }
                break;
            case FLASH_CMD_TYPE_PROCESSING:
                if (flash_status_check_func(id, (flash_cmd->spi_frf_mode), (flash_cmd->cmd)[FLASH_CMD_INDEX],
                                            (flash_cmd->cmd)[FLASH_CMD_OFFSET_INDEX],
                                            (flash_cmd->cmd)[FLASH_CMD_VALUE_INDEX]) != ERRCODE_SUCC) {
                    return ERRCODE_FLASH_SPI_TRANS_FAIL;
                }
                break;
            case FLASH_CMD_TYPE_CHECK:
                if (flash_status_check_func(id, (flash_cmd->spi_frf_mode), (flash_cmd->cmd)[FLASH_CMD_INDEX],
                                            (flash_cmd->cmd)[FLASH_CMD_OFFSET_INDEX],
                                            (flash_cmd->cmd)[FLASH_CMD_VALUE_INDEX]) == ERRCODE_SUCC) {
                    return ERRCODE_SUCC;
                }
                break;
            default:
                return ERRCODE_SUCC;
        }
        flash_cmd++;
    }
    return ERRCODE_SUCC;
}

errcode_t flash_save_manufacturer(flash_id_t id, uint32_t manufacture_id)
{
    for (uint8_t count = 0; count < FLASH_MANUFACTURER_MAX; count++) {
        if (manufacture_id == g_flash_device_parameter[count].manufacturer_id) {
            g_flash_config[id].flash_manufacturer = (flash_support_manufacturer_t)count;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FLASH_CONFIG_FAIL;
}

#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
static void flash_set_cmd_at_xip_mode(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);

    mode_config->enter_xip_before_wait_cycles_config = WAIT_CYCLES_6;
    mode_config->enter_xip_before_inst_l = HAL_SPI_INST_LEN_8;
    mode_config->enter_xip_before_addr_l = HAL_SPI_ADDR_LEN_24;
    mode_config->enter_xip_after_wait_cycles_config = WAIT_CYCLES_6;
    mode_config->enter_xip_after_inst_l = HAL_SPI_INST_LEN_8;
    mode_config->enter_xip_after_addr_l = HAL_SPI_ADDR_LEN_24;
    g_flash_config[id].need_cmd_at_xip_mode = true;
    return;
}

void uapi_flash_config_cmd_at_xip_mode(flash_id_t id, uint8_t flash_unique_id)
{
    uint32_t j;
    uint32_t k;

    if (g_flash_config[id].flash_manufacturer >= FLASH_MANUFACTURER_MAX) {
        return;
    }

    uint32_t manufacturer = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].manufacturer_id;
    for (j = 0; j < sizeof(g_winbond_flash_manufact_id) / sizeof(g_winbond_flash_manufact_id[0]); j++) {
        if (manufacturer != g_winbond_flash_manufact_id[j]) {
            continue;
        }
        g_flash_config[id].unique_id = flash_unique_id;
        for (k = 0; k < sizeof(g_flash_need_cmd_unique_id) / sizeof(g_flash_need_cmd_unique_id[0]); k++) {
            if (flash_unique_id == g_flash_need_cmd_unique_id[k]) {
                flash_set_cmd_at_xip_mode(id);
                return;
            }
        }
    }
    return;
}

static errcode_t flash_send_xip_cmd(flash_id_t id, flash_qspi_xip_config_t *mode_config)
{
    if (uapi_flash_send_cmd_exe(id, g_flash_device_parameter[g_flash_config[id].flash_manufacturer].
                                enter_xip_mode_cmd) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    spi_xfer_data_t xfer_data = { 0 };
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->frame_size = HAL_SPI_FRAME_SIZE_32;
    attr->ndf = FLASH_BUFF_BYTES;
    extra_attr->qspi_param.wait_cycles = mode_config->enter_xip_before_wait_cycles_config;
    extra_attr->qspi_param.inst_len = mode_config->enter_xip_before_inst_l;
    extra_attr->qspi_param.addr_len = mode_config->enter_xip_before_addr_l;
    extra_attr->qspi_param.trans_type = mode_config->enter_xip_before_trans_type;

    /* Send cmd switch to xip. */
    if (mode_config->enter_xip_before_inst_l == HAL_SPI_INST_LEN_8) {
        xfer_data.cmd = FLASH_QRD_CMD;
        xfer_data.addr = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].enter_xip;
    } else {
        xfer_data.addr = FLASH_QRD_CMD << SHIFT_WORD_15_8_BITS_TO_BYTE | FLASH_DUMMY;
        uint32_t enter_xip = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].enter_xip;
        xfer_data.tx_buff = (uint8_t *)&enter_xip;
        xfer_data.tx_bytes = FLASH_WORD_ALIGN;
    }

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }
    if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (uapi_spi_master_write(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    return ERRCODE_SUCC;
}

void uapi_flash_get_info(flash_id_t id, flash_info_t *flash_info)
{
    flash_info->flash_id = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].manufacturer_id;
    flash_info->flash_size = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].flash_size;
    flash_info->flash_unique_id = g_flash_config[id].unique_id;
    return;
}

static bool flash_xip_use_cmd_mode(flash_id_t id)
{
    flash_info_t flash_info;
    uapi_flash_get_info(id, &flash_info);
    uint32_t xip_use_cmd_num = (uint32_t)sizeof(g_xip_use_cmd_mode) / (uint32_t)sizeof(g_xip_use_cmd_mode[0]);
    for (uint32_t i = 0; i < xip_use_cmd_num; i++) {
        if (flash_info.flash_id == g_xip_use_cmd_mode[i]) {
            return true;
        }
    }
    if (g_flash_config[id].need_cmd_at_xip_mode) {
        return true;
    }
    return false;
}

static void flash_switch_xip_spi_config(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;
    /* Change buad when enter enter xip mode. */
    flash_porting_set_enter_xip_clk_div(id, &g_flash_config[id].attr);
    attr->ndf = FLASH_XIP_8_BYTE;
    extra_attr->qspi_param.wait_cycles = mode_config->enter_xip_after_wait_cycles_config;
    extra_attr->qspi_param.inst_len = mode_config->enter_xip_after_inst_l;
    extra_attr->qspi_param.addr_len = mode_config->enter_xip_after_addr_l;
    extra_attr->qspi_param.trans_type = mode_config->enter_xip_after_trans_type;

    /* Set dma cfg for xip data convert in hardware. */
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 0);
    hal_spi_dmacr_set_rdmae(g_flash_config[id].bus, 1);
    hal_spi_dmardlr_set_dmardl(g_flash_config[id].bus, 1);
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 1);
}

errcode_t uapi_flash_switch_to_xip_mode(flash_id_t id)
{
    if (!g_flash_config[id].is_xip) {
        return ERRCODE_SUCC;
    }
    uint32_t recv_data[1] = { 0 };
    if (hal_xip_is_enable((xip_id_t)id)) {
        return ERRCODE_SUCC;
    }
    if (hal_xip_get_cur_mode((xip_id_t)id) == XIP_MODE_DISABLE) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    if (mode_config == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }
    uint32_t irqs = flash_porting_spi_lock(id);
    if (flash_send_xip_cmd(id, mode_config) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    spi_xfer_data_t xfer_data = { 0 };
    xfer_data.rx_buff = (uint8_t *)recv_data;
    xfer_data.rx_bytes = FLASH_WORD_ALIGN;
    if (uapi_spi_master_read(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_print("switch to xip error\r\n");
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    /* Switch xip config. */
    flash_switch_xip_spi_config(id);

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }
    if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    /* Xip enable. */
    hal_xip_enable((xip_id_t)id, g_flash_device_parameter[g_flash_config[id].flash_manufacturer].enter_xip,
                   mode_config->enter_xip_after_enable_32bit_addr, mode_config->enter_xip_after_enable_wrap,
                   flash_xip_use_cmd_mode(id));
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

static void flash_exit_xip_spi_config(flash_id_t id)
{
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    spi_attr_t *attr = &g_flash_config[id].attr;
    spi_extra_attr_t *extra_attr = &g_flash_config[id].extra_attr;

    /* Change buad when enter exit xip mode. */
    flash_porting_set_exit_xip_clk_div(id, &g_flash_config[id].attr);
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
    attr->frame_size = HAL_SPI_FRAME_SIZE_32;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;
    attr->ndf = FLASH_BUFF_BYTES;
    extra_attr->qspi_param.wait_cycles = mode_config->enter_xip_after_wait_cycles_config;
    extra_attr->qspi_param.inst_len = HAL_SPI_INST_LEN_0;
    extra_attr->qspi_param.addr_len = mode_config->enter_xip_after_addr_l;
    extra_attr->qspi_param.trans_type = mode_config->enter_xip_after_trans_type;

    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 0);
    hal_spi_dmacr_set_rdmae(g_flash_config[id].bus, 0);
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 1);
}

errcode_t uapi_flash_exit_from_xip_mode(flash_id_t id)
{
    if (!g_flash_config[id].is_xip) {
        return ERRCODE_SUCC;
    }
    flash_porting_wait_exit_xip_mode();

    if (hal_xip_get_cur_mode((xip_id_t)id) == XIP_MODE_DISABLE) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }
    if (!hal_xip_is_enable((xip_id_t)id)) {
        return ERRCODE_SUCC;
    }
    uint32_t recv_data;
    spi_xfer_data_t xfer_data = { 0 };
    flash_qspi_xip_config_t *mode_config = flash_get_xip_config(id);
    if (mode_config == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }
    uint32_t irqs = flash_porting_spi_lock(id);

    /* Xip disable. */
    hal_xip_disable((xip_id_t)id);

    /* Exit xip config. */
    flash_exit_xip_spi_config(id);

    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }
    if (uapi_spi_set_extra_attr(g_flash_config[id].bus, &g_flash_config[id].extra_attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    xfer_data.addr = g_flash_device_parameter[g_flash_config[id].flash_manufacturer].exit_xip;
    xfer_data.rx_buff = (uint8_t *)(uintptr_t)&recv_data;
    xfer_data.rx_bytes = FLASH_WORD_ALIGN;

    if (uapi_spi_master_writeread(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    flash_porting_spi_unlock(id, irqs);
    return ERRCODE_SUCC;
}

errcode_t uapi_flash_switch_to_force_bypass_mode(flash_id_t id)
{
    hal_xip_set_cur_mode((xip_id_t)id, XIP_MODE_BYPASS);
    return uapi_flash_switch_to_xip_mode(id);
}

errcode_t uapi_flash_switch_to_cache_mode(flash_id_t id)
{
    hal_xip_set_cur_mode((xip_id_t)id, XIP_MODE_NORMAL);
    return uapi_flash_switch_to_xip_mode(id);
}

static bool flash_xip_mode_config_qspi(flash_id_t id)
{
    if (!g_flash_config[id].is_xip) {
        return false;
    }
    if (hal_xip_is_enable((xip_id_t)id)) {
        g_flash_config[id].mode = HAL_SPI_FRAME_FORMAT_QUAD;
        g_flash_config[id].qspi_isinit = 1;
        hal_xip_set_cur_mode((xip_id_t)id, XIP_MODE_NORMAL);
        return true;
    }

    return false;
}
#endif

static errcode_t flash_enter_to_qspi_mode(flash_id_t id)
{
    if (uapi_flash_send_cmd_exe(id, g_flash_device_parameter[g_flash_config[id].flash_manufacturer]
                                .enter_qspi_mode_cmd) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    g_flash_config[id].mode = HAL_SPI_FRAME_FORMAT_QUAD;
    return ERRCODE_SUCC;
}

static void flash_qspi_init(flash_id_t id)
{
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 0);
    hal_spi_rx_sample_dly_set_rsd(g_flash_config[id].bus, 1);
    hal_spi_ser_set_ser(g_flash_config[id].bus, 1);
    hal_spi_txftlr_set_tft(g_flash_config[id].bus, CONFIG_SPI_TX_FIFO_THRESHOLD);
    hal_spi_rxftlr_set_rft(g_flash_config[id].bus, CONFIG_SPI_RX_FIFO_THRESHOLD);
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 1);
}

static errcode_t flash_config_qspi(flash_id_t id)
{
#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
    if (g_flash_config[id].is_xip) {
        hal_xip_disable((xip_id_t)id);
    }
#endif

    /* Change buad when enter qspi mode. */
    flash_porting_set_enter_qspi_clk_div(id, &g_flash_config[id].attr);
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    flash_qspi_init(id);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_switch_to_qspi_init(flash_id_t id)
{
    if (g_flash_config[id].qspi_isinit == 1) {
        return ERRCODE_SUCC;
    }
    uint32_t irqs = flash_porting_spi_lock(id);

#if defined(CONFIG_FLASH_SUPPORT_XIP) && (CONFIG_FLASH_SUPPORT_XIP == 1)
    /* Xip is enable means flash is in qspi mode. */
    if (flash_xip_mode_config_qspi(id)) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_SUCC;
    }
#endif

    if (flash_config_qspi(id) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (flash_enter_to_qspi_mode(id) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    g_flash_config[id].qspi_isinit = 1;
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

static errcode_t uapi_flash_send_cmd(flash_id_t id, const uint8_t *send_cmd, uint32_t cmd_length,
                                     uint8_t *receive_buff, uint32_t buff_length)
{
    spi_xfer_data_t xfer_data = { 0 };
    spi_attr_t *attr = &g_flash_config[id].attr;

    attr->ndf = buff_length;
    attr->spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    attr->frame_size = HAL_SPI_FRAME_SIZE_8;
    attr->tmod = HAL_SPI_TRANS_MODE_EEPROM;

    xfer_data.tx_buff = (uint8_t *)send_cmd;
    xfer_data.tx_bytes = cmd_length;
    xfer_data.rx_buff = receive_buff;
    xfer_data.rx_bytes = buff_length;

    uint32_t irqs = flash_porting_spi_lock(id);
    if (uapi_spi_set_attr(g_flash_config[id].bus, &g_flash_config[id].attr) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_CONFIG_FAIL;
    }

    if (uapi_spi_master_writeread(g_flash_config[id].bus, &xfer_data, CONFIG_SPI_TRAN_MAX_TIMEOUT) != ERRCODE_SUCC) {
        flash_porting_spi_unlock(id, irqs);
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }
    flash_porting_spi_unlock(id, irqs);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_read_id(flash_id_t id, uint32_t *manufacture_id)
{
    uint8_t read_id_cmd = FLASH_RDID_CMD;
    uint8_t readid[MANUFACTURE_ID_LENGTH] = { 0 };
    if (manufacture_id == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (uapi_flash_send_cmd(id, &read_id_cmd, sizeof(read_id_cmd), readid, MANUFACTURE_ID_LENGTH) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    *manufacture_id = (uint32_t)(readid[0]) + (uint32_t)(readid[1] << SHIFT_WORD_15_8_BITS_TO_BYTE) +
                     (uint32_t)(readid[2] << SHIFT_WORD_23_16_BITS_TO_BYTE);  /* 2  manufacture_id_23_16. */

    if (flash_save_manufacturer(id, *manufacture_id) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_read_device_id(flash_id_t id, uint16_t *device_id)
{
    uint8_t device_cmd[] = {FLASH_MSID_CMD, FLASH_DUMMY, FLASH_DUMMY, FLASH_DUMMY};
    uint8_t flash_device_id[DEVICE_ID_LENGTH] = { 0 };

    if (device_id == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (uapi_flash_send_cmd(id, device_cmd, sizeof(device_cmd), flash_device_id, DEVICE_ID_LENGTH) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    (*device_id) = (uint16_t)(flash_device_id[0]) + (uint16_t)(flash_device_id[1] << SHIFT_WORD_15_8_BITS_TO_BYTE);

    return ERRCODE_SUCC;
}

errcode_t uapi_flash_read_unique_id(flash_id_t id, uint8_t *unique_id)
{
    uint8_t unique_cmd[] = { FLASH_UQID_CMD, FLASH_DUMMY, FLASH_DUMMY, FLASH_DUMMY, FLASH_DUMMY};
    uint8_t flash_unique_id = 0;

    if (unique_id == NULL) {
        return ERRCODE_FLASH_CONFIG_FAIL;
    }

    if (uapi_flash_send_cmd(id, unique_cmd, sizeof(unique_cmd), &flash_unique_id, FLASH_BUFF_BYTES) != ERRCODE_SUCC) {
        return ERRCODE_FLASH_SPI_TRANS_FAIL;
    }

    (*unique_id) = flash_unique_id;

    return ERRCODE_SUCC;
}

void uapi_flash_set_spi_baud(flash_id_t id, uint32_t bus_clk, uint32_t freq_mhz)
{
    if (freq_mhz == 0) {
        return;
    }
    spi_attr_t *attr = &g_flash_config[id].attr;
    attr->bus_clk = bus_clk;
    attr->freq_mhz = freq_mhz;
    uint32_t clk_div;
    clk_div = (uint32_t)(bus_clk / freq_mhz / SPI_MHZ_TO_HZ);
    if (clk_div < SPI_MINUMUM_CLK_DIV) {
        clk_div = SPI_MINUMUM_CLK_DIV;
    }

    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 0);
    hal_spi_baudr_set_sckdv(g_flash_config[id].bus, clk_div);
    hal_spi_ssienr_set_ssi_en(g_flash_config[id].bus, 1);
}
