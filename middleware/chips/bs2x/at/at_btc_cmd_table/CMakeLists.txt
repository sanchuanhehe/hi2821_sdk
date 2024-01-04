#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "at_btc_cmd")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at_btc_product.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at_bt_cmd_table.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    true
)

build_component()