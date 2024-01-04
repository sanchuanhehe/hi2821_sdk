/*
 * Copyright (c) @CompanyNameMagicTag 2018-2021. All rights reserved.
 * Description:  NON-OS QSPI DRIVER API
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#ifndef NON_OS_QSPI_H
#define NON_OS_QSPI_H

#include "core.h"
#include "hal_qspi.h"

/**
 * @defgroup connectivity_drivers_non_os_qspi QSPI
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
#define QSPI_BUS_MAX_NUMBER 4

#define QSPI_IS_BUSY (true)
#define QSPI_IS_IDLE (false)

#define QSPI_WAIT_FIFO_DEFAULT_TIMEOUT 1024
#define QSPI_WAIT_FIFO_LONG_TIMEOUT    0xffff
#define QSPI_WAIT_READING_TIMEOUT      512

typedef enum qspi_state_t {
    QSPI_UNINIT_STATE = 0,
    QSPI_WORK_STATE,
    QSPI_DP_STATE,
    QSPI_SUSPENDING_STATE,
} qspi_state_t;

/**
 * @brief  Qspi dma control register.
 */
typedef enum {
    QSPI_DMA_CONTROL_DISABLE    = 0,
    QSPI_DMA_CONTROL_RX_ENABLE  = 1,
    QSPI_DMA_CONTROL_RX_DISABLE = 2,
    QSPI_DMA_CONTROL_TX_ENABLE  = 3,
    QSPI_DMA_CONTROL_TX_DISABLE = 4,
    QSPI_DMA_CONTROL_MAX_NUM,
    QSPI_DMA_CONTROL_NONE = QSPI_DMA_CONTROL_MAX_NUM,
} qspi_dma_control_t;

typedef struct qspi_fsm_stat_s {
    uint32_t recc_timeout_cnt;
    uint32_t wait_trans_timeout_cnt;
} qspi_fsm_stat_t;

typedef struct qspi_fsm_s {
    qspi_state_t curr_state;
    bool is_busy;
    qspi_fsm_stat_t stat;
} qspi_fsm_t;

/**
 * @brief  Send qsip data by words.
 * @param  id qspi id.
 * @param  buffer The buffer to save data.
 * @param  length The lenght of word to send, one word is 4 Bytes.
 * @return QSPI_RET_OK If send sucuess, otherwise failed.
 */
qspi_ret_t qspi_send_data_by_word(qspi_bus_t id, uint32_t *buffer, uint32_t length);

/**
 * @brief  Receive qsip data by words.
 * @param  id qspi id.
 * @param  buffer The buffer to save data.
 * @param  length The lenght of word to read, one word is 4 Bytes.
 * @return QSPI_RET_OK If read sucuess, otherwise failed.
 */
qspi_ret_t qspi_recv_data_by_word(qspi_bus_t id, uint32_t *buffer, uint32_t length);

/**
 * @brief  Receive qsip data by bytes.
 * @param  id qspi id.
 * @param  buffer The buffer to save data.
 * @param  length The length of byte to read.
 * @return QSPI_RET_OK If read sucuess, otherwise failed.
 */
qspi_ret_t qspi_recv_data_by_byte(qspi_bus_t id, uint8_t *buffer, uint32_t length);

/**
 * @brief  Wait qspi transfer complete.
 * @param  id qspi id.
 * @param  timeout The timeout of count.
 * @return QSPI_RET_OK If read sucuess, otherwise failed.
 */
qspi_ret_t qspi_trans_complete_wait_timeout(qspi_bus_t id, uint32_t timeout);

/**
 * @brief  Set qspi state.
 * @param  id qspi id.
 * @param  state The state to set.
 */
void qspi_set_state(qspi_bus_t id, qspi_state_t state);

/**
 * @brief  Get qspi state.
 * @param  id qspi id.
 * @return Current qspi state(qspi_state_t).
 */
qspi_state_t qspi_get_state(qspi_bus_t id);

/**
 * @brief  Claim qspi for use.
 * @param  id qspi id.
 * @return true If can be used.
 */
bool qspi_claim(qspi_bus_t id);

/**
 * @brief  Release qspi.
 * @param  id qspi id.
 */
void qspi_release(qspi_bus_t id);

/**
 * @brief  Enable or disable qpsi dma tx/rx.
 * @param  id qspi id.
 * @param  control The state of qspi dma.
 */
void qspi_dma_control(qspi_bus_t id, qspi_dma_control_t control);

/**
 * @brief  Set qspi dma watermark.
 * @param  id qspi id.
 */
void qspi_dma_set_data_level(qspi_bus_t id);

/**
 * @brief  Qspi enter or exit sleep mode.
 * @param  sleep Ture qspi enter sleep, false exit sleep.
 */
void qspi_enter_sleep(bool sleep);

/**
 * @brief  Connect qspi interrupt to a core, interrupt can connect multiple core.
 * @param  core The core to connect.
 */
void qspi_configure_dma_interrupt_connection(cores_t core);

/**
 * @brief  Disconnect qspi interrupt to a core, interrupt can connect multiple core.
 * @param  core The core to connect.
 */
void qspi_configure_dma_interrupt_disconnection(cores_t core);

/**
 * @brief  Config qspi dma connection.
 * @param  core The core to connect.
 */
void qspi_configure_dma_connection(cores_t core);

/**
 * @}
 */
#endif
