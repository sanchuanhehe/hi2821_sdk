/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: At plt cmd table \n
 * Author: @CompanyNameTag \n
 */

#if !defined(AT_RADAR_CMD_TALBE_H)
#define AT_RADAR_CMD_TALBE_H

#include "at.h"

typedef struct {
    uint32_t            para_map;
    int32_t             status; /* Range: 0..3 */
} radarsetst_args_t;

typedef struct {
    uint32_t            para_map;
    int32_t             times; /* Range: 0..255 */
    int32_t             loop; /* Range: 0..32 */
    int32_t             ant; /* Range: 0..3 */
    int32_t             wave; /* Range: 0..10 */
    int32_t             dbg_type; /* Range: 0..2 */
    int32_t             period; /* Range: 1000..20000 */
} radarsetpara_args_t;

typedef struct {
    uint32_t para_map;
    uint32_t dly_time;
} radarsetdlytime_args_t;

/* AT+RADARSETST=status */
at_ret_t at_radar_set_sts(const radarsetst_args_t *args);

/* AT+RADARSETST=times,loop,ant,wave,period */
at_ret_t at_radar_set_para(const radarsetpara_args_t *args);

/* AT+RADARGETST */
at_ret_t at_radar_get_sts(void);

/* AT+RADARGETRES */
at_ret_t at_radar_get_res(void);

/* AT+RADARGETISO */
at_ret_t at_radar_get_isolation(void);

/* AT+RADARSETDLY=dly_time */
at_ret_t at_radar_set_dly_time(const radarsetdlytime_args_t *args);

/* AT+RADARGETDLY */
at_ret_t at_radar_get_dly_time(void);

const at_para_parse_syntax_t radarsetst_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3,
        .offset = offsetof(radarsetst_args_t, status)
    },
};

const at_para_parse_syntax_t radarsetpara_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 20,
        .offset = offsetof(radarsetpara_args_t, times)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 2,
        .entry.int_range.max_val = 32,
        .offset = offsetof(radarsetpara_args_t, loop)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3,
        .offset = offsetof(radarsetpara_args_t, ant)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10,
        .offset = offsetof(radarsetpara_args_t, wave)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 2,
        .offset = offsetof(radarsetpara_args_t, dbg_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 100000,
        .offset = offsetof(radarsetpara_args_t, period)
    },
};

const at_para_parse_syntax_t radarsetdlytime_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 3600,
        .offset = offsetof(radarsetdlytime_args_t, dly_time)
    },
};

const at_cmd_entry_t at_radar_cmd_parse_table[] = {
    {
        "RADARSETST",
        10,
        0,
        radarsetst_syntax,
        NULL,
        (at_set_func_t)at_radar_set_sts,
        NULL,
        NULL,
    },
    {
        "RADARSETPARA",
        12,
        0,
        radarsetpara_syntax,
        NULL,
        (at_set_func_t)at_radar_set_para,
        NULL,
        NULL,
    },
    {
        "RADARGETST",
        10,
        0,
        NULL,
        at_radar_get_sts,
        NULL,
        NULL,
        NULL,
    },
    {
        "RADARGETRES",
        11,
        0,
        NULL,
        at_radar_get_res,
        NULL,
        NULL,
        NULL,
    },
    {
        "RADARGETISO",
        11,
        0,
        NULL,
        at_radar_get_isolation,
        NULL,
        NULL,
        NULL,
    },
    {
        "RADARSETDLY",
        10,
        0,
        radarsetdlytime_syntax,
        NULL,
        (at_set_func_t)at_radar_set_dly_time,
        NULL,
        NULL,
    },
    {
        "RADARGETDLY",
        10,
        0,
        NULL,
        at_radar_get_dly_time,
        NULL,
        NULL,
        NULL,
    },
};

#endif  /* AT_RADAR_CMD_AT_CMD_TALBE_H */
