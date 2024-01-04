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
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "tcxo.h"

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

const spi_mouse_cfg_t g_paw3395db_cfg[] = {
    { WRITE, 0x3A, 0x5A, NULL },
    { DELAY, 5000, 0x00, NULL },
    { WRITE, 0x40, 0x80, NULL },
    { WRITE, 0x7F, 0x0E, NULL },
    { WRITE, 0x55, 0x0D, NULL },
    { WRITE, 0x56, 0x1B, NULL },
    { WRITE, 0x57, 0xE8, NULL },
    { WRITE, 0x58, 0xD5, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x42, 0xBC, NULL },
    { WRITE, 0x43, 0x74, NULL },
    { WRITE, 0x4B, 0x20, NULL },
    { WRITE, 0x4D, 0x00, NULL },
    { WRITE, 0x53, 0x0D, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x55, 0xCA, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x62, 0x64, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0x0F, NULL },
    { WRITE, 0x70, 0x02, NULL },
    { WRITE, 0x4A, 0x2A, NULL },
    { WRITE, 0x60, 0x26, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x6D, 0x70, NULL },
    { WRITE, 0x6E, 0x60, NULL },
    { WRITE, 0x6F, 0x04, NULL },
    { WRITE, 0x53, 0x02, NULL },
    { WRITE, 0x55, 0x11, NULL },
    { WRITE, 0x7D, 0x51, NULL },
    { WRITE, 0x7F, 0x08, NULL },
    { WRITE, 0x71, 0x4F, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x62, 0x1F, NULL },
    { WRITE, 0x63, 0x1F, NULL },
    { WRITE, 0x65, 0x03, NULL },
    { WRITE, 0x66, 0x03, NULL },
    { WRITE, 0x67, 0x1F, NULL },
    { WRITE, 0x68, 0x1F, NULL },
    { WRITE, 0x69, 0x03, NULL },
    { WRITE, 0x6A, 0x03, NULL },
    { WRITE, 0x6C, 0x1F, NULL },
    { WRITE, 0x6D, 0x1F, NULL },
    { WRITE, 0x51, 0x04, NULL },
    { WRITE, 0x53, 0x20, NULL },
    { WRITE, 0x54, 0x20, NULL },
    { WRITE, 0x71, 0x0F, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x4C, 0x14, NULL },
    { WRITE, 0x55, 0x19, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x63, 0x16, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x41, 0x30, NULL },
    { WRITE, 0x55, 0x14, NULL },
    { WRITE, 0x49, 0x0A, NULL },
    { WRITE, 0x42, 0x00, NULL },
    { WRITE, 0x44, 0x0A, NULL },
    { WRITE, 0x5A, 0x0A, NULL },
    { WRITE, 0x5F, 0x1E, NULL },
    { WRITE, 0x5B, 0x05, NULL },
    { WRITE, 0x5E, 0x0F, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x48, 0xDC, NULL },
    { WRITE, 0x5A, 0x29, NULL },
    { WRITE, 0x5B, 0x47, NULL },
    { WRITE, 0x5C, 0x81, NULL },
    { WRITE, 0x5D, 0x40, NULL },
    { WRITE, 0x71, 0xDC, NULL },
    { WRITE, 0x70, 0x07, NULL },
    { WRITE, 0x73, 0x00, NULL },
    { WRITE, 0x72, 0x08, NULL },
    { WRITE, 0x75, 0xDC, NULL },
    { WRITE, 0x74, 0x07, NULL },
    { WRITE, 0x77, 0x00, NULL },
    { WRITE, 0x76, 0x08, NULL },
    { WRITE, 0x7F, 0x10, NULL },
    { WRITE, 0x4C, 0xD0, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x63, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x63, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x77, 0x4F, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x5B, 0x40, NULL },
    { WRITE, 0x66, 0x13, NULL },
    { WRITE, 0x67, 0x0F, NULL },
    { WRITE, 0x78, 0x01, NULL },
    { WRITE, 0x79, 0x9C, NULL },
    { WRITE, 0x55, 0x02, NULL },
    { WRITE, 0x23, 0x70, NULL },
    { WRITE, 0x22, 0x01, NULL },
    { DELAY, 1000, 0x00, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_mid_init_1 },
    { WRITE, 0x22, 0x00, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x40, 0x00, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x41, 0x30, NULL },
    { WRITE, 0x43, 0x20, NULL },
    { WRITE, 0x44, 0x0D, NULL },
    { WRITE, 0x4A, 0x12, NULL },
    { WRITE, 0x4B, 0x09, NULL },
    { WRITE, 0x4C, 0x30, NULL },
    { WRITE, 0x4E, 0x08, NULL },
    { WRITE, 0x53, 0x16, NULL },
    { WRITE, 0x55, 0x14, NULL },
    { WRITE, 0x5A, 0x0D, NULL },
    { WRITE, 0x5B, 0x05, NULL },
    { WRITE, 0x5F, 0x1E, NULL },
    { WRITE, 0x66, 0x30, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x6E, 0x0F, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x71, 0x0F, NULL },
    { WRITE, 0x72, 0x0A, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4E, 0x09, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x40, 0x80, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x4D, 0x01, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x54, 0x01, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x44, 0x44, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x48, 0xDD, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { READ, 0x02, 0x80, NULL },
    { READ, 0x03, 0xFE, NULL },
    { READ, 0x04, 0xFF, NULL },
    { READ, 0x05, 0x01, NULL },
    { READ, 0x06, 0x00, NULL },
    { WRITE, 0x68, 0x01, NULL },
    { WRITE, 0x48, 0x3F, NULL },
    { WRITE, 0x49, 0x00, NULL },
    { WRITE, 0x4A, 0x3F, NULL },
    { WRITE, 0x4B, 0x00, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x48, 0xDC, NULL },
    { WRITE, 0x7F, 0x00, NULL },
};

static void mouse_sensor_mid_init_1(void)
{
    for (int i = 0; i < MOUSE_3395_READ_TIMES; i++) {
        if (mouse_spi_read_reg(MOUSE_3395_READ_REG) == MOUSE_3395_READ_TARGET_VAL) {
            return;
        }
        uapi_tcxo_delay_us(MOUSE_3395_READ_DELAY);
    }
    spi_mouse_cfg_t cfg[] = {
        {0x1, 0x7F, 0x14, NULL},
        {0x1, 0x6C, 0x00, NULL},
        {0x1, 0x7F, 0x00, NULL},
    };

    mouse_sensor_spi_opration(cfg, sizeof(cfg) / sizeof(spi_mouse_cfg_t));
}

static void paw3395_chaneg_dpi(void)
{
    spi_mouse_cfg_t dpi_reg[] = {
        { WRITE, 0x5A, 0x90, NULL },
        { WRITE, 0x48, 0x27, NULL },
        { WRITE, 0x4A, 0x27, NULL },
        { WRITE, 0x47, 0x01, NULL },
    };
    mouse_sensor_spi_opration(dpi_reg, sizeof(dpi_reg) / sizeof(spi_mouse_cfg_t));
}

static mouse_freq_t paw_3395_mouse_init(void)
{
    mouse_sensor_spi_open();
    mouse_sensor_spi_opration(g_paw3395db_cfg, sizeof(g_paw3395db_cfg)/ sizeof(spi_mouse_cfg_t));
    paw3395_chaneg_dpi();
    return MOUSE_FREQ_8K;
}

static void paw3395_get_xy(int16_t *x, int16_t *y)
{
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_spi_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    *x = ((recv_motion_data[X_LOW_8BIT] | (recv_motion_data[X_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
    *y = ((recv_motion_data[Y_LOW_8BIT] | (recv_motion_data[Y_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
}

mouse_sensor_oprator_t g_usb_paw3395_operator = {
    .get_xy = paw3395_get_xy,
    .init = paw_3395_mouse_init,
};

mouse_sensor_oprator_t usb_mouse_get_paw3395_operator(void)
{
    return g_usb_paw3395_operator;
}