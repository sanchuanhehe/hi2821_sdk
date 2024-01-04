/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse Button header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#ifndef MOUSE_BUTTON_H
#define MOUSE_BUTTON_H

#include <stdint.h>
#include "system_init.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
        uint8_t reserved   : 5;
    } b;
    uint8_t d8;
} mouse_key_t;

void mouse_button_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif