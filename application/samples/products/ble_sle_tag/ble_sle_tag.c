/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE SLE TAG Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */

#include "cmsis_os2.h"
#include "common_def.h"
#include "osal_debug.h"
#include "bts_le_gap.h"
#include "app_init.h"
#include "sle_connection_manager.h"
#include "ble_server/ble_server_adv.h"
#include "ble_server/ble_uuid_server.h"
#include "sle_server/sle_server_adv.h"
#include "sle_server/sle_uuid_server.h"

#define BLE_SLE_TAG_TASK_STACK_SIZE 0x800
#define BLE_SLE_TAG_TASK_PRIO (osPriority_t)(17)
#define BLE_SLE_TAG_TASK_DURATION_MS 15

static void *ble_sle_tag_task(const char *arg)
{
    unused(arg);

    ble_uuid_server_init();
    sle_uuid_server_init();

    return NULL;
}

static void ble_sle_tag_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "BLESLETagTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BLE_SLE_TAG_TASK_STACK_SIZE;
    attr.priority = BLE_SLE_TAG_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)ble_sle_tag_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the ble_mouse_entry. */
app_run(ble_sle_tag_entry);