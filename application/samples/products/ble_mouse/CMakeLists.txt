if(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_MOUSE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_mouse_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_hid_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ble_mouse_server/ble_bas_and_dis_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_wheel/mouse_wheel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_button/mouse_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_spi.c
)

if(DEFINED CONFIG_SAMPLE_BLE_SUPPORT_SENSOR_3395)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3395.c)
elseif(DEFINED CONFIG_SAMPLE_BLE_SUPPORT_SENSOR_3805)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3805.c)
elseif(DEFINED CONFIG_SAMPLE_BLE_SUPPORT_SENSOR_3220)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3220.c)
elseif(DEFINED CONFIG_SAMPLE_BLE_SUPPORT_SENSOR_3816)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_pmw3816.c)
else()
    message(FATAL_ERROR "Please define sensor type!")
endif()

endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
