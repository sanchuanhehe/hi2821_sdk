/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides power management api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "securec.h"
#include "common_def.h"
#include "soc_osal.h"
#include "osal_list.h"
#include "interrupt/osal_interrupt.h"
#include "errcode.h"
#include "pm_fsm.h"

typedef struct pm_fsm_list {
    pm_fsm_id_t fsm_id;
    pm_states_t state;
    pm_fsm_content_t content;
    struct osal_list_head node;
} pm_fsm_list_t;

static OSAL_LIST_HEAD(g_pm_fsm_list);

errcode_t uapi_pm_register_fsm_handler(pm_fsm_id_t id, pm_states_t state, pm_fsm_content_t *content)
{
    if (id >= PM_FSM_ID_MAX || state >= PM_STATE_MAX || content == NULL || content->handler == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    pm_fsm_list_t *fsm_list = NULL;
    uint32_t irq_sts = osal_irq_lock();
    osal_list_for_each_entry(fsm_list, &g_pm_fsm_list, node) {
        if (fsm_list->fsm_id == id && fsm_list->state == state) {
            fsm_list->content.len = content->len;
            fsm_list->content.handler = content->handler;
            (void)memcpy_s(fsm_list->content.data, content->len, content->data, content->len);
            osal_irq_restore(irq_sts);
            return ERRCODE_SUCC;
        }
    }
    osal_irq_restore(irq_sts);
    pm_fsm_list_t *fsm_node = (pm_fsm_list_t *)osal_kmalloc(sizeof(pm_fsm_list_t), 0);
    if (fsm_node == NULL) {
        return ERRCODE_MALLOC;
    }
    fsm_node->fsm_id = id;
    fsm_node->state = state;
    (void)memcpy_s(&fsm_node->content, sizeof(pm_fsm_content_t), content, sizeof(pm_fsm_content_t));
    if (fsm_node->content.len != 0) {
        fsm_node->content.data = osal_kmalloc(content->len + 1, 0);
        if (fsm_node->content.data == NULL) {
            osal_kfree(fsm_node);
            return ERRCODE_MALLOC;
        }

        (void)memcpy_s(fsm_node->content.data, content->len, content->data, content->len);
        fsm_node->content.data[content->len] = '\0';
    }
    uint32_t irq_sts1 = osal_irq_lock();
    osal_list_add_tail(&fsm_node->node, &g_pm_fsm_list);
    osal_irq_restore(irq_sts1);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_unregister_fsm_handler(pm_fsm_id_t id, pm_states_t state)
{
    if (id >= PM_FSM_ID_MAX || state >= PM_STATE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    pm_fsm_list_t *pm_fsm_list = NULL;
    pm_fsm_list_t *pm_fsm_list_tmp = NULL;
    uint32_t irq_sts = osal_irq_lock();
    osal_list_for_each_entry_safe(pm_fsm_list, pm_fsm_list_tmp, &g_pm_fsm_list, node) {
        if (pm_fsm_list->fsm_id == id && pm_fsm_list->state == state) {
            osal_list_del(&pm_fsm_list->node);
            osal_kfree(pm_fsm_list->content.data);
            osal_kfree(pm_fsm_list);
            break;
        }
    }

    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_process_fsm_handler(pm_fsm_id_t id, pm_states_t state)
{
    if (id >= PM_FSM_ID_MAX || state >= PM_STATE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    pm_fsm_content_t *content = NULL;
    pm_fsm_list_t *fsm_list = NULL;
    uint32_t irq_sts = osal_irq_lock();
    osal_list_for_each_entry(fsm_list, &g_pm_fsm_list, node) {
        if (fsm_list->fsm_id == id && fsm_list->state == state) {
            content = &(fsm_list->content);
            break;
        }
    }
    osal_irq_restore(irq_sts);

    if (content != NULL && content->handler != NULL) {
        content->handler(content->data, content->len);
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}
