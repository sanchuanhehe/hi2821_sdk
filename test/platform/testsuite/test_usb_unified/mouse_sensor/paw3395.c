/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#include "stdbool.h"
#include "pinctrl.h"
#include "mouse_spi.h"
#include "high_speed_mouse.h"
#include "tcxo.h"
#include "watchdog.h"
#include "watchdog_porting.h"

#define BURST_MOTION_READ               0x16
#define READ_LENGTH                     6
#define POWER_UP_DELAY_MS               40

#define WRITE_RESET_ADDR                0xba     // Write operation, MSB is 1.
#define RESET_VALUE                     0x5a
#define READ_ID_ADDR                    0x00
#define LEN_1                           1
#define LEN_2                           2
#define RAW_DATA_GRAB_ADDR              0x58
#define RAW_DATA_GRAB_STATUS_ADDR       0x59
#define RAW_DATA_LEN                    1225
#define PIN_MOTION                      S_MGPIO21
#define USB_8K_MOUSE_REPORT_DELAY       125
#define MOUSE_TO_BT_DATA_LEN            5
#define MOUSE_3395_READ_TIMES           60
#define MOUSE_3395_READ_REG             0x6c
#define MOUSE_3395_READ_TARGET_VAL      0x80
#define MOUSE_3395_READ_DELAY           1000

#define BT_MOUSE_REPORT_PARAM_NUM       4
#define SPI_NUM_5                       5
#define XY_DATA_SHIFT_LEN               8
#define X_LOW_8BIT                      2
#define X_HIGH_8BIT                     3
#define Y_LOW_8BIT                      4
#define Y_HIGH_8BIT                     5

static void mouse_sensor_mid_init_1(void);

const spi_mouse_cfg_t g_paw3395db_cfg_1[] = {
    { WRITE, {{ 0x3a, 0x5A }} },
    { DELAY, { .delay = 5000 }},
    { WRITE, {{ 0x40, 0x80 }} },
    { WRITE, {{ 0x7f, 0x0E }} },
    { WRITE, {{ 0x55, 0x0D }} },
    { WRITE, {{ 0x56, 0x1B }} },
    { WRITE, {{ 0x57, 0xE8 }} },
    { WRITE, {{ 0x58, 0xD5 }} },
    { WRITE, {{ 0x7f, 0x14 }} },
    { WRITE, {{ 0x42, 0xBC }} },
    { WRITE, {{ 0x43, 0x74 }} },
    { WRITE, {{ 0x4b, 0x20 }} },
    { WRITE, {{ 0x4d, 0x00 }} },
    { WRITE, {{ 0x53, 0x0D }} },
    { WRITE, {{ 0x7f, 0x05 }} },
    { WRITE, {{ 0x51, 0x40 }} },
    { WRITE, {{ 0x53, 0x40 }} },
    { WRITE, {{ 0x55, 0xCA }} },
    { WRITE, {{ 0x61, 0x31 }} },
    { WRITE, {{ 0x62, 0x64 }} },
    { WRITE, {{ 0x6d, 0xB8 }} },
    { WRITE, {{ 0x6e, 0x0F }} },
    { WRITE, {{ 0x70, 0x02 }} },
    { WRITE, {{ 0x4a, 0x2A }} },
    { WRITE, {{ 0x60, 0x26 }} },
    { WRITE, {{ 0x7f, 0x06 }} },
    { WRITE, {{ 0x6d, 0x70 }} },
    { WRITE, {{ 0x6e, 0x60 }} },
    { WRITE, {{ 0x6f, 0x04 }} },
    { WRITE, {{ 0x53, 0x02 }} },
    { WRITE, {{ 0x55, 0x11 }} },
    { WRITE, {{ 0x7d, 0x51 }} },
    { WRITE, {{ 0x7f, 0x08 }} },
    { WRITE, {{ 0x71, 0x4F }} },
    { WRITE, {{ 0x7f, 0x09 }} },
    { WRITE, {{ 0x62, 0x1F }} },
    { WRITE, {{ 0x63, 0x1F }} },
    { WRITE, {{ 0x65, 0x03 }} },
    { WRITE, {{ 0x66, 0x03 }} },
    { WRITE, {{ 0x67, 0x1F }} },
    { WRITE, {{ 0x68, 0x1F }} },
    { WRITE, {{ 0x69, 0x03 }} },
    { WRITE, {{ 0x6a, 0x03 }} },
    { WRITE, {{ 0x6c, 0x1F }} },
    { WRITE, {{ 0x6d, 0x1F }} },
    { WRITE, {{ 0x51, 0x04 }} },
    { WRITE, {{ 0x53, 0x20 }} },
    { WRITE, {{ 0x54, 0x20 }} },
    { WRITE, {{ 0x71, 0x0F }} },
    { WRITE, {{ 0x7f, 0x0A }} },
    { WRITE, {{ 0x4a, 0x14 }} },
    { WRITE, {{ 0x4c, 0x14 }} },
    { WRITE, {{ 0x55, 0x19 }} },
    { WRITE, {{ 0x7f, 0x14 }} },
    { WRITE, {{ 0x63, 0x16 }} },
    { WRITE, {{ 0x7f, 0x0C }} },
    { WRITE, {{ 0x41, 0x30 }} },
    { WRITE, {{ 0x55, 0x14 }} },
    { WRITE, {{ 0x49, 0x0A }} },
    { WRITE, {{ 0x42, 0x00 }} },
    { WRITE, {{ 0x44, 0x0A }} },
    { WRITE, {{ 0x5a, 0x0A }} },
    { WRITE, {{ 0x5f, 0x1E }} },
    { WRITE, {{ 0x5b, 0x05 }} },
    { WRITE, {{ 0x5e, 0x0F }} },
    { WRITE, {{ 0x7f, 0x0D }} },
    { WRITE, {{ 0x48, 0xDC }} },
    { WRITE, {{ 0x5a, 0x29 }} },
    { WRITE, {{ 0x5b, 0x47 }} },
    { WRITE, {{ 0x5c, 0x81 }} },
    { WRITE, {{ 0x5d, 0x40 }} },
    { WRITE, {{ 0x71, 0xDC }} },
    { WRITE, {{ 0x70, 0x07 }} },
    { WRITE, {{ 0x73, 0x00 }} },
    { WRITE, {{ 0x72, 0x08 }} },
    { WRITE, {{ 0x75, 0xDC }} },
    { WRITE, {{ 0x74, 0x07 }} },
    { WRITE, {{ 0x77, 0x00 }} },
    { WRITE, {{ 0x76, 0x08 }} },
    { WRITE, {{ 0x7f, 0x10 }} },
    { WRITE, {{ 0x4c, 0xD0 }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { WRITE, {{ 0x4f, 0x63 }} },
    { WRITE, {{ 0x4e, 0x00 }} },
    { WRITE, {{ 0x52, 0x63 }} },
    { WRITE, {{ 0x51, 0x00 }} },
    { WRITE, {{ 0x77, 0x4F }} },
    { WRITE, {{ 0x47, 0x01 }} },
    { WRITE, {{ 0x5b, 0x40 }} },
    { WRITE, {{ 0x66, 0x13 }} },
    { WRITE, {{ 0x67, 0x0F }} },
    { WRITE, {{ 0x78, 0x01 }} },
    { WRITE, {{ 0x79, 0x9C }} },
    { WRITE, {{ 0x55, 0x02 }} },
    { WRITE, {{ 0x23, 0x70 }} },
    { WRITE, {{ 0x22, 0x01 }} },
    { DELAY, { .delay = 1000 }}
};
const spi_mouse_cfg_t g_paw3395db_cfg_2[] = {
    { WRITE, {{ 0x22, 0x00 }} },
    { WRITE, {{ 0x55, 0x00 }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { WRITE, {{ 0x40, 0x00 }} },
    { WRITE, {{ 0x7f, 0x0C }} },
    { WRITE, {{ 0x41, 0x30 }} },
    { WRITE, {{ 0x43, 0x20 }} },
    { WRITE, {{ 0x44, 0x0D }} },
    { WRITE, {{ 0x4a, 0x12 }} },
    { WRITE, {{ 0x4b, 0x09 }} },
    { WRITE, {{ 0x4c, 0x30 }} },
    { WRITE, {{ 0x4e, 0x08 }} },
    { WRITE, {{ 0x53, 0x16 }} },
    { WRITE, {{ 0x55, 0x14 }} },
    { WRITE, {{ 0x5a, 0x0D }} },
    { WRITE, {{ 0x5b, 0x05 }} },
    { WRITE, {{ 0x5f, 0x1E }} },
    { WRITE, {{ 0x66, 0x30 }} },
    { WRITE, {{ 0x7f, 0x05 }} },
    { WRITE, {{ 0x6e, 0x0F }} },
    { WRITE, {{ 0x7f, 0x09 }} },
    { WRITE, {{ 0x71, 0x0F }} },
    { WRITE, {{ 0x72, 0x0A }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { WRITE, {{ 0x7f, 0x0C }} },
    { WRITE, {{ 0x4e, 0x09 }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { WRITE, {{ 0x40, 0x80 }} },
    { WRITE, {{ 0x7f, 0x05 }} },
    { WRITE, {{ 0x4d, 0x01 }} },
    { WRITE, {{ 0x7f, 0x06 }} },
    { WRITE, {{ 0x54, 0x01 }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { WRITE, {{ 0x7f, 0x05 }} },
    { WRITE, {{ 0x44, 0x44 }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { WRITE, {{ 0x7f, 0x0D }} },
    { WRITE, {{ 0x48, 0xDD }} },
    { WRITE, {{ 0x7f, 0x00 }} },
    { READ, {{ 0x02, 0x80 }} },
    { READ, {{ 0x03, 0xFE }} },
    { READ, {{ 0x04, 0xFF }} },
    { READ, {{ 0x05, 0x01 }} },
    { READ, {{ 0x06, 0x00 }} },
    { WRITE, {{ 0x68, 0x01 }} },
    { WRITE, {{ 0x48, 0x3F }} },
    { WRITE, {{ 0x49, 0x00 }} },
    { WRITE, {{ 0x4a, 0x3F }} },
    { WRITE, {{ 0x4b, 0x00 }} },
    { WRITE, {{ 0x47, 0x01 }} },
    { WRITE, {{ 0x7f, 0x0D }} },
    { WRITE, {{ 0x48, 0xDC }} },
    { WRITE, {{ 0x7f, 0x00 }} },
};

static void mouse_sensor_mid_init_1(void)
{
    for (int i = 0; i < MOUSE_3395_READ_TIMES; i++) {
        if (mouse_read_reg(MOUSE_3395_READ_REG) == MOUSE_3395_READ_TARGET_VAL) {
            return;
        }
        uapi_tcxo_delay_us(MOUSE_3395_READ_DELAY);
    }
    spi_mouse_cfg_t cfg[] = {
        {0x1, {{ 0x7F, 0x14 }} },
        {0x1, {{ 0x6C, 0x00 }} },
        {0x1, {{ 0x7F, 0x00 }} },
    };

    mouse_opration(cfg, sizeof(cfg) / sizeof(spi_mouse_cfg_t));
}

static void paw3395_chaneg_dpi(void)
{
    spi_mouse_cfg_t dpi_reg[] = {
        { WRITE, {{ 0x5A, 0x90 }} },
        { WRITE, {{ 0x48, 0x27 }} },
        { WRITE, {{ 0x4A, 0x27 }} },
        { WRITE, {{ 0x47, 0x01 }} },
    };
    mouse_opration(dpi_reg, sizeof(dpi_reg) / sizeof(spi_mouse_cfg_t));
}

static mouse_freq_type_t paw_3395_mouse_init(void)
{
    print("3395 init\r\n");
    mouse_spi_open();
    mouse_opration(g_paw3395db_cfg_1, sizeof(g_paw3395db_cfg_1) / sizeof(spi_mouse_cfg_t));
    mouse_sensor_mid_init_1();
    mouse_opration(g_paw3395db_cfg_2, sizeof(g_paw3395db_cfg_2) / sizeof(spi_mouse_cfg_t));
    uint8_t pid = mouse_read_reg(0);
    paw3395_chaneg_dpi();
    print("pid:0x%x\r\n", pid);
    return MOUSE_FREQ_8K;
}

static void paw3395_get_xy(int16_t *x, int16_t *y)
{
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    *x = ((recv_motion_data[X_LOW_8BIT] | (recv_motion_data[X_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
    *y = ((recv_motion_data[Y_LOW_8BIT] | (recv_motion_data[Y_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
}

high_mouse_oprator_t g_paw3395_operator = {
    .get_xy = paw3395_get_xy,
    .init = paw_3395_mouse_init,
};

high_mouse_oprator_t mouse_get_paw3395_operator(void)
{
    print("3395\n");
    return g_paw3395_operator;
}