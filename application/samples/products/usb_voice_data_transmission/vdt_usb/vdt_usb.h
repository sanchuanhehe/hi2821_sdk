/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone USB Header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-12, Create file. \n
 */
#ifndef VDT_USB_H
#define VDT_USB_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VDT_USB_LOG "[voice data transmission usb]"

int32_t vdt_usb_uac_init(void);
int32_t vdt_usb_uac_send_data(const uint8_t *buf, int len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif