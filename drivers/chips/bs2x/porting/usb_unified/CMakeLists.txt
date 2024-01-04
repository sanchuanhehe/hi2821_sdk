#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "usb_unified_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/phy/usb_phy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/phy/usb2.0_phy_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/usbd_hid_mouse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/usbd_hid.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stub
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

build_component()
