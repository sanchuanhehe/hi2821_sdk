/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#include "high_speed_mouse.h"

typedef high_mouse_oprator_t (*get_mouse_operator)(void);

get_mouse_operator g_high_speed_mouse_operators[] = {
    mouse_get_paw3395_operator,
    mouse_get_paw3320_operator,
    mouse_get_paw3816_operator,
};

high_mouse_oprator_t get_high_mouse_operator(mouse_sensor_t mouse)
{
    get_mouse_operator operator_func = g_high_speed_mouse_operators[mouse];
    return operator_func();
}
