/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm veto port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "uart.h"
#include "pm_veto_porting.h"

bool pm_port_get_customized_sleep_veto(void)
{
    // 检查uart tx状态，有数据传输则投票不睡眠。
    if (uapi_uart_has_pending_transmissions(UART_BUS_0) || uapi_uart_has_pending_transmissions(UART_BUS_1)) {
        return true;
    }
    return false;
}