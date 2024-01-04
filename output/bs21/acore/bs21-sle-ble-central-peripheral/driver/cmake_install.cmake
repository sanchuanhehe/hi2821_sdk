# Install script for directory: D:/work/Hisilicon/Hi2821/sdk/sdk/drivers/drivers/driver

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
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/adc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/cpu_trace/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/dma/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/i2s/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/keyscan/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/lpc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/lpm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/memory_core/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/otp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/pinmux/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/pmp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/qdec/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/qspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/security/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/systick/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/tcxo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/touch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/ulp_aon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/watchdog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/pdm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/sfc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/pm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/usb_unified/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/rtc_unified/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/driver/efuse/cmake_install.cmake")
endif()

