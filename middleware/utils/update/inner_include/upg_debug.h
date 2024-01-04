/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: UPG debug functions header file
 */

#ifndef UPG_DEBUG_H
#define UPG_DEBUG_H

#include "upg_config.h"
#include "upg_definitions.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (UPG_CFG_DEBUG_PRINT_ENABLED == YES)

void upg_print(const char *s);
void upg_msg0(const char *s);
void upg_msg1(const char *s, uint32_t h);
void upg_msg2(const char *s, uint32_t h1, uint32_t h2);
void upg_msg4(const char *s, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4);

void upg_print_flag(fota_upgrade_flag_area_t *upg_flag);

#else

#define upg_print(...)
#define upg_msg0(...)
#define upg_msg1(...)
#define upg_msg2(...)
#define upg_msg4(...)

#define upg_print_flag(...)

#endif /* UPG_CFG_DEBUG_PRINT_ENABLED */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_DEBUG_H */
