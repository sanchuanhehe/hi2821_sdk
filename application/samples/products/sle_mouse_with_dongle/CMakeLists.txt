if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_client/sle_mouse_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_low_latency_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_with_dongle.c
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_MOUSE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_button/mouse_button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse_wheel/mouse_wheel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_server/sle_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_low_latency_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_mouse_with_dongle.c
)
if(DEFINED CONFIG_SAMPLE_SUPPORT_SENSOR_3395)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3395.c)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SENSOR_3805)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_paw3805.c)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SENSOR_3816)
    list(APPEND SOURCES_LIST ${CMAKE_CURRENT_SOURCE_DIR}/mouse_sensor/mouse_sensor_pmw3816.c)
else()
    message(FATAL_ERROR "Please define sensor type!")
endif()
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
