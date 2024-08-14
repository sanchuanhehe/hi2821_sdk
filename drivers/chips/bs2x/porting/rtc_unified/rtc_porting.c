/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide rtc port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-02, Create file. \n
 */

#include "soc_osal.h"
#include "common_def.h"
#include "hal_rtc_v150.h"
#include "hal_rtc_v150_regs_def.h"
#include "chip_core_irq.h"
#include "rtc_porting.h"

#define RTC_BASE_ADDR                         0x57024000
#define RTC_0_BASE_ADDR                       (RTC_BASE_ADDR + 0x100)
#define RTC_1_BASE_ADDR                       (RTC_BASE_ADDR + 0x200)
#define RTC_2_BASE_ADDR                       (RTC_BASE_ADDR + 0x300)
#define RTC_3_BASE_ADDR                       (RTC_BASE_ADDR + 0x400)

typedef hal_rtc_funcs_t *(*hal_rtc_get_funcs_t)(void);

typedef struct {
    rtc_index_t                    index;
    hal_rtc_get_funcs_t            func;
} hal_rtc_index_func_t;

static uintptr_t g_rtc_base_addr[RTC_MAX_NUM] = {
    RTC_0_BASE_ADDR,
    RTC_1_BASE_ADDR,
    RTC_2_BASE_ADDR,
    RTC_3_BASE_ADDR,
};

static hal_rtc_index_func_t g_hal_funcs[RTC_MAX_NUM] = {
    { RTC_0, hal_rtc_v150_get_funcs },
    { RTC_1, hal_rtc_v150_get_funcs },
    { RTC_2, hal_rtc_v150_get_funcs },
    { RTC_3, hal_rtc_v150_get_funcs },
};

uintptr_t rtc_porting_comm_addr_get(void)
{
    return RTC_BASE_ADDR;
}

uintptr_t rtc_porting_base_addr_get(rtc_index_t index)
{
    return g_rtc_base_addr[index];
}

void rtc_port_register_hal_funcs(rtc_index_t index)
{
    hal_rtc_register_funcs(index, g_hal_funcs[index].func());
}

void rtc_port_unregister_hal_funcs(rtc_index_t index)
{
    hal_rtc_unregister_funcs(index);
}

static int rtc0_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_rtc_v150_irq_handler(RTC_0);
    osal_irq_clear(RTC_0_IRQN);
    return 0;
}

static int rtc1_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_rtc_v150_irq_handler(RTC_1);
    osal_irq_clear(RTC_1_IRQN);
    return 0;
}

static int rtc2_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_rtc_v150_irq_handler(RTC_2);
    osal_irq_clear(RTC_2_IRQN);
    return 0;
}


static int rtc3_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_rtc_v150_irq_handler(RTC_3);
    osal_irq_clear(RTC_3_IRQN);
    return 0;
}

static osal_irq_handler const rtc_irq_handler[RTC_MAX_NUM] = {
    rtc0_irq_handler,
    rtc1_irq_handler,
    rtc2_irq_handler,
    rtc3_irq_handler,
};

void rtc_port_register_irq(rtc_index_t index, uint32_t id, uint16_t priority)
{
    osal_irq_disable(id);
    osal_irq_request(id, rtc_irq_handler[index], NULL, NULL, NULL);
    osal_irq_set_priority(id, priority);
    osal_irq_enable(id);
}

void rtc_port_unregister_irq(rtc_index_t index, uint32_t id)
{
    unused(index);
    osal_irq_disable(id);
    osal_irq_free(id, NULL);
}

static uint32_t g_rtc_clock_value = CONFIG_RTC_CLOCK_VALUE;

void rtc_porting_clock_value_update(uint32_t clock)
{
    g_rtc_clock_value = clock;
}

uint32_t rtc_porting_clock_value_get(void)
{
    return g_rtc_clock_value;
}

uint64_t rtc_porting_ms_2_cycle(uint32_t ms)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return ((uint64_t)ms * clock_value / MS_PER_S);
}

uint32_t rtc_porting_cycle_2_ms(uint64_t cycle)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return (uint32_t)((cycle * MS_PER_S) / (clock_value));
}

uint64_t rtc_hw_porting_ms_2_cycle(uint64_t ms)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return (ms * (uint64_t)clock_value / MS_PER_S);
}

uint32_t rtc_porting_cycle_2_us(uint64_t cycle)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return (uint32_t)((cycle * (MS_PER_S * US_PER_MS)) / (clock_value));
}

uint32_t rtc_porting_compensat_by_tcxo(uint64_t diff)
{
    if (diff < RTC_COMPENSAT_1_CYCLE_BY_TCXO) {
        return 0;
    } else if (diff < RTC_COMPENSAT_2_CYCLE_BY_TCXO) {
        return RTC_1_CYCLE;
    } else if (diff < RTC_COMPENSAT_3_CYCLE_BY_TCXO) {
        return RTC_2_CYCLE;
    } else {
        return diff / RTC_COMPENSAT_1_CYCLE_BY_TCXO;
    }
}