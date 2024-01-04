/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE Gamepad Button header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */
#ifndef SLE_GAMEPAD_BUTTON_H
#define SLE_GAMEPAD_BUTTON_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef int (*keyscan_report_callback_t)(int argc, uint8_t argv[]);

void sle_gamepad_button_init(keyscan_report_callback_t callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif