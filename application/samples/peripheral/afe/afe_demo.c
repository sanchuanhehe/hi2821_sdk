/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: AFE AMIC Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-27, Create file. \n
 */
#include "gpio.h"
#include "pinctrl.h"
#include "adc.h"
#include "adc_porting.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define AFE_SET_GPIO_DIR                  0
#define AFE_SET_PIN_PULL                  0
#define AFE_SET_PIN_IE                    1
#define AFE_GADC_CHANNEL7                 7
#define AFE_GADC_CHANNEL6                 6
#define ADC_TASK_STACK_SIZE               0x1000
#define ADC_TASK_PRIO                     (osPriority_t)(17)

static void app_afe_set_io(pin_t pin)
{
    uapi_pin_set_mode(pin, CONFIG_AFE_PIN_MODE);
    uapi_gpio_set_dir(pin, AFE_SET_GPIO_DIR);
    uapi_pin_set_pull(pin, AFE_SET_PIN_PULL);
    uapi_pin_set_ie(pin, AFE_SET_PIN_IE);
}

static void *afe_task(const char *arg)
{
    UNUSED(arg);
    uapi_pin_init();
    uapi_gpio_init();
    osal_printk("---start afe sample test start---\r\n");
    app_afe_set_io(CONFIG_AFE_USE_PIN1);
    app_afe_set_io(CONFIG_AFE_USE_PIN2);
    uapi_adc_init(ADC_CLOCK_NONE);
    uapi_adc_power_en(AFE_AMIC_MODE, true);
    uapi_adc_open_differential_channel(AFE_GADC_CHANNEL7, AFE_GADC_CHANNEL6);
    adc_calibration(AFE_AMIC_MODE, true, true, true);
    uapi_adc_auto_sample(AMIC_CHANNEL_0);

    osal_printk("---start afe sample test end---\r\n");

    return NULL;
}

static void afe_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "AFETask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = ADC_TASK_STACK_SIZE;
    attr.priority = ADC_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)afe_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the afe_entry. */
app_run(afe_entry);