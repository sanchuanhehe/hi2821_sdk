#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "update_storage_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/upg_backup.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
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
