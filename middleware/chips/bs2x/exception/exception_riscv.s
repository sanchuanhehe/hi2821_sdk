/******************************************************************************
 * @brief    exception asm module implementation
 * Copyright (c) 2018 @CompanyNameMagicTag
******************************************************************************/
    .align 2
    .type   HardFault_Handler, %function
    .type   NMI_Handler, %function
    .extern do_hard_fault_handler
    .extern do_NMI_Handler

HardFault_Handler:
    .global HardFault_Handler

NMI_Handler:
    .global NMI_Handler

