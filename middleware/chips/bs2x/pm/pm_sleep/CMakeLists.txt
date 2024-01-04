#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_sleep_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_sleep_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lowpower.S
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(WHOLE_LINK
    true
)

build_component()