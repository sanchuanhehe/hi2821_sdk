/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: at plt cmd func \n
 * Author: @CompanyNameTag \n
 */

#include "at_bt_cmd_table.h"
#include "at_btc_product.h"
at_cmd_entry_t at_bt_cmd_parse_table[AT_TABLE_SIZE] = {
    {
        "DIEID",
        1, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_read_dieid_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BLETX",
        2, // ID
        0, // Attribute
        ble_rf_tx_write_param_syntax,
        NULL,
        (at_set_func_t)bt_at_ble_rf_tx_cmd,
        NULL,
        NULL,
    },
    {
        "BLERX",
        3, // ID
        0, // Attribute
        ble_rf_rx_write_param_syntax,
        NULL,
        (at_set_func_t)bt_at_ble_rf_rx_cmd,
        NULL,
        NULL,
    },
    {
        "BLETRXEND",
        4, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_ble_rf_trxend_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BLERST",
        5, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_ble_reset_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLETX",
        6, // ID
        0, // Attribute
        sle_rf_tx_write_param_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_rf_tx_cmd,
        NULL,
        NULL,
    },
    {
        "SLERX",
        7, // ID
        0, // Attribute
        sle_rf_rx_write_param_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_rf_rx_cmd,
        NULL,
        NULL,
    },
    {
        "SLETRXEND",
        8, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_rf_trxend_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLERST",
        9, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_reset_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTRFCALI",
        10, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_rf_cali_nv_cmd,
        NULL,
        (at_read_func_t)bt_at_read_cali_nv_cmd,
        NULL,
    },
    {
        "BTSETNV",
        11, // ID
        0,  // Attribute
        bt_write_customize_nv_param_syntax,
        NULL,
        (at_set_func_t)bt_at_customize_nv_cmd,
        (at_read_func_t)bt_at_read_customize_nv_cmd,
        NULL,
    },
    {
        "BTTXLO",
        12, // ID
        0,  // Attribute
        rf_single_tone_param_syntax,
        NULL,
        (at_set_func_t)bt_at_rf_single_tone_cmd,
        NULL,
        NULL,
    },
    {
        "XOCALI",
        13, // ID
        0,  // Attribute
        xo_ctrim_cali_param_syntax,
        NULL,
        (at_set_func_t)xo_ctrim_cali_cmd,
        NULL,
        NULL,
    },
    {
        "XOSETEFUSE",
        14,   // ID
        0,    // Attribute
        NULL, // 将当前reg中校准code写入efuse，无参数
        (at_cmd_func_t)xo_ctrim_cali_write_efuse_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "XOREGVAL",
        15,   // ID
        0,    // Attribute
        NULL, // 将当前reg中校准code写入efuse，无参数
        (at_cmd_func_t)xo_ctrim_get_reg_val_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLEENABLE",
        16, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_enable_sle_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BLEFACCALLBACK",
        17, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_ble_register_callback_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "SLEFACCALLBACK",
        18, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_register_callback_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTWRCALI",
        19, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_write_cali_nv_cmd,
        NULL,
        NULL,
        NULL,
    },
};

uint32_t uapi_get_bt_table_size(void)
{
    return sizeof(at_bt_cmd_parse_table) / sizeof(at_cmd_entry_t);
}

at_cmd_entry_t *uapi_get_bt_at_table(void)
{
    return at_bt_cmd_parse_table;
}
