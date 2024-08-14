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
#include "osal_task.h"
#include "app_init.h"

#define BLINKY_TASK_STACK_SIZE    0x400
#define BLINKY_TASK_PRIO          (osPriority_t)(17)

static int blinky_task(void *arg)
{
    unused(arg);

    uapi_pin_set_mode(BSP_LED_0, HAL_PIO_FUNC_GPIO);

    uapi_gpio_set_dir(BSP_LED_0, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(BSP_LED_0, GPIO_LEVEL_LOW);

    while (1) {
        osal_mdelay(CONFIG_BLINKY_DURATION_MS);
        uapi_gpio_toggle(BSP_LED_0);
        test_suite_log_stringf("Blinky working.\r\n");
    }

    return 0;
}

static void blinky_entry(void)
{
    if (osal_kthread_create(blinky_task, NULL, "Blinky Task", 0) == NULL) {
        /* Create task fail. */
    }
}

/* Run the blinky_entry. */
app_run(blinky_entry);