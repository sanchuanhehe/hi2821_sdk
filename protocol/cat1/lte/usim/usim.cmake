set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bip_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bip_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bip_code.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bip_context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bip_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bip_fsm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_base_apdu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_base_apdu_cmd_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_base_apdu_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_base_global.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_file_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_logic_channel_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_main_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_pin_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_proact_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_stk_cmd_codec_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_stk_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_stub.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_task_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usim_timer_proc.c
)

# 本模块需要引用的头文件
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/usim_stub
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../infra/list/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../infra/common_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../infra/soc
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/common/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../phy/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/mmc/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/emm/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/esm/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/nas_pub_stub/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/comm/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../nas/comm/public/global
    ${CMAKE_CURRENT_SOURCE_DIR}/../l2/l2_common/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../l2/l2_stub/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../l2/rlc/ul/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../l2/mac/common/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/fsm/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../rrc/stub
)

# 可对外提供的公共头文件
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
true
)

#===============================================================================
# 带main函数才写TRUE
#===============================================================================
set(MAIN_COMPONENT //
false
)
