/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR NEC Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */
#include "gpio.h"
#include "tcxo.h"
#include "ir_tx.h"
#include "ir_nec.h"

#define PATTERN_LEN          68
#define GPIO0_29             29
#define GPIO0_28             28
#define NEC_FREQ             38000
#define NEC_START_HIGH       9000
#define NEC_START_LOW        4500
#define NEC_END_HIGH         560
#define NEC_END_LOW          20000
#define NEC_ONE_H            560
#define NEC_ONE_L            1690
#define NEC_ZERO_H           560
#define NEC_ZERO_L           560
#define PARAM_2              2
#define PARAM_18             18
#define PARAM_34             34
#define PARAM_50             50
#define PARAM_66             66
#define PARAM_67             67
#define BIT_LEN_8            8
#define BIT_LEN_32           32
#define DATA_FILL_POSITION   0x80000000
#define LEVEL_ERROR          200

static int g_pattern[PATTERN_LEN] = {0};

static void ir_ie_one_data(uint8_t ie_one_data, int start_bit)
{
    int start = start_bit;
    uint8_t data = ie_one_data;
    for (int i = 0; i < BIT_LEN_8; i++) {
        if (data & 0x01) {
            /* send one */
            g_pattern[start + i] = NEC_ONE_H;
            g_pattern[start + 1 + i] = NEC_ONE_L;
        } else {
            /* send zero */
            g_pattern[start + i] = NEC_ZERO_H;
            g_pattern[start + 1 + i] = NEC_ZERO_L;
        }
        start++;
        data >>= 1;
    }
}

void ir_transmit_nec(uint8_t user_code_h, uint8_t data_code)
{
    /* start */
    g_pattern[0] = NEC_START_HIGH;
    g_pattern[1] = NEC_START_LOW;
    /* user_code_h */
    ir_ie_one_data(user_code_h, PARAM_2);
    /* user_code_l */
    ir_ie_one_data(~user_code_h, PARAM_18);
    /* data_code */
    ir_ie_one_data(data_code, PARAM_34);
    /* ~data_code */
    ir_ie_one_data(~data_code, PARAM_50);
    g_pattern[PARAM_66] = NEC_END_HIGH;
    g_pattern[PARAM_67] = NEC_END_LOW;

    ir_init((pin_t)GPIO0_29);
    ir_transmit(NEC_FREQ, g_pattern, PATTERN_LEN);
}

uint32_t ir_receive_nec(void)
{
    uapi_pin_set_mode((pin_t)GPIO0_28, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir((pin_t)GPIO0_28, GPIO_DIRECTION_INPUT);
    gpio_level_t gpio_level = uapi_gpio_get_val((pin_t)GPIO0_28);
    uint32_t* rx_pattern = (uint32_t*)malloc(sizeof(uint32_t) * PATTERN_LEN);
    uint32_t* time_us = (uint32_t*)malloc(sizeof(uint32_t) * PARAM_2);
    gpio_level_t ir_gpio_level;
    int count = 0;
    while (count <= PARAM_66) {
        ir_gpio_level = uapi_gpio_get_val((pin_t)GPIO0_28);
        if (gpio_level != ir_gpio_level) {
            if (count % PARAM_2 == 0 && count != 0) {
                time_us[1] = uapi_tcxo_get_us();
                rx_pattern[count++] = time_us[1] - time_us[0];
            } else if (count % PARAM_2 == 1) {
                time_us[0] = uapi_tcxo_get_us();
            }
            count++;
            gpio_level = ir_gpio_level;
        }
    }
    uint32_t data = 0;
    for (int i = 1; i <= BIT_LEN_32; i++) {
        if (rx_pattern[i] > (NEC_ONE_H - LEVEL_ERROR) && rx_pattern[i] < (NEC_ONE_H + LEVEL_ERROR)) {
            data = data >> 1;
        } else if (rx_pattern[i] > (NEC_ONE_L - LEVEL_ERROR) && rx_pattern[i] < (NEC_ONE_L + LEVEL_ERROR)) {
            data = data >> 1;
            data |= DATA_FILL_POSITION;         /* 接收数据为1时，给对应位补1 */
        }
    }
    free(rx_pattern);
    free(time_us);
    return data;
}