/*
 * Copyright (c) @CompanyNameMagicTag 2018-2019. All rights reserved.
 * Description:   LOG OAM LOGGER MODULE
 */
#include "log_oam_logger.h"

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#include <stdarg.h>
#include "log_common.h"

#if CORE == CORE_LOGGING
#include "log_uart.h"
#endif

#ifdef BTH_LOG_BUG_FIX
#define LOG_LEVEL_DEBUG_TO_ERROR 0x40
#endif

/* Module log level defination */
om_log_module_lev_t g_module_log_level[MODULEID_BUTT] = {
    { LOG_WIFIMODULE,   LOG_LEVEL_INFO },
    { LOG_BTMODULE,     LOG_LEVEL_INFO },
    { LOG_BTHMODULE,   LOG_LEVEL_INFO },
    { LOG_NFCMODULE,   LOG_LEVEL_INFO },
    { LOG_PFMODULE,     LOG_LEVEL_INFO },
};

#ifdef CONFIG_DFX_SUPPORT_USERS_PRINT
static log_other_print_t g_other_print_func = NULL;

void log_other_print_register(log_other_print_t users_print_func)
{
    if (users_print_func == NULL) {
        return;
    }
    g_other_print_func = users_print_func;
}

void log_other_print0(uint32_t log_header, uint32_t log_level, const char *fmt)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, 0);
}

void log_other_print1(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, 1, p0);
}

void log_other_print2(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0, uint32_t p1)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, LOG_OAM_INDEX_2, p0, p1);
}

void log_other_print3(uint32_t log_header, uint32_t log_level, const char *fmt, uint32_t p0, uint32_t p1, uint32_t p2)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, LOG_OAM_INDEX_3, p0, p1, p2);
}

void log_other_print4(uint32_t log_header, uint32_t log_level, const char *fmt,
                      uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
    if (g_other_print_func == NULL) {
        return;
    }
    g_other_print_func(log_header, log_level, fmt, LOG_OAM_INDEX_4, p0, p1, p2, p3);
}
#endif

void log_event_print0(uint32_t log_header, uint32_t presspara)
{
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ZERO_ARG_SEND + OML_LOG_TAIL_LENTH];

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(presspara) == LOG_BTHMODULE && getlog_level(presspara) == LOG_LEVEL_NONE) {
        presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_ZERO_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;
    log_oam_entry[LOG_OAM_INDEX_3] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_ZERO_ARG_SEND));
}

void log_event_print1(uint32_t log_header, uint32_t presspara, uint32_t para1)
{
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ONE_ARG_SEND + OML_LOG_TAIL_LENTH];

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(presspara) == LOG_BTHMODULE && getlog_level(presspara) == LOG_LEVEL_NONE) {
        presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_ONE_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_ONE_ARG_SEND));
}

void log_event_print2(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2)
{
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_TWO_ARG_SEND + OML_LOG_TAIL_LENTH];

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(presspara) == LOG_BTHMODULE && getlog_level(presspara) == LOG_LEVEL_NONE) {
        presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_TWO_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = para2;
    log_oam_entry[LOG_OAM_INDEX_5] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_TWO_ARG_SEND));
}

void log_event_print3(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_THREE_ARG_SEND + OML_LOG_TAIL_LENTH];

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(presspara) == LOG_BTHMODULE && getlog_level(presspara) == LOG_LEVEL_NONE) {
        presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_THREE_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = para2;
    log_oam_entry[LOG_OAM_INDEX_5] = para3;
    log_oam_entry[LOG_OAM_INDEX_6] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_THREE_ARG_SEND));
}

void log_event_print4(uint32_t log_header, uint32_t presspara, uint32_t para1, uint32_t para2,
                      uint32_t para3, uint32_t para4)
{
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_FOUR_ARG_SEND + OML_LOG_TAIL_LENTH];

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(presspara) == LOG_BTHMODULE && getlog_level(presspara) == LOG_LEVEL_NONE) {
        presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] =
        (uint32_t)log_lenth_and_sn_press(OML_LOG_FOUR_ARG_SEND, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;
    log_oam_entry[LOG_OAM_INDEX_3] = para1;
    log_oam_entry[LOG_OAM_INDEX_4] = para2;
    log_oam_entry[LOG_OAM_INDEX_5] = para3;
    log_oam_entry[LOG_OAM_INDEX_6] = para4;
    log_oam_entry[LOG_OAM_INDEX_7] = OM_FRAME_DELIMITER;

    log_event((uint8_t *)log_oam_entry, oal_log_lenth(OML_LOG_FOUR_ARG_SEND));
}

void log_event_print_alterable_para_press(uint32_t log_header, uint32_t presspara, uint32_t paranum, ...)
{
    uint8_t uc_para_num;
    uint8_t uc_loop;
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ALTER_PARA_MAX_NUM + OML_LOG_TAIL_LENTH];
    va_list args;

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

#ifdef BTH_LOG_BUG_FIX
    if (get_module_id(presspara) == LOG_BTHMODULE && getlog_level(presspara) == LOG_LEVEL_NONE) {
        presspara |= LOG_LEVEL_DEBUG_TO_ERROR;
    }
#endif

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    uc_para_num = (uint8_t)((paranum > OML_LOG_ALTER_PARA_MAX_NUM) ? (OML_LOG_ALTER_PARA_MAX_NUM) : paranum);

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] = (uint32_t)log_lenth_and_sn_press(uc_para_num, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;

    va_start(args, paranum);
    for (uc_loop = 0; uc_loop < uc_para_num; uc_loop++) {
        log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);
    log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = OM_FRAME_DELIMITER;
    log_event((uint8_t *)log_oam_entry, oal_log_lenth(uc_para_num));
}

void log_event_wifi_print0(uint32_t presspara)
{
    log_event_print0(log_head_press(OM_WIFI), presspara);
}

void log_event_wifi_print1(uint32_t presspara, uint32_t para1)
{
    log_event_print1(log_head_press(OM_WIFI), presspara, para1);
}

void log_event_wifi_print2(uint32_t presspara, uint32_t para1, uint32_t para2)
{
    log_event_print2(log_head_press(OM_WIFI), presspara, para1, para2);
}

void log_event_wifi_print3(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3)
{
    log_event_print3(log_head_press(OM_WIFI), presspara, para1, para2, para3);
}

void log_event_wifi_print4(uint32_t presspara, uint32_t para1, uint32_t para2, uint32_t para3, uint32_t para4)
{
    log_event_print4(log_head_press(OM_WIFI), presspara, para1, para2, para3, para4);
}

#if CORE == CORE_LOGGING
void log_event_print_alterable_para_press_by_write_uart_fifo(uint32_t log_header, uint32_t presspara,
                                                             uint32_t paranum, ...)
{
    uint8_t uc_para_num;
    uint8_t uc_loop;
    uint32_t log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + OML_LOG_ALTER_PARA_MAX_NUM + OML_LOG_TAIL_LENTH];
    va_list args;

    // Module ID is unknown
    if (get_module_id(presspara) >= MODULEID_BUTT) {
        return;
    }

    if (getlog_level(presspara) > (uint8_t)log_get_local_log_level()) {
        return;
    }

    uc_para_num = (uint8_t)((paranum > OML_LOG_ALTER_PARA_MAX_NUM) ? (OML_LOG_ALTER_PARA_MAX_NUM) : paranum);

    log_oam_entry[LOG_OAM_INDEX_0] = log_header;
    log_oam_entry[LOG_OAM_INDEX_1] = (uint32_t)log_lenth_and_sn_press(uc_para_num, (uint32_t)get_log_sn_number());
    log_oam_entry[LOG_OAM_INDEX_2] = presspara;

    va_start(args, paranum);
    for (uc_loop = 0; uc_loop < uc_para_num; uc_loop++) {
        log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = (uint32_t)va_arg(args, uint32_t);
    }
    va_end(args);
    log_oam_entry[OML_LOG_HEADER_ARRAY_LENTH + uc_loop] = OM_FRAME_DELIMITER;
    log_uart_send_buffer((uint8_t *)log_oam_entry, oal_log_lenth(uc_para_num));
}
#endif

#endif /* USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG */
