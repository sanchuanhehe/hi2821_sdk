/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: diag common header file
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_COMMON_H
#define DIAG_COMMON_H

#include "errcode.h"
#include "zdiag_config.h"

#define DIAG_ROUTER_FRAME_START_FLAG    0xAF
#define DIAG_ROUTER_FRAME_END_FLAG      0xAF
#define MUX_START_FLAG                  DIAG_ROUTER_FRAME_START_FLAG
#define AES_KEY_LEN                     16
#define AES_IV_LEN                      12
#define AES_TAG_LEN                     16

#define DIAG_SERVICE_HEADER_LEN         8
#define DIAG_ROUTER_HEADER_LEN          4
#define DIAG_ROUTER_HEADER_MAX_LEN      7

#define DIAG_ROUTER_FID_LEN             1
#define DIAG_ROUTER_SN_LEN              1
#define DIAG_ROUTER_ECHO_LEN            1
#define DIAG_ROUTER_CRC_LEN             2   // CRC16
#define DIAG_ROUTER_SECURE_LEN          28
#define DIAG_ROUTER_EOF_LEN             1

#define DIAG_ROUTER_CTAL_CRC_BIT        0
#define DIAG_ROUTER_CTAL_FID_BIT        1
#define DIAG_ROUTER_CTAL_SN_BIT         2
#define DIAG_ROUTER_CTAL_ACK_TYPE_BIT   4
#define DIAG_ROUTER_CTAL_SECURE_FLAG_BIT    6
#define DIAG_ROUTER_CTAL_EOF_BIT        7

#define DIAG_ROUTER_CTAL_CRC_EN_MASK    0x01
#define DIAG_ROUTER_CTAL_FID_EN_MASK    0x02
#define DIAG_ROUTER_CTAL_SN_MASK        0x0C
#define DIAG_ROUTER_CTAL_ACK_TYPE_MASK  0x30
#define DIAG_ROUTER_CTAL_SECURE_FLAG_MASK   0x40
#define DIAG_ROUTER_CTAL_EOF_EN_MASK    0x80

#define DIAG_ROUTER_FID_SRC_BIT         0
#define DIAG_ROUTER_FID_DST_BIT         4
#define DIAG_ROUTER_FID_MASK            0xF

#define DIAG_FRAME_SHIFT_8              8

// a: en_crc, b: en_fid, c:en_sn, d: ack_type, e: en_eof
#define frame_ctrl_build(a, b, c, d, e) (((a) << DIAG_ROUTER_CTAL_CRC_BIT) | \
                                        ((b) << DIAG_ROUTER_CTAL_FID_BIT) | \
                                        ((c) << DIAG_ROUTER_CTAL_SN_BIT)  | \
                                        ((d) << DIAG_ROUTER_CTAL_ACK_TYPE_BIT) | \
                                        ((e) << DIAG_ROUTER_CTAL_EOF_BIT))

#define get_frame_ctrl_crc_en(x)        (((x) & DIAG_ROUTER_CTAL_CRC_EN_MASK) >> DIAG_ROUTER_CTAL_CRC_BIT)
#define get_frame_ctrl_fid_en(x)        (((x) & DIAG_ROUTER_CTAL_FID_EN_MASK) >> DIAG_ROUTER_CTAL_FID_BIT)
#define get_frame_ctrl_sn(x)            (((x) & DIAG_ROUTER_CTAL_SN_MASK) >> DIAG_ROUTER_CTAL_SN_BIT)
#define get_frame_ctrl_ack_type(x)      (((x) & DIAG_ROUTER_CTAL_ACK_TYPE_MASK) >> DIAG_ROUTER_CTAL_ACK_TYPE_BIT)
#define get_frame_ctrl_secure_flag(x)   (((x) & DIAG_ROUTER_CTAL_SECURE_FLAG_MASK) >> DIAG_ROUTER_CTAL_SECURE_FLAG_BIT)
#define get_frame_ctrl_eof_en(x)        (((x) & DIAG_ROUTER_CTAL_EOF_EN_MASK) >> DIAG_ROUTER_CTAL_EOF_BIT)

#define get_frame_fid_src(x)   ((x) & DIAG_ROUTER_FID_MASK)
#define get_frame_fid_dst(x)   (((x) >> DIAG_ROUTER_FID_DST_BIT) & DIAG_ROUTER_FID_MASK)


#define get_frame_len(x, y)    (((x) << DIAG_FRAME_SHIFT_8) | (y))

/* Ack_type definitions */
enum {
    FRAME_ACK_TYPE_NONE = 0,
    FRAME_ACK_TYPE_ACK  = 1,
    FRAME_ACK_TYPE_ECHO = 3,
    FRAME_ACK_TYPE_MAX,
};

typedef enum {
    DIAG_PKT_PROC_ASYNC,   /* 发出去的异步命令或应答 */
    DIAG_PKT_PROC_SYNC,    /* 同步命令或应答 */
} diag_pkt_mode_t;

/* FID definitions */
typedef enum {
    DIAG_FRAME_FID_LOCAL = 0x00,
    DIAG_FRAME_FID_PC    = 0x01,
    DIAG_FRAME_FID_MCU   = 0x02,
    DIAG_FRAME_FID_BT    = 0x03,
    DIAG_FRAME_FID_PHONE = 0x04,
    DIAG_FRAME_FID_RES   = 0x05,
    DIAG_FRAME_FID_MAX   = 0x0F,
} diag_frame_fid_t;

typedef enum {
    DIAG_FRAME_SN_NONE       = 0x0,
    DIAG_FRAME_SN_START      = 0x1,
    DIAG_FRAME_SN_INSIDE     = 0x2,
    DIAG_FRAME_SN_END        = 0x3,
    DIAG_FRAME_SN_MAX        = 0x4,
} diag_frame_sn_t;

typedef struct {
    uint32_t module;   /* source module ID */
    uint32_t dest_mod; /* destination module ID */
    uint32_t no;       /* No. */
    uint32_t id;       /* ID */
    uint32_t time;
    int8_t data[0]; /* User Data Cache */
} diag_cmd_log_layer_stru_t;
#define DIAG_FULL_LOG_HEADER_SIZE sizeof(diag_cmd_log_layer_stru_t)

typedef struct __attribute__ ((packed)) {
    uint16_t no;       /* No. */
    uint32_t id;       /* ID */
    int8_t data[0]; /* User Data Cache */
} diag_cmd_minimal_log_layer_t;
#define DIAG_MINIMAL_LOG_HEADER_SIZE sizeof(diag_cmd_minimal_log_layer_t)

typedef struct __attribute__ ((packed)) {
    uint8_t src_mod;
    uint8_t dst_mod;
    uint16_t no;       /* No. */
    uint32_t id;       /* ID */
    uint32_t time;
    int8_t data[0]; /* User Data Cache */
} diag_cmd_normal_log_layer_t;
#define DIAG_NORMAL_LOG_HEADER_SIZE sizeof(diag_cmd_normal_log_layer_t)

typedef struct __attribute__ ((packed)) {
    uint8_t core : 3;      /* core */
    uint8_t sub_mod : 5;   /* sub module ID */
    uint8_t no;        /* No. */
    uint32_t id;       /* ID */
    uint32_t time;
    int8_t data[0]; /* User Data Cache */
} diag_cmd_extend_log_layer_t;
#define DIAG_EXTEND_LOG_HEADER_SIZE sizeof(diag_cmd_extend_log_layer_t)

typedef struct {
    uint8_t mod_id;     /* mod id */
    uint8_t cmd_id;     /* cmd id */
    uint8_t param[0];    /* data */
} diag_head_req_stru_t;
#define DIAG_REQ_HEADER_SIZE sizeof(diag_head_req_stru_t)

typedef struct {
    uint8_t mod_id;     /* mod id */
    uint8_t cmd_id;     /* cmd id */
    uint8_t param[0];    /* data */
} diag_head_ind_stru_t;
#define DIAG_IND_HEADER_SIZE sizeof(diag_head_ind_stru_t)

typedef struct {
    uint8_t en_crc   : 1;
    uint8_t en_fid   : 1;
    uint8_t en_sn    : 2;
    uint8_t ack_type : 2;
    uint8_t res      : 1;
    uint8_t en_eof   : 1;
} diag_router_ctrl_t;

typedef struct {
    diag_router_ctrl_t ctrl;
    uint8_t fid;
    uint8_t sn_count;
    uint8_t echo;
    uint16_t mfs;
    uint16_t data_len;       /* Payload len */
    uint8_t *data;
} diag_router_data_t;

typedef struct __attribute__ ((packed)) {
    uint8_t sof;         /* start of frame fixed 0xAF */
    uint8_t ctrl;        /* data contrl options.      */
    uint8_t len_lsb;     /* data len, (low 8bit)      */
    uint8_t len_msb;     /* data len, (high 8bit)     */
    uint8_t payload[0];  /* Valid Data Pointer        */
} diag_router_frame_t;

#define DIAG_FRAME_HEADER_SIZE DIAG_ROUTER_HEADER_LEN

#endif
