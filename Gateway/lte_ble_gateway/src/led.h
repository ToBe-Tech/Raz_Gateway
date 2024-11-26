#ifndef _LED_H_
#define _LED_H_

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

#define LED_RED 29
#define LED_GREEN 30
#define LED_BLUE 31

void led_blue(void);

void led_green(void);

void led_red(void);

void led_magenta(void);

void led_cyan(void);

void led_yellow(void);

void leds_off(void);

void init_led(void);

void led_off(uint8_t led);

#endif /* _LED_H_ */