# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/ncs/v1.9.0/bootloader/mcuboot/boot/zephyr"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/mcuboot"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix/tmp"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix/src/mcuboot_subimage-stamp"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix/src"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix/src/mcuboot_subimage-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix/src/mcuboot_subimage-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_hci/modules/mcuboot/mcuboot_subimage-prefix/src/mcuboot_subimage-stamp${cfgdir}") # cfgdir has leading slash
endif()
