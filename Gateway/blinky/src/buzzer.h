#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>


void dgc_buzz_success(void);

void dgc_buzz_fail(void);

#endif /* _BUZZER_H_ */