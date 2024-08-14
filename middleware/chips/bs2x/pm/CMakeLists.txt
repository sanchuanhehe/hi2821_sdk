#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ulp_rtc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(WHOLE_LINK
    true
)

build_component()

add_subdirectory(pm_dev)
add_subdirectory(pm_fsm)
add_subdirectory(pm_veto)
add_subdirectory(pm_sleep)
add_subdirectory(pm_sys)