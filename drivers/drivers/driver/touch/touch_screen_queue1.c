/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: touch screen msg queue.
 * Author: @CompanyNameTag
 * Create: 2022-04-18
 */

#include "touch_screen_queue1.h"
#include "securec.h"
#include "soc_osal.h"
#include "debug_print.h"

#define ts_queue_err(x, ...) \
    do { \
        PRINT("[ERROR]%s(%d): "x"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

osal_mutex g_ts_mutex;

static ext_errno ts_lock_init(void)
{
    ext_errno ret;
    ret = osal_mutex_init(&(g_ts_mutex));
    if (ret != EXT_ERR_SUCCESS) {
        g_ts_mutex.mutex = NULL;
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static void ts_lock_deinit(void)
{
    osal_mutex_destroy(&(g_ts_mutex));
}

static void ts_lock(void)
{
    osal_mutex_lock(&(g_ts_mutex));
}

static void ts_unlock(void)
{
    osal_mutex_unlock(&(g_ts_mutex));
}

ts_queue_t *ts_init_queue(uint32_t capacity)
{
    ext_errno ret;
    ts_queue_t *q = (ts_queue_t *)osal_kmalloc(sizeof(ts_queue_t), OSAL_GFP_KERNEL);
    if (q == NULL) {
        return NULL;
    }
    q->cap = capacity;
    q->cnt = 0;
    q->head = q->tail = NULL;

    ret = ts_lock_init();
    if (ret != EXT_ERR_SUCCESS) {
        ts_mem_free(q);
    }

    return q;
}

int32_t ts_queue_get_length(ts_queue_t *q)
{
    return (int32_t)q->cnt;
}

int32_t ts_queue_is_empty(const ts_queue_t *q)
{
    return (q->cnt == 0);
}

int32_t ts_queue_is_full(const ts_queue_t *q)
{
    return (q->cnt == q->cap);
}

int32_t ts_de_queue(ts_queue_t *q, input_event_info *msg)
{
    if (q == NULL || q->head == NULL || msg == NULL || ts_queue_is_empty(q) != 0) {
        return -1;
    }

    ts_lock();
    struct ts_list *node = q->head;
    errno_t ret = memcpy_s(msg, sizeof(input_event_info), node->msg, sizeof(input_event_info));
    if (ret != EOK) {
        PRINT("de queue memcpy_s fail\r\n");
    }
    q->head = q->head->next;
    q->cnt--;

    ts_mem_free(node->msg);
    ts_mem_free(node);
    if (q->cnt == 0) {
        q->tail = NULL;
    }
    ts_unlock();

    return 0;
}

int32_t ts_en_queue(ts_queue_t *q, input_event_info *msg)
{
    if (q == NULL || msg == NULL) {
        return -1;
    }
    if (ts_queue_is_full(q) != 0) {
        input_event_info tmp_msg = {0};
        ts_de_queue(q, &tmp_msg);
    }

    struct ts_list *node = osal_kmalloc(sizeof(struct ts_list), OSAL_GFP_KERNEL);
    if (node == NULL) {
        return -1;
    }
    node->msg = (input_event_info *)osal_kmalloc(sizeof(input_event_info), OSAL_GFP_KERNEL);
    if (node->msg == NULL) {
        ts_mem_free(node);
        return -1;
    }

    errno_t ret = memcpy_s(node->msg, sizeof(input_event_info), msg, sizeof(input_event_info));
    if (ret != EOK) {
        PRINT("en queue memcpy_s fail\r\n");
    }
    node->next = NULL;

    ts_lock();
    if (ts_queue_is_empty(q) != 0) {
        q->head = node;
    } else {
        q->tail->next = node;
    }
    q->tail = node;
    q->cnt++;
    ts_unlock();

    return 0;
}

void ts_clear_queue(ts_queue_t *q)
{
    ts_lock();
    if (q == NULL) {
        ts_unlock();
        return;
    }
    struct ts_list *curr = q->head;
    struct ts_list *next = curr->next;

    while (curr != NULL) {
        ts_mem_free(curr->msg);
        ts_mem_free(curr);
        curr = next;
        if (next != NULL) {
            next = next->next;
        }
    }
    q->cnt = 0;
    ts_unlock();
}

void ts_delete_queue(ts_queue_t *q)
{
    ts_lock();
    if (q == NULL) {
        ts_unlock();
        return;
    }
    struct ts_list *curr = q->head;
    struct ts_list *next = curr->next;

    while (curr != NULL) {
        ts_mem_free(curr->msg);
        ts_mem_free(curr);
        curr = next;
        if (next != NULL) {
            next = next->next;
        }
    }
    q->cnt = 0;
    ts_mem_free(q);

    ts_unlock();
    ts_lock_deinit();
}