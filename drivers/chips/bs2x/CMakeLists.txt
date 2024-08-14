#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(porting)
add_subdirectory_if_exist(rom)
add_subdirectory_if_exist(boot)
add_subdirectory_if_exist(test_suite_stub)
set(COMPONENT_NAME "chip_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/src/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/src/interrupt_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/src/mpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/src/vectors.s
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/src/patch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/interrupt_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/vectors.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/boot
    ${CMAKE_CURRENT_SOURCE_DIR}/boot
)

if(DEFINED OS AND NOT ${OS} STREQUAL "non-os")
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/arch/src/oam_trace.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/interrupt_os_adapter.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/idle_config.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/tick_timer.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/memory_info.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${OS}/os_dfx.c)
    list(APPEND PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/liteos)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "clocks_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/clocks/clocks.c
)

if(NOT ((${APPLICATION} STREQUAL "rom") OR (${APPLICATION} STREQUAL "loaderboot") OR
        (${APPLICATION} STREQUAL "flashboot") OR (${APPLICATION} STREQUAL "romboot")))
set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/clocks/clock_calibration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/clocks/hal_32k_clock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/clocks/clocks_switch.c
)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/clocks/
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_CLOCKS
    SUPPORT_CLOCKS_CORE
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

set(COMPONENT_NAME "pmu_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pmu/pmu_interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pmu/pmu_ldo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pmu/pmu.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/pmu
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_CLOCKS
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

install_sdk("${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin" "*.bin")
