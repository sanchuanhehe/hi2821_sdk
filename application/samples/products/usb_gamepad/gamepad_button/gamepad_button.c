/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Gamepad Button source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */
#include "keyscan.h"
#include "keyscan_porting.h"
#include "osal_debug.h"
#include "gamepad_button.h"

uint8_t g_keyscan_map_usb[KEYSCAN_MAX_ROW][KEYSCAN_MAX_COL] = {
    { 0x29, 0x2B },
    { 0x3D, 0x3C },
    { 0x00, 0x39 },
};

void gamepad_button_init(gamepad_button_report_callback_t callback)
{
    osal_printk("gamepad_button_init.\r\n");
    uapi_set_keyscan_value_map((uint8_t **)g_keyscan_map_usb, KEYSCAN_MAX_ROW, KEYSCAN_MAX_COL);

    keyscan_porting_type_sel(SIX_KEYS_TYPE);

    uapi_keyscan_init(EVERY_ROW_PULSE_40_US, HAL_KEYSCAN_MODE_0, KEYSCAN_INT_VALUE_RDY);
    uapi_keyscan_register_callback((keyscan_report_callback_t)callback);
    uapi_keyscan_enable();
}