/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Eflash Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-27, Create file. \n
 */
#include "eflash.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define EFLASH_TASK_STACK_SIZE        0x1000
#define EFLASH_TASK_PRIO              (osPriority_t)(17)
#define EFLASH_REGION_1               1
#define EFLASH_READ_LEN               10
#define EFLASH_ERGION_1_RWE_ADDR      0x1FFF00

uint32_t g_eflash_write_data[20];

static void init_eflash_write_data(void)
{
    uint8_t *write_value = (uint8_t *)&g_eflash_write_data;
    for (uint8_t i = 0; i < sizeof(g_eflash_write_data); i++) {
        *write_value = i;
        write_value++;
    }
}

static void *eflash_task(const char *arg)
{
    unused(arg);
    uint8_t read_buff[EFLASH_READ_LEN] = { 0 };

    uapi_eflash_init(EFLASH_REGION_1);
    uapi_eflash_set_freq(EFLASH_REGION_1, CLOCK_32M);

    uapi_eflash_read(EFLASH_ERGION_1_RWE_ADDR, (uint32_t *)read_buff, EFLASH_READ_LEN);
    for (uint32_t i = 0; i < EFLASH_READ_LEN; i++) {
        osal_printk("Eflash_region_1:%x = 0x%x\r\n", &read_buff[i], read_buff[i]);
    }

    uapi_eflash_erase(EFLASH_ERGION_1_RWE_ADDR, EFLASH_READ_LEN);
    init_eflash_write_data();
    uint32_t *value = (uint32_t *)((uint32_t)&g_eflash_write_data);
    uapi_eflash_write(EFLASH_ERGION_1_RWE_ADDR, value, EFLASH_READ_LEN);
    uapi_eflash_read(EFLASH_ERGION_1_RWE_ADDR, (uint32_t *)read_buff, EFLASH_READ_LEN);
    for (uint32_t i = 0; i < EFLASH_READ_LEN; i++) {
        osal_printk("Eflash_region_1:%x = 0x%x\r\n", &read_buff[i], read_buff[i]);
    }
    return NULL;
}

static void eflash_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "EflashTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = EFLASH_TASK_STACK_SIZE;
    attr.priority = EFLASH_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)eflash_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the eflash_entry. */
app_run(eflash_entry);