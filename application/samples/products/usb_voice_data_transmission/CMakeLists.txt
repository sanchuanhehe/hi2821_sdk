#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/vdt_codec/vdt_codec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vdt_pdm/vdt_pdm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vdt_usb/vdt_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_vdt.c
)
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
