/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: tma525b touch driver.
 * Author: @CompanyNameTag
 * Create: 2023-02-06
 */

#include "tma525b_ctrl.h"
#include "securec.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "soc_osal.h"
#include "chip_io.h"
#include "time.h"
#include "tcxo.h"
#include "i2c.h"
#include "gpio.h"
#include "pinctrl.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "touch_screen_def.h"

bool g_host_dev_init_flag = TD_FALSE;
tma525b_ctrl_attr g_ctrl_attr;
touch_callback g_tma525b_callback = NULL;

static void tma525b_gpio_isr_handle(pin_t pin, uintptr_t param)
{
    unused(pin);
    unused(param);
    if (g_tma525b_callback != NULL) {
        g_tma525b_callback();
    }
}

ext_errno tma525b_register_handle(tma525b_ctrl_ops *peri_data, touch_callback func)
{
    ext_errno ret;
    tma525b_ctrl_ops *ops = peri_data;
    if (peri_data == NULL || func == NULL) {
        tp_err("invalid func parameters!");
        return EXT_ERR_INVALID_PARAM;
    }

    g_tma525b_callback = func;

    gpio_select_core(TMA525B_INT_GPIO, CORE);
    uapi_gpio_set_dir(TMA525B_INT_GPIO, GPIO_DIRECTION_INPUT);
    ret = uapi_gpio_register_isr_func(TMA525B_INT_GPIO, GPIO_INTERRUPT_FALLING_EDGE, tma525b_gpio_isr_handle);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("tma525b int callback register fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_unregister_handle(tma525b_ctrl_ops *peri_data)
{
    ext_errno ret;
    tma525b_ctrl_ops *ops = peri_data;
    if (ops == NULL) {
        tp_err("invalid func parameters!");
        return EXT_ERR_INVALID_PARAM;
    }

    /* unregister interrupt handler */
    uapi_gpio_unregister_isr_func(TMA525B_INT_GPIO);
    g_tma525b_callback = NULL;

    return EXT_ERR_SUCCESS;
}

static ext_errno tp_peripheral_init_status_check(void)
{
    if (g_host_dev_init_flag != TD_TRUE) {
        return EXT_ERR_TP_DEV_INFO_NOT_REGISTER;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_ctrl_attr_check(const tma525b_ctrl_attr *device_attr)
{
    unused(device_attr);
    return EXT_ERR_SUCCESS;
}

static ext_errno tp_i2c_rd_wr_param_check(const uint8_t *data_buf, uint32_t data_len)
{
    unused(data_buf);
    unused(data_len);
    ext_errno ret = tp_peripheral_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_data_read(uint16_t reg_addr, uint8_t *data_buf, uint32_t data_len)
{
    errcode_t ret;
    uint8_t retry;
    i2c_data_t data;
    uint8_t tp_reg_addr[TMA525B_REG_BYTE_LEN];

    ret = tp_i2c_rd_wr_param_check(data_buf, data_len);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("tp data read param invalid! ret=0x%x", ret);
        return ret;
    }

    memset_s(data_buf, data_len, 0, data_len);
    tp_reg_addr[TMA525B_I2C_SEND_INDEX0] = reg_addr & 0xff;
    tp_reg_addr[TMA525B_I2C_SEND_INDEX1] = (reg_addr >> OFFSET_8_BITS) & 0xff;

    data.send_buf = tp_reg_addr;
    data.send_len = TMA525B_CMD_SEND_LEN;
    data.receive_buf = data_buf;
    data.receive_len = data_len;

    for (retry = 0; retry < TMA525B_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_read(TMA525B_I2C_BUS, TMA525B_I2C_ADDR, &data);
        if (ret == ERRCODE_SUCC) {
            break;
        }
    }
    if (ret != ERRCODE_SUCC) {
        tp_err("tp data write fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_cmd_write(uint16_t cmd)
{
    errcode_t ret;
    uint8_t retry;
    i2c_data_t data;
    uint8_t tp_reg_addr[TMA525B_CMD_SEND_LEN];

    ret = tp_peripheral_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("host peripheral not init! ret=0x%x", ret);
        return ret;
    }
    tp_reg_addr[TMA525B_I2C_SEND_INDEX0] = cmd & 0xff;
    tp_reg_addr[TMA525B_I2C_SEND_INDEX1] = (cmd >> OFFSET_8_BITS) & 0xff;

    data.send_buf = tp_reg_addr;
    data.send_len = TMA525B_CMD_SEND_LEN;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < TMA525B_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_write(TMA525B_I2C_BUS, TMA525B_I2C_ADDR, &data);
        if (ret == ERRCODE_SUCC) {
            break;
        }
    }
    if (ret != ERRCODE_SUCC) {
        tp_err("tp data write fail! ret=0x%x", ret);
        return ret;
    }
    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_reg_write(uint16_t reg_addr, uint16_t reg_cfg)
{
    errcode_t ret;
    uint8_t retry;
    i2c_data_t data;
    uint8_t tp_reg_addr[TMA525B_REG_WRITE_LEN];

    ret = tp_peripheral_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("host peripheral not init! ret=0x%x", ret);
        return ret;
    }

    tp_reg_addr[TMA525B_I2C_SEND_INDEX0] = reg_addr & 0xff;
    tp_reg_addr[TMA525B_I2C_SEND_INDEX1] = (reg_addr >> OFFSET_8_BITS) & 0xff;
    tp_reg_addr[TMA525B_I2C_SEND_INDEX2] = reg_cfg & 0xff;
    tp_reg_addr[TMA525B_I2C_SEND_INDEX3] = (reg_cfg >> OFFSET_8_BITS) & 0xff;

    data.send_buf = tp_reg_addr;
    data.send_len = TMA525B_REG_WRITE_LEN;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < TMA525B_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_write(TMA525B_I2C_BUS, TMA525B_I2C_ADDR, &data);
        if (ret == ERRCODE_SUCC) {
            break;
        }
    }
    if (ret != ERRCODE_SUCC) {
        tp_err("tp data write fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_data_write(uint16_t reg_addr, uint8_t *data_buf, uint32_t data_len)
{
    errcode_t ret;
    uint8_t retry;
    i2c_data_t data;
    uint8_t *write_data = NULL;
    uint32_t send_len;

    ret = tp_i2c_rd_wr_param_check(data_buf, data_len);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("tp data write param invalid! ret=0x%x", ret);
        return ret;
    }

    send_len = data_len + sizeof(uint16_t);

    write_data = (uint8_t *)osal_kmalloc(send_len, OSAL_GFP_KERNEL);
    if (write_data == NULL) {
        tp_err("tp send buff malloc fail!");
        return EXT_ERR_MALLOC_FAILURE;
    }

    write_data[TMA525B_I2C_SEND_INDEX0] = reg_addr & 0xff;
    write_data[TMA525B_I2C_SEND_INDEX1] = (reg_addr >> OFFSET_8_BITS) & 0xff;
    ret = (uint32_t)memcpy_s(write_data + sizeof(uint16_t), send_len - sizeof(uint16_t), data_buf, data_len);
    if (ret != EOK) {
        tp_err("tp send_buf content copy fail! ret=0x%x", ret);
        return ret;
    }

    data.send_buf = write_data;
    data.send_len = send_len;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < TMA525B_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_write(TMA525B_I2C_BUS, TMA525B_I2C_ADDR, &data);
        if (ret == ERRCODE_SUCC) {
            break;
        }
    }
    if (ret != ERRCODE_SUCC) {
        tp_err("tp data write fail! ret=0x%x", ret);
        return ret;
    }

    tp_mem_free(write_data);
    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_host_peripheral_init(tma525b_ctrl_attr *attr)
{
    ext_errno ret;

    if (attr == NULL) {
        tp_err("tma525b communication interface cfg is empty!");
        return EXT_ERR_FAILURE;
    }
    ret = tma525b_ctrl_attr_check(attr);
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("tp invalid dev attr! ret=0x%x", ret);
        return ret;
    }

    ret = (ext_errno)memcpy_s(&(g_ctrl_attr), sizeof(tma525b_ctrl_attr), attr, sizeof(tma525b_ctrl_attr));
    if (ret != EOK) {
        tp_err("tp attr content copy fail! ret=0x%x", ret);
        return ret;
    }

    /* init i2c */
    errcode_t result;
    result = uapi_i2c_master_init(TMA525B_I2C_BUS, 400000, I2C_SPEED_MODE_FS); /* 400000: 400KHz */
    if (result != EXT_ERR_SUCCESS) {
        tp_err("tp i2c init fail! ret=0x%x", result);
        return EXT_ERR_SUCCESS;
    }

    tp_print("TOUCH:i2c_id = %d  i2c_speed = %d  slave_addr  = 0x%x reset_gpio = %d\r\n",
        g_ctrl_attr.i2c_id, g_ctrl_attr.i2c_speed, TMA525B_I2C_ADDR, g_ctrl_attr.reset_gpio);
    g_host_dev_init_flag = true;

    return EXT_ERR_SUCCESS;
}

ext_errno tma525b_host_init_sample(void)
{
    ext_errno ret;
    ret = tma525b_host_peripheral_init(&g_ctrl_attr);
    ret = tma525b_init();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("tp host init fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

#ifdef PRE_ASIC
ext_errno tma525b_reset(void)
{
    uapi_gpio_set_dir(TMA525B_RESET_GPIO, GPIO_DIRECTION_OUTPUT);
    /* Config Gpio For Reset Pin */
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_HIGH);
    /* reset high delay 20ms */
    osDelay(20);
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_LOW);
    /* reset low delay 100ms */
    osDelay(100);
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_HIGH);
    /* reset high delay 200ms */
    osDelay(200);
    return EXT_ERR_SUCCESS;
}

void tma525b_resume_reset(void)
{
    uapi_gpio_set_dir(TMA525B_RESET_GPIO, GPIO_DIRECTION_OUTPUT);
    /* drvdata is global variable point and no need to handle null */
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_HIGH);
    osDelay(1);
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_LOW);
    /* delay 10ms */
    osDelay(10);
    uapi_gpio_set_val(TMA525B_RESET_GPIO, GPIO_LEVEL_HIGH);
    /* delay 100ms */
    osDelay(100);
    uapi_gpio_set_dir(TMA525B_RESET_GPIO, GPIO_DIRECTION_INPUT);
}
#else
ext_errno tma525b_reset(void)
{
    /* Config Gpio For Reset Pin */
    writel(TMA525B_RST_REG, TMA525B_RST_HIGH);
    /* reset high delay 20ms */
    osDelay(20);
    writel(TMA525B_RST_REG, TMA525B_RST_LOW);
    /* reset low delay 100ms */
    osDelay(100);
    writel(TMA525B_RST_REG, TMA525B_RST_HIGH);
    /* reset high delay 200ms */
    osDelay(200);
    /* Record Status For Reset */

    return EXT_ERR_SUCCESS;
}

void tma525b_resume_reset(void)
{
    /* drvdata is global variable point and no need to handle null */
    writel(TMA525B_RST_REG, TMA525B_RST_HIGH);
    osDelay(1);
    writel(TMA525B_RST_REG, TMA525B_RST_LOW);
    /* delay 10ms */
    osDelay(10);
    writel(TMA525B_RST_REG, TMA525B_RST_HIGH);
    /* delay 100ms */
    osDelay(100);
}
#endif
