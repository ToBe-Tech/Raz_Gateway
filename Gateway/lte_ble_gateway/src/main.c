#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include "led.h"
const struct device *dev;
void long_beep(void){
	for (int i = 1; i < 150; ++i)
  {
	gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
	k_msleep(1);
	gpio_pin_set(dev, 28,0);
	k_msleep(1);
  }
}
void main(void)
{
    printk("Starting LED Test Program\n");
    
    // Initialize LEDs
    init_led();
    printk("LED initialization complete\n");
    
    while (1) {
        // Test individual colors
        printk("LED RED\n");
        leds_off();  // Clear previous state
        led_red();
        k_msleep(2000);
        
        printk("LED GREEN\n");
        leds_off();  // Clear previous state
        led_green();
        k_msleep(2000);
        
        printk("LED BLUE\n");
        leds_off();  // Clear previous state
        led_blue();
        k_msleep(2000);
        
        // Test mixed colors
        printk("LED MAGENTA\n");
        leds_off();  // Clear previous state
        led_magenta();
        k_msleep(2000);
        
        printk("LED CYAN\n");
        leds_off();  // Clear previous state
        led_cyan();
        k_msleep(2000);
        
        printk("LED YELLOW\n");
        leds_off();  // Clear previous state
        led_yellow();
        k_msleep(2000);
        
        // Turn off all LEDs
        printk("LEDs OFF\n");
        leds_off();
        k_msleep(2000);
    }
}