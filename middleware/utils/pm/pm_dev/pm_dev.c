/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides device operation source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-09， Create file. \n
 */

#include "securec.h"
#include "soc_osal.h"
#include "osal_list.h"
#include "interrupt/osal_interrupt.h"
#include "pm_dev.h"

typedef struct pm_dev_node {
    struct osal_list_head node;
    pm_dev_id_t dev_id;
    pm_dev_ops_t dev_ops;
} pm_dev_node_t;

static OSAL_LIST_HEAD(g_pm_dev_list);

errcode_t uapi_pm_register_dev_ops(pm_dev_id_t dev_id, pm_dev_ops_t *dev_ops)
{
    if (dev_id >= PM_DEV_MAX || dev_ops == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    pm_dev_node_t *dev_list = NULL;
    uint32_t irq_sts = osal_irq_lock();
    osal_list_for_each_entry(dev_list, &g_pm_dev_list, node) {
        if (dev_list->dev_id == dev_id) {
            (void)memcpy_s(&dev_list->dev_ops, sizeof(pm_dev_ops_t), dev_ops, sizeof(pm_dev_ops_t));
            osal_irq_restore(irq_sts);
            return ERRCODE_SUCC;
        }
    }
    osal_irq_restore(irq_sts);

    pm_dev_node_t *dev_node = osal_kmalloc(sizeof(pm_dev_node_t), 0);
    if (dev_node == NULL) {
        return ERRCODE_MALLOC;
    }
    memset_s(dev_node, sizeof(pm_dev_node_t), 0, sizeof(pm_dev_node_t));
    dev_node->dev_id = dev_id;

    (void)memcpy_s(&dev_node->dev_ops, sizeof(pm_dev_ops_t), dev_ops, sizeof(pm_dev_ops_t));

    uint32_t irq_sts1 = osal_irq_lock();
    osal_list_add_tail(&dev_node->node, &g_pm_dev_list);
    osal_irq_restore(irq_sts1);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_unregister_dev_ops(pm_dev_id_t dev_id)
{
    if (dev_id >= PM_DEV_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    pm_dev_node_t *dev_list = NULL;
    pm_dev_node_t *dev_list_tmp = NULL;
    uint32_t irq_sts = osal_irq_lock();
    osal_list_for_each_entry_safe(dev_list, dev_list_tmp, &g_pm_dev_list, node) {
        if (dev_list->dev_id == dev_id) {
            osal_list_del(&dev_list->node);
            osal_kfree(dev_list);
            break;
        }
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_dev_suspend(void)
{
    pm_dev_node_t *dev_list = NULL;
    osal_list_for_each_entry(dev_list, &g_pm_dev_list, node) {
        if (dev_list->dev_ops.suspend != NULL) {
            uintptr_t arg = dev_list->dev_ops.suppend_arg;
            errcode_t ret;
            ret = dev_list->dev_ops.suspend(arg);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_dev_resume(void)
{
    pm_dev_node_t *dev_list = NULL;
    osal_list_for_each_entry_reverse(dev_list, &g_pm_dev_list, node) {
        if (dev_list->dev_ops.resume != NULL) {
            uintptr_t arg = dev_list->dev_ops.resume_arg;
            errcode_t ret;
            ret = dev_list->dev_ops.resume(arg);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
    }
    return ERRCODE_SUCC;
}