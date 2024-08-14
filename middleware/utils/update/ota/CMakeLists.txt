#===============================================================================
# @brief    cmake make file
# Copyright (c) 2022 @CompanyNameMagicTag
#===============================================================================
set(COMPONENT_NAME "ota_upgrade")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ota_upgrade.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ota_upgrade_handle.c
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
