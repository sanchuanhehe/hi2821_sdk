#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(porting)
add_subdirectory_if_exist(board)

if("ROM_EC" IN_LIST DEFINES)
    add_subdirectory_if_exist(rom_config)
else()
    add_subdirectory_if_exist(rom_info)
endif()
