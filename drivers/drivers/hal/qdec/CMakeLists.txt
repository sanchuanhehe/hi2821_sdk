#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_qdec")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_qdec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_qdec_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_qdec_v150_regs_op.c
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

# use this when you want to add ccflags like -include xxx
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
