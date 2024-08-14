/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides sleep veto source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-09， Create file. \n
 */

#include "securec.h"
#include "soc_osal.h"
#include "pm_veto.h"

#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
typedef struct {
    uint64_t veto_time;
    uint16_t veto_id;
    uint16_t veto_counts;
    uint32_t veto_lr;
    uint32_t veto_timeout;
} pm_veto_track_info_t;

typedef struct {
    pm_veto_track_info_t *track_info;
    uint32_t track_mask;
    uint8_t track_len;
    uint8_t track_loc;
} pm_veto_track_t;

pm_veto_track_t g_pm_veto_track = { 0 };
#endif

static pm_veto_t g_pm_veto_info = { 0 };

void uapi_pm_veto_init(void)
{
    (void)memset_s((void *)&g_pm_veto_info, sizeof(pm_veto_t), 0, sizeof(pm_veto_t));
    g_pm_veto_info.last_veto_id = PM_VETO_ID_MAX;
}

#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
errcode_t uapi_pm_veto_start_track(uint8_t veto_mask, uint8_t num)
{
    g_pm_veto_track.track_mask = veto_mask;
    g_pm_veto_track.track_len = num;
    g_pm_veto_track.track_loc = 0;

    if (g_pm_veto_track.track_info != NULL) {
        return ERRCODE_SUCC;
    }
    g_pm_veto_track.track_info = osal_kmalloc(sizeof(pm_veto_track_info_t) * num, 0);
    if (g_pm_veto_track.track_info == NULL) {
        return ERRCODE_MALLOC;
    }
    return ERRCODE_SUCC;
}

void uapi_pm_veto_stop_track(void)
{
    g_pm_veto_track.track_mask = 0;
    if (g_pm_veto_track.track_info != NULL) {
        osal_kfree(g_pm_veto_track.track_info);
    }
    g_pm_veto_track.track_loc = 0;
    g_pm_veto_track.track_info = NULL;
}
#endif

static void pm_add_sleep_veto_record(pm_veto_id_t veto_id, uint32_t timeout_ms)
{
    unused(veto_id);
    unused(timeout_ms);
#if defined(CONFIG_PM_VETO_TRACK_ENABLE) && (CONFIG_PM_VETO_TRACK_ENABLE == 1)
    uint8_t track_loc = g_pm_veto_track.track_loc;
    uint8_t track_len = g_pm_veto_track.track_len;
    if ((bit(veto_id) & g_pm_veto_track.track_mask) > 0) {
        g_pm_veto_track.track_info[track_loc].veto_id = veto_id;
        g_pm_veto_track.track_info[track_loc].veto_counts = g_pm_veto_info.veto_counts.total_counts;
        g_pm_veto_track.track_info[track_loc].veto_lr = g_pm_veto_info.last_veto_lr;
        g_pm_veto_track.track_info[track_loc].veto_time = PM_GET_CURRENT_MS;
        g_pm_veto_track.track_info[track_loc].veto_timeout = timeout_ms;

        g_pm_veto_track.track_loc = (track_loc == track_len) ? track_loc++ : 0;
    }
#endif
}

errcode_t uapi_pm_add_sleep_veto(pm_veto_id_t veto_id)
{
    if (veto_id >= PM_VETO_ID_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t status = osal_irq_lock();
    if (g_pm_veto_info.veto_counts.sub_counts[veto_id] == 0) {
        g_pm_veto_info.veto_counts.total_counts++;
        g_pm_veto_info.veto_counts.sub_counts[veto_id] = 1;
        g_pm_veto_info.last_veto_lr = PM_GET_LR;
        g_pm_veto_info.last_veto_id = veto_id;

        pm_add_sleep_veto_record(veto_id, 0);
    }
    osal_irq_restore(status);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_add_sleep_veto_with_timeout(pm_veto_id_t veto_id, uint32_t timeout_ms)
{
    if (veto_id >= PM_VETO_ID_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t status = osal_irq_lock();
    if (timeout_ms == 0) {
        g_pm_veto_info.veto_timeout_timestamp = 0;
    } else {
        uint64_t expect_timestamp = timeout_ms + PM_GET_CURRENT_MS;
        if (g_pm_veto_info.veto_timeout_timestamp < expect_timestamp) {
            g_pm_veto_info.veto_timeout_timestamp = expect_timestamp;
        }
    }

    g_pm_veto_info.last_veto_lr = PM_GET_LR;
    g_pm_veto_info.last_veto_id = veto_id;

    pm_add_sleep_veto_record(veto_id, timeout_ms);
    osal_irq_restore(status);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_remove_sleep_veto(pm_veto_id_t veto_id)
{
    if (veto_id >= PM_VETO_ID_MAX) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t status = osal_irq_lock();
    if (g_pm_veto_info.veto_counts.sub_counts[veto_id] > 0) {
        g_pm_veto_info.veto_counts.sub_counts[veto_id] = 0;
        g_pm_veto_info.veto_counts.total_counts--;
    }
    osal_irq_restore(status);
    return ERRCODE_SUCC;
}

bool uapi_pm_get_sleep_veto(void)
{
    if (g_pm_veto_info.veto_counts.total_counts != 0) {
        return true;
    }

    if (g_pm_veto_info.veto_timeout_timestamp > PM_GET_CURRENT_MS) {
        return true;
    }

    return pm_port_get_customized_sleep_veto();
}

pm_veto_t *uapi_pm_veto_get_info(void)
{
    return &g_pm_veto_info;
}