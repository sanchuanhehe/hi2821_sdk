/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Gamepad Button header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */
#ifndef GAMEPAD_BUTTON_H
#define GAMEPAD_BUTTON_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef int (*gamepad_button_report_callback_t)(int argc, uint8_t argv[]);

void gamepad_button_init(gamepad_button_report_callback_t callback);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif