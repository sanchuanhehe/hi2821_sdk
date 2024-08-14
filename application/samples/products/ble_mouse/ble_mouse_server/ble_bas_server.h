/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: BLE Mouse BAS and DIS Service Server. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-20, Create file. \n
 */
#ifndef SRC_BLE_BAS_SERV_H
#define SRC_BLE_BAS_SERV_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define BAS_SERVICE_BASE_ID 30

typedef struct {
    uint16_t service_handle_in;
    uint16_t handle_out;
    uint16_t value_handle_out;
} chara_handle_t;

typedef struct {
    uint16_t service_handle_in;
    uint16_t handle_out;
} descriptor_handle_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif

#endif /* SRC_BLE_BAS_SERV_H_ */