#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_KEYBOARD)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_keyboard/sle_keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_keyboard/sle_keyboard_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_keyboard/sle_keyboard_server_adv.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_KEYBOARD_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_keyboard_dongle/sle_keyboard_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_keyboard_dongle/sle_keyboard_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_keyboard_dongle/sle_keyboard_client.c
)
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
