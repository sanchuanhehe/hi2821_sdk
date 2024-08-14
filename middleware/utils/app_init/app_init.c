/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides the entries for initializing and starting services and features. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-14, Create file. \n
 */
#include "app_init.h"

extern init_call_t __zinitcall_run_start;
extern init_call_t __zinitcall_run_end;

void app_tasks_init(void)
{
    init_call_t *initcall = &__zinitcall_run_start;
    init_call_t *initend = &__zinitcall_run_end;
    for (; initcall < initend; initcall++) {
        (*initcall)();
    }
}

__attribute__((weak)) void system_init(void)
{
}