/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: transmit data
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_api.h"
#include "transmit_item.h"
#include "transmit_write_read.h"
#include "diag_common.h"
#include "transmit_debug.h"
#include "zdiag_adapt_layer.h"
#include "dfx_feature_config.h"

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
#include "transmit_file_operation.h"
STATIC errcode_t soc_dfx_save_file_emmc_async_local(char *file_name, uint8_t *buf, uint32_t size,
    save_file_result_hook handler, uintptr_t usr_data)
{
    dfx_assert(file_name);
    dfx_assert(buf);
    int32_t len = transmit_file_write(file_name, 0, buf, size);
    handler(len, usr_data);
    return ERRCODE_SUCC;
}
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */

#if defined CONFIG_FEATURE_SUPPORT_REMOTE_EMMC
STATIC errcode_t soc_dfx_save_file_async_remote(char *file_name, uint8_t *buf, uint32_t size,
    save_file_result_hook handler, uintptr_t usr_data)
{
    dfx_assert(file_name);
    dfx_assert(buf);
    diag_addr dst = diag_adapt_get_emmc_dst();
    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = dst;

    transmit_item_t *item = transmit_item_init(0);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    unused(handler);
    unused(usr_data);

    transmit_item_init_permanent(item, false);
    transmit_item_init_local_start(item, true);
    transmit_item_init_local_src(item, true);
    transmit_item_init_remote_type(item, TRANSMIT_TYPE_SAVE_FILE);
    transmit_item_init_local_type(item, TRANSMIT_LOCAL_TYPE_READ_DATA);
    transmit_item_init_read_handler(item, bus_read_data, (uintptr_t)item);
    transmit_item_init_file_name(item, file_name);
    transmit_item_init_local_bus_addr(item, (uint32_t)(uintptr_t)buf);
    transmit_item_init_option(item, &option);
    transmit_item_init_down_machine(item, true);
    transmit_item_init_total_size(item, size);

    if (transmit_item_init_is_success(item) == false) {
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }

    transmit_printf_item("save file", item);
    transmit_item_enable(item);
    return ERRCODE_SUCC;
}
#endif

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES) || defined(CONFIG_FEATURE_SUPPORT_REMOTE_EMMC)
STATIC errcode_t soc_dfx_save_file_emmc_async(char *file_name, uint8_t *buf, uint32_t size,
    save_file_result_hook handler, uintptr_t usr_data)
{
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    return soc_dfx_save_file_emmc_async_local(file_name, buf, size, handler, usr_data);
#elif defined(CONFIG_FEATURE_SUPPORT_REMOTE_EMMC)
    return soc_dfx_save_file_async_remote(file_name, buf, size, handler, usr_data);
#endif
}
#endif

errcode_t soc_dfx_save_file_async(char *file_name, uint8_t *buf, uint32_t size, save_file_result_hook handler,
    uintptr_t usr_data)
{
    dfx_assert(file_name);
    dfx_assert(buf);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES) || defined(CONFIG_FEATURE_SUPPORT_REMOTE_EMMC)
    return soc_dfx_save_file_emmc_async(file_name, buf, size, handler, usr_data);
#endif
    return ERRCODE_SUCC;
}
