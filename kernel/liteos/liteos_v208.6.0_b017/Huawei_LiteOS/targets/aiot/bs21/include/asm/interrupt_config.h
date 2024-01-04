/* ----------------------------------------------------------------------------
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: interrupt config \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-10, Create file. \n
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _ASM_INTERRUPT_CONFIG_H
#define _ASM_INTERRUPT_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*  26 + 27 */
#define NUM_HAL_INTERRUPT_UART           39

#define NUM_HAL_INTERRUPT_TIMER          7
#define OS_TICK_INT_NUM                  NUM_HAL_INTERRUPT_TIMER

#define NUM_HAL_INTERRUPT_NMI            12

#define OS_RISCV_SYS_VECTOR_CNT          26

#define OS_RISCV_MIE_IRQ_VECTOR_CNT      6

#define OS_RISCV_LOCAL_IRQ_VECTOR_CNT   (OS_RISCV_MIE_IRQ_VECTOR_CNT + OS_RISCV_SYS_VECTOR_CNT)

#define __NVIC_PRIO_BITS          3U    /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0     /* Set to 1 if different SysTick Config is used */
#define __DSP_PRESENT             1     /* DSP extension present */
#define __SAUREGION_PRESENT       1
#define __FPU_PRESENT             1

// #ifndef __ASSEMBLER__
// extern UINTPTR __irq_stack_top__;
// extern UINTPTR __excp_stack_top__;
// extern UINTPTR __nmi_stack_top__;
// #endif

// #define __irq_stack_top __irq_stack_top__
// #define __exc_stack_top __excp_stack_top__
// #define __nmi_stack_top __nmi_stack_top__

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _ASM_INTERRUPT_CONFIG_H */
