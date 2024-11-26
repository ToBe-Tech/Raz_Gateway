#include "battery.h"
#include <zephyr.h>
#include <zephyr/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <nrf_errno.h>
#include <nrf_modem_at.h>
#include <modem/lte_lc.h>
#include <modem/lte_lc_trace.h>
#include <modem/at_cmd_parser.h>
#include <modem/at_params.h>
#include <modem/at_monitor.h>
#include <logging/log.h>
#include<stdio.h>
#include <stdlib.h>



int get_battery(void){
    int err;
    char buf2[20];
	err = nrf_modem_at_cmd(buf2, sizeof(buf2), "AT%%XVBAT\n");
		if (err == 0) {
			char *end = strstr(buf2, "\r\nOK");

			if (end) {
				*end = '\0';
			}
		}
    char bat_buf[10];
    memcpy(bat_buf, buf2+8,4);
    int bat_raw = atoi(bat_buf);
    int bat_int = bat_raw/1000.0;
    int bat_float = bat_raw%1000;
    return bat_raw;
}