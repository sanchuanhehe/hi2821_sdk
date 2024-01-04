/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: dfx resource id
 * This file should be changed only infrequently and with great care.
 */
#ifndef DFX_RESOURCE_ID
#define DFX_RESOURCE_ID

#include <stdint.h>

typedef enum {
    FAULT_DFX_MONITOR_REGISTER_FAIL,
    FAULT_DFX_MONITOR_INIT_FAIL,
    FAULT_DFX_DIAG_REGISTER_CMD_FAIL,
    FAULT_DFX_DIAG_UNREGISTER_CMD_FAIL,
} dfx_fault_event_id_t; /* default event id */

typedef enum {
    DFX_EVENT_TEST = 0x1
} dfx_event_id_t; /* event id */

typedef enum {
    DFX_MSG_ID_DIAG_PKT, /* pkt msg,msg size is */
    DFX_MSG_ID_SDT_MSG,
    DFX_MSG_ID_BEAT_HEART,
    DFX_MSG_ID_TRANSMIT_FILE,
    DFX_MSG_ID_RESERVE_MAX   = 0x10, /* 0x10 user defined */
} dfx_msg_id_t; /* msg id */

#define DFX_MSG_MAX_SIZE 0x20 /* msg element size of msg queue */
#define DFX_MSG_ID_LEN   sizeof(uint32_t)
#define DFX_QUEUE_MAX_SIZE 64

typedef enum {
    DIAG_CHANNEL_ID_0,
    DIAG_CHANNEL_ID_1,
    DIAG_CHANNEL_ID_2,
    DIAG_SUPPORT_CHANNEL_CNT,
    DIAG_CHANNEL_ID_INVALID = 0xFF,
} diag_channel_id_t; /* diag : physical channel id */

#define CONFIG_DIAG_CMD_TBL_NUM 10      /* diag:Maximum number of cmd tbl cnt */
#define CONFIG_DIAG_IND_TBL_NUM 3       /* diag:Maximum number of ind tbl cnt */
#define CONFIG_STAT_CMD_LIST_NUM 10     /* diag:Maximum number of stat tbl cnt */
#define CONFIG_DIAG_RX_BUF_SIZE 0x400   /* diag:rx buf size */

typedef uint8_t diag_addr;

typedef enum {
    DIAG_SAMPLE_DATA_TRANSMIT_ID_SCO_IN = 0xA,
    DIAG_SAMPLE_DATA_TRANSMIT_ID_SCO_OUT = 0xB,
    DIAG_SAMPLE_DATA_TRANSMIT_ID_SNOOP = 0xC,
} diag_sample_data_transmit_id_t;

#define DIAG_SAMPLE_DATA_TRANSMIT_ID_COUNT 3

#endif