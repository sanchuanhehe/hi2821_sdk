#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "standard_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.S
    ${CMAKE_CURRENT_SOURCE_DIR}/app_os_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/system_init.c
)

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/version
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

set(BUILD_AS_OBJ
false
)

set(MAIN_COMPONENT
true
)

if(DEFINED CONFIG_SAMPLE_ENABLE)
    if(NOT "SAMPLE_SUPPORT_CMD" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES SW_UART_DEBUG)
        list(APPEND COMPONENT_PUBLIC_CCFLAGS -UTEST_SUITE)
        list(APPEND COMPONENT_PUBLIC_CCFLAGS -UAT_COMMAND)
        list(APPEND COMPONENT_PUBLIC_CCFLAGS -UAT_ONLY)
    endif()
endif()

build_component()
