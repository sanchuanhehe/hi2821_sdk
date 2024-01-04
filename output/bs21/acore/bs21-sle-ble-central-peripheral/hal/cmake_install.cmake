# Install script for directory: D:/work/Hisilicon/Hi2821/sdk/sdk/drivers/drivers/hal

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
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/adc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/cpu_core/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/cpu_trace/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/dma/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/keyscan/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/lpc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/mips/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/otp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/pinmux/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/pmp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/qdec/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/qspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/reboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/reg_config/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/security/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/sec_common/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/sec_trng2/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/systick/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/tcxo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/watchdog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/xip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/pdm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/sfc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/sio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/work/Hisilicon/Hi2821/sdk/sdk/output/bs21/acore/bs21-sle-ble-central-peripheral/hal/rtc_unified/cmake_install.cmake")
endif()

