#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "update_common")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_alloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_verify.c
)

set(PUBLIC_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/security/hash
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
