#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_BLINKY_BASED_CMSIS)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/blinky_cmsis.c" PARENT_SCOPE)
elseif(DEFINED CONFIG_BLINKY_BASED_OSAL)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/blinky_osal.c" PARENT_SCOPE)
endif()
