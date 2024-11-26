#ifndef _LISTING_H_
#define _LISTING_H_

#include <zephyr.h>
#include "fs.h" 
#define PERIODIC_SENSOR_DATA_LISTING_LENGTH 28
#define IGNITION_DATA_LISTING_BODY_LENGTH 36
#define GATEWAY_STATUS_LISTING_BODY_LENGTH 24
#define DIAGNOSTICS_LISTING_BODY_LENGTH 13
#define CARTRIDGE_LISTING_BODY_LENGTH 5
#define FULL_LISTING_HEADER_LEN 33
#define MAX_LISTINGS_NUM 400
#define MAX_LISTING_BODY_SIZE 100

#define DGC_LISTING_LENGTH 50
#define SHORT_TREATMENT_LISTING_LENGTH 36



/*
    Concatenate listings to flash storage from the current listings instance 
*/
void save_listings_from_inst();

void compile_ignition_listings(uint8_t * adv_data, uint8_t len, device * devices);

void compile_periodic_listings(uint8_t * adv_data, uint8_t len, device * devices);

uint16_t get_listings_length(void);

uint8_t * get_all_listings_hex(void);

char *  get_single_listings_string(int len, int ind);

void handle_dgc(uint8_t *listings,uint16_t listings_len);

void send_treatment(uint8_t * listings);

void reset_listings(void);

void compile_gateway_status_listing(uint8_t len);

void remove_posted_listings(uint8_t * listings_hex_arr, uint16_t * del_listing_ind,uint16_t list_size,int listings_post_count,uint16_t listings_length);


typedef struct
{
    uint16_t listing_arr_pos;									// index of latest listing added
	uint16_t listing_arr_len;									// length of listing array

	listing listing_arr[MAX_LISTINGS_NUM];								// listings array


} listing_instance_t;

#endif