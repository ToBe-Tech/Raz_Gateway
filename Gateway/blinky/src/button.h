#ifndef __M_BUTTON__
#define __M_BUTTON__
#include <drivers/uart.h>
#include <logging/log.h>
#include <modem/lte_lc.h>

#include <random/rand32.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include <dk_buttons_and_leds.h>

#define BUTTON_1 BIT(0)
#define BUTTON_2 BIT(1)
#define SWITCH_1 BIT(2)
#define SWITCH_2 BIT(3)
#define ADV_CPM_TIMEOUT 5000

void button_handler(uint32_t button_states, uint32_t has_changed);
void adv_normal_mode_handler(void);
void button_time_count_handler(struct k_work *work);
void block_button(void);
void release_button(void);
#endif//__M_BUTTONS__