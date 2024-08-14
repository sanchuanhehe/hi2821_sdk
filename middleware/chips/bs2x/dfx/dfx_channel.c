/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: dfx channel
 * This file should be changed only infrequently and with great care.
 */
#include <stdint.h>
#include "dfx_adapt_layer.h"
#include "diag.h"
#include "diag_ind_src.h"
#include "diag_msg.h"
#include "diag_channel.h"
#include "zdiag_adapt_layer.h"
#include "log_uart.h"
#include "dfx_channel.h"
#ifdef SUPPORT_BT_UPG
#include "bts_def.h"
#endif

diag_channel_id_t g_diag_channel_id = DIAG_CHANNEL_ID_INVALID;
#ifdef SUPPORT_BT_UPG
errcode_t bth_ota_reg_cbk(void *data_report, void *status_report);
errcode_t bth_ota_data_send(uint8_t *data_ptr, uint16_t data_len);
#endif

void diag_uart_rx_proc(uint8_t *buffer, uint16_t length)
{
#ifdef SUPPORT_DIAG_V2_PROTOCOL
    uapi_diag_channel_rx_mux_char_data(g_diag_channel_id, buffer, length);
#else
    soc_diag_channel_rx_mux_char_data(g_diag_channel_id, buffer, length);
#endif
}

static int32_t diag_channel_uart_output(void *fd, dfx_data_type_t data_type,
                                        uint8_t *data[], uint16_t len[], uint8_t cnt)
{
    unused(fd);
    unused(data_type);
    for (uint8_t i = 0; i < cnt; i++) {
        if (osal_in_interrupt() || data_type == DFX_DATA_DIAG_PKT_CRITICAL) {
            log_uart_send_buffer(data[i], len[i]);
        } else {
            log_uart_write_blocking(data[i], len[i]);
        }
    }
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_BT_UPG
static int32_t diag_channel_bt_output(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[], uint8_t cnt)
{
    unused(fd);
    unused(data_type);
    unused(cnt);
    return (int32_t)bth_ota_data_send(data[0], len[0]);
}

static errcode_t diag_channel_rx_bt_data(uint8_t *data, uint16_t size)
{
    return uapi_diag_channel_rx_frame_data(DIAG_CHANNEL_ID_1, data, size);
}

static void diag_channel_register_bt_callback(void)
{
    bth_ota_reg_cbk(diag_channel_rx_bt_data, NULL);
}
#endif

errcode_t diag_register_channel(void)
{
#ifdef SUPPORT_DIAG_V2_PROTOCOL
    g_diag_channel_id = DIAG_CHANNEL_ID_0;
    if (uapi_diag_channel_init(g_diag_channel_id, DIAG_CHANNEL_ATTR_NEED_RX_BUF) != (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (uapi_diag_channel_set_connect_hso_addr(g_diag_channel_id, diag_adapt_get_default_dst()) !=
        (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (uapi_diag_channel_set_tx_hook(g_diag_channel_id, diag_channel_uart_output) != (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#ifdef SUPPORT_BT_UPG
    if (uapi_diag_channel_init(DIAG_CHANNEL_ID_1, DIAG_CHANNEL_ATTR_NONE) != (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (uapi_diag_channel_set_tx_hook(DIAG_CHANNEL_ID_1, diag_channel_bt_output) != (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    diag_channel_register_bt_callback();
#endif
#else
    g_diag_channel_id = DIAG_CHANNEL_ID_0;
    if (soc_diag_channel_init(g_diag_channel_id, SOC_DIAG_CHANNEL_ATTR_NEED_RX_BUF) != (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (soc_diag_channel_set_connect_hso_addr(g_diag_channel_id, diag_adapt_get_default_dst()) !=
        (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    if (soc_diag_channel_set_tx_hook(g_diag_channel_id, diag_channel_uart_output) != (errcode_t)ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}