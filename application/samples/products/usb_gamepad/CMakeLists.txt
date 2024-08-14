#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/gamepad_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gamepad_button/gamepad_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gamepad_joystick/gamepad_joystick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_gamepad.c
)
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)