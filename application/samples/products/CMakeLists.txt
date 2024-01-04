#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_USB_KEYBOARD)
    add_subdirectory_if_exist(usb_keyboard)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_USB_MOUSE)
    add_subdirectory_if_exist(usb_mouse)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART)
    add_subdirectory_if_exist(sle_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_UART)
    add_subdirectory_if_exist(ble_uart)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_GAMEPAD_WITH_DONGLE)
    add_subdirectory_if_exist(sle_gamepad_with_dongle)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_KEYBOARD_WITH_DONGLE)
    add_subdirectory_if_exist(sle_keyboard_with_dongle)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_RCU)
    add_subdirectory_if_exist(rcu)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_WITH_DONGLE)
    add_subdirectory_if_exist(sle_mouse_with_dongle)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_MOUSE)
    add_subdirectory_if_exist(ble_mouse)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_KEYBOARD)
    add_subdirectory_if_exist(ble_keyboard)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_KEYBOARD_SIMULATOR)
    add_subdirectory_if_exist(ble_keyboard_simulator)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_USB_VDT)
    add_subdirectory_if_exist(usb_voice_data_transmission)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_VDT_WITH_DONGLE)
    add_subdirectory_if_exist(sle_voice_data_trans_with_dongle)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_USB_KEYBOARD)
    add_subdirectory_if_exist(usb_keyboard)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_USB_GAMEPAD)
    add_subdirectory_if_exist(usb_gamepad)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_USB_MICOPHONE)
    add_subdirectory_if_exist(usb_micophone)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE)
    add_subdirectory_if_exist(sle_microphone)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SLE_TAG)
    add_subdirectory_if_exist(ble_sle_tag)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_WITH_DONGLE)
    add_subdirectory_if_exist(air_mouse)
endif()
set(SOURCES "${SOURCES}" PARENT_SCOPE)