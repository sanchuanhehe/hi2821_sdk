#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nv_porting")
if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_crypto.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/middleware/chips/${CHIP}/nv/include
    ${CMAKE_SOURCE_DIR}/middleware/chips/${CHIP}/nv/nv_config/${NV_CFG}/include
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
