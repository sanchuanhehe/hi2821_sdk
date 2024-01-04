#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server/sle_uart_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_server/sle_uart_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_UART_CLIENT)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart_client/sle_uart_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_uart.c
)
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
