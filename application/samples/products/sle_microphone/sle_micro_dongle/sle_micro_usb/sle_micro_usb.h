/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone USB Header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */
#ifndef MICROPHONE_USB_H
#define MICROPHONE_USB_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MICROPHONE_USB_LOG "[sle_micro usb]"

int32_t sle_micro_usb_uac_init(void);
int32_t sle_micro_usb_uac_send_data(const uint8_t *buf, int len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif