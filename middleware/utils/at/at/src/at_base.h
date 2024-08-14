/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at base server header. Only for AT module. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-02, Create file. \n
 */

#ifndef    AT_BASE_H
#define    AT_BASE_H

#include "at_product.h"

#ifdef CONFIG_AT_SUPPORT_LOG
#define at_log_normal(buf, buf_size, level) at_log(buf, strlen(buf), 0)
#else
#define at_log_normal(buf, buf_size, level)
#endif

void at_base_toupper(char *str, uint32_t len);

void* at_malloc(uint32_t size);

void at_free(void *addr);

void at_msg_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id);

uint32_t at_msg_queue_write(unsigned long queue_id, void *msg_ptr, uint32_t msg_size, uint32_t timeout);

uint32_t at_msg_queue_read(unsigned long queue_id, void *buf_ptr, uint32_t *buf_size, uint32_t timeout);

void at_yield(void);

void at_log(const char *buf, uint16_t buf_size, uint8_t level);

bool at_cmd_attr(uint16_t attr);

#endif
