#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER AND DEFINED CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_hid_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/sle_rcu_server/sle_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_BLE_RCU_SERVER)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_hid_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server_adv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/ble_rcu_server/ble_rcu_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rcu/rcu.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_RCU_DONGLE)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_dongle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_hid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dongle/sle_rcu_client.c
)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_IR)
set(SOURCES_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_nec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_tx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_demo.c
)
endif()
set(SOURCES "${SOURCES}" ${SOURCES_LIST} PARENT_SCOPE)
