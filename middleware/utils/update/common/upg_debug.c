/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: UPG debug functions source file
 */

#include "upg_config.h"
#include "upg_common.h"
#include "upg_debug.h"

#if (UPG_CFG_DEBUG_PRINT_ENABLED == YES)

#define UART_DR                     0x0
#define UART_FR                     0x18
#define UARTFR_TXFF_MASK            0x20
#define BITS_PER_BYTE               8
#define DECIMAL                     10

STATIC void upg_print_c(const char c)
{
    if (upg_get_func_list()->serial_putc != NULL) {
        return upg_get_func_list()->serial_putc(c);
    }
}

void upg_print(const char *s)
{
    while (*s != NULL) {
        upg_print_c(*s++);
    }
}

STATIC void upg_print_hex(uint32_t hex)
{
    int32_t i;
    char c;
    char mark = 0;
    uint32_t h = hex;

    upg_print("0x");

    for (i = 0; i < BITS_PER_BYTE; i++) {
        c = (h >> 28) & 0x0F; /* u32 right shift 28 */

        if (c >= DECIMAL) {
            c = (c - DECIMAL) + 'A';
        } else {
            c = c + '0';
        }

        /* 如果不是最后一个数且之前数字都为0 */
        if ((mark == 0) && (i != BITS_PER_BYTE - 1)) {
            if (c != '0') {
                mark = 1;
                upg_print_c(c);
            }
        } else {
            upg_print_c(c);
        }

        h = h << 4; /* u32 left shift 4 */
    }
}

void upg_msg0(const char *s)
{
    upg_print("[UPG] ");
    upg_print(s);
    upg_print("\r\n");
}

void upg_msg1(const char *s, uint32_t h)
{
    upg_print("[UPG] ");
    upg_print(s);
    upg_print_hex(h);
    upg_print("\r\n");
}

void upg_msg2(const char *s, uint32_t h1, uint32_t h2)
{
    upg_print("[UPG] ");
    upg_print(s);
    upg_print_hex(h1);
    upg_print(" ");
    upg_print_hex(h2);
    upg_print("\r\n");
}

void upg_msg4(const char *s, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4)
{
    upg_print("[UPG] ");
    upg_print(s);
    upg_print_hex(h1);
    upg_print(" ");
    upg_print_hex(h2);
    upg_print(" ");
    upg_print_hex(h3);
    upg_print(" ");
    upg_print_hex(h4);
    upg_print("\r\n");
}

void upg_print_flag(fota_upgrade_flag_area_t *upg_flag)
{
    uint32_t i;
    upg_msg1("head_magic: ", upg_flag->head_magic);
    upg_msg1("head_before_offset: ", upg_flag->head_before_offset);
    upg_msg1("package_length: ", upg_flag->package_length);
    upg_msg1("firmware_num: ", upg_flag->firmware_num);
    for (i = 0; i < UPG_FIRMWARE_MAX_NUM; i++) {
        upg_msg4("firmware_flag: ", upg_flag->firmware_flag[i][0],
                 upg_flag->firmware_flag[i][1],     /* 1:  the 2rd flag */
                 upg_flag->firmware_flag[i][2], 0); /* 2:  the 3th flag */
    }
    upg_msg4("nv_flag: ", upg_flag->nv_flag[0], upg_flag->nv_flag[1], upg_flag->nv_flag[2], 0); /* 0 1 2: three flags */
    upg_msg1("update_result: ", upg_flag->update_result);
    upg_msg1("nv_data_offset: ", upg_flag->nv_data_offset);
    upg_msg1("nv_data_len: ", upg_flag->nv_data_len);
    upg_msg1("nv_hash_offset: ", upg_flag->nv_hash_offset);
    upg_msg1("nv_hash_len: ", upg_flag->nv_hash_len);
    upg_msg1("complete_flag: ", upg_flag->complete_flag);
    upg_msg1("head_end_magic: ", upg_flag->head_end_magic);
}

#endif /* #ifdef UPG_CFG_DEBUG_PRINT_ENABLED */