/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides uart port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "uart_porting.h"
#include "hal_uart_v151.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "uart.h"
#include "platform_core.h"
#include "osal_interrupt.h"
#include "std_def.h"
#include "debug_print.h"
#include "osal_interrupt.h"
#include "arch_port.h"
#if defined(CONFIG_UART_SUPPORT_LPC)
#include "pm_clock.h"
#endif

#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma_porting.h"
#endif
#define UART_BUS_0_BASE_ADDR UART0_BASE
#define UART_BUS_1_BASE_ADDR UART1_BASE
#define UART_BUS_2_BASE_ADDR UART2_BASE
#define UART_TRANS_LEN_MAX   1024

#define CLK_32M     32000000ULL
#define CLK_64M     64000000ULL

typedef struct {
    uint16_t tx_mode;
    uint16_t rx_mode;
}pin_mode_map_t;

const uintptr_t g_uart_base_addrs[UART_BUS_MAX_NUM] = {
#if UART_BUS_MAX_NUM > 0
    (uintptr_t)UART_BUS_0_BASE_ADDR,
#endif
#if UART_BUS_MAX_NUM > 1
    (uintptr_t)UART_BUS_1_BASE_ADDR,
#endif
#if UART_BUS_MAX_NUM > 2
    (uintptr_t)UART_BUS_2_BASE_ADDR,
#endif
};

uintptr_t uart_porting_base_addr_get(uart_bus_t bus)
{
    return g_uart_base_addrs[bus];
}

#if defined (BUILD_LOADBOOT) || defined(BUILD_ROMBOOT)  || defined(BUILD_FLASHBOOT)
static const uart_pin_config_t g_uart0_pin_config_map[] = {
    /* UART_BUS_0_PORT0 */
    {
        .tx_pin     = S_MGPIO19,
        .rx_pin     = S_MGPIO20,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT1 */
    {
        .tx_pin     = S_MGPIO23,
        .rx_pin     = S_MGPIO24,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT2 */
    {
        .tx_pin     = S_MGPIO26,
        .rx_pin     = S_MGPIO29,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT3 */
    {
        .tx_pin     = S_MGPIO3,
        .rx_pin     = S_MGPIO4,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT4 */
    {
        .tx_pin     = S_MGPIO7,
        .rx_pin     = S_MGPIO8,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT5 */
    {
        .tx_pin     = S_MGPIO5,
        .rx_pin     = S_MGPIO6,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT6 */
    {
        .tx_pin     = S_MGPIO9,
        .rx_pin     = S_MGPIO10,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
    /* UART_BUS_0_PORT7 */
    {
        .tx_pin     = S_MGPIO15,
        .rx_pin     = S_MGPIO16,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
};
#else
static pin_mode_map_t g_pin_mode_map[UART_BUS_MAX_NUM] = {
#if UART_BUS_MAX_NUM > 0
    {
        .tx_mode = HAL_PIO_UART_L0_TXD,
        .rx_mode = HAL_PIO_UART_L0_RXD
    },
#endif
#if UART_BUS_MAX_NUM > 1
    {
        .tx_mode = HAL_PIO_UART_H0_TXD,
        .rx_mode = HAL_PIO_UART_H0_RXD
    },
#endif
#if UART_BUS_MAX_NUM > 2
    {
        .tx_mode = HAL_PIO_UART_L1_TXD,
        .rx_mode = HAL_PIO_UART_L1_RXD
    },
#endif
};
#endif

static uart_pin_config_t g_pin_config_map[UART_BUS_MAX_NUM] = {
#if UART_BUS_MAX_NUM > 0
    /* UART_BUS_0 */
    {
        .tx_pin     = S_MGPIO19,
        .rx_pin     = S_MGPIO20,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
#endif
#if UART_BUS_MAX_NUM > 1
    /* UART_BUS_1 */
    {
#if defined(PRODUCT_CD23_MOUSE)
        .tx_pin     = S_MGPIO26,
        .rx_pin     = S_MGPIO27,
#else
        .tx_pin     = S_MGPIO17,
        .rx_pin     = S_MGPIO18,
#endif
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    },
#endif
#if UART_BUS_MAX_NUM > 2
    /* UART_BUS_2 */
    {
        .tx_pin     = PIN_NONE,
        .rx_pin     = PIN_NONE,
        .cts_pin    = PIN_NONE,
        .rts_pin    = PIN_NONE
    }
#endif
};

typedef struct uart_interrupt {
    core_irq_t irq_num;
    osal_irq_handler irq_func;
}
hal_uart_interrupt_t;

static const hal_uart_interrupt_t g_uart_interrupt_lines[UART_BUS_MAX_NUM] = {
#if UART_BUS_MAX_NUM > 0
    { UART_0_IRQN, (osal_irq_handler)irq_uart0_handler },
#endif
#if UART_BUS_MAX_NUM > 1
    { UART_1_IRQN, (osal_irq_handler)irq_uart1_handler },
#endif
#if UART_BUS_MAX_NUM > 2
    { UART_2_IRQN, (osal_irq_handler)irq_uart2_handler },
#endif
};

void uart_port_register_hal_funcs(uart_bus_t bus)
{
    hal_uart_register_funcs(bus, hal_uart_v151_funcs_get());
}

void uart_port_unregister_hal_funcs(uart_bus_t bus)
{
    hal_uart_unregister_funcs(bus);
}

uint32_t uart_port_get_clock_value(uart_bus_t bus)
{
    unused(bus);
    return CLK_32M;
}

void uart_port_config_pinmux(uart_bus_t bus)
{
#if defined (BUILD_LOADBOOT) || defined(BUILD_ROMBOOT)  || defined(BUILD_FLASHBOOT)
    uapi_pin_set_mode(g_pin_config_map[bus].rx_pin, (pin_mode_t)HAL_PIO_UART_L0_RXD);
    uapi_pin_set_mode(g_pin_config_map[bus].tx_pin, (pin_mode_t)HAL_PIO_UART_L0_TXD);
#else
    uapi_pin_set_mode(g_pin_config_map[bus].rx_pin, (pin_mode_t)g_pin_mode_map[bus].rx_mode);
    uapi_pin_set_mode(g_pin_config_map[bus].tx_pin, (pin_mode_t)g_pin_mode_map[bus].tx_mode);
#endif
}

void uart_port_register_irq(uart_bus_t bus)
{
#ifdef BUILD_ROMBOOT
    unused(bus);
    return;
#else
    osal_irq_request(g_uart_interrupt_lines[bus].irq_num, g_uart_interrupt_lines[bus].irq_func, NULL, NULL, NULL);
    osal_irq_set_priority(g_uart_interrupt_lines[bus].irq_num, irq_prio(g_uart_interrupt_lines[bus].irq_num));
    osal_irq_enable(g_uart_interrupt_lines[bus].irq_num);
#endif
}

#if UART_BUS_MAX_NUM > 0
void irq_uart0_handler(void)
{
    hal_uart_irq_handler(UART_BUS_0);
}
#endif

#if UART_BUS_MAX_NUM > 1
void irq_uart1_handler(void)
{
    hal_uart_irq_handler(UART_BUS_1);
}
#endif

#if UART_BUS_MAX_NUM > 2
void irq_uart2_handler(void)
{
    hal_uart_irq_handler(UART_BUS_2);
}
#endif

void uart_port_unregister_irq(uart_bus_t bus)
{
#if defined (BUILD_LOADBOOT) || defined(BUILD_ROMBOOT)  || defined(BUILD_FLASHBOOT)
    unused(bus);
    return;
#else
    osal_irq_disable(g_uart_interrupt_lines[bus].irq_num);
#endif
}

void uart_port_set_pending_irq(uart_bus_t uart)
{
    int_set_pendind_irq(g_uart_interrupt_lines[uart].irq_num);
}

#ifdef SW_UART_DEBUG
#include <stdio.h>
#include "securec.h"
#ifdef USE_CMSIS_OS
#define DEBUG_UART_RX_BUFFER_SIZE 32
#else
#define DEBUG_UART_RX_BUFFER_SIZE 1
#endif

uart_bus_t g_sw_debug_uart = SW_DEBUG_UART_BUS;
static bool g_sw_debug_uart_enabled = false;
uint8_t g_uart_rx_buffer[DEBUG_UART_RX_BUFFER_SIZE];

static void uart_rx_callback(const void *buf, uint16_t buf_len, bool remaining);

void sw_debug_uart_deinit(void)
{
    uapi_uart_deinit(g_sw_debug_uart);
    g_sw_debug_uart_enabled = false;
}

void sw_debug_set_uart_bus(uart_bus_t bus)
{
    if (bus >= UART_BUS_NONE) {
        return;
    }
    g_sw_debug_uart = bus;
}

void uart_rx_callback(const void *buf, uint16_t buf_len, bool remaining)
{
    UNUSED(remaining);
    if (!g_sw_debug_uart_enabled) {
        return;
    }
    uapi_uart_write(g_sw_debug_uart, (const void *)buf, buf_len, 0);
}

void sw_debug_uart_init(uint32_t baud_rate)
{
    uart_pin_config_t uart_pins;
    uart_attr_t uart_line_config;
    uart_buffer_config_t uart_buffer_config;
    // TX configuration
    uart_pins.tx_pin = CHIP_FIXED_TX_PIN;
    uart_pins.rts_pin = PIN_NONE;

    // RX configuration
    uart_pins.rx_pin = CHIP_FIXED_RX_PIN;
    uart_pins.cts_pin = PIN_NONE;

    uart_line_config.baud_rate = baud_rate;
    uart_line_config.data_bits = UART_DATA_BIT_8;
    uart_line_config.parity = UART_PARITY_NONE;
    uart_line_config.stop_bits = UART_STOP_BIT_1;

    uart_buffer_config.rx_buffer_size = DEBUG_UART_RX_BUFFER_SIZE;
    uart_buffer_config.rx_buffer = g_uart_rx_buffer;

    (void)uapi_uart_init(g_sw_debug_uart, &uart_pins, &uart_line_config, NULL, &uart_buffer_config);
    uapi_uart_register_rx_callback(g_sw_debug_uart, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                   DEBUG_UART_RX_BUFFER_SIZE, uart_rx_callback);

    g_sw_debug_uart_enabled = true;
}

void print_str(const char *str, ...)
{
    static uint8_t s[UART_TRANS_LEN_MAX];  // This needs to be large enough to store the string TODO Change magic number
    va_list args;
    int32_t str_len;

    if ((str == NULL) || (strlen(str) == 0)) {
        return;
    }

    //lint -esym(526, __builtin_va_start) -esym(628, __builtin_va_start)   Lint hopelessly confused about va_args
    va_start(args, str);  //lint !e530 Lint hopelessly confused about va_args
    str_len = vsprintf_s((char *)s, sizeof(s), str, args);
    va_end(args);

    if (str_len < 0) {
        return;  //lint !e527  unreachable code
    }
    uapi_uart_write(g_sw_debug_uart, (const void *)s, str_len, 0);
}
#endif

void hal_uart_clear_pending(uart_bus_t uart)
{
    switch (uart) {
#if UART_BUS_MAX_NUM > 0
        case UART_BUS_0:
            osal_irq_clear(UART_0_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUM > 1
        case UART_BUS_1:
            osal_irq_clear(UART_1_IRQN);
            break;
#endif
#if UART_BUS_MAX_NUM > 1
        case UART_BUS_2:
            osal_irq_clear(UART_2_IRQN);
            break;
#endif
        default:
            break;
    }
}

#if defined(CONFIG_UART_SUPPORT_DMA)
uint8_t uart_port_get_dma_trans_dest_handshaking(uart_bus_t bus)
{
    switch (bus) {
        case UART_BUS_0:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_L0_TX;
        case UART_BUS_1:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_H0_TX;
        case UART_BUS_2:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_L1_TX;
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}

uint8_t uart_port_get_dma_trans_src_handshaking(uart_bus_t bus)
{
    switch (bus) {
        case UART_BUS_0:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_L0_RX;
        case UART_BUS_1:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_H0_RX;
        case UART_BUS_2:
            return (uint8_t)HAL_DMA_HANDSHAKING_UART_L1_RX;
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}
#endif /* CONFIG_UART_SUPPORT_DMA */

uint32_t uart_porting_lock(uart_bus_t bus)
{
    unused(bus);
    return osal_irq_lock();
}

void uart_porting_unlock(uart_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
    osal_irq_restore(irq_sts);
}

#if defined (BUILD_LOADBOOT) || defined(BUILD_ROMBOOT)  || defined(BUILD_FLASHBOOT)
uint32_t uart_update_pincfg(uart_bus_t bus, uint8_t uart_pin_id, uintptr_t *pin_cfg)
{
    /* get uart pin cfg */
    if (memcpy_s(pin_cfg, sizeof(uart_pin_config_t), (uint32_t *)&g_uart0_pin_config_map[uart_pin_id],
        sizeof(uart_pin_config_t)) != EOK) {
        return ERRCODE_FAIL;
    }
    /* upadte uart pin map cfg */
    if (memcpy_s(&g_pin_config_map[bus], sizeof(uart_pin_config_t), (uint32_t *)&g_uart0_pin_config_map[uart_pin_id],
        sizeof(uart_pin_config_t)) != EOK) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

#if defined(CONFIG_UART_SUPPORT_LPC)
void uart_port_clock_enable(uart_bus_t bus, bool on)
{
    clock_control_type_t m_control_type;
    clock_mclken_aperp_type_t aperp_type;

    m_control_type = (on == true) ? CLOCK_CONTROL_MCLKEN_ENABLE : CLOCK_CONTROL_MCLKEN_DISABLE;

    switch (bus) {
        case UART_BUS_0:
            aperp_type = CLOCK_APERP_UART_L0_CLKEN;
            break;

        case UART_BUS_1:
            if (on) {
                uapi_clock_crg_config(CLOCK_CRG_ID_MCU_PERP_UART, CLOCK_CLK_SRC_TCXO, 0x1);
            } else {
                uapi_clock_crg_config(CLOCK_CRG_ID_MCU_PERP_UART, CLOCK_CLK_SRC_NONE, 0x1);
            }
            aperp_type = CLOCK_APERP_UART_H0_CLKEN;
            break;

        case UART_BUS_2:
            aperp_type = CLOCK_APERP_UART_L1_CLKEN;
            break;

        default:
            return;
    }

    uapi_clock_control(m_control_type, aperp_type);
}
#endif