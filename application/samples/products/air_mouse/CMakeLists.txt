if(DEFINED CONFIG_SAMPLE_SUPPORT_AIR_MOUSE_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/air_mouse_usb/usb_init_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse/sle_air_mouse_client/sle_air_mouse_client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_air_mouse_with_dongle.c
)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_AIR_MOUSE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse/sle_air_mouse_server/sle_air_mouse_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse/sle_air_mouse_server/sle_air_mouse_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sle_air_mouse_with_dongle.c
)
endif()

set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)