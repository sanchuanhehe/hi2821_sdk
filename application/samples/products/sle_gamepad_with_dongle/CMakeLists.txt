#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_GAMEPAD)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad/sle_gamepad_button/sle_gamepad_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad/sle_gamepad_joystick/sle_gamepad_joystick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad/sle_gamepad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad/sle_gamepad_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad/sle_gamepad_server_adv.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_GAMEPAD_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad_dongle/sle_gamepad_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad_dongle/sle_gamepad_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_gamepad_dongle/sle_gamepad_client.c
)
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)