/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: transmit data
 * This file should be changed only infrequently and with great care.
 */
#ifndef __TRANSMIT_API_H__
#define __TRANSMIT_API_H__
#include "zdiag_config.h"
#include "errcode.h"

typedef void (*save_file_result_hook)(int32_t len, uintptr_t usr_data);
errcode_t soc_dfx_save_file_async(char *file_name, uint8_t *buf, uint32_t size, save_file_result_hook handler,
    uintptr_t usr_data);
#endif