#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CHIP})
    add_subdirectory(${CHIP})
endif()

if (${CHIP} MATCHES "bs20|bs21|bs21a|bs22|bs26")
    add_subdirectory_if_exist(bs2x)
endif()