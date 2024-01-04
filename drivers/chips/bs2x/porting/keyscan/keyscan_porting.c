/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides keyscan port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-15， Create file. \n
 */

#include "chip_io.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "platform_types.h"
#include "platform_core.h"
#include "arch_port.h"
#include "soc_osal.h"
#include "oal_interface.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "timer.h"
#include "pm_veto.h"
#include "pm_veto_porting.h"
#include "tcxo.h"
#include "hal_keyscan_v150.h"
#include "hal_keyscan_v150_regs_op.h"
#include "hal_keyscan_v150_regs_def.h"
#include "keyscan_porting.h"

#define KEYSCAN_BASE_ADDR 0x5208D000
#define M_CTL_BASE_ADDR 0x520004A0

uintptr_t g_keyscan_base_addr =  (uintptr_t)KEYSCAN_BASE_ADDR;
uintptr_t g_keyscan_m_ctl_base_addr =  (uintptr_t)M_CTL_BASE_ADDR;
keyscan_keys_type_t g_keyboard_type_sel = FULL_KEYS_TYPE;

uint32_t g_gpio_map[KEYSCAN_GPIO_NUM] = {2, 3, 4, 5, 6, 21, 9, 29, 31, 24, 14, 23, 27,
                                         28, 10, 11, 30, 13, 15, 16, 25, 26, 12, 22};

uint32_t g_gpio_six_map[KEYSCAN_GPIO_SIX_NUM] = {2, 3, 31, 24, 14};

#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
static timer_handle_t g_keyscan_timer = NULL;

static void keyscan_timer_irq(uintptr_t data)
{
    unused(data);
    hal_keyscan_regs_set_slp_req(1);
    uint64_t base_time = uapi_tcxo_get_us();
    uint64_t cur_time = base_time;
    while (cur_time - base_time < REQ_WAIT_FOR_ACK) {
        if (hal_keyscan_regs_get_slp_step_sta(KEYSCAN_SLEEP_ACK) == 1) {
            hal_keyscan_regs_set_clk_keep(1);
            uapi_pm_remove_sleep_veto(PM_VETO_ID_KEYSCAN);
            return;
        }
        cur_time = uapi_tcxo_get_us();
    }
    hal_keyscan_regs_set_slp_req(0);
}
#endif

uintptr_t keyscan_porting_base_addr_get(void)
{
    return g_keyscan_base_addr;
}

uintptr_t keyscan_m_ctl_porting_base_addr_get(void)
{
    return g_keyscan_m_ctl_base_addr;
}

void keyscan_port_register_hal_funcs(void)
{
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    uapi_pm_add_sleep_veto(PM_VETO_ID_KEYSCAN);
#endif
    hal_keyscan_register_funcs(hal_keyscan_v150_funcs_get());
}

void keyscan_port_unregister_hal_funcs(void)
{
    hal_keyscan_unregister_funcs();
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    uapi_pm_remove_sleep_veto(PM_VETO_ID_KEYSCAN);
#endif
}

void keyscan_port_register_irq(uint32_t int_id)
{
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    uapi_timer_adapter(TIMER_INDEX_2, TIMER_2_IRQN, irq_prio(TIMER_2_IRQN));
    uapi_timer_create(TIMER_INDEX_2, &g_keyscan_timer);
#endif
    osal_irq_request(int_id, irq_keyscan_handler, NULL, NULL, NULL);
    osal_irq_set_priority(int_id, irq_prio(int_id));
    osal_irq_enable(int_id);
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    osal_irq_request(KEY_SCAN_LOW_POWER_IRQN, irq_keyscan_slp_handler, NULL, NULL, NULL);
    osal_irq_set_priority(KEY_SCAN_LOW_POWER_IRQN, irq_prio(KEY_SCAN_LOW_POWER_IRQN));
    osal_irq_enable(KEY_SCAN_LOW_POWER_IRQN);
    uapi_timer_start(g_keyscan_timer, CONFIG_KEYSCAN_IDLE_WAIT_US, keyscan_timer_irq, 1);
#endif
}

void keyscan_port_unregister_irq(uint32_t int_id)
{
    osal_irq_free(int_id, NULL);
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    uapi_timer_delete(g_keyscan_timer);
    osal_irq_free(KEY_SCAN_LOW_POWER_IRQN, NULL);
#endif
}

int irq_keyscan_handler(int a, void *tmp)
{
    unused(a);
    unused(tmp);
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    uapi_timer_stop(g_keyscan_timer);
#endif
    hal_keyscan_v150_irq();
    osal_irq_clear(KEY_SCAN_IRQN);
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    uapi_timer_start(g_keyscan_timer, CONFIG_KEYSCAN_IDLE_WAIT_US, keyscan_timer_irq, 1);
#endif
    return 0;
}

#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
int irq_keyscan_slp_handler(int a, void *tmp)
{
    unused(a);
    unused(tmp);
    if (hal_keyscan_regs_get_slp_step_sta(KEYSCAN_SLEEP_REQUIRE) == 0) {
        return 0;
    }
    uapi_pm_add_sleep_veto(PM_VETO_ID_KEYSCAN);
    hal_keyscan_regs_set_clk_keep(0);
    hal_keyscan_regs_set_slp_req(0);
    hal_keyscan_v150_irq();
    osal_irq_clear(KEY_SCAN_LOW_POWER_IRQN);
    uapi_timer_start(g_keyscan_timer, CONFIG_KEYSCAN_IDLE_WAIT_US, keyscan_timer_irq, 1);
    return 0;
}
#endif

void keyscan_porting_type_sel(keyscan_keys_type_t keys_type)
{
    g_keyboard_type_sel = keys_type;
}

keyscan_keys_type_t keyscan_porting_get_type_sel(void)
{
    return g_keyboard_type_sel;
}

void keyscan_porting_config_pins(void)
{
}

void keyscan_porting_pin_set(void)
{
    int gpio_index = 0;
    if (g_keyboard_type_sel == FULL_KEYS_TYPE) {
        for (pin_t i = S_MGPIO2; i <= S_MGPIO6; i++) {
            uapi_pin_set_mode(i, HAL_PIO_KEY_SCAN);
        }
        for (pin_t i = S_MGPIO9; i <= S_MGPIO16; i++) {
            uapi_pin_set_mode(i, HAL_PIO_KEY_SCAN);
        }
        for (pin_t i = S_MGPIO21; i <= S_MGPIO31; i++) {
            uapi_pin_set_mode(i, HAL_PIO_KEY_SCAN);
        }

        for (int i = 0; i <= NUM_OF_CFG_PIN_REGS; i++) {
            reg16_setbits(KEYSCAN_PIN_SEL_BASE_ADDR + (i * INTERVAL_OF_REGS), EVEN_SEL_BIT, BIT_SEL_LEN,
                          g_gpio_map[gpio_index]);
            gpio_index++;
            reg16_setbits(KEYSCAN_PIN_SEL_BASE_ADDR + (i * INTERVAL_OF_REGS), ODD_SEL_BIT, BIT_SEL_LEN,
                          g_gpio_map[gpio_index]);
            gpio_index++;
        }
    } else if (g_keyboard_type_sel == SIX_KEYS_TYPE) {
        for (uint8_t i = 0; i < KEYSCAN_GPIO_SIX_NUM; i++) {
            uapi_pin_set_mode((pin_t)g_gpio_six_map[i], HAL_PIO_KEY_SCAN);
        }

        reg16_setbits(KEYSCAN_SIX_TYPE_COL_REG, EVEN_SEL_BIT, BIT_SEL_LEN, g_gpio_six_map[gpio_index++]);
        reg16_setbits(KEYSCAN_SIX_TYPE_COL_REG, ODD_SEL_BIT, BIT_SEL_LEN, g_gpio_six_map[gpio_index++]);
        reg16_setbits(KEYSCAN_SIX_TYPE_ROW0_REG, EVEN_SEL_BIT, BIT_SEL_LEN, g_gpio_six_map[gpio_index++]);
        reg16_setbits(KEYSCAN_SIX_TYPE_ROW0_REG, ODD_SEL_BIT, BIT_SEL_LEN, g_gpio_six_map[gpio_index++]);
        reg16_setbits(KEYSCAN_SIX_TYPE_ROW1_REG, EVEN_SEL_BIT, BIT_SEL_LEN, g_gpio_six_map[gpio_index]);
    }
}