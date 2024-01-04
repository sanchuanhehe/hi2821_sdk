#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "dfx_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfx_system_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_cmd_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/last_dump_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_data_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zdiag_adapt_sdt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_adapt_flash.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/
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

set(LOG_DEF
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_bt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_dsp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_def_pf.h
)

build_component()
