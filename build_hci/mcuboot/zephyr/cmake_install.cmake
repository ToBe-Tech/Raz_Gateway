# Install script for directory: C:/ncs/v1.9.0/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
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
  set(CMAKE_OBJDUMP "C:/ncs/toolchains/cf2149caf2/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/arch/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/lib/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/soc/arm/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/boards/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/subsys/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/drivers/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/nrf/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/mcuboot/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/mbedtls/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/trusted-firmware-m/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/cjson/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/cddl-gen/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/memfault-firmware-sdk/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/openthread/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/CANopenNode/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/civetweb/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/cmsis/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/fatfs/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/hal_nordic/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/st/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/libmetal/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/littlefs/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/loramac-node/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/lvgl/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/lz4/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/mipi-sys-t/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/nanopb/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/nrf_hw_models/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/open-amp/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/segger/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/tinycbor/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/tinycrypt/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/TraceRecorder/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/zscilib/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/nrfxlib/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/modules/connectedhomeip/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/kernel/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/cmake/flash/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/cmake/usage/cmake_install.cmake")
  include("C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot/zephyr/cmake/reports/cmake_install.cmake")

endif()

