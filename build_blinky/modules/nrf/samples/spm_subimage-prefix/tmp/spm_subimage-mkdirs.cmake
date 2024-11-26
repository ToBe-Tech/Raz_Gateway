# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/ncs/v1.9.0/nrf/samples/spm"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix/tmp"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix/src/spm_subimage-stamp"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix/src"
  "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix/src/spm_subimage-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix/src/spm_subimage-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/modules/nrf/samples/spm_subimage-prefix/src/spm_subimage-stamp${cfgdir}") # cfgdir has leading slash
endif()
