#include <stdio.h>
#include <stdbool.h>
#include <zephyr.h>
#include <device.h>
#include <init.h>
#include <pm/pm.h>
#include <hal/nrf_gpio.h>
#include "nrf_modem.h"
#include <nrfx_uarte.h>
#include <drivers/src/prs/nrfx_prs.h>

static bool low_power_enter = false;

static int low_power_usart(bool is_enable) {
    if (is_enable) {
        nrf_uarte_enable(NRF_UARTE0); // Console
        nrf_uarte_enable(NRF_UARTE2); // BLE-H4_UART
    } else {
        nrf_uarte_disable(NRF_UARTE0);// Console
        nrf_uarte_disable(NRF_UARTE2);// BLE-H4_UART
    }
	return 0;
}

void low_power_init(void) {

}

void enter_low_power(void) {
    low_power_enter = true;
    low_power_usart(false);
    k_sleep(K_MSEC(100));
}

void exit_low_power(void) {
    low_power_enter = false;
    low_power_usart(true);
    k_sleep(K_MSEC(100));
} 
