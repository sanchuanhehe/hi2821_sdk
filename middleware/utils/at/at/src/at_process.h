/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at process service api header. Only for AT module. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-02, Create file. \n
 */

#ifndef    AT_PROCESS_H
#define    AT_PROCESS_H

#include "at_product.h"

at_ret_t at_proc_cmd_handle(uint16_t channel_id);

uint16_t at_proc_get_current_channel_id(void);

#endif