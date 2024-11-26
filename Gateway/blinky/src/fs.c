#include "fs.h"


// const char * scratchData;


// #define FLASH_OFFSET            DT_FLASH_AREA_IMAGE_SCRATCH_OFFSET
// #define FLASH_PAGE_SIZE      DT_FLASH_ERASE_BLOCK_SIZE
// #define FLASH_SIZE                  DT_FLASH_AREA_IMAGE_SCRATCH_SIZE


// flashDev = device_get_binding( DT_FLASH_DEV_NAME );

// scratchData = (char *)FLASH_OFFSET;

// // Erase

// flash_write_protection_set( flashDev, false );
// flash_erase( flashDev, FLASH_OFFSET, FLASH_PAGE_SIZE );
// flash_write_protection_set( flashDev, true );

// // Write Block

// if (blockOffset + blockSize) > FLASH_SIZE )
// {
//   LOG_ERR( "Write Exceeds Scratchpad Size" );
//   return(-1);
// }

// blockOffset += FLASH_OFFSET;
// flash_write_protection_set( flashDev, false );
// flash_write( flashDev, blockOffset, blockData, blockSize );
// flash_write_protection_set( flashDev, true );


#include <device.h>
#include <fs/nvs.h>
#include "fs.h"
#include <zephyr.h>
#include <sys/reboot.h>
#include <string.h>
#include <drivers/flash.h>
#include <storage/flash_map.h>
#include <stdlib.h>

#define BUSINESS_ID 1
#define TIMESTAMP_ID 2
#define GPS_ID 3
#define STRING_ID 4
#define MAC_ID 5
#define STATUS_ID 6
#define POST_FLAG_ID 7
#define LISTINGS_NUM_ID 8
#define LISTINGS_ID 10
#define LISTINGS_SIZE_ID 9



static struct nvs_fs fs;

#define STORAGE_NODE_LABEL storage


// struct flash_pages_info info;
// fs.offset = DT_FLASH_AREA_STORAGE_OFFSET;
int rc = 0, cnt = 0, cnt_his = 0;
char biz_buf[2];
uint8_t timestamp[4], longarray[128];
uint8_t location[8];
uint8_t mac_nvs[6];
uint8_t status_nvs;
uint32_t reboot_counter = 0U, reboot_counter_his;
struct flash_pages_info info;
const struct device *flash_dev;
uint8_t listings_size[2] = {0x00,0x00};
// uint8_t * listings_raw;

uint8_t m = 0x9b;

uint8_t initial_listing_data[] = { 0x00};


void init_nvs(){
	
  
    flash_dev = FLASH_AREA_DEVICE(STORAGE_NODE_LABEL);
	if (!device_is_ready(flash_dev)) {
		printk("Flash device %s is not ready\n", flash_dev->name);
		return;
	}
	fs.offset = 0xb1000;
	rc = flash_get_page_info_by_offs(flash_dev, fs.offset, &info);
	if (rc) {
		printk("Unable to get page info\n");
		return;
	}
fs.sector_size = 0xf000;
fs.sector_count = 3U;

	rc = nvs_init(&fs, flash_dev->name);
	if (rc) {
		printk("Flash Init failed\n");
		return;
	}
    	printk("Sector size: %d | Sector count: %d | Total size = %d\n",info.size, 3, 3*info.size);
	// nvs_clear(&fs);

	//write initial_listing_data to flash
	ssize_t freespace = nvs_calc_free_space(&fs);
	printk("Remaining free space in nvs sector is %d Bytes\n", freespace);

}

uint8_t * nvs_read_timestamp(){
	rc = nvs_read(&fs, TIMESTAMP_ID, &timestamp, sizeof(timestamp));
	if (rc > 0) { /* item was found, show it */
		printk("Timestamp found\n");
	} else {
		// printk("Timestamp not found\n");
	
	} 
	return timestamp;
	
}

//big endian
int nvs_write_timestamp(uint32_t timestamp){
	int err = 0;
	uint8_t timestamp_arr[4] = {timestamp >> 24, timestamp >> 16, timestamp >> 8, timestamp};
	int rc = nvs_write(&fs, TIMESTAMP_ID, timestamp_arr, sizeof(timestamp_arr));
	if (rc > 0) 
	{
		printk("Timestamp written: ");
		for (int i = 0; i < 4; i++)
		{
			printk("%02X", timestamp_arr[i]);
		}
		printk("\n");
		err = SUCCESS;
		/* item was found, show it */
	}
	else{
		nvs_delete(&fs, TIMESTAMP_ID);
		printk("Timestamp write failed. retrying\n");
		nvs_write_timestamp(timestamp);
	}
	return err;

}


config_t * nvs_read_config(){
  
	config_t * config_read= malloc(sizeof(config_t));
	rc = nvs_read(&fs, BUSINESS_ID, &biz_buf, sizeof(biz_buf));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, Business: %s\n", BUSINESS_ID, biz_buf);
		strcpy(config_read->biz_name, biz_buf);
	} else   {/* item was not found, add it */
	printk("No business found");
		strcpy(config_read->biz_name, "empty-biz");

		// (void)nvs_write(&fs, BUSINESS_ID, &biz_buf, strlen(biz_buf)+1);
	}

	rc = nvs_read(&fs, TIMESTAMP_ID, &timestamp, sizeof(timestamp));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, timestamp: ", TIMESTAMP_ID);
		for (int n = 0; n < 4; n++) {
			printk("%x ", timestamp[n]);
		}
		printk("\n");
		config_read->timestamp = timestamp[3]<<24 | timestamp[2]<<16 | timestamp[1]<<8 | timestamp[0];
	} else   {/* item was not found, add it */
		printk("No timestamp found");
		config_read->timestamp = 0x00<<24 |  0x00<<16 |  0x00<<8 |  0x00;
		
		// (void)nvs_write(&fs, TIMESTAMP_ID, &timestamp, sizeof(timestamp));
	}

	rc = nvs_read(&fs, GPS_ID, &location, sizeof(location));
	if (rc > 0) { /* item was found, show it */
		printk("location read success\n");

			config_read->longitude[0] = location[0];
			config_read->longitude[1] = location[1];
			config_read->longitude[2] = location[2];
			config_read->longitude[3] = location[3];
			config_read->latitude[0] = location[4];
			config_read->latitude[1] = location[5];
			config_read->latitude[2] = location[6];
			config_read->latitude[3] = location[7];
			
	} else   {/* item was not found, add it */
		printk("No location found",
		       GPS_ID);
		config_read->longitude[0] = 0x00;
			config_read->longitude[1] = 0x00;
			config_read->longitude[2] = 0x00;
			config_read->longitude[3] = 0x00;
			config_read->latitude[0] = 0x00;
			config_read->latitude[1] = 0x00;
			config_read->latitude[2] = 0x00;
			config_read->latitude[3] = 0x00;

		// (void)nvs_write(&fs, GPS_ID, &location,
		// 	  sizeof(location));
	}
	//read status from flash
	rc = nvs_read(&fs, STATUS_ID, &status_nvs, sizeof(status_nvs));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, status: %d\n",
			STATUS_ID, status_nvs);
		config_read->status = status_nvs;
	} else   {/* item was not found, add it */
		printk("No status found",
		       STATUS_ID);
		config_read->status = 0x01;
		nvs_write_status(config_read->status);
	}

	return config_read;

}

void factory_reset(){
	nvs_clear(&fs);
	printk("Factory reset\n");
	sys_reboot(SYS_REBOOT_COLD);
}

void nvs_delete_listings(void){
	int err;
	err = nvs_delete(&fs, LISTINGS_ID);
	if (err) {
		printk("Failed to delete listings\n");
	}
	
	err = nvs_delete(&fs, LISTINGS_SIZE_ID);
	if (err) {
		printk("Failed to delete listings size\n");
	}
	nvs_write_listings_num(0);
	
}
uint16_t nvs_read_listings_len(){
	uint16_t buf_len = 0;
	uint8_t listings_sz[2];
	rc = nvs_read(&fs, LISTINGS_SIZE_ID, &listings_sz, sizeof(listings_sz));
	if (rc > 0) { /* item was found, show it */
		buf_len = listings_sz[0]<<8 | listings_sz[1];
		return buf_len;
		// printk("Id: %d, listings_size: %d\n", LISTINGS_SIZE_ID, listings_size);
	} else   {/* item was not found, add it */
		printk("No listings_size found\n");
		return 0;
	}
	
}

void nvs_read_listings( uint8_t **list){
	uint16_t buf_len = 0;
	buf_len = nvs_read_listings_len();
	// uint8_t listings_raw[156] = {0};

	rc = nvs_read(&fs, LISTINGS_ID, *list, buf_len * sizeof(uint8_t));
	if (rc > 0) { /* item was found, show it */
		// printk("Id: %d, listings size: %d \n", LISTINGS_ID, buf_len);

		// printk("\n");
		
	} else   {/* item was not found, add it */
		printk("No listings found\n");
	}
	
}

void nvs_write_listings_len(uint16_t len){
	uint8_t list_len[2];
	list_len[0] = len>>8;
	list_len[1] = len;
	rc = nvs_write(&fs, LISTINGS_SIZE_ID, list_len, sizeof(list_len));
	if (rc>0) {
		printk("listings size written: %d \n", len);
	}
	else if (rc==0){
		nvs_delete(&fs, LISTINGS_SIZE_ID);
		printk("listing write failed. retrying\n");
		nvs_write_listings_len(len);
	}
}

void nvs_write_listings(uint8_t *write_list){
	uint16_t buf_len = 0;
	buf_len = nvs_read_listings_len();
	// printk("buff len: %d \n", buf_len);
	rc = nvs_write(&fs, LISTINGS_ID, write_list, buf_len * sizeof(uint8_t));

	if (rc > 0) { /* item was found, show it */
		printk("Listings write success\n");
		return;
		
	} else if (rc==0) {/* item was not found, add it */
		printk("listings write fail, retrying\n");
		nvs_delete(&fs, LISTINGS_ID);
		nvs_write_listings(write_list);
		
	}
	
}

int nvs_write_business_serial_id(uint16_t id){
	int err = 0;
	uint8_t biz_id[2] = {id>>8, id};
	rc = nvs_write(&fs, BUSINESS_ID, biz_id, sizeof(id));
	if (rc>0) {
		printk("business serial id Listing write success\n");
		err = SUCCESS;
	}
	else if (rc==0){
		printk("business serial id write fail, retrying\n");
		nvs_delete(&fs, BUSINESS_ID);
		return nvs_write_business_serial_id(id);
	}
	return err;
}

int nvs_write_location(uint8_t * gps){
	int err = 0;
	rc = nvs_write(&fs, GPS_ID, gps, sizeof(location));
	if (rc>0) {
		err = SUCCESS;
		printk("location write success: \n");
		for (int i = 0; i < 8; i++){
			printk("%02X ", gps[i]);
		}
		printk("\n");
	}
	else if (rc==0){
		printk("location write fail, retrying\n");
		nvs_delete(&fs, GPS_ID);
		return nvs_write_location(gps);
	}
	return err;
}

int nvs_write_mac(uint8_t * mac){
	int err = 0;

	rc = nvs_write(&fs, MAC_ID, mac, 6);
	if (rc>0) {
		printk("MAC write success\n");
		err = SUCCESS;
	}
	else if (rc==0){
		printk("MAC write fail, retrying\n");
		nvs_delete(&fs, MAC_ID);
		return nvs_write_mac(mac);
	}
	return err;
}

uint8_t * nvs_read_mac(){
	rc = nvs_read(&fs, MAC_ID, &mac_nvs, sizeof(mac_nvs));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       MAC_ID, mac_nvs[0], mac_nvs[1], mac_nvs[2], mac_nvs[3], mac_nvs[4], mac_nvs[5]);
	} else   {/* item was not found, add it */
		printk("No mac found\n",
		       MAC_ID);
	}
	return mac_nvs;
}

int nvs_write_status(uint8_t status){
	int err = 0;
	rc = nvs_write(&fs, STATUS_ID, &status, sizeof(status));
	if (rc>0) {
		err = SUCCESS;
		printk("Status Listing write success\n");
	}
	else if (rc==0){
		printk("Status write fail, retrying\n");
		nvs_delete(&fs, STATUS_ID);
		return nvs_write_status(status);
	}
	return err;
}

uint8_t nvs_read_status(){
	rc = nvs_read(&fs, STATUS_ID, &status_nvs, sizeof(status_nvs));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, status: %d\n",
		       STATUS_ID, status_nvs);
	} else   {/* item was not found, add it */
		printk("No status found\n",
		       STATUS_ID);
	}
	return status_nvs;
}

uint16_t nvs_read_business_id(void){
	uint16_t biz_id = 0;
	rc = nvs_read(&fs, BUSINESS_ID, &biz_buf, sizeof(biz_buf));
	if (rc > 0) { /* item was found, show it */
		printk("biz read success\n");
	} else   {/* item was not found, add it */
		printk("No business_id found\n",
		       BUSINESS_ID);
	}
	biz_id = biz_buf[0]<<8 | biz_buf[1];
	return biz_id;
}

uint8_t * nvs_read_location(void){
	rc = nvs_read(&fs, GPS_ID, &location, sizeof(location));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, location: %d\n",
		       GPS_ID, location);
	} else   {/* item was not found, add it */
		printk("No location found\n",
		       GPS_ID);
	}
	return location;
}

uint8_t nvs_read_post_flag(void){
	uint8_t post_flag;
	rc = nvs_read(&fs, POST_FLAG_ID, &post_flag, sizeof(post_flag));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, post_flag: %d\n",
		       POST_FLAG_ID, post_flag);
	} else   {/* item was not found, add it */
		printk("No post_flag found\n",
		       POST_FLAG_ID);
	}
	return post_flag;

}


void nvs_write_post_flag(uint8_t flag){
	rc = nvs_write(&fs, POST_FLAG_ID, &flag, sizeof(flag));
	if (rc>0) {
		printk("Post flag Listing write success\n");
	}
	else if (rc==0){
		printk("Post flag write fail, retrying\n");
		nvs_delete(&fs, POST_FLAG_ID);
		nvs_write_post_flag(flag);
	}
}


uint16_t nvs_read_listings_num(void){
	uint16_t list_num = 0;
	uint8_t list_num_buf[2];
	rc = nvs_read(&fs, LISTINGS_NUM_ID, &list_num_buf, sizeof(list_num_buf));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, listings_num: %d\n",
		       LISTINGS_NUM_ID, list_num_buf);
	} else   {/* item was not found, add it */
		printk("No listings_num found\n",
		       LISTINGS_NUM_ID);
	}
	list_num = list_num_buf[0]<<8 | list_num_buf[1];
	return list_num;
}

void nvs_write_listings_num(uint16_t num){
	uint8_t list_num_buf[2] = {num>>8, num};
	rc = nvs_write(&fs, LISTINGS_NUM_ID, list_num_buf, sizeof(list_num_buf));
	if (rc>0) {
		printk("Listings num Listing write success\n");
	}
	else if (rc==0){
		printk("Listings num write fail, retrying\n");
		nvs_delete(&fs, LISTINGS_NUM_ID);
		nvs_write_listings_num(num);
	}
	
}
