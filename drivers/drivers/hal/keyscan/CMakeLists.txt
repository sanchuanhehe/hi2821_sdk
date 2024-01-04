#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_keyscan")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_keyscan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_keyscan_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_keyscan_v150_regs_op.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

