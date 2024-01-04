/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT process source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-02， Create file. \n
 */

#include "at_base.h"
#include "at_channel.h"
#include "at_cmd.h"
#include "at_parse.h"
#include "at_process.h"

#define AT_FRAME_ERROR_STRING "NO MATCH THIS AT"
#define AT_FRAME_SUCCESS_STRING "MATCH THIS AT"

typedef struct {
    uint16_t channel_id;
    const at_cmd_entry_t *entry;
    at_format_t format;
    at_cmd_type_t type;
    void *args;
} at_control_block_t;

at_control_block_t g_at_control_block = {0};

uint16_t at_proc_get_current_channel_id(void)
{
    return g_at_control_block.channel_id;
}

static at_ret_t at_proc_perform_command_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->cmd) {
        ret = g_at_control_block.entry->cmd();
    } else {
        return AT_RET_PROC_CMD_FUNC_MISSING;
    }
    return ret;
}

static at_ret_t at_proc_perform_read_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->read) {
        ret = g_at_control_block.entry->read();
    } else {
        return AT_RET_PROC_READ_FUNC_MISSING;
    }
    return ret;
}

static at_ret_t at_proc_perform_test_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->test) {
        ret = g_at_control_block.entry->test();
    } else {
        return AT_RET_PROC_TEST_FUNC_MISSING;
    }
    return ret;
}

static at_ret_t at_proc_perform_set_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->set) {
        ret = g_at_control_block.entry->set(g_at_control_block.args);
    } else {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    return ret;
}

#ifdef CONFIG_AT_SUPPORT_QUERY
static at_ret_t at_proc_perform_query_cmd(void)
{
    at_ret_t ret;
    if (g_at_control_block.entry->query) {
        ret = g_at_control_block.entry->query(g_at_control_block.args);
    } else {
        return AT_RET_PROC_SET_FUNC_MISSING;
    }
    return ret;
}
#endif

static at_ret_t at_proc_perform_current_cmd(void)
{
    at_ret_t ret;

    switch (g_at_control_block.type) {
        case AT_CMD_TYPE_CMD:
            ret = at_proc_perform_command_cmd();
            break;
        case AT_CMD_TYPE_READ:
            ret = at_proc_perform_read_cmd();
            break;
        case AT_CMD_TYPE_TEST:
            ret = at_proc_perform_test_cmd();
            break;
        case AT_CMD_TYPE_SET:
            ret = at_proc_perform_set_cmd();
            break;
#ifdef CONFIG_AT_SUPPORT_QUERY
        case AT_CMD_TYPE_QUERY:
            ret = at_proc_perform_query_cmd();
            break;
#endif
        default:
            return AT_RET_CMD_TYPE_ERROR;
    }

    return ret;
}

static void at_proc_para_arguments_finish(void)
{
    if (g_at_control_block.args == NULL) {
        return;
    }

    at_parse_free_arguments(g_at_control_block.args, g_at_control_block.entry->syntax);
    at_free(g_at_control_block.args);
    g_at_control_block.args = NULL;
}

static at_ret_t at_proc_para_arguments_prepare(const char *str)
{
    at_ret_t ret;
    uint32_t struct_max_size;

#ifdef CONFIG_AT_SUPPORT_QUERY
    if (((g_at_control_block.type != AT_CMD_TYPE_SET) && (g_at_control_block.type != AT_CMD_TYPE_QUERY))
        || (g_at_control_block.entry->syntax == NULL)) {
#else
    if ((g_at_control_block.type != AT_CMD_TYPE_SET) || (g_at_control_block.entry->syntax == NULL)) {
#endif
        return AT_RET_OK;
    }

    struct_max_size = at_cmd_get_max_struct_size();
    g_at_control_block.args = at_malloc(struct_max_size);
    if (g_at_control_block.args == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    memset_s(g_at_control_block.args, struct_max_size, 0, struct_max_size);

    ret = at_parse_para_arguments(str, g_at_control_block.args,
                                  g_at_control_block.entry->syntax);
    if (ret != AT_RET_OK) {
        at_proc_para_arguments_finish();
        return ret;
    }
    return AT_RET_OK;
}

static at_ret_t at_proc_exec_cmd(const at_cmd_info_t *cmd_info)
{
    at_ret_t ret;
    uint16_t str_offset = 0;

    g_at_control_block.channel_id = cmd_info->channel_id;

    g_at_control_block.format = at_parse_format_of_cmd(cmd_info, &str_offset);
    if (g_at_control_block.format == AT_FORMAT_ERROR) {
        return AT_RET_CMD_FORMAT_ERROR;
    }

    g_at_control_block.entry = at_cmd_find_entry(cmd_info->cmd_str, &str_offset);
    if (g_at_control_block.entry == NULL) {
        return AT_RET_CMD_NO_MATCH;
    }

#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    if (g_at_control_block.entry->attribute != 0) {
        if (at_cmd_attr(g_at_control_block.entry->attribute) == false) {
            return AT_RET_CMD_ATTR_NOT_ALLOW;
        }
    }
#endif

    g_at_control_block.type = at_parse_cmd_type(cmd_info->cmd_str, &str_offset);

    /* The AT command parameters are transferred by filling the corresponding fields in the parameter structure. */
    ret = at_proc_para_arguments_prepare(cmd_info->cmd_str + str_offset);
    if (ret != AT_RET_OK) {
        return ret;
    }

    ret = at_proc_perform_current_cmd();
    at_proc_para_arguments_finish();
    return ret;
}

static at_ret_t at_proc_cmd_process(void)
{
    at_ret_t ret = AT_RET_OK;
    at_cmd_info_t *cmd_info;

    cmd_info = at_parse_get_next_remain_cmd();
    while (cmd_info != NULL) {
        ret = at_proc_exec_cmd(cmd_info);
        if (ret != AT_RET_OK) {
            uapi_at_report(AT_RESPONSE_ERROR);
        } else {
            uapi_at_report(AT_RESPONSE_OK);
        }
        at_parse_del_one_remain_cmd(cmd_info);
        cmd_info = at_parse_get_next_remain_cmd();
    }

    return AT_RET_OK;
}

at_ret_t at_proc_cmd_handle(uint16_t channel_id)
{
    at_ret_t ret;
    char *str = NULL;

    str = (char*)at_channel_get_data(channel_id);
    if (str == NULL) {
        at_channel_data_reset(channel_id);
        return AT_RET_CHANNEL_DATA_NULL;
    }
    at_log_normal(str, strlen(str), 0);

    ret = at_parse_split_combine_cmd(str, (uint32_t)strlen(str), channel_id);
    if (ret != AT_RET_OK) {
        at_channel_data_reset(channel_id);
        return ret;
    }

    ret = at_proc_cmd_process();
    at_channel_data_reset(channel_id);
    return ret;
}
