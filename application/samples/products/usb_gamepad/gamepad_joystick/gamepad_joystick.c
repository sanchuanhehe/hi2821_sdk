/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Gamepad Joystick source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */
#include "osal_debug.h"
#include "gamepad_joystick.h"

void gamepad_joysticks_init(void)
{
    osal_printk("gamepad_joysticks_init.\r\n");
    uapi_adc_init(ADC_CLOCK_NONE);
    uapi_adc_power_en(AFE_GADC_MODE, true);
    adc_calibration(AFE_GADC_MODE, true, true, false);
    uapi_adc_manual_sample(GADC_CHANNEL_1);
}