#ifndef _FS_H_
#define _FS_H_


#include <zephyr.h>

#define SYS_REBOOT_WARM   0
#define SYS_REBOOT_COLD   1

#define FW_VERSION {0x00, 0x05,0x01}
#define HW_VERSION {0x00, 0x05,0x01}

#define MAX_UUID_SIZE 11
#define MAX_TARGET_UUID_SIZE 6

#define TIMESTAMP_SIZE 4

#define TARGET_UUID_OFFSET 12
#define FIRST_ENT_OFFSET 4 + TARGET_UUID_OFFSET
#define TARGET_REC_OFFSET 4 + FIRST_ENT_OFFSET
#define CLOUD_REC_OFFSET 4 + TARGET_REC_OFFSET
#define CLOUD_SIGN_OFFSET 1 + CLOUD_REC_OFFSET
#define DATA_ID_OFFSET 1 + CLOUD_SIGN_OFFSET
#define BODY_OFFSET 1 + DATA_ID_OFFSET
#define HEADER_SIZE BODY_OFFSET +11
#define MAX_LISTING_SIZE 100

enum {
	FAILURE =0,
	SUCCESS
};

typedef struct{
	uint32_t timestamp;
}events;

typedef struct{
	uint8_t mac_addr[6];
}device;

typedef struct{
	device * device;
	uint16_t devices_len;
}devices;


typedef struct{
	char biz_name[2]; 
	uint32_t timestamp;
	uint8_t longitude[4];
	uint8_t latitude[4];
	uint8_t status;
}config_t;

typedef struct {
	uint8_t longitude[4];
	uint8_t latitude[4];
}location_t;

// devices devices_init[] ={
// 	{
// 		.device = NULL,
// 		.devices_len = 0
// 	}

// };

typedef struct{
	uint8_t header_len;
	uint8_t listing_uuid[MAX_UUID_SIZE];
	uint8_t target_uuid[MAX_TARGET_UUID_SIZE];
	uint32_t first_ent_timestamp;
	uint32_t target_rec_timestamp;
	uint32_t cloud_rec_timestamp;
	uint8_t cloud_sign;
	uint8_t data_id;
	uint16_t body_len;
	uint8_t body[MAX_LISTING_SIZE];
} listing;






extern devices * devices_list;

// extern listings * listings;
void init_nvs(void);
config_t * nvs_read_config(void);
void factory_reset(void);

void nvs_read_listings(uint8_t ** list);

uint16_t nvs_read_listings_len(void);

uint8_t * nvs_read_timestamp(void);

int nvs_write_timestamp(uint32_t  timestamp);

int nvs_write_business_serial_id(uint16_t id);

int nvs_write_location(uint8_t * gps);

void nvs_write_listings(uint8_t *write_list);

void nvs_write_listings_len(uint16_t len);

uint8_t * nvs_read_mac(void);

int nvs_write_mac(uint8_t * mac);

int nvs_write_status(uint8_t status);

uint8_t nvs_read_status(void);

uint16_t nvs_read_business_id(void);

uint8_t * nvs_read_location(void);

uint8_t nvs_read_post_flag(void);

void nvs_write_post_flag(uint8_t flag);

void nvs_delete_listings(void);

uint16_t nvs_read_listings_num(void);

void nvs_write_listings_num(uint16_t num);

#endif
