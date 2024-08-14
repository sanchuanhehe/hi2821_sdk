#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(diag_audio_proc)
add_subdirectory_if_exist(diag_audio_dump)
add_subdirectory_if_exist(diag_audio_probe)
add_subdirectory_if_exist(diag_nv)
add_subdirectory_if_exist(diag_update)
add_subdirectory_if_exist(diag_charger)
add_subdirectory_if_exist(diag_logfile)
add_subdirectory_if_exist(diag_gpu_proc)

set(COMPONENT_NAME "diag_dfx_cmd")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_beat_heart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_connect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_filter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_get_mem_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_get_task_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_get_res_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_get_init_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_get_cpup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_log_level.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_mem_read_write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_password.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_trace_info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/diag_cmd_stat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_mocked_shell/diag_mocked_shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/last_dump/last_dump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data/diag_bt_sample_data.c
)

if("AUDIO_SAMPLE_DATA_SUPPORT" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data/diag_audio_sample_data.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/diag_audio_hook/zdiag_audio_hook.c)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/include
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/dfx_include
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_mocked_shell/include
    ${CMAKE_CURRENT_SOURCE_DIR}/last_dump/dfx_include
    ${CMAKE_CURRENT_SOURCE_DIR}/last_dump/include
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data/dfx_include
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data/include
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/include/middleware/utils
)

if("AUDIO_SAMPLE_DATA_SUPPORT" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/diag_audio_hook/include)
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-incompatible-pointer-types-discards-qualifiers
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag_system_cmd/dfx_include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag_mocked_shell/include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/last_dump/include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/last_dump/dfx_include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data/include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag_sample_data/dfx_include/" "*.h")

build_component()
