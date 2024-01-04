/*
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
 * Description:  LiteOs Heap and Stack info
 * Author: @CompanyNameTag
 * Create: 2022-09-27
 */
#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#define MEMORY_INFO_CRTL_REG 0x52000010
#define MEMORY_INFO_CLOSE 0x5a

/**
 * @brief  Print LiteOs stack info.
*/
void print_stack_waterline_riscv(void);

/**
 * @brief  Print LiteOs heap info.
*/
void print_heap_statistics_riscv(void);
/**
 * @}
 */
#endif