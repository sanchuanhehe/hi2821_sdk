if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_KEYBOARD)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server/ble_keyboard_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server/ble_keyboard_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_keyboard_server/ble_hid_keyboard_server.c
)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
