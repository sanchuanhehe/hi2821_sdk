/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test usb timer source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-09, Create file. \n
 */
#include "cmsis_os2.h"
#include "non_os.h"
#include "chip_io.h"
#include "td_base.h"
#include "osal_interrupt.h"
#include "timer_porting.h"
#include "arch_port.h"
#include "test_usb_timer.h"

#define TIMER_USER_MODE_EN   0x3
#define TIKES_PER_MS         (32000000 / 1000)
#define TIKES_PER_US         (32000000 / 1000000)

typedef struct {
    timer_callback_t func;
    uint32_t times;
} g_test_usb_timer_func_t;

g_test_usb_timer_func_t g_test_usb_timer_func = { 0 };

uint32_t g_test_timer_times = 0;

static void timer1_irq(void)
{
    timer_port_clear_eoi(1);
    int_clear_pending_irq(TIMER_1_IRQN);
    if (g_test_usb_timer_func.func == NULL) {
        test_usb_timer1_stop();
        return;
    }
    g_test_usb_timer_func.func();
    if (g_test_usb_timer_func.times == 0) {
        return;
    }
    g_test_timer_times++;
    if (g_test_timer_times == g_test_usb_timer_func.times) {
        test_usb_timer1_stop();
        g_test_timer_times = 0;
    }
}

// When the value of times is 0, the system never stops.
void test_usb_timer1_start(uint32_t ms, timer_callback_t cb, uint32_t times)
{
    // before config timer, disable it
    writel((TIMER_1_BASE_ADDR + TIMER_CONTROL_REG), 0);

    writel((TIMER_1_BASE_ADDR + TIMER_LOAD_COUNT), TIKES_PER_MS * ms);
    // user define mode and enable it
    writel((TIMER_1_BASE_ADDR + TIMER_CONTROL_REG), TIMER_USER_MODE_EN);
    osal_irq_request(TIMER_1_IRQN, (osal_irq_handler)timer1_irq, NULL, NULL, NULL);
    osal_irq_set_priority(TIMER_1_IRQN, irq_prio(TIMER_1_IRQN));
    osal_irq_enable(TIMER_1_IRQN);
    g_test_usb_timer_func.func = cb;
    g_test_usb_timer_func.times = times;
}

void test_usb_timer1_star_us(uint32_t us, timer_callback_t cb, uint32_t times)
{
    uapi_unused(times);
    // before config timer, disable it
    writel((TIMER_1_BASE_ADDR + TIMER_CONTROL_REG), 0);

    writel((TIMER_1_BASE_ADDR + TIMER_LOAD_COUNT), TIKES_PER_US * us);

    // user define mode and enable it
    writel((TIMER_1_BASE_ADDR + TIMER_CONTROL_REG), TIMER_USER_MODE_EN);
    osal_irq_request(TIMER_1_IRQN, (osal_irq_handler)cb, NULL, NULL, NULL);
    osal_irq_set_priority(TIMER_1_IRQN, irq_prio(TIMER_1_IRQN));
    osal_irq_enable(TIMER_1_IRQN);
    g_test_usb_timer_func.func = cb;
    g_test_usb_timer_func.times = times;
}

void test_usb_timer1_stop(void)
{
    writel((TIMER_1_BASE_ADDR + TIMER_CONTROL_REG), 0);
    osal_irq_disable(TIMER_1_IRQN);
    g_test_timer_times = 0;
    g_test_usb_timer_func.func = NULL;
    g_test_usb_timer_func.times = 0;
}

int test_usb_stop_simulate(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    test_usb_timer1_stop();
    return 0;
}
