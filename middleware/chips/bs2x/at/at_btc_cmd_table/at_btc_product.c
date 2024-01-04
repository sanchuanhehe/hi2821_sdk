/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: at plt cmd func \n
 * Author: @CompanyNameTag \n
 */

#include "debug_print.h"
#include "bts_factory.h"
#include "at_config.h"
#include "at_product.h"
#include "nv.h"
#include "key_id.h"
#include "otp.h"
#include "chip_io.h"
#include "at_bt_cmd_table.h"
#include "clock_calibration.h"
#include "sle_factory_manager.h"
#include "sle_device_discovery.h"

#define BT_CUSTOMIZE_NV_VALID 21
#define BT_NV_START_OFFSET 8
#define BT_NV_CUSTOMIZE_PARAM 2

extern int ble_at_cmd_factory_register_callbacks(void);
extern int sle_at_cmd_factory_register_cbks(void);

const int32_t xo_ctrim_cali_increase_switch[] = {
    0, 1
};

const at_para_parse_syntax_t xo_ctrim_cali_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, xo_ctrim_cali_increase_switch},
        .offset = offsetof(xo_ctrim_cali_param_args_t, increase)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(xo_ctrim_cali_param_args_t, step_num)
    },
};

const int32_t ble_pk_payload_type_values[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};
const int32_t ble_rf_tx_phy_values[] = {
    1, 2, 3, 4
};
const at_para_parse_syntax_t ble_rf_tx_write_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 39,
        .offset = offsetof(ble_rf_tx_param_args_t, tx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 37,
        .entry.int_range.max_val = 255,
        .offset = offsetof(ble_rf_tx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, ble_pk_payload_type_values},
        .offset = offsetof(ble_rf_tx_param_args_t, pk_payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, ble_rf_tx_phy_values},
        .offset = offsetof(ble_rf_tx_param_args_t, tx_phy)
    },
};

const int32_t ble_rf_rx_phy_values[] = {
    1, 2, 3, 4
};
const int32_t ble_modulation_index_values[] = {
    0, 1
};
const at_para_parse_syntax_t ble_rf_rx_write_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 39,
        .offset = offsetof(ble_rf_rx_param_args_t, rx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, ble_rf_rx_phy_values},
        .offset = offsetof(ble_rf_rx_param_args_t, rx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, ble_modulation_index_values},
        .offset = offsetof(ble_rf_rx_param_args_t, modulation_index)
    },
};

const int32_t sle_rf_tx_power_values[] = {
    0, 1, 2, 3, 4, 5
};
const int32_t sle_rf_tx_payload_type_values[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};
const int32_t sle_rf_tx_phy_values[] = {
    0, 1, 4
};
const int32_t sle_rf_tx_format_values[] = {
    0, 2
};
const int32_t sle_rf_tx_rate_values[] = {
    0, 2, 3
};
const int32_t sle_rf_tx_pilot_ratio_values[] = {
    0, 1, 2, 3
};
const int32_t sle_rf_tx_polar_r_values[] = {
    0, 1, 2, 3
};
const at_para_parse_syntax_t sle_rf_tx_write_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_tx_param_args_t, tx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {6, sle_rf_tx_power_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_power)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 37,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_tx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, sle_rf_tx_payload_type_values},
        .offset = offsetof(sle_rf_tx_param_args_t, pk_payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, sle_rf_tx_phy_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, sle_rf_tx_format_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_format)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, sle_rf_tx_rate_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_rate)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, sle_rf_tx_pilot_ratio_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_pilot_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, sle_rf_tx_polar_r_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_polar_r)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 7,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_tx_param_args_t, tx_interval)
    },
};

const int32_t sle_rf_rx_phy_values[] = {
    0, 1, 4
};
const int32_t sle_rf_rx_format_values[] = {
    0, 2
};
const int32_t sle_rf_rx_pilot_ratio_values[] = {
    0, 1, 2, 3
};
const at_para_parse_syntax_t sle_rf_rx_write_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 79,
        .offset = offsetof(sle_rf_rx_param_args_t, rx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, sle_rf_rx_phy_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, sle_rf_rx_format_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_format)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, sle_rf_rx_pilot_ratio_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_pilot_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 7,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_rx_param_args_t, rx_interval)
    },
};

const int32_t rf_single_tone_mode[] = {
    0, 255
};
const at_para_parse_syntax_t rf_single_tone_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 79,
        .offset = offsetof(rf_single_tone_param_args_t, rf_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, rf_single_tone_mode},
        .offset = offsetof(rf_single_tone_param_args_t, rf_mode)
    },
};

const at_para_parse_syntax_t bt_write_customize_nv_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET, // 支持最大偏移为当前有效定制化的最大偏移
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset4)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value4)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset5)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value5)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset6)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value6)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset7)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value7)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset8)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value8)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset9)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value9)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset10)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value10)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset11)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value11)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset12)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value12)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset13)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value13)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset14)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value14)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = BT_CUSTOMIZE_NV_VALID - BT_NV_START_OFFSET,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_offset15)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL,
        .offset = offsetof(bt_write_customize_nv_param_args_t, customize_value15)
    },
};

/* XO CTRIM CALIBRATION */
at_ret_t xo_ctrim_cali_cmd(const xo_ctrim_cali_param_args_t *args)
{
#ifdef XO_32M_CALI
    bool step_increase = args->increase == 0x1 ? true : false;
    uint8_t xo_ctrim_step = (uint8_t)args->step_num;
    uint8_t xo_ctrim_value = 0;
    // 通过命令发送增加或减少左右电容的step，并配置寄存器
    calibration_xo_core_ctrim_algorithm(step_increase, xo_ctrim_step);
    // 回读寄存器确认值成功写入
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    PRINT("xo_ctrim_reg_val:0x%x\n", xo_ctrim_value);
#else
    UNUSED(args);
#endif
    return AT_RET_OK;
}

/* XO CTRIM CALIBRATION WRITE EFUSE */
at_ret_t xo_ctrim_cali_write_efuse_cmd(void)
{
#ifdef XO_32M_CALI
    uint8_t xo_ctrim_value = 0;
    // 读当前寄存器内值并写入efuse
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    calibration_save_xo_core_ctrim(xo_ctrim_value);
    // 回读efuse检查是否写入，efuse只能写一次
    calibration_read_xo_core_ctrim(&xo_ctrim_value);
    PRINT("efuse_val:0x%x\n", xo_ctrim_value);
#endif
    return AT_RET_OK;
}

/* XO CTRIM CALIBRATION GET REG VAL */
at_ret_t xo_ctrim_get_reg_val_cmd(void)
{
#ifdef XO_32M_CALI
    uint8_t xo_ctrim_value = 0;
    // 读取当前寄存器配置
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    PRINT("reg_val:0x%x\n", xo_ctrim_value);
#endif
    return AT_RET_OK;
}

/* BLE长发 */
at_ret_t bt_at_ble_rf_tx_cmd(const ble_rf_tx_param_args_t *args)
{
    ble_rf_tx_start_t rf_tx_start = { 0 };
    rf_tx_start.tx_freq = (uint8_t)args->tx_freq;
    rf_tx_start.test_data_len = (uint8_t)args->test_data_len;
    rf_tx_start.payload_type = (uint8_t)args->pk_payload_type;
    rf_tx_start.tx_phy = (uint8_t)args->tx_phy;
    return ble_factory_rf_tx_start(&rf_tx_start);
}

/* BLE长收 */
at_ret_t bt_at_ble_rf_rx_cmd(const ble_rf_rx_param_args_t *args)
{
    ble_rf_rx_start_t rf_rx_start = { 0 };
    rf_rx_start.rx_freq = (uint8_t)args->rx_freq;
    rf_rx_start.rx_phy = (uint8_t)args->rx_phy;
    rf_rx_start.modulation_index = (uint8_t)args->modulation_index;
    return ble_factory_rf_rx_start(&rf_rx_start);
}

/* BLE TRX END */
at_ret_t bt_at_ble_rf_trxend_cmd(void)
{
    return ble_factory_rf_trx_end();
}

/* BLE RESET */
at_ret_t bt_at_ble_reset_cmd(void)
{
    return ble_factory_rf_reset();
}

/* sle长发 */
at_ret_t bt_at_sle_rf_tx_cmd(const sle_rf_tx_param_args_t *args)
{
    sle_rf_tx_start_t rf_tx_start = { 0 };
    rf_tx_start.tx_freq = (uint8_t)args->tx_freq;
    rf_tx_start.tx_power = (uint8_t)args->tx_power;
    rf_tx_start.test_data_len = (uint16_t)args->test_data_len;
    rf_tx_start.pk_payload_type = (uint8_t)args->pk_payload_type;
    rf_tx_start.tx_phy = (uint8_t)args->tx_phy;
    rf_tx_start.tx_format = (uint8_t)args->tx_format;
    rf_tx_start.tx_rate = (uint8_t)args->tx_rate;
    rf_tx_start.tx_pilot_ratio = (uint8_t)args->tx_pilot_ratio;
    rf_tx_start.tx_polar_r = (uint8_t)args->tx_polar_r;
    rf_tx_start.tx_interval = (uint16_t)args->tx_interval;
    sle_rf_tx_start(&rf_tx_start);
    return AT_RET_OK;
}

/* sle长收 */
at_ret_t bt_at_sle_rf_rx_cmd(const sle_rf_rx_param_args_t *args)
{
    sle_rf_rx_start_t rf_rx_start = { 0 };
    rf_rx_start.rx_freq = (uint8_t)args->rx_freq;
    rf_rx_start.rx_phy = (uint8_t)args->rx_phy;
    rf_rx_start.rx_format = (uint8_t)args->rx_format;
    rf_rx_start.rx_pilot_ratio = (uint8_t)args->rx_pilot_ratio;
    rf_rx_start.rx_interval = (uint16_t)args->rx_interval;
    sle_rf_rx_start(&rf_rx_start);
    return AT_RET_OK;
}

/* sle TRX END */
at_ret_t bt_at_sle_rf_trxend_cmd(void)
{
    sle_rf_trx_end();
    return AT_RET_OK;
}

/* sle RESET */
at_ret_t bt_at_sle_reset_cmd(void)
{
    sle_rf_reset();
    return AT_RET_OK;
}

/* BT CALI NV */
at_ret_t bt_at_rf_cali_nv_cmd(void)
{
    ble_factory_rf_cali_nv();
    return AT_RET_OK;
}

at_ret_t bt_at_read_cali_nv_cmd(void)
{
    // TODO
    return AT_RET_OK;
}

at_ret_t bt_at_write_cali_nv_cmd(void)
{
    // TODO
    return AT_RET_OK;
}

at_ret_t bt_at_read_customize_nv_cmd(void)
{
    // TODO
    return AT_RET_OK;
}
/* BT LOAD CUSTOMIZE NV */
at_ret_t bt_at_customize_nv_cmd(const bt_write_customize_nv_param_args_t *args)
{
    UNUSED(args);
    // TODO
    return AT_RET_OK;
}

/* BT SINGLE TONE */
at_ret_t bt_at_rf_single_tone_cmd(const rf_single_tone_param_args_t *args)
{
    ble_rf_single_tone_t param = { 0 };
    param.rf_freq = (uint8_t)args->rf_freq;
    param.rf_mode = (uint8_t)args->rf_mode;
    ble_factory_rf_single_tone(&param);
    return AT_RET_OK;
}

/* BT SINGLE TONE */
at_ret_t bt_at_enable_sle_cmd(void)
{
    enable_sle();
    return AT_RET_OK;
}

/* BLE REGISTER CALLBACK */
at_ret_t bt_at_ble_register_callback_cmd(void)
{
    ble_at_cmd_factory_register_callbacks();
    return AT_RET_OK;
}

/* BLE REGISTER CALLBACK */
at_ret_t bt_at_sle_register_callback_cmd(void)
{
    sle_at_cmd_factory_register_cbks();
    return AT_RET_OK;
}

/* DIE ID */
at_ret_t bt_at_read_dieid_cmd(void)
{
    int8_t index;
    PRINT("[AT]DIE ID:\r\n");
    for (index = 0; index <= 0x20; index += 0x4) {
        PRINT("%04x ", readw(0x57028800 + index));
    }
    PRINT("\r\n");
    return AT_RET_OK;
}
