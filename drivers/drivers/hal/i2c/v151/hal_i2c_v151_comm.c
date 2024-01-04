/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL i2c \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15, Create file. \n
 */

#include "common_def.h"
#include "tcxo.h"
#include "hal_i2c.h"
#include "hal_i2c_v151_regs_op.h"
#include "hal_i2c_v151_comm.h"

#define HAL_I2C_VALID_ADDR_VALUE_MASK    0x3FF  /* Address bits mask for VALID address */

static hal_i2c_inner_ctrl_t *g_hal_i2c_ctrl_func[I2C_BUS_MAX_NUM];
static hal_i2c_v151_ctrl_info_t g_hal_i2c_ctrl_info[I2C_BUS_MAX_NUM];

hal_i2c_callback_t g_hal_i2c_v151_callback;

static hal_i2c_v151_addr_attr_t *hal_i2c_v151_get_address_attr(uint16_t addr)
{
#ifdef CONFIG_I2C_ADDR_VALID_CHECK
    /* Addr permission default table, if needed, please customize for system */
    static hal_i2c_v151_addr_region_attr_t i2c_v151_valid_addr[] = {
        {{0x0, 0x0}, {I2C_V151_ADDR_TYPE_FOR_GC, I2C_V151_7_BITS_ADDR, I2C_V151_ADDR_PERMISION_SLAVE, 0}},
        {{0x1, 0x1}, {I2C_V151_ADDR_TYPE_FOR_CBUS, I2C_V151_7_BITS_ADDR, I2C_V151_ADDR_PERMISION_INVALID, 0}},
        {{0x2, 0x3}, {I2C_V151_ADDR_TYPE_FOR_RESERVED, I2C_V151_7_BITS_ADDR, I2C_V151_ADDR_PERMISION_INVALID, 0}},
        {{0x4, 0x7}, {I2C_V151_ADDR_TYPE_FOR_HS_MODE, I2C_V151_7_BITS_ADDR, I2C_V151_ADDR_PERMISION_INVALID, 0}},
        {{0x8, 0x77},
         {I2C_V151_ADDR_TYPE_FOR_I2C_TARGET, I2C_V151_7_BITS_ADDR, I2C_V151_ADDR_PERMISION_MASTER_SLAVE, 0}},
        {{0x78, 0x77FF},
         {I2C_V151_ADDR_TYPE_INVALID, I2C_V151_INVALID_BITS_ADDR, I2C_V151_ADDR_PERMISION_INVALID, 0}},
        {{0x7800, 0x7BFF},
         {I2C_V151_ADDR_TYPE_FOR_I2C_TARGET, I2C_V151_10_BITS_ADDR, I2C_V151_ADDR_PERMISION_MASTER_SLAVE, 0}},
        {{0x7C00, 0x7FFF},
         {I2C_V151_ADDR_TYPE_FOR_RESERVED, I2C_V151_10_BITS_ADDR, I2C_V151_ADDR_PERMISION_INVALID, 0}},
    };
#else
    /* Addr permission default table, just check max addr. */
    static hal_i2c_v151_addr_region_attr_t i2c_v151_valid_addr[] = {
        {{0x0, 0x7F},
         {I2C_V151_ADDR_TYPE_FOR_I2C_TARGET, I2C_V151_7_BITS_ADDR, I2C_V151_ADDR_PERMISION_MASTER_SLAVE, 0}},
        {{0x80, 0x7FFF},
         {I2C_V151_ADDR_TYPE_FOR_I2C_TARGET, I2C_V151_10_BITS_ADDR, I2C_V151_ADDR_PERMISION_MASTER_SLAVE, 0}},
    };
#endif

    for (uint32_t i = 0; i < sizeof(i2c_v151_valid_addr) / sizeof(i2c_v151_valid_addr[0]); i++) {
        hal_i2c_v151_addr_region_t *region = &i2c_v151_valid_addr[i].region;
        if ((addr >= region->min_addr) && (addr <= region->max_addr)) {
            return &i2c_v151_valid_addr[i].attr;
        }
    }
    return NULL;
}

bool hal_i2c_v151_get_address_cfg(uint16_t addr, hal_i2c_v151_addr_permission_t permission,
                                  hal_i2c_v151_addr_cfg_attr_t *cfg_attr)
{
    hal_i2c_v151_addr_attr_t *attr = hal_i2c_v151_get_address_attr(addr);

    if (attr == NULL) {
        return false;
    }

    if (((uint8_t)permission & attr->permission) != (uint8_t)permission) {
        return false;
    }

    cfg_attr->real_addr = addr & HAL_I2C_VALID_ADDR_VALUE_MASK;
    cfg_attr->addr_type = attr->type;
    cfg_attr->addr_width = attr->width;

    return true;
}

void hal_i2c_v151_com_clr_int(i2c_bus_t bus)
{
    (void)hal_i2c_v151_clr_intr_get_clr_tx_abrt(bus);
    hal_i2c_v151_clr_all_int(bus);
}

void hal_i2c_v151_com_prepare_int(i2c_bus_t bus)
{
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_START_DET);
#endif
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_STOP_DET);
}

void hal_i2c_v151_com_restore_int(i2c_bus_t bus)
{
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_START_DET);
#endif
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_STOP_DET);
}

void hal_i2c_v151_prepare_int_tx(i2c_bus_t bus)
{
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_TX_OVER);
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_RD_REQ);
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_TX_ABRT);
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_TX_EMPTY);
}

void hal_i2c_v151_restore_int_tx(i2c_bus_t bus)
{
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_TX_EMPTY);
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_TX_OVER);
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_RD_REQ);
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_TX_ABRT);
}

void hal_i2c_v151_prepare_int_rx(i2c_bus_t bus)
{
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_RX_OVER);
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_RX_FULL);
    hal_i2c_v151_unmask_int(bus, I2C_V151_INT_RX_DONE);
}

void hal_i2c_v151_restore_int_rx(i2c_bus_t bus)
{
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_RX_OVER);
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_RX_FULL);
    hal_i2c_v151_mask_int(bus, I2C_V151_INT_RX_DONE);
}

static uint32_t hal_i2c_v151_get_tx_fifo_remain(i2c_bus_t bus)
{
    hal_i2c_v151_ctrl_info_t *i2c_ctrl = &g_hal_i2c_ctrl_info[bus];
    if (i2c_ctrl->tx_fifo_max_deepth > i2c_ctrl->tx_tl) {
        return i2c_ctrl->tx_fifo_max_deepth - i2c_ctrl->tx_tl;
    }
    return 0;
}

static uint32_t hal_i2c_v151_get_default_rx_tl(i2c_bus_t bus)
{
    hal_i2c_v151_ctrl_info_t *i2c_ctrl = &g_hal_i2c_ctrl_info[bus];
    uint32_t rx_burst_num = i2c_ctrl->rx_fifo_max_deepth >> 1;
    return rx_burst_num > 1 ? rx_burst_num : 1;
}

void hal_i2c_v151_update_rx_tl(i2c_bus_t bus, uint32_t rx_tl)
{
    uint32_t real_rx_tl = rx_tl > 1 ? rx_tl : 1;
    hal_i2c_v151_set_receive_threshold_level(bus, real_rx_tl - 1);
}

errcode_t hal_i2c_v151_check_transmit_abrt(i2c_bus_t bus, uintptr_t param)
{
    uint32_t *abrt_status = (uint32_t *)param;

    *abrt_status = hal_i2c_v151_get_int(bus, I2C_V151_RAW_INTR_STAT, I2C_V151_INT_TX_ABRT);

    return ERRCODE_SUCC;
}

errcode_t hal_i2c_v151_get_write_num(i2c_bus_t bus, uintptr_t param)
{
    *(uint32_t *)param = hal_i2c_v151_get_tx_fifo_remain(bus);
    return ERRCODE_SUCC;
}

errcode_t hal_i2c_v151_get_read_num(i2c_bus_t bus, uintptr_t param)
{
    *(uint32_t *)param = hal_i2c_v151_get_default_rx_tl(bus);
    return ERRCODE_SUCC;
}

errcode_t hal_i2c_v151_check_rx_available(i2c_bus_t bus, uintptr_t param)
{
    unused(bus);
    *(uint32_t *)param = true;
    return ERRCODE_SUCC;
}

errcode_t hal_i2c_v151_check_tx_transmit_end(i2c_bus_t bus, uintptr_t param)
{
    *(uint32_t *)param = hal_i2c_v151_get_int(bus, I2C_V151_RAW_INTR_STAT, I2C_V151_INT_STOP_DET);
    return ERRCODE_SUCC;
}

static uint32_t hal_i2c_v151_tx_not_full(i2c_bus_t bus)
{
    ic_v151_status_data_t status;
    status.d32 = hal_i2c_v151_get_status(bus);
    return status.b.tfnf;
}

void hal_i2c_v151_register_callback(hal_i2c_callback_t callback)
{
    g_hal_i2c_v151_callback = callback;
}

void hal_i2c_v151_load_ctrl_func(i2c_bus_t bus, hal_i2c_inner_ctrl_t *func_table)
{
    g_hal_i2c_ctrl_func[bus] = func_table;
}

void hal_i2c_v151_unload_ctrl_func(i2c_bus_t bus)
{
    g_hal_i2c_ctrl_func[bus] = NULL;
}

hal_i2c_v151_ctrl_info_t *hal_i2c_v151_get_ctrl_info(i2c_bus_t bus)
{
    return &g_hal_i2c_ctrl_info[bus];
}

errcode_t hal_i2c_v151_write(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data)
{
    if ((data == NULL) || (data->buffer == NULL) || data->len == 0) {
        return ERRCODE_I2C_SEND_PARAM_INVALID;
    }

    uint64_t start_time = uapi_tcxo_get_ms();
    uint32_t loop = 0;
    uint8_t *buffer = data->buffer;
    uint32_t len = data->len;

    do {
        if ((uapi_tcxo_get_ms() - start_time) > CONFIG_I2C_WAIT_CONDITION_TIMEOUT) {
            return ERRCODE_I2C_TIMEOUT;
        }
        if (hal_i2c_v151_tx_not_full(bus) != 0) {
            uint32_t first_flag = (loop == 0);
            uint32_t last_flag = ((loop + 1) == len);
            uint32_t restart_flag = data->restart_flag;
            uint32_t stop_flag = data->stop_flag;

            uint32_t write_cmd = hal_i2c_v151_get_data_write_cmd(first_flag, restart_flag, last_flag, stop_flag);
            hal_i2c_v151_set_transmit_data(bus, write_cmd, buffer[loop]);
            loop++;
        }
    } while (loop < data->len);
    return ERRCODE_SUCC;
}

static bool hal_i2c_v151_rx_not_empty(i2c_bus_t bus)
{
    ic_v151_status_data_t status;
    status.d32 = hal_i2c_v151_get_status(bus);
    return (bool)status.b.rfne;
}

errcode_t hal_i2c_v151_read(i2c_bus_t bus, hal_i2c_buffer_wrap_t *data)
{
    if ((data == NULL) || (data->buffer == NULL) || data->len == 0) {
        return ERRCODE_I2C_RECEIVE_PARAM_INVALID;
    }

    uint64_t start_time = uapi_tcxo_get_ms();
    uint32_t loop = 0;

    do {
        if ((uapi_tcxo_get_ms() - start_time) > CONFIG_I2C_WAIT_CONDITION_TIMEOUT) {
            return ERRCODE_I2C_TIMEOUT;
        }
        if (hal_i2c_v151_rx_not_empty(bus)) {
            data->buffer[loop] = hal_i2c_v151_get_receive_data(bus);
            loop++;
        }
    } while (loop < data->len);
    return ERRCODE_SUCC;
}

errcode_t hal_i2c_v151_deinit(i2c_bus_t bus)
{
    hal_i2c_v151_set_ic_disable(bus);
    hal_i2c_v151_mask_all_int(bus);
    hal_i2c_v151_com_clr_int(bus);
    hal_i2c_v151_unload_ctrl_func(bus);
    return ERRCODE_SUCC;
}

void hal_i2c_v151_init_comp_param(i2c_bus_t bus)
{
    hal_i2c_v151_ctrl_info_t *hal_i2c_ctrl_info = &g_hal_i2c_ctrl_info[bus];

    hal_i2c_ctrl_info->tx_fifo_max_deepth = CONFIG_I2S_TX_BUFFER_DEPTH;
    hal_i2c_ctrl_info->rx_fifo_max_deepth = CONFIG_I2S_RX_BUFFER_DEPTH;

    hal_i2c_ctrl_info->tx_tl = hal_i2c_ctrl_info->tx_fifo_max_deepth >> 1;
    hal_i2c_ctrl_info->rx_tl = (hal_i2c_ctrl_info->rx_fifo_max_deepth >> 1) - 1;

    hal_i2c_v151_set_transmit_threshold_level(bus, hal_i2c_ctrl_info->tx_tl);
    hal_i2c_v151_set_receive_threshold_level(bus, hal_i2c_ctrl_info->rx_tl);

#if defined(CONFIG_I2S_HAS_DMA)
        hal_i2c_ctrl_info->tx_dma_valid = true;
        /* The dma_tx_req signal is generated when the number of valid data entries in the transmit FIFO */
        hal_i2c_ctrl_info->tx_dma_tl = HAL_I2C_V151_TX_DMA_TL;

        hal_i2c_ctrl_info->rx_dma_valid = true;
        /* Dma_rx_req is generated when the number of valid data entries in the receive FIFO */
        hal_i2c_ctrl_info->rx_dma_tl = HAL_I2C_V151_RX_DMA_TL;
#elif
        hal_i2c_ctrl_info->tx_dma_valid = false;
        hal_i2c_ctrl_info->rx_dma_valid = false;
#endif
}

errcode_t hal_i2c_v151_ctrl(i2c_bus_t bus, hal_i2c_ctrl_id_t id, uintptr_t param)
{
    if (g_hal_i2c_ctrl_func[bus] == NULL) {
        return ERRCODE_I2C_NOT_INIT;
    }
    if (g_hal_i2c_ctrl_func[bus][id] != NULL) {
        return g_hal_i2c_ctrl_func[bus][id](bus, param);
    }
    return ERRCODE_I2C_NOT_IMPLEMENT;
}

errcode_t hal_i2c_v151_get_data_addr(i2c_bus_t bus, uintptr_t param)
{
    uint32_t *addr = (uint32_t *)param;
    *addr = (uint32_t)(uintptr_t)(&g_i2c_v151_regs[bus]->data_cmd);
    return ERRCODE_SUCC;
}

static uint32_t hal_i2c_v151_get_tx_fifo_status(i2c_bus_t bus)
{
    ic_v151_status_data_t status;
    status.d32 = hal_i2c_v151_get_status(bus);
    return status.b.tfe;
}

static bool hal_i2c_v151_check_tx_fifo_timeout_by_count(uint32_t *trans_time, uint32_t timeout)
{
    (*trans_time)++;
    return ((*trans_time > timeout) ? true : false);
}

errcode_t hal_i2c_v151_check_tx_fifo_empty(i2c_bus_t bus, uintptr_t param)
{
    uint32_t timeout = (uint32_t)param;
    uint32_t trans_time = 0;
    while (hal_i2c_v151_get_tx_fifo_status(bus) != 1) {
        if (hal_i2c_v151_check_tx_fifo_timeout_by_count(&trans_time, timeout)) {
            return ERRCODE_SPI_TIMEOUT;
        }
    }
    return ERRCODE_SUCC;
}

void hal_i2c_v151_irq_handler(i2c_bus_t bus)
{
    if (!g_hal_i2c_v151_callback) {
        return;
    }

    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_STOP_DET) != 0) {
        hal_i2c_v151_set_normal_mode(bus);
        hal_i2c_v151_com_clr_int(bus);
        hal_i2c_v151_com_restore_int(bus);
        hal_i2c_v151_restore_int_tx(bus);
        hal_i2c_v151_set_clr_intr(bus, I2C_V151_INT_STOP_DET);
        g_hal_i2c_v151_callback(bus, I2C_EVT_TRANSMITION_DONE, 0);
    }
    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_RD_REQ)) {
        hal_i2c_v151_prepare_int_tx(bus);
        hal_i2c_v151_com_prepare_int(bus);
        hal_i2c_v151_clr_all_int(bus);
        hal_i2c_v151_mask_int(bus, I2C_V151_INT_RD_REQ);
        g_hal_i2c_v151_callback(bus, I2C_EVT_TX_AVAILABLE, 0);
    }
    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_TX_EMPTY) != 0) {
        g_hal_i2c_v151_callback(bus, I2C_EVT_TX_READY, 0);
        hal_i2c_v151_set_clr_intr(bus, I2C_V151_INT_TX_EMPTY);
    }
    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_RX_FULL) != 0) {
        g_hal_i2c_v151_callback(bus, I2C_EVT_RX_READY, 0);
        hal_i2c_v151_set_clr_intr(bus, I2C_V151_INT_RX_FULL);
    }
    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_RD_REQ) != 0) {
        g_hal_i2c_v151_callback(bus, I2C_EVT_TX_AVAILABLE, 0);
        hal_i2c_v151_set_clr_intr(bus, I2C_V151_INT_RD_REQ);
    }
    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_START_DET) != 0) {
        g_hal_i2c_v151_callback(bus, I2C_EVT_TRANSMITION_START, 0);
        hal_i2c_v151_set_clr_intr(bus, I2C_V151_INT_START_DET);
    }
    if (hal_i2c_v151_get_int(bus, I2C_V151_INTR_STAT, I2C_V151_INT_TX_ABRT) != 0) {
        g_hal_i2c_v151_callback(bus, I2C_EVT_TRANSMITION_ABRT, 0);
        hal_i2c_v151_set_clr_intr(bus, I2C_V151_INT_TX_ABRT);
    }
}