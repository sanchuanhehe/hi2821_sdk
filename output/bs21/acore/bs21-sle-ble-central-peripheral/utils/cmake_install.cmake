# Install script for directory: D:/work/Hisilicon/Hi2821/sdk/sdk/middleware/utils

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "c:/Program Files (x86)/bs21_CFBB")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/work/Hisilicon/Hi2821/sdk/sdk/tools/bin/compiler/riscv/cc_riscv32_musl_b090/cc_riscv32_musl_fp_win/bin/riscv32-linux-musl-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/algorithm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/app_init/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/app_version/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/at/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/build_version/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/common_headers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/cpu_load/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/dfx/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/error_code/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/pm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/nv/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/partition/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/update/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/mips/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/utils/usb_class/cmake_install.cmake")
endif()

