#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_adc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_adc.c
)

if(DEFINED CONFIG_ADC_SUPPORT_AFE AND DEFINED CONFIG_ADC_SUPPORT_HAFE)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v152/hal_adc_v152.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v152/hal_adc_v152_regs_op.c")
elseif(DEFINED CONFIG_ADC_SUPPORT_AFE AND DEFINED CONFIG_ADC_SUPPORT_AMIC)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v153/hal_adc_v153.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v153/hal_adc_v153_regs_op.c")
else()
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_adc_v151.c")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/v151/hal_adc_v151_regs_op.c")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_ADC_SUPPORT_AFE AND DEFINED CONFIG_ADC_SUPPORT_HAFE)
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v152")
elseif(DEFINED CONFIG_ADC_SUPPORT_AFE AND DEFINED CONFIG_ADC_SUPPORT_AMIC)
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v153")
else()
    list(APPEND PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/v151")
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
