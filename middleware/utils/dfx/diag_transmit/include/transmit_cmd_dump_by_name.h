/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dump by name header file
 * This file should be changed only infrequently and with great care.
 */
#ifndef __TRANSMIT_CMD_DUMP_BY_NAME_H__
#define __TRANSMIT_CMD_DUMP_BY_NAME_H__

#include "errcode.h"
#include "diag.h"

errcode_t transmit_cmd_dump_by_file_name(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option);
#endif
