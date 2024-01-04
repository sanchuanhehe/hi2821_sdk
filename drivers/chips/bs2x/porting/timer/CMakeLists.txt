#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "timer_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/timer_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PUBLIC_DEFINES
    DEFAULT_TIMER=0
    CONFIG_TIMER_MAX_NUM=4
    CONFIG_TIMER_MAX_TIMERS_NUM=8
    CONFIG_TIMER_CLOCK_VALUE=32000000
    CONFIG_TIMER_0_WIDTH_64=0
)

set(WHOLE_LINK
    true
)

build_component()
