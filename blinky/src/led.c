#include "led.h"

extern const struct device *dev;


void init_led(void){
    dev = device_get_binding("GPIO_0");

    gpio_pin_configure(dev,28, GPIO_OUTPUT); //GPIO P0.28 // buzzer

	gpio_pin_configure(dev, LED_RED, GPIO_OUTPUT);
	gpio_pin_configure(dev, LED_GREEN, GPIO_OUTPUT);
	gpio_pin_configure(dev, LED_BLUE, GPIO_OUTPUT);

}

void led_on(uint8_t led){
    gpio_pin_set(dev, led, 1);
}

void led_blue(void){
    led_on(LED_BLUE);
}

void led_green(void){
    led_on(LED_GREEN);
}

void led_red(void){
    led_on(LED_RED);
}

void led_off(uint8_t led){
    gpio_pin_set(dev, led, 0);
}

void leds_off(void){
    led_off(LED_RED);
    led_off(LED_GREEN);
    led_off(LED_BLUE);
}

void led_magenta(void){
    led_on(LED_RED);
    led_on(LED_BLUE);
}

void led_cyan(void){
    led_on(LED_GREEN);
    led_on(LED_BLUE);
}

void led_yellow(void){
    led_on(LED_RED);
    led_on(LED_GREEN);
}

void blink_led_yellow(void){
    uint32_t curr_time = get_timestamp_secs();
}