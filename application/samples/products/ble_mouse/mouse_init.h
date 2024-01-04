/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Low Lantency Mouse Header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#ifndef SLE_LOW_LAYENCY_SERVICE_H
#define SLE_LOW_LAYENCY_SERVICE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define BLE_MOUSE_TASK_DURATION_MS              2000

void ble_mouse_init(void);

void ble_get_mouse_data(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif