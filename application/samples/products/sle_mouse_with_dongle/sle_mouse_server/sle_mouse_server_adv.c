/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Mouse server adv source.
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_mouse_server_adv.h"

#define NAME_MAX_LENGTH                           15
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT                 0x64
/* 连接调度间隔12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT                 0x64
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT              0xC8
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT              0xC8
/* 超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT      0x1F4
/* 超时时间4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY                      0x1F3
/* 广播发送功率 */
#define SLE_ADV_TX_POWER                          10
/* 广播ID */
#define SLE_ADV_HANDLE_DEFAULT                    1
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX                      251

#define SLE_ADV_DATA_LEN 29
#define SLE_ADV_RSP_DATA_LEN 11
#define BD_ADDR_LEN6 6

static uint8_t g_sle_adv_data[SLE_ADV_DATA_LEN] = {
    // flag
    0x01,
    0x01, 0x01,
    0x05,
    0x04, 0x0B, 0x06, 0x09, 0x06,
    // appearance
    0x03,
    0x12, 0x09, 0x06, 0x07, 0x03, 0x02, 0x05, 0x00,
    0x06, 0x09,
    's', 'l', 'e', '_', 'm', 'o', 'u', 's', 'e'
};

static uint8_t g_sle_adv_rsp_data[SLE_ADV_RSP_DATA_LEN] = {
    0x0B,
    0x09, 's', 'l', 'e', '_', 'm', 'o', 'u', 's', 'e',
};

static uint8_t g_sle_mac_addr[BD_ADDR_LEN6] = {0x33, 0x11, 0x44, 0x66, 0x55, 0x14};

static void sle_set_addr(void)
{
    uint8_t *addr = g_sle_mac_addr;

    sle_addr_t sle_addr = {0};
    sle_addr.type = 0;
    if (memcpy_s(sle_addr.addr, BD_ADDR_LEN6, addr, BD_ADDR_LEN6) != EOK) {
        osal_printk("addr memcpy fail \r\n");
    } else {
        osal_printk("set sle mac in sle adv SUCC \r\n");
    }
    sle_set_local_addr(&sle_addr);
}
static int sle_set_default_announce_param(void)
{
    sle_set_addr();
    sle_announce_param_t param = {0};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    if (memcpy_s(param.own_addr.addr, BD_ADDR_LEN6, g_sle_mac_addr, BD_ADDR_LEN6) != EOK) {
        osal_printk("set sle adv param memcpy addr fail\r\n");
        return ERRCODE_SLE_PARAM_ERR;
    }
    return sle_set_announce_param(param.announce_handle, &param);
}

static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;

    osal_printk("set adv data default\r\n");
    data.announce_data = g_sle_adv_data;
    data.announce_data_len = SLE_ADV_DATA_LEN;
    data.seek_rsp_data = g_sle_adv_rsp_data;
    data.seek_rsp_data_len = SLE_ADV_RSP_DATA_LEN;

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        osal_printk("[SLE DD SDK] set announce data success.");
    } else {
        osal_printk("[SLE DD SDK] set adv param fail.");
    }
    return ERRCODE_SLE_SUCCESS;
}

static void sle_adv_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s sle announce enable id:%02x, state:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, announce_id, status);
}

static void sle_adv_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    osal_printk("%s sle announce disable id:%02x, state:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, announce_id, status);
}

static void sle_adv_announce_terminal_cbk(uint32_t announce_id)
{
    osal_printk("%s sle announce terminal id:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, announce_id);
}

static void sle_adv_enable_cbk(errcode_t status)
{
    osal_printk("%s sle enable status:%02x\r\n", SLE_MOUSE_DONGLE_SERVER_LOG, status);
}

static void sle_adv_announce_register_cbks(void)
{
    sle_announce_seek_callbacks_t seek_cbks = { 0 };
    seek_cbks.announce_enable_cb = sle_adv_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_adv_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_adv_announce_terminal_cbk;
    seek_cbks.sle_enable_cb = sle_adv_enable_cbk;
    sle_announce_seek_register_callbacks(&seek_cbks);
}

void sle_mouse_server_adv_init(void)
{
    osal_printk("%s sle_mouse_server_adv_init in\r\n", SLE_MOUSE_DONGLE_SERVER_LOG);
    sle_adv_announce_register_cbks();
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    osal_printk("%s sle_mouse_server_adv_init out\r\n", SLE_MOUSE_DONGLE_SERVER_LOG);
}
