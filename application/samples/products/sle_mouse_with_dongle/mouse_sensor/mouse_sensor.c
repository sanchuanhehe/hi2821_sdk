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

get_mouse_operator g_get_mouse_sensor_operators[] = {
#ifdef CONFIG_SAMPLE_SUPPORT_SENSOR_3395
    sle_mouse_get_paw3395_operator
#endif
#ifdef CONFIG_SAMPLE_SUPPORT_SENSOR_3805
    sle_mouse_get_paw3805_operator
#endif
#ifdef CONFIG_SAMPLE_SUPPORT_SENSOR_3816
    sle_mouse_get_pmw3816_operator
#endif
};

mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor)
{
    get_mouse_operator operator_func = g_get_mouse_sensor_operators[mouse_sensor];
    return operator_func();
}
