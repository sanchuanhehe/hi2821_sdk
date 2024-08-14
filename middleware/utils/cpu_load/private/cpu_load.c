/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description:   CPU load
 * Author:
 * Create: 2021-04-06
 */
#include "cpu_load.h"
#include <stdint.h>
#include "debug_print.h"
#include "chip_io.h"
#include "memory_config_common.h"
#ifdef BUILD_APPLICATION_STANDARD
#include "log_oam_logger.h"
#include "log_def.h"
#include "log_printf.h"
#endif

#define IMAGE_COPY_LEN       4
#define ADDR_TRANS           4
#define STACK_DEFAULT_VALUE  0xdeadbeef
#define BSS_DEFAULT_VALUE    0x0
#define RESEVED_VALUE        0xffffffff
#ifdef CHIP_SW39
#define MASTER_XIP_ADDR_MASK 0xffffffff
#else
#define MASTER_XIP_ADDR_MASK 0x1fffffff
#endif

#ifdef MASTER_LOAD_SLAVE
//lint -esym(752,*)
extern uint32_t g_system_stack_begin;
extern uint32_t g_system_stack_end;
extern uint32_t __ramtext_begin__;
extern uint32_t __ramtext_end__;
extern uint32_t __ramtext_load__;
extern uint32_t __data_begin__;
extern uint32_t __data_end__;
extern uint32_t __data_load__;
extern uint32_t __bss_begin__;
extern uint32_t __bss_end__;
#ifndef CHIP_SW39
extern uint32_t __rom_data_begin__;
extern uint32_t __rom_data_end__;
extern uint32_t __rom_data_load__;
extern uint32_t __rom_bss_begin__;
extern uint32_t __rom_bss_end__;
extern uint32_t __patch_data_begin__;
extern uint32_t __patch_data_end__;
extern uint32_t __patch_data_load__;
#ifdef AUTO_GEN_ROM
extern uint32_t __rom_ram_begin__;
extern uint32_t __rom_ram_end__;
extern uint32_t __rom_ram_load__;
#endif
#else
extern uint32_t __text_begin__;
extern uint32_t __text_end__;
extern uint32_t __text_load__;
#endif

#if CORE == BT
#ifdef CHIP_SW39
#define SECTION_NUMBER  6
#else
#define SECTION_NUMBER  9
#endif
extern uint32_t __btc_text_begin__;
extern uint32_t __btc_text_end__;
extern uint32_t __btc_text_load__;
static const section_head_t g_section_information __attribute__((section(".build_info"))) = {
    SECTION_NUMBER,
    0,
    {{SECTION_TYPE_INIT, &g_system_stack_begin, &g_system_stack_end, NULL, RESEVED_VALUE},
#ifndef CHIP_SW39
     {SECTION_TYPE_RELOAD, &__rom_data_begin__, &__rom_data_end__, &__rom_data_load__, RESEVED_VALUE},
     {SECTION_TYPE_CLEAR, &__rom_bss_begin__, &__rom_bss_end__, NULL, RESEVED_VALUE},
     {SECTION_TYPE_RELOAD, &__patch_data_begin__, &__patch_data_end__, &__patch_data_load__, RESEVED_VALUE},
#ifdef AUTO_GEN_ROM
     {SECTION_TYPE_RELOAD, &__rom_ram_begin__, &__rom_ram_end__, &__rom_ram_load__, RESEVED_VALUE},
#endif
#endif
     {SECTION_TYPE_RELOAD, NULL, (void *)(uintptr_t)BT_VECTORS_LENGTH,
      (void *)(uintptr_t)BT_VECTORS_LOAD_ORIGIN, RESEVED_VALUE},
#ifndef CHIP_SW39
     {SECTION_TYPE_RELOAD, &__btc_text_begin__, &__btc_text_end__, &__btc_text_load__, RESEVED_VALUE},
#else
     {SECTION_TYPE_RELOAD, &__text_begin__, &__text_end__, &__text_load__, RESEVED_VALUE},
#endif
     {SECTION_TYPE_RELOAD, &__ramtext_begin__, &__ramtext_end__, &__ramtext_load__, RESEVED_VALUE},
     {SECTION_TYPE_RELOAD, &__data_begin__, &__data_end__, &__data_load__, RESEVED_VALUE},
     {SECTION_TYPE_CLEAR, &__bss_begin__, &__bss_end__, NULL, RESEVED_VALUE}}
};
#elif CORE == GNSS
#define SECTION_NUMBER  5
static const section_head_t g_section_information __attribute__((section(".build_info"))) = {
    SECTION_NUMBER,
    0,
    {{SECTION_TYPE_INIT, &g_system_stack_begin, &g_system_stack_end, NULL, RESEVED_VALUE},
     {SECTION_TYPE_RELOAD, NULL, (void *)(uintptr_t)GNSS_VECTORS_LENGTH,
      (void *)(uintptr_t)GNSS_VECTORS_LOAD_ORIGIN, RESEVED_VALUE},
     {SECTION_TYPE_RELOAD, &__ramtext_begin__, &__ramtext_end__, &__ramtext_load__, RESEVED_VALUE},
     {SECTION_TYPE_RELOAD, &__data_begin__, &__data_end__, &__data_load__, RESEVED_VALUE},
     {SECTION_TYPE_CLEAR, &__bss_begin__, &__bss_end__, NULL, RESEVED_VALUE}}
};
#else
static const section_head_t g_section_information __attribute__((section(".build_info"))) = {
    0,
    0,
    {{0}}
};
#endif
//lint -esym(752,*)
#endif

void cpu_load_image_process(uint32_t type, uint32_t *src, uint32_t *des, uint32_t size)
{
    if (((type == SECTION_TYPE_RELOAD) && (src == NULL)) || (des == NULL) || (size & 0x3) || (size == 0)) {
        PRINT("img download size:0x%x is not 4 byte aligned, type:%d\r\n", size, type);
#ifdef BUILD_APPLICATION_STANDARD
        oml_pf_log_print2(LOG_BCORE_PLT_LPM_DSP_POWER, LOG_NUM_LPM_DSP_POWER, LOG_LEVEL_INFO, \
            "[LOW_POWER]img download size:0x%x is not 4 byte aligned, type:%d\r\n", size, type);
#endif
        return;
    }
    switch (type) {
        case SECTION_TYPE_INIT:
            for (uint32_t offset = 0; offset < size; offset += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offset, STACK_DEFAULT_VALUE);
            }
            break;
        case SECTION_TYPE_CLEAR:
            for (uint32_t offset = 0; offset < size; offset += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offset, BSS_DEFAULT_VALUE);
            }
            break;
        case SECTION_TYPE_RELOAD:
            for (uint32_t offset = 0; offset < size; offset += IMAGE_COPY_LEN) {
                writel((uintptr_t)des + offset, readl((uintptr_t)src + offset));
            }
            break;
        default:
            break;
    }
}

const section_head_t *cpu_load_get_section_info(core_images_e image)
{
    UNUSED(image);
#if defined(MASTER_LOAD_SLAVE)
    return (section_head_t *)&g_section_information;
#else
    return NULL;
#endif
}

static uint32_t cpu_load_caculate_offset(uint32_t section_run_begin, core_images_e image)
{
    UNUSED(section_run_begin);
    if (image == CORE_IMAGES_BT) {
        return 0x29800000; //!< M3 REMAP_ADDR_OFFSET = 0xa6000000 / 4
    }
    if (image == CORE_IMAGES_EXTERN0) {
#if CHIP_LIBRA
        return (section_run_begin >= GNSS_DTCM_ORIGIN) ? 0x22800000 : 0x2A000000;
#endif
    }
    return 0;
}

#if CHIP_BRANDY
uint32_t* cpu_load_translate_into_mcu_xip_addr(uint32_t *addr, core_images_e image)
{
    // loadaddr must be non-zero for translate
    if (image == CORE_IMAGES_BT && addr) {
        return (uint32_t*)((uint32_t)addr - BCPU_XIP_FLASH_ADDR + MCU_XIP_FLASH_ADDR);
    } else {
#ifdef BUILD_APPLICATION_STANDARD
        oml_pf_log_print2(LOG_BCORE_PLT_LPM_DSP_POWER, LOG_NUM_LPM_DSP_POWER, LOG_LEVEL_INFO, \
            "[%d]core flash address[0x%x] translate int mcu flash.\r\n", image, addr);
#endif
        return NULL;
    }
}
#endif

void cpu_load_image(const section_head_t *load_info, core_images_e image)
{
    section_head_t *head = NULL;
    uint32_t type;
    uint32_t *run_addr_begin = NULL;
    uint32_t *run_addr_end = NULL;
    uint32_t *load_addr = NULL;
    uint32_t section_size;
    head = (section_head_t *)load_info;
    if ((head == NULL) || (head->section_num == 0xFFFFFFFF)) {
        PRINT("img head is null\n");
        return;
    }

#ifdef CHIP_SW39
    PRINT("sections max num = %x\r\n", head->section_num);
#endif
    for (uint32_t i = 0; i < head->section_num; i++) {
        type = head->section_info[i].section_type;
        run_addr_begin = (uint32_t *)((char *)head->section_info[i].section_run_begin);
        run_addr_end = (uint32_t *)((char *)head->section_info[i].section_run_end);
#if CHIP_BRANDY
        load_addr = cpu_load_translate_into_mcu_xip_addr((uint32_t*)head->section_info[i].section_load_addr, image);
#else
        load_addr = (uint32_t *)((char *)((uint32_t)head->section_info[i].section_load_addr & MASTER_XIP_ADDR_MASK));
#endif
        section_size = (uint32_t)((run_addr_end - run_addr_begin) * ADDR_TRANS);
        run_addr_begin += cpu_load_caculate_offset((uint32_t)run_addr_begin, image);
        PRINT("copy sections num = %x\r\n", i);
        PRINT("type = %x\r\n", type);
        PRINT("size = %x\r\n", section_size);
        cpu_load_image_process(type, load_addr, run_addr_begin, section_size);
    }
}
