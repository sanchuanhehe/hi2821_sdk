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

void sle_low_latency_mouse_app_init(void);

void sle_low_latency_dongle_init(int usb_hid_index);

mouse_freq_t mouse_init(uint32_t sensor_id);
void sle_mouse_get_key(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif