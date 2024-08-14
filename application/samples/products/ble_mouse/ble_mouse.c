/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE MOUSE Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-20, Create file. \n
 */
#include "soc_osal.h"
#include "common_def.h"
#include "osal_debug.h"
#include "app_init.h"
#include "bts_le_gap.h"
#include "ble_mouse_server/ble_mouse_server_adv.h"
#include "ble_mouse_server/ble_mouse_server.h"
#include "ble_mouse_server/ble_hid_mouse_server.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_init.h"

static void ble_mouse_entry(void)
{
    ble_mouse_init();
    ble_mouse_server_init();
}

/* Run the ble_mouse_entry. */
app_run(ble_mouse_entry);