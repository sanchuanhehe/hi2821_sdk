#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/
        .extern TrapVector
        .section .text.entry
        .global _start
        .option norvc
_start:
        j Reset_Handler

Reset_Handler:
        li a0, 0
        li a1, 0
        mv t4, a0
        mv t5, a1
        la t0, TrapVector
        csrw    mtvec, t0
        csrwi   mstatus, 0
        csrwi   mie, 0
        la t3, g_intheap_begin

        /* Copy application core version table to heap begin */
set_app_table_loop:
        lw     t0, (t4)
        sw     t0, (t3)
        addi   t4, t4, 4
        addi   t3, t3, 4
        addi   t5, t5, -1               /* Decrease the number of bytes to do by a quad octet */
        blt    x0, t5, set_app_table_loop   /* Keep going until it is all done */
        la     t5, g_intheap_begin

        .option push
        .option norelax
        /* initialize global pointer */
        la      gp, _gp_
        .option pop

        # initialize stack pointer
        la sp, __stack_top__

        /* init stack */
        la      t0, g_system_stack_begin
        la      t1, g_system_stack_end
        beq     t0, t1, end_set_stack_loop
        li      t2, 0xCACACACA

set_stack_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, set_stack_loop
        li      t2, 0xCCCCCCCC
        la      t0, g_system_stack_begin
        sw      t2, (t0)
        la      t0, __stack_top__
        sw      t2, (t0)
        la      t0, __irq_stack_top__
        sw      t2, (t0)
        la      t0, __excp_stack_top__
        sw      t2, (t0)
end_set_stack_loop:

        /* set data section */
        la      t0, __data_begin__
        la      t1, __data_load__
        la      t2, __data_size__
        beq     t2, x0, end_set_data_loop

set_data_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_data_loop
end_set_data_loop:

        /* setup .ramtext section */
        la      t0, __ramtext_begin__
        la      t1, __ramtext_load__
        la      t2, __ramtext_size__
        beq     t2, x0, end_set_ramtext_loop

set_ramtext_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_ramtext_loop
end_set_ramtext_loop:

        /* setup .patch_data section */
        la      t0, __patch_data_begin__
        la      t1, __patch_data_load__
        la      t2, __patch_data_size__
        beq     t2, x0, end_set_patch_data_loop

set_patch_data_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_patch_data_loop
end_set_patch_data_loop:

        /* setup .romtext section */
        la      t0, __romtext_begin__
        la      t1, __romtext_load__
        la      t2, __romtext_size__
        beq     t2, x0, end_set_romtext_loop
set_romtext_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_romtext_loop

end_set_romtext_loop:


        /* clear rombss section */
        la      t0, __rombss_begin__
        la      t1, __rombss_end__
        beq     t0, t1, end_clear_rombss_loop
        li      t2, 0x00000000

clear_rombss_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, clear_rombss_loop
end_clear_rombss_loop:

        /* clear romram_bss section */
        la      t0, __romram_bss_begin__
        la      t1, __romram_bss_end__
        beq     t0, t1, end_clear_romram_bss_loop
        li      t2, 0x00000000

clear_romram_bss_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, clear_romram_bss_loop
end_clear_romram_bss_loop:

        /* clear bss section */
        la      t0, __bss_begin__
        la      t1, __bss_end__
        beq     t0, t1, end_clear_bss_loop
        li      t2, 0x00000000

clear_bss_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, clear_bss_loop
end_clear_bss_loop:
        li      t0, 0x00006000
        csrs    mstatus, t0
        fssr    x0
        mv a0, t5
        la t0, main
        jr t0
