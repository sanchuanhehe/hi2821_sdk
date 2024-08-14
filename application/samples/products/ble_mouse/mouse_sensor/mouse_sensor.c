/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */

#include "mouse_sensor.h"

typedef mouse_sensor_oprator_t (*get_mouse_operator)(void);

mouse_sensor_oprator_t get_mouse_sensor_operator(void)
{
    return ble_mouse_get_operator();
}
