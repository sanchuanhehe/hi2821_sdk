/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides usb port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-8-25， Create file. \n
 */

#ifndef USB_PORTING_H
#define USB_PORTING_H

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_usb USB
 * @ingroup  drivers_port_usb
 * @{
 */

typedef void(*usb_sof_cb)(uint8_t **data, uint16_t *len, uint8_t *device_index);

/**
 * @brief  Register usb callback, This callback will be invoked every 125 us.
 * @param  [in]  sof_cb  The usb 8K callback.
 */
void usb_register_callback(usb_sof_cb sof_cb);

/**
 * @brief  Unregister usb callback.
 */
void usb_unregister_callback(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif