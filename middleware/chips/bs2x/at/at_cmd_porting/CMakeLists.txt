#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "at_cmd_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
)

set(PRIVATE_DEFINES
    LOSCFG_BASE_CORE_USE_MULTI_LIST
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
    true
)

build_component()