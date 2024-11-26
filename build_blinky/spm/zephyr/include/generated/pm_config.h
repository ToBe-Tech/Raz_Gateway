/* File generated by C:\ncs\v1.9.0\nrf\scripts\partition_manager_output.py, do not modify */
#ifndef PM_CONFIG_H__
#define PM_CONFIG_H__
#define PM_MCUBOOT_OFFSET 0x0
#define PM_MCUBOOT_ADDRESS 0x0
#define PM_MCUBOOT_END_ADDRESS 0xc000
#define PM_MCUBOOT_SIZE 0xc000
#define PM_MCUBOOT_NAME mcuboot
#define PM_MCUBOOT_ID 0
#define PM_mcuboot_ID PM_MCUBOOT_ID
#define PM_mcuboot_IS_ENABLED 1
#define PM_0_LABEL MCUBOOT
#define PM_MCUBOOT_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_MCUBOOT_PAD_OFFSET 0xc000
#define PM_MCUBOOT_PAD_ADDRESS 0xc000
#define PM_MCUBOOT_PAD_END_ADDRESS 0xc200
#define PM_MCUBOOT_PAD_SIZE 0x200
#define PM_MCUBOOT_PAD_NAME mcuboot_pad
#define PM_MCUBOOT_PAD_ID 1
#define PM_mcuboot_pad_ID PM_MCUBOOT_PAD_ID
#define PM_mcuboot_pad_IS_ENABLED 1
#define PM_1_LABEL MCUBOOT_PAD
#define PM_MCUBOOT_PAD_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_MCUBOOT_PRIMARY_OFFSET 0xc000
#define PM_MCUBOOT_PRIMARY_ADDRESS 0xc000
#define PM_MCUBOOT_PRIMARY_END_ADDRESS 0x75000
#define PM_MCUBOOT_PRIMARY_SIZE 0x69000
#define PM_MCUBOOT_PRIMARY_NAME mcuboot_primary
#define PM_MCUBOOT_PRIMARY_ID 2
#define PM_mcuboot_primary_ID PM_MCUBOOT_PRIMARY_ID
#define PM_mcuboot_primary_IS_ENABLED 1
#define PM_2_LABEL MCUBOOT_PRIMARY
#define PM_MCUBOOT_PRIMARY_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_MCUBOOT_PRIMARY_APP_OFFSET 0xc200
#define PM_MCUBOOT_PRIMARY_APP_ADDRESS 0xc200
#define PM_MCUBOOT_PRIMARY_APP_END_ADDRESS 0x75000
#define PM_MCUBOOT_PRIMARY_APP_SIZE 0x68e00
#define PM_MCUBOOT_PRIMARY_APP_NAME mcuboot_primary_app
#define PM_MCUBOOT_PRIMARY_APP_ID 3
#define PM_mcuboot_primary_app_ID PM_MCUBOOT_PRIMARY_APP_ID
#define PM_mcuboot_primary_app_IS_ENABLED 1
#define PM_3_LABEL MCUBOOT_PRIMARY_APP
#define PM_MCUBOOT_PRIMARY_APP_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_SPM_OFFSET 0xc200
#define PM_SPM_ADDRESS 0xc200
#define PM_SPM_END_ADDRESS 0x18200
#define PM_SPM_SIZE 0xc000
#define PM_SPM_NAME spm
#define PM_SPM_ID 4
#define PM_spm_ID PM_SPM_ID
#define PM_spm_IS_ENABLED 1
#define PM_4_LABEL SPM
#define PM_SPM_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_APP_OFFSET 0x18200
#define PM_APP_ADDRESS 0x18200
#define PM_APP_END_ADDRESS 0x6f000
#define PM_APP_SIZE 0x56e00
#define PM_APP_NAME app
#define PM_APP_ID 5
#define PM_app_ID PM_APP_ID
#define PM_app_IS_ENABLED 1
#define PM_5_LABEL APP
#define PM_APP_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_NVS_STORAGE_OFFSET 0x6f000
#define PM_NVS_STORAGE_ADDRESS 0x6f000
#define PM_NVS_STORAGE_END_ADDRESS 0x75000
#define PM_NVS_STORAGE_SIZE 0x6000
#define PM_NVS_STORAGE_NAME nvs_storage
#define PM_NVS_STORAGE_ID 6
#define PM_nvs_storage_ID PM_NVS_STORAGE_ID
#define PM_nvs_storage_IS_ENABLED 1
#define PM_6_LABEL NVS_STORAGE
#define PM_NVS_STORAGE_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_MCUBOOT_SECONDARY_OFFSET 0x75000
#define PM_MCUBOOT_SECONDARY_ADDRESS 0x75000
#define PM_MCUBOOT_SECONDARY_END_ADDRESS 0xde000
#define PM_MCUBOOT_SECONDARY_SIZE 0x69000
#define PM_MCUBOOT_SECONDARY_NAME mcuboot_secondary
#define PM_MCUBOOT_SECONDARY_ID 7
#define PM_mcuboot_secondary_ID PM_MCUBOOT_SECONDARY_ID
#define PM_mcuboot_secondary_IS_ENABLED 1
#define PM_7_LABEL MCUBOOT_SECONDARY
#define PM_MCUBOOT_SECONDARY_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_MCUBOOT_SCRATCH_OFFSET 0xde000
#define PM_MCUBOOT_SCRATCH_ADDRESS 0xde000
#define PM_MCUBOOT_SCRATCH_END_ADDRESS 0xfc000
#define PM_MCUBOOT_SCRATCH_SIZE 0x1e000
#define PM_MCUBOOT_SCRATCH_NAME mcuboot_scratch
#define PM_MCUBOOT_SCRATCH_ID 8
#define PM_mcuboot_scratch_ID PM_MCUBOOT_SCRATCH_ID
#define PM_mcuboot_scratch_IS_ENABLED 1
#define PM_8_LABEL MCUBOOT_SCRATCH
#define PM_MCUBOOT_SCRATCH_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_EMPTY_0_OFFSET 0xfc000
#define PM_EMPTY_0_ADDRESS 0xfc000
#define PM_EMPTY_0_END_ADDRESS 0xfe000
#define PM_EMPTY_0_SIZE 0x2000
#define PM_EMPTY_0_NAME EMPTY_0
#define PM_EMPTY_0_ID 9
#define PM_empty_0_ID PM_EMPTY_0_ID
#define PM_empty_0_IS_ENABLED 1
#define PM_9_LABEL EMPTY_0
#define PM_EMPTY_0_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_SETTINGS_STORAGE_OFFSET 0xfe000
#define PM_SETTINGS_STORAGE_ADDRESS 0xfe000
#define PM_SETTINGS_STORAGE_END_ADDRESS 0x100000
#define PM_SETTINGS_STORAGE_SIZE 0x2000
#define PM_SETTINGS_STORAGE_NAME settings_storage
#define PM_SETTINGS_STORAGE_ID 10
#define PM_settings_storage_ID PM_SETTINGS_STORAGE_ID
#define PM_settings_storage_IS_ENABLED 1
#define PM_10_LABEL SETTINGS_STORAGE
#define PM_SETTINGS_STORAGE_DEV_NAME "NRF_FLASH_DRV_NAME"
#define PM_OTP_OFFSET 0x0
#define PM_OTP_ADDRESS 0xff8108
#define PM_OTP_END_ADDRESS 0xff83fc
#define PM_OTP_SIZE 0x2f4
#define PM_OTP_NAME otp
#define PM_SPM_SRAM_OFFSET 0x0
#define PM_SPM_SRAM_ADDRESS 0x20000000
#define PM_SPM_SRAM_END_ADDRESS 0x20008000
#define PM_SPM_SRAM_SIZE 0x8000
#define PM_SPM_SRAM_NAME spm_sram
#define PM_SRAM_SECURE_OFFSET 0x0
#define PM_SRAM_SECURE_ADDRESS 0x20000000
#define PM_SRAM_SECURE_END_ADDRESS 0x20008000
#define PM_SRAM_SECURE_SIZE 0x8000
#define PM_SRAM_SECURE_NAME sram_secure
#define PM_NRF_MODEM_LIB_CTRL_OFFSET 0x8000
#define PM_NRF_MODEM_LIB_CTRL_ADDRESS 0x20008000
#define PM_NRF_MODEM_LIB_CTRL_END_ADDRESS 0x200084e8
#define PM_NRF_MODEM_LIB_CTRL_SIZE 0x4e8
#define PM_NRF_MODEM_LIB_CTRL_NAME nrf_modem_lib_ctrl
#define PM_NRF_MODEM_LIB_SRAM_OFFSET 0x8000
#define PM_NRF_MODEM_LIB_SRAM_ADDRESS 0x20008000
#define PM_NRF_MODEM_LIB_SRAM_END_ADDRESS 0x2000c4e8
#define PM_NRF_MODEM_LIB_SRAM_SIZE 0x44e8
#define PM_NRF_MODEM_LIB_SRAM_NAME nrf_modem_lib_sram
#define PM_SRAM_NONSECURE_OFFSET 0x8000
#define PM_SRAM_NONSECURE_ADDRESS 0x20008000
#define PM_SRAM_NONSECURE_END_ADDRESS 0x20040000
#define PM_SRAM_NONSECURE_SIZE 0x38000
#define PM_SRAM_NONSECURE_NAME sram_nonsecure
#define PM_NRF_MODEM_LIB_TX_OFFSET 0x84e8
#define PM_NRF_MODEM_LIB_TX_ADDRESS 0x200084e8
#define PM_NRF_MODEM_LIB_TX_END_ADDRESS 0x2000a4e8
#define PM_NRF_MODEM_LIB_TX_SIZE 0x2000
#define PM_NRF_MODEM_LIB_TX_NAME nrf_modem_lib_tx
#define PM_NRF_MODEM_LIB_RX_OFFSET 0xa4e8
#define PM_NRF_MODEM_LIB_RX_ADDRESS 0x2000a4e8
#define PM_NRF_MODEM_LIB_RX_END_ADDRESS 0x2000c4e8
#define PM_NRF_MODEM_LIB_RX_SIZE 0x2000
#define PM_NRF_MODEM_LIB_RX_NAME nrf_modem_lib_rx
#define PM_SRAM_PRIMARY_OFFSET 0xc4e8
#define PM_SRAM_PRIMARY_ADDRESS 0x2000c4e8
#define PM_SRAM_PRIMARY_END_ADDRESS 0x20040000
#define PM_SRAM_PRIMARY_SIZE 0x33b18
#define PM_SRAM_PRIMARY_NAME sram_primary
#define PM_NUM 11
#define PM_ALL_BY_SIZE "mcuboot_pad otp nrf_modem_lib_ctrl EMPTY_0 nrf_modem_lib_rx nrf_modem_lib_tx settings_storage nrf_modem_lib_sram nvs_storage spm_sram sram_secure mcuboot spm mcuboot_scratch sram_primary sram_nonsecure app mcuboot_primary_app mcuboot_secondary mcuboot_primary"
#define PM_ADDRESS 0xc200
#define PM_SIZE 0xc000
#define PM_SRAM_ADDRESS 0x20000000
#define PM_SRAM_SIZE 0x8000
#endif /* PM_CONFIG_H__ */