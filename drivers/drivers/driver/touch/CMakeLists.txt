#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "touch")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/touch_screen_drv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/touch_screen_queue1.c
)

if("TPTYPE_TMA525B" IN_LIST DEFINES)
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/tma525b_touch/tma525b_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tma525b_touch/tma525b_drv1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tma525b_touch/tma525b_touch1.c
)
elseif("TPTYPE_ZTW523" IN_LIST DEFINES)
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/ztw523_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/ztw523_drv1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/ztw523_touch1.c
)
else()
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/ztw523_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/ztw523_drv1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/ztw523_touch1.c
)
endif()

if("TPTYPE_TMA525B" IN_LIST DEFINES)
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tma525b_touch
    ${ROOT_DIR}/middleware/services/gui/hal/input/include
)
elseif("TPTYPE_ZTW523" IN_LIST DEFINES)
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch
    ${ROOT_DIR}/middleware/services/gui/hal/input/include
)
else()
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch
    ${ROOT_DIR}/middleware/services/gui/hal/input/include
)
endif()

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
    -Wno-incompatible-function-pointer-types
    -Wno-unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
