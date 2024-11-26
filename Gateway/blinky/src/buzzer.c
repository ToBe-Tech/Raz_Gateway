#include "buzzer.h"


extern const struct device *dev;



void dgc_buzz_success(void){
    //make a success sound
   for (int i = 1; i < 50; ++i)
  {
	gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
	k_msleep(1*i);
	gpio_pin_set(dev, 28,0);
	k_msleep(1*i);
  }
}

void dgc_buzz_fail(void){
    //make a fail sound
   for (int i = 1; i < 50; ++i)
  {
    gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
    k_msleep(60 - 1*i);
    gpio_pin_set(dev, 28,0);
    k_msleep(60 - 1*i);
  }
}
