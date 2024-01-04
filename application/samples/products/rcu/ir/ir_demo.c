/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-10, Create file. \n
 */

#include "cmsis_os2.h"
#include "common_def.h"
#include "pinctrl.h"
#include "app_init.h"
#include "osal_debug.h"
#include "ir_nec.h"

#define IR_TASK_STACK_SIZE 0x8000
#define IR_TASK_PRIO (osPriority_t)(17)
#define IR_TASK_DELAY_MS 10
#define IR_DELAY_MS                 1000
#define USER_CODE_H                 0xED
#define DATA_CODE                   0x66

static void *ir_task(const char *arg)
{
    unused(arg);

    osal_printk("start ir sample\r\n");
    ir_transmit_nec(USER_CODE_H, DATA_CODE);
    return NULL;
}

static void ir_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "IRTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = IR_TASK_STACK_SIZE;
    attr.priority = IR_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)ir_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the eflash_entry. */
app_run(ir_entry);
