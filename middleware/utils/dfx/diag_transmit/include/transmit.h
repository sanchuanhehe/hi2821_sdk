/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: transmit header file
 * This file should be changed only infrequently and with great care.
 */
#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__

#include "errcode.h"
#include "diag_service.h"

errcode_t uapi_transmit_init(void);

errcode_t transmit_msg_proc(uint32_t msg_id, const uint8_t *msg, uint32_t msg_len);

#endif
