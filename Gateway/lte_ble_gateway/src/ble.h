/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef _BLE_H_
#define _BLE_H_

#define BT_PER_COMP_NAME "TOBE"

#define GATEWAY_SERIAL_ID 0x0017 // 28

//define treatment struct
typedef struct {
    uint16_t delay;
    uint8_t type;
    bool send;

} treatment;

void ble_init(void);
void ble_scan_stop();
void ble_scan_start(void);
void adv_work_handler(void);
void ble_advertise_time(uint32_t adv_time);
void ble_advertise_battery(void);

uint8_t * get_mfg_data(void);

void reboot_work_handler(void);

void update_treatment_params(uint16_t delay, uint8_t type);

void stop_send_treatment(void);

void set_ble_adv_pm(void);

void ble_advertise_lte_vals(void);


void ble_set_adv_data_flag(uint8_t flag);
// uint32_t GetFreeMemorySize(void);

typedef struct {
    bool connected;
    bool is_pm;
} ble_conn_handler;

#endif /* _BLE_H_ */
