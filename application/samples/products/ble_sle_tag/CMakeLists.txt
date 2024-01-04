if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_SLE_TAG)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_sle_tag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_server/ble_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_server/ble_uuid_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_server/sle_uuid_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_server/sle_server_adv.c
)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
