/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: NV common header file.
 */

#ifndef COMMON_H
#define COMMON_H

#include "stdint.h"
/* 修改此文件后需要先编译A核任意版本生成中间文件application.etypes后才能在编译nv.bin时生效 */
#define BT_CUSTOMIZE_NV_RESERVED 86
#define BT_CUSTOMIZE_CHNL_MAP_LEN 10
/* 基础类型无需在此文件中定义，直接引用即可，对应app.json中的sample0 */

/* 蓝牙地址长度 */
#define BD_ADDR_LEN 6
/* 蓝牙名称长度 */
#define BD_NAME_MAX_LEN       32
/* 蓝牙秘钥索引长度 */
#define BTH_SYS_MASK_LEN  4
/* 蓝牙本端配对地址长度 */
#define BTH_BLE_OWN_ADDR_LEN  24
/* 蓝牙产品信息预留长度 */
#define BTH_PRODUCT_INFORMATION_RESERVE_LEN   32
/* 蓝牙秘钥长度 */
#define BTH_BLE_SMP_DATA_LEN  632
/* 蓝牙预留长度 */
#define BTH_BLE_RESERVE_LEN   128
/* HOST NV配置数据长度 */
#define BT_CONFIG_DATA_LEN 541
/* HOST NV配置数据长度 */
#define SLE_CONFIG_SYNC_DATA_SIZE 108
#define SLE_SM_OWN_ADDR_LEN 24
#define SLE_NV_RESERV_LEN   128
/* SLDM NV配置主锚点+从锚点数量 */
#define SLDM_NV_ANCHOR_NUM  8
#define SLDM_NV_NUM_OF_FEATURES 8
#define SLDM_NV_NUM_OF_R 12
#define SLDM_NV_HALF_NUM_OF_R 6

/* HOST NV配置结构 */
typedef struct {
    uint8_t bd_addr[BD_ADDR_LEN];
    uint8_t bd_name[BD_NAME_MAX_LEN];
    uint8_t product_type;
    uint8_t sys_mask[BTH_SYS_MASK_LEN];
    uint8_t reserve[BTH_PRODUCT_INFORMATION_RESERVE_LEN];
} bth_product_information_config_t;
/* HOST NV配置结构 */
typedef struct {            /* Keys that indexes by addr */
    uint8_t smp_index;
    uint8_t keys[BTH_BLE_SMP_DATA_LEN];     /* own address, dule-mode? */
} bth_smp_keys_store_nv_stru_t;

typedef struct {            /* Keys that indexes by addr */
    uint8_t reserve[BTH_BLE_RESERVE_LEN];
} bth_ble_nv_reserved_struct_t;

/* SLE HOST NV配置结构 */
typedef struct {
    uint8_t sle_addr[BD_ADDR_LEN];
    uint8_t sle_name[BD_NAME_MAX_LEN];
    uint8_t sys_mask[BTH_SYS_MASK_LEN];
    uint8_t reserve[BTH_PRODUCT_INFORMATION_RESERVE_LEN];
} sle_product_data_config_stru_t;


/* sle hadm 配置结构 */
typedef struct {
    uint16_t debug_flag;
    uint8_t posalg_freq;
    uint8_t hadm_addrm[BD_ADDR_LEN];  /* master */
    uint8_t hadm_addrk1[BD_ADDR_LEN];  /* key */
    uint8_t hadm_addrk2[BD_ADDR_LEN];  /* key */
    uint8_t hadm_addr[7][BD_ADDR_LEN];  /* slave0 */
    uint8_t hadm_role;
    uint8_t slave_num;
    uint8_t slave_index;
    uint16_t reserve;
    uint32_t offset[SLDM_NV_ANCHOR_NUM];
    uint32_t x[SLDM_NV_ANCHOR_NUM];
    uint32_t y[SLDM_NV_ANCHOR_NUM];
    uint32_t z[SLDM_NV_ANCHOR_NUM];
    uint32_t threshold_cond2;
    uint16_t rssi_limit;
    uint16_t reserve2;
    uint8_t r_start;
    uint8_t anchor_used_num_fusion;
} sle_hadm_data_config_t;


typedef struct {
    uint32_t e_r[SLDM_NV_NUM_OF_R];
} sle_hadm_pos_in_out_config_t;

/* sle hadm 配置结构 */
typedef struct {
    uint32_t intercepts;
    uint32_t index[SLDM_NV_NUM_OF_FEATURES];
    uint32_t mean[SLDM_NV_NUM_OF_FEATURES];
    uint32_t scale[SLDM_NV_NUM_OF_FEATURES];
    uint32_t coef[SLDM_NV_NUM_OF_FEATURES];
} sle_hadm_posalg_config_t;

#endif /* COMMON_H */