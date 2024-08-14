#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_systick")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_systick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_systick_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_systick_v150_regs_op.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/v150
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()