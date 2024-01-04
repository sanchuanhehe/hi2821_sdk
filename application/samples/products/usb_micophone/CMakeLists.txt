#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/micophone_usb/micophone_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/es7243l_ds.c
)
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
