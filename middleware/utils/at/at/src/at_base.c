/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides AT base source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-02， Create file. \n
 */

#include "at_base.h"

static at_base_api_t g_at_base_api = {0};

void at_base_toupper(char *str, uint32_t len)
{
    char *ch = str;
    for (uint32_t index = 0; index < len; index++) {
        if (*ch <= 'z' && *ch >= 'a') {
            *ch = (char)((uint8_t)*ch - (uint8_t)'a' + (uint8_t)'A');
        }
        ch++;
    }
}

errcode_t uapi_at_base_api_register(at_base_api_t base_api)
{
    if ((base_api.msg_queue_create_func == NULL) ||
        (base_api.msg_queue_read_func == NULL) ||
        (base_api.msg_queue_write_func == NULL) ||
        (base_api.malloc_func == NULL) ||
        (base_api.free_func == NULL) ||
        (base_api.task_pause_func == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    if (base_api.cmd_attr_func == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
#endif

    if (memcpy_s(&g_at_base_api, sizeof(at_base_api_t), &base_api, sizeof(at_base_api_t)) != EOK) {
        return ERRCODE_MEMCPY;
    }
    return ERRCODE_SUCC;
}

void* at_malloc(uint32_t size)
{
    if (g_at_base_api.malloc_func != NULL) {
        return g_at_base_api.malloc_func(size);
    }
    return 0;
}

void at_free(void *addr)
{
    if (g_at_base_api.free_func != NULL) {
        g_at_base_api.free_func(addr);
    }
}

void at_msg_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id)
{
    if (g_at_base_api.msg_queue_create_func != NULL) {
        g_at_base_api.msg_queue_create_func(msg_count, msg_size, queue_id);
    }
    return;
}

uint32_t at_msg_queue_write(unsigned long queue_id, void *msg_ptr, uint32_t msg_size, uint32_t timeout)
{
    if (g_at_base_api.msg_queue_write_func != NULL) {
        return g_at_base_api.msg_queue_write_func(queue_id, msg_ptr, msg_size, timeout);
    }
    return 0;
}

uint32_t at_msg_queue_read(unsigned long queue_id, void *buf_ptr, uint32_t *buf_size, uint32_t timeout)
{
    if (g_at_base_api.msg_queue_read_func != NULL) {
        return g_at_base_api.msg_queue_read_func(queue_id, buf_ptr, buf_size, timeout);
    }
    return 0;
}

void at_yield(void)
{
    if (g_at_base_api.task_pause_func != NULL) {
        g_at_base_api.task_pause_func();
    }
    return;
}

void at_log(const char *buf, uint16_t buf_size, uint8_t level)
{
#ifndef CONFIG_AT_SUPPORT_LOG
    unused(buf);
    unused(buf_size);
    unused(level);
#else
    if (g_at_base_api.log_func == NULL) {
        return;
    }

    g_at_base_api.log_func(buf, buf_size, level);
#endif
}

bool at_cmd_attr(uint16_t attr)
{
#ifdef CONFIG_AT_SUPPORT_CMD_ATTR
    if (g_at_base_api.cmd_attr_func == NULL) {
        return true;
    }

    return g_at_base_api.cmd_attr_func(attr);
#else
    unused(attr);
    return true;
#endif
}
