add_custom_target(spm_shared_property_target)
set_property(TARGET spm_shared_property_target  PROPERTY KERNEL_HEX_NAME;zephyr.hex)
set_property(TARGET spm_shared_property_target  PROPERTY ZEPHYR_BINARY_DIR;C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm/zephyr)
set_property(TARGET spm_shared_property_target  PROPERTY KERNEL_ELF_NAME;zephyr.elf)
set_property(TARGET spm_shared_property_target  PROPERTY BUILD_BYPRODUCTS;C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm/zephyr/zephyr.hex;C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm/zephyr/zephyr.elf)
set_property(TARGET spm_shared_property_target APPEND PROPERTY BUILD_BYPRODUCTS;C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm/libspmsecureentries.a)
set_property(TARGET spm_shared_property_target APPEND PROPERTY PM_YML_DEP_FILES;C:/ncs/v1.9.0/nrf/samples/spm/pm.yml)
set_property(TARGET spm_shared_property_target APPEND PROPERTY PM_YML_FILES;C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm/zephyr/include/generated/pm.yml)
set_property(TARGET spm_shared_property_target APPEND PROPERTY PM_YML_DEP_FILES;C:/ncs/v1.9.0/nrf/subsys/partition_manager/pm.yml.trustzone)
set_property(TARGET spm_shared_property_target APPEND PROPERTY PM_YML_FILES;C:/ncs/v1.9.0/nrf/samples/my_samples/Raz_GateWay/build_blinky/spm/modules/nrf/subsys/partition_manager/pm.yml.trustzone)