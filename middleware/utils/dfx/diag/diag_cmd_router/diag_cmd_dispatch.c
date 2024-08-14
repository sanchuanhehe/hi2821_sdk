/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: diag cmd dispatch
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_dispatch.h"
#include "diag.h"
#include "zdiag_adapt_layer.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"
#include "diag_cmd_dst.h"
#include "diag_ind_dst.h"

errcode_t uapi_zdiag_cmd_process(diag_ser_data_t *data)
{
    errcode_t ret;
    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = data->header.src;

    ret = diag_pkt_router_run_cmd(data, &option);
    if (ret != ERRCODE_SUCC) {
        return diag_pkt_router_run_ind(data, &option);
    }
    return ERRCODE_FAIL;
}
