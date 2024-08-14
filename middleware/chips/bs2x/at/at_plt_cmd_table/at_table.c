/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: at plt cmd func \n
 * Author: @CompanyNameTag \n
 */

#include "at_table.h"

at_cmd_entry_t at_plt_parse_table[] = {
    {
        "TESTSUITE",
        1,
        0,
        NULL,
        (at_cmd_func_t)uapi_at_sw_testsuite,
        NULL,
        NULL,
        NULL,
    }
};

uint32_t uapi_get_plt_table_size(void)
{
    return sizeof(at_plt_parse_table) / sizeof(at_cmd_entry_t);
}

at_cmd_entry_t* uapi_get_plt_at_table(void)
{
    return at_plt_parse_table;
}