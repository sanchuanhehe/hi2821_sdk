#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_uart_server/ble_uart_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_uart_server/ble_uart_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_uart.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_uart_client/ble_uart_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_uart_client/ble_uart_client_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_uart.c
)
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
