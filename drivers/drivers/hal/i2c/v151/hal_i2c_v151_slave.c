/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides V151 HAL i2c slave api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15, Create file. \n
 */

#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "i2c_porting.h"
#include "hal_i2c.h"
#include "hal_i2c_v151_comm.h"
#include "hal_i2c_v151_slave.h"

static errcode_t hal_i2c_v151_slave_write_prepare(i2c_bus_t bus, uintptr_t param)
{
    hal_i2c_prepare_config_t *cfg = (hal_i2c_prepare_config_t *)param;

    hal_i2c_v151_get_ctrl_info(bus)->write_operate_type = cfg->operation_type;
    if (cfg->operation_type == I2C_DATA_OPERATION_TYPE_INT) {
        hal_i2c_v151_unmask_int(bus, I2C_V151_INT_RD_REQ);
    } else if (cfg->operation_type == I2C_DATA_OPERATION_TYPE_DMA) {
        hal_i2c_v151_set_transmit_dma_en(bus, true);
        hal_i2c_v151_set_transmit_dma_level(bus, HAL_I2C_V151_TX_DMA_TL);
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v151_slave_write_restore(i2c_bus_t bus, uintptr_t param)
{
    unused(param);
    hal_i2c_v151_com_clr_int(bus);
    hal_i2c_v151_com_restore_int(bus);
    hal_i2c_v151_restore_int_tx(bus);
    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v151_slave_read_prepare(i2c_bus_t bus, uintptr_t param)
{
    hal_i2c_prepare_config_t *cfg = (hal_i2c_prepare_config_t *)param;

    hal_i2c_v151_get_ctrl_info(bus)->read_operate_type = cfg->operation_type;
    if (cfg->operation_type == I2C_DATA_OPERATION_TYPE_INT) {
        hal_i2c_v151_prepare_int_rx(bus);
    } else if (cfg->operation_type == I2C_DATA_OPERATION_TYPE_DMA) {
        hal_i2c_v151_set_receive_dma_level(bus, HAL_I2C_V151_RX_DMA_TL - 1);
        hal_i2c_v151_set_receive_dma_en(bus, true);
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v151_slave_read_restore(i2c_bus_t bus, uintptr_t param)
{
    unused(param);
    hal_i2c_v151_com_clr_int(bus);
    hal_i2c_v151_com_restore_int(bus);
    hal_i2c_v151_restore_int_rx(bus);
    hal_i2c_v151_set_receive_dma_en(bus, false);
    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v151_slave_check_tx_available(i2c_bus_t bus, uintptr_t param)
{
    *(uint32_t *)param = hal_i2c_v151_get_int(bus, I2C_V151_RAW_INTR_STAT, I2C_V151_INT_RD_REQ);
    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v151_slave_flush_rx_fifo(i2c_bus_t bus, uintptr_t param)
{
    hal_i2c_buffer_wrap_t *read_data = (hal_i2c_buffer_wrap_t *)param;
    if ((read_data == NULL) || read_data->len == 0) {
        return ERRCODE_I2C_RECEIVE_PARAM_INVALID;
    }

    uint32_t len = read_data->len;
    hal_i2c_v151_update_rx_tl(bus, len);

    return ERRCODE_SUCC;
}

static errcode_t hal_i2c_v151_slave_check_rx_transmit_end(i2c_bus_t bus, uintptr_t param)
{
    uint32_t *transmit_end = (uint32_t *)param;

    uint32_t stop_flag = hal_i2c_v151_get_int(bus, I2C_V151_RAW_INTR_STAT, I2C_V151_INT_STOP_DET);
    uint32_t done_flag = hal_i2c_v151_get_int(bus, I2C_V151_RAW_INTR_STAT, I2C_V151_INT_RX_DONE);

    *transmit_end = stop_flag | done_flag;

    return ERRCODE_SUCC;
}

static hal_i2c_inner_ctrl_t g_hal_i2c_slave_ctrl_func_array[I2C_CTRL_MAX] = {
    hal_i2c_v151_slave_write_prepare,                 /* I2C_CTRL_WRITE_PREPARE */
    hal_i2c_v151_slave_write_restore,                 /* I2C_CTRL_WRITE_RESTORE */
    hal_i2c_v151_slave_read_prepare,                  /* I2C_CTRL_READ_PREPARE */
    hal_i2c_v151_slave_read_restore,                  /* I2C_CTRL_READ_RESTORE */
    hal_i2c_v151_get_write_num,                       /* I2C_CTRL_GET_WRITE_NUM */
    hal_i2c_v151_get_read_num,                        /* I2C_CTRL_GET_READ_NUM */
    hal_i2c_v151_slave_check_tx_available,            /* I2C_CTRL_CHECK_TX_AVAILABLE */
    hal_i2c_v151_check_rx_available,                  /* I2C_CTRL_CHECK_RX_AVAILABLE */
    hal_i2c_v151_slave_flush_rx_fifo,                 /* I2C_CTRL_FLUSH_RX_FIFO */
    hal_i2c_v151_check_tx_transmit_end,               /* I2C_CTRL_CHECK_TX_PROCESS_DONE */
    hal_i2c_v151_slave_check_rx_transmit_end,         /* I2C_CTRL_CHECK_RX_PROCESS_DONE */
    NULL,                                               /* I2C_CTRL_CHECK_RESTART_READY */
    hal_i2c_v151_check_transmit_abrt,                 /* I2C_CTRL_CHECK_TRANSMIT_ABRT */
    hal_i2c_v151_get_data_addr,                       /* I2C_CTRL_GET_DMA_DATA_ADDR */
    hal_i2c_v151_check_tx_fifo_empty,                 /* I2C_CTRL_CHECK_TX_FIFO_EMPTY */
};

errcode_t hal_i2c_v151_slave_init(i2c_bus_t bus, uint32_t baudrate,
                                  uint16_t addr, hal_i2c_callback_t callback)
{
    unused(baudrate);
    hal_i2c_v151_regs_init();

    hal_i2c_v151_addr_cfg_attr_t cfg_attr;
    bool cfg_valid = hal_i2c_v151_get_address_cfg(addr, I2C_V151_ADDR_PERMISION_MASTER_SLAVE, &cfg_attr);
    if (cfg_valid == false) {
        return ERRCODE_I2C_ADDRESS_INVLID;
    }

    hal_i2c_v151_deinit(bus);

    hal_i2c_v151_init_comp_param(bus);
    hal_i2c_v151_set_slave_address_mode(bus, cfg_attr.addr_width);
    hal_i2c_v151_set_slave_address(bus, cfg_attr.real_addr);

    hal_i2c_v151_set_con_slave_mode(bus);
    hal_i2c_v151_set_con_stop_det_ifaddressed(bus, IC_V151_CON_DISABLED_BIT);
    hal_i2c_v151_set_con_rx_full_hold(bus, IC_V151_CON_ENABLED_BIT);

    hal_i2c_v151_mask_all_int(bus);
    hal_i2c_v151_com_clr_int(bus);
    hal_i2c_v151_set_ic_enable(bus);

    hal_i2c_v151_register_callback(callback);

    hal_i2c_v151_load_ctrl_func(bus, g_hal_i2c_slave_ctrl_func_array);
    return ERRCODE_SUCC;
}
