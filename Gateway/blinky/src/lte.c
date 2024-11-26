#include "lte.h"
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

uint8_t * get_signal_vals(void){
    uint8_t * vals = (uint8_t *)malloc(2*sizeof(uint8_t));
    char buf[40] = {0};
    memcpy(buf,get_signal_buf(), sizeof(buf) -1);
    int rssi, ber;
    sscanf(buf, "+CESQ: %*d, %*d, %*d, %*d, %d, %d", &rssi, &ber);
    vals[0] = rssi;
    vals[1] = ber;
    return vals;
}



char * get_signal_buf(void){
         int err;
    const char signal_strength_command[] = "AT+CESQ";
    char * cmd_rcv_buf = malloc(40 * sizeof(char));
    err = nrf_modem_at_cmd(cmd_rcv_buf, 40,signal_strength_command);
    printk("answ lte: %s\n", cmd_rcv_buf);
    return cmd_rcv_buf;
        
}