/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ls cmd header file
 * This file should be changed only infrequently and with great care.
 */
#ifndef __TRANSMIT_CMD_LS_H__
#define __TRANSMIT_CMD_LS_H__

#include "errcode.h"
#include "diag.h"

errcode_t transmit_cmd_ls(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
#endif
