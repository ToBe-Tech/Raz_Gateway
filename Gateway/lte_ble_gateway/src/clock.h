#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define SEC_INTERVAL 1000
void clock_work_handler(void);
void clock_init(void);
void update_timestamp(uint32_t timestamp);
void pause_clock(void);
void resume_clock(void);
uint32_t get_timestamp_secs(void);
uint32_t get_timestamp_mins(void);


// typedef void (*clock_time_handler_t)(const uint8_t h, const uint8_t m);


// calendar instance
typedef struct
{
	uint32_t timestamp_min;									// timestamp value in minutes starting 1/1/2020 00:00
	uint32_t timestamp_sec;									// timestamp value in seconds starting 1/1/2020 00:00
	uint64_t last_timestamp_ms;								// last value of qs_timestamp_get_ms()


	bool is_paused;											// if true execution of calendar event handlers is paused
	bool is_timestamp_valid;								// if true value of timestamp_min is valid

} clock_instance_t;


#endif /* _CLOCK_H_ */
