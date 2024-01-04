/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */


#include "stdbool.h"
#include "non_os.h"
#include "pinctrl.h"
#include "mouse_spi.h"
#include "high_speed_mouse.h"

#define PIN_MOTION                      S_MGPIO21
#define SPI_RECV_DATA_LEN           1
#define SPI_SEND_DATA_LEN           2
#define PAW3220_DATA_BIT_LEN        12

const spi_mouse_cfg_t g_paw3220db_cfg[] = {
    {WRITE, {{ 0x09, 0x5A }}},
    {WRITE, {{ 0x4B, 0x00 }}},
    {WRITE, {{ 0x5C, 0xD4 }}},
    {WRITE, {{ 0x0D, 0x1A }}},
    {WRITE, {{ 0x0E, 0x1C }}},
    {WRITE, {{ 0x7F, 0x01 }}},
    {WRITE, {{ 0x42, 0x4F }}},
    {WRITE, {{ 0x43, 0x93 }}},
    {WRITE, {{ 0x44, 0x48 }}},
    {WRITE, {{ 0x45, 0xF2 }}},
    {WRITE, {{ 0x47, 0x4F }}},
    {WRITE, {{ 0x48, 0x93 }}},
    {WRITE, {{ 0x49, 0x48 }}},
    {WRITE, {{ 0x4A, 0xF3 }}},
    {WRITE, {{ 0x64, 0x66 }}},
    {WRITE, {{ 0x79, 0x08 }}},
    {WRITE, {{ 0x7F, 0x00 }}},
    {WRITE, {{ 0x09, 0x00 }}},
};

static void paw3220_get_xy(int16_t *x, int16_t *y)
{
    non_os_enter_critical();
    uint8_t motion = mouse_read_reg(0x2);
    if (!(motion & 0x80)) {
        non_os_exit_critical();
        *x = 0;
        *y = 0;
        return;
    }
    uint8_t _x = mouse_read_reg(0x3);
    uint8_t _y = mouse_read_reg(0x4);
    uint8_t xy = mouse_read_reg(0x12);
    /*
    * Sensor combines 12-bit x and y data into three uint8_ts for transmission
    * The higher four bits of recv_delta_xy are the higher four bits of the 12-bit x data
    * The lower four bits of recv_delta_xy are the higher four bits of the 12-bit y data
    */

    int16_t temp_x =  (_x | ((xy & 0xf0) << 4));
    int16_t temp_y =  (_y | ((xy & 0x0f) << 8));
    *x = trans_to_16_bit((uint16_t)temp_x, PAW3220_DATA_BIT_LEN);
    *y = -trans_to_16_bit((uint16_t)temp_y, PAW3220_DATA_BIT_LEN);
    non_os_exit_critical();
}

static mouse_freq_type_t paw_3220_mouse_init(void)
{
    mouse_spi_open();
    mouse_opration(g_paw3220db_cfg, sizeof(g_paw3220db_cfg) / sizeof(spi_mouse_cfg_t));
    uint8_t pid = mouse_read_reg(1);
    print("mouse3220 pid1:%d\r\n", pid);
    pid = mouse_read_reg(0);
    print("mouse3220 pid0:%d\r\n", pid);
    return MOUSE_FREQ_2K;
}

high_mouse_oprator_t g_paw3220_operator = {
    .get_xy = paw3220_get_xy,
    .init = paw_3220_mouse_init,
};

high_mouse_oprator_t mouse_get_paw3220_operator(void)
{
    return g_paw3220_operator;
}