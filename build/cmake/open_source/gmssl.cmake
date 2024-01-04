set(GMSSL_PATH ${CMAKE_DIR}/../../open_source/GmSSL3.0)

set(COMPONENT_NAME "gmssl_hmac_sm3")

set(SOURCES
    ${GMSSL_PATH}/src/sm3.c
    ${GMSSL_PATH}/src/sm3_hmac.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${GMSSL_PATH}/include/
    ${GMSSL_PATH}/include/gmssl/
    ${GMSSL_PATH}/src/
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Wno-sign-compare"  "-Wno-missing-declarations" "-Wno-missing-prototypes"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "gmssl_sm4_ccm")

set(SOURCES
    ${GMSSL_PATH}/src/sm4_enc.c
    ${GMSSL_PATH}/src/sm4_common.c
    ${GMSSL_PATH}/src/sm4_setkey.c
)

set(PRIVATE_HEADER
    ${GMSSL_PATH}/include/
    ${GMSSL_PATH}/include/gmssl/
    ${GMSSL_PATH}/src/
)

build_component()
