#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================

if (${TARGET_NAME} STREQUAL "flashboot")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg AND
    EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}_bak.cfg)
    add_custom_target(GENERAT_SIGNBIN ALL
        COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul &&
                ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}_bak.cfg 1>nul 2>nul
        COMMENT "sign file:gen boot sign file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        add_custom_target(COPY_SIGNBIN ALL
            COMMAND cp ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/flashboot_sign_a.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/flashboot_sign_a.bin -f &&
                    cp ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/flashboot_sign_b.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/flashboot_sign_b.bin -f
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
endif()
elseif (${TARGET_NAME} STREQUAL "loaderboot")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
    add_custom_target(GENERAT_SIGNBIN ALL
        COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul
        COMMENT "sign file:gen boot sign file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        add_custom_target(COPY_SIGNBIN ALL
            COMMAND cp ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/loaderboot_sign.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/loaderboot_sign.bin -f
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
endif()
elseif (${TARGET_NAME} MATCHES "application*" OR ${TARGET_NAME} STREQUAL "ate_debug" OR ${TARGET_NAME} STREQUAL "ate")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
add_custom_target(GENERAT_SIGNBIN ALL
    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg  1>nul 2>nul
    COMMENT "sign file:gen boot sign file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)
endif()
elseif (${TARGET_NAME} MATCHES "control_ws53*")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
add_custom_target(GENERAT_SIGNBIN ALL
    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg
    COMMENT "sign file:gen boot sign file"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)
endif()
endif()
if (${CHIP} STREQUAL "ws63")
add_custom_target(WS63_GENERAT_SIGNBIN ALL
    COMMAND sh ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/params_and_bin_sign.sh
    COMMENT "ws63 image sign"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(TARGET GENERAT_ROM_PATCH)
    add_dependencies(WS63_GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
endif()

if (${CHIP} STREQUAL "ws53")
add_custom_target(WS53_GENERAT_SIGNBIN ALL
    COMMAND sh ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/params_and_bin_sign.sh
    COMMENT "ws53 image sign"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(TARGET GENERAT_ROM_PATCH)
    add_dependencies(WS53_GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
endif()

if(TARGET GENERAT_ROM_PATCH AND TARGET GENERAT_SIGNBIN)
    add_dependencies(GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
