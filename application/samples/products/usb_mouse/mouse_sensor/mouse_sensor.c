/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#include "mouse_sensor.h"

typedef mouse_sensor_oprator_t (*get_mouse_operator_t)(void);

get_mouse_operator_t g_get_mouse_sensor_operators[] = {
    usb_mouse_get_paw3395_operator
};

mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor)
{
    get_mouse_operator_t operator_func = g_get_mouse_sensor_operators[mouse_sensor];
    return operator_func();
}
