#===============================================================================
# @brief    cmake make file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_cmd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_report.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_base.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
)

build_component()