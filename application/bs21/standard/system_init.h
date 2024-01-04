/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Application core system initialize interface hearder for standard \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "platform_core.h"
#include "pinctrl.h"
#include "uart.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void system_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SYSTEM_INIT_H */