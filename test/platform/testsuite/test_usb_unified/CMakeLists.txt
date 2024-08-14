#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "test_usb_unified")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_usb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_usb_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_usb_mouse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_usb_multiple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_usb_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfu/test_usb_dfu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/high_speed_mouse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mx8650.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/paw3220.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/paw3320.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/paw3395.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/paw3399.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/paw3816.c
)

if(DEFINED CONFIG_DRIVERS_USB_SERIAL_GADGET)
list(APPEND SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/serial/test_usb_serial.c)
endif()

if(CONFIG_DRIVER_SUPPORT_KEYSCAN)
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_usb_keyboard.c
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/dfu
    ${CMAKE_CURRENT_SOURCE_DIR}/serial
)

if(DEFINED CONFIG_DRIVERS_USB_SERIAL_GADGET)
list(APPEND PRIVATE_HEADER  ${CMAKE_CURRENT_SOURCE_DIR}/serial)
endif()

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

build_component()
