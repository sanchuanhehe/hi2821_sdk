/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  NON-OS QSPI DRIVER
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */
#include "soc_osal.h"
#include "chip_io.h"
#include "qspi.h"

static uint32_t g_ul_spi_recv_data_timeout = QSPI_WAIT_READING_TIMEOUT;

qspi_fsm_t g_qspi_curr_state[QSPI_BUS_NONE] = {0};

#define QSPI_DMA_TDLR_VALUE 8
#define QSPI_DMA_RDLR_VALUE 0

void qspi_enter_sleep(bool sleep)
{
    if (sleep) {
        hal_qspi_enter_sleep_mode();
    } else {
        hal_qspi_exit_sleep_mode();
    }
}

void qspi_set_state(qspi_bus_t id, qspi_state_t state)
{
    uint32_t irq_sts = osal_irq_lock();
    g_qspi_curr_state[id].curr_state = state;
    osal_irq_restore(irq_sts);
}

qspi_state_t qspi_get_state(qspi_bus_t id)
{
    return g_qspi_curr_state[id].curr_state;
}

bool qspi_claim(qspi_bus_t id)
{
    uint32_t irq_sts = osal_irq_lock();
    if (g_qspi_curr_state[id].is_busy) {
        osal_irq_restore(irq_sts);
        return false;
    }
    g_qspi_curr_state[id].is_busy = QSPI_IS_BUSY;
    osal_irq_restore(irq_sts);
    return true;
}

void qspi_release(qspi_bus_t id)
{
    uint32_t irq_sts = osal_irq_lock();
    g_qspi_curr_state[id].is_busy = QSPI_IS_IDLE;
    osal_irq_restore(irq_sts);
}

qspi_ret_t qspi_trans_complete_wait_timeout(qspi_bus_t id, uint32_t timeout)
{
    uint32_t ul_trans_timeout = 0;
    uint32_t ul_spi_state;
    uint32_t ul_spi_trans_fifo_num;

    do {
        ul_spi_trans_fifo_num = hal_qspi_get_tx_fifo_num(id);
        ul_spi_state = hal_qspi_get_state(id);
        ul_trans_timeout++;

        if (ul_trans_timeout >= timeout) {
            g_qspi_curr_state[id].stat.wait_trans_timeout_cnt++;
            return QSPI_RET_TIMEOUT;
        }

        ul_spi_state &= 1;
    } while (ul_spi_state != 0 || ul_spi_trans_fifo_num > 0);

    return QSPI_RET_OK;
}

qspi_ret_t qspi_send_data_by_word(qspi_bus_t id, uint32_t *buffer, uint32_t length)
{
    uint32_t ul_loop;

    if (buffer == NULL) {
        return QSPI_RET_ERROR;
    }

    uint32_t irq_sts = osal_irq_lock();
    for (ul_loop = 0; ul_loop < length; ul_loop++) {
        hal_qspi_write_data(id, buffer[ul_loop]);
    }
    osal_irq_restore(irq_sts);
    return QSPI_RET_OK;
}

qspi_ret_t qspi_recv_data_by_word(qspi_bus_t id, uint32_t *buffer, uint32_t length)
{
    uint32_t ul_loop;
    uint32_t ul_recv_timeout;
    uint32_t ul_recv_num;
    uint32_t irq_sts = osal_irq_lock();

    for (ul_loop = 0, ul_recv_timeout = 0; ul_loop < length;) {
        ul_recv_num = hal_qspi_get_rx_fifo_num(id);

        while (ul_recv_num > 0) {
            buffer[ul_loop] = hal_qspi_read_data(id);
            ul_loop++;
            ul_recv_num--;
            ul_recv_timeout = 0;
        }

        ul_recv_timeout++;

        if (ul_recv_timeout >= g_ul_spi_recv_data_timeout) {
            g_qspi_curr_state[id].stat.recc_timeout_cnt++;
            osal_irq_restore(irq_sts);
            return QSPI_RET_TIMEOUT;
        }
    }
    osal_irq_restore(irq_sts);

    return QSPI_RET_OK;
}

qspi_ret_t qspi_recv_data_by_byte(qspi_bus_t id, uint8_t *buffer, uint32_t length)
{
    if (buffer == NULL) {
        return QSPI_RET_ERROR;
    }
    uint32_t ul_loop;
    uint32_t ul_recv_timeout;
    uint32_t ul_recv_num;
    uint32_t irq_sts = osal_irq_lock();

    for (ul_loop = 0, ul_recv_timeout = 0; ul_loop < length;) {
        ul_recv_num = hal_qspi_get_rx_fifo_num(id);

        while (ul_recv_num > 0) {
            buffer[ul_loop] = (uint8_t)hal_qspi_read_data(id);
            ul_loop++;
            ul_recv_num--;
            ul_recv_timeout = 0;
        }

        ul_recv_timeout++;

        if (ul_recv_timeout >= g_ul_spi_recv_data_timeout) {
            g_qspi_curr_state[id].stat.recc_timeout_cnt++;
            osal_irq_restore(irq_sts);
            return QSPI_RET_TIMEOUT;
        }
    }
    osal_irq_restore(irq_sts);

    return QSPI_RET_OK;
}

void qspi_dma_control(qspi_bus_t id, qspi_dma_control_t control)
{
    hal_qspi_dma_control(id, (hal_qspi_dma_control_t)control);
}

void qspi_dma_set_data_level(qspi_bus_t id)
{
    hal_qspi_dmardlr_cfg(id, QSPI_DMA_RDLR_VALUE);
    hal_qspi_dmatdlr_cfg(id, QSPI_DMA_TDLR_VALUE);
}

void qspi_configure_dma_interrupt_connection(cores_t core)
{
    hal_qspi_config_dma_interrupt_connection(core);
}

void qspi_configure_dma_interrupt_disconnection(cores_t core)
{
    hal_qspi_config_dma_interrupt_disconnection(core);
}

void qspi_configure_dma_connection(cores_t core)
{
    hal_qspi_config_dma_connection(core);
}
