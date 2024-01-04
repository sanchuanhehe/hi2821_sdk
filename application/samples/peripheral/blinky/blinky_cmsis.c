/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Blinky Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "boards.h"
#include "pinctrl.h"
#include "gpio.h"
#include "test_suite_log.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define BLINKY_TASK_STACK_SIZE    0x1000
#define BLINKY_TASK_PRIO          (osPriority_t)(17)

static void *blinky_task(const char *arg)
{
    unused(arg);

    uapi_pin_set_mode(BSP_LED_0, HAL_PIO_FUNC_GPIO);

    uapi_gpio_set_dir(BSP_LED_0, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED_0, GPIO_LEVEL_LOW);

    while (1) {
        osDelay(CONFIG_BLINKY_DURATION_MS);
        uapi_gpio_toggle(BSP_LED_0);
    }

    return NULL;
}

static void blinky_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "BlinkyTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BLINKY_TASK_STACK_SIZE;
    attr.priority = BLINKY_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)blinky_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the blinky_entry. */
app_run(blinky_entry);