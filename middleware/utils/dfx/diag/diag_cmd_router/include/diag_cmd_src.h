/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description:  cmd producer
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_CMD_SRC_H
#define DIAG_CMD_SRC_H
#include "diag_common.h"
#include "diag.h"
errcode_t uapi_diag_send_cmd(uint16_t cmd_id, uint8_t *data, uint16_t data_size, diag_option_t *option);
#endif
