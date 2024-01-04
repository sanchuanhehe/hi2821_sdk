/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR TX Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */

#include <stdbool.h>
#include "soc_osal.h"
#include "securec.h"
#include "gpio_porting.h"
#include "gpio.h"
#include "ir_tx.h"

#define S_2_US                         1000000
#define US_2_NS                        1000
#define BIT_LEN_8                      8
#define PARAM_2                        2
#define GPIO_DELAY_NS                  7500
#define ONE_INSTRUCTION_CYCLE_NS       156

static pin_t g_gpio;

void ir_init(pin_t gpio)
{
    g_gpio = gpio;
    uapi_pin_set_mode(g_gpio, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(g_gpio, GPIO_DIRECTION_OUTPUT);
}

static void instruction_accurate_timing(int cycle)
{
    for (int i = 0; i < cycle; i++) {
        __asm__ __volatile__("nop");
    }
}

errcode_t ir_transmit(int freq, int *pattern, int len)
{
    if (freq <= 0 || pattern == NULL || len <= 0) {
        return ERRCODE_INVALID_PARAM;
    }

    int* level_cycle = (int*)malloc(sizeof(int) * len);
    int* delay_cycle = (int*)malloc(sizeof(int) * len);

    if (level_cycle == NULL || delay_cycle == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    int ir_freq = freq;
    int pulse_time = S_2_US / ir_freq;

    for (int i = 0; i < len; i++) {
        level_cycle[i] = pattern[i] / pulse_time;
        if (i % PARAM_2 == 0) {
            delay_cycle[i] = (pulse_time * US_2_NS - GPIO_DELAY_NS * PARAM_2) / (PARAM_2 * ONE_INSTRUCTION_CYCLE_NS);
        } else {
            delay_cycle[i] = (pulse_time * US_2_NS - GPIO_DELAY_NS) / ONE_INSTRUCTION_CYCLE_NS;
        }
    }

    uint32_t sts = osal_irq_lock();
    for (int i = 0; i < len; i++) {
        if (i % PARAM_2 == 0) {
            /* 高电平 */
            for (int j = 0; j < level_cycle[i]; j++) {
                uapi_gpio_set_val(g_gpio, GPIO_LEVEL_HIGH);
                instruction_accurate_timing(delay_cycle[i]);
                uapi_gpio_set_val(g_gpio, GPIO_LEVEL_LOW);
                instruction_accurate_timing(delay_cycle[i]);
            }
        } else {
            /* 低电平 */
            for (int j = 0; j < level_cycle[i]; j++) {
                uapi_gpio_set_val(g_gpio, GPIO_LEVEL_LOW);
                instruction_accurate_timing(delay_cycle[i]);
            }
        }
    }
    osal_irq_restore(sts);
    free(level_cycle);
    free(delay_cycle);
    return ERRCODE_SUCC;
}