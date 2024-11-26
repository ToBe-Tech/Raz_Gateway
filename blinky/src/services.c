#include "services.h"

#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include "fs.h"
#include "listing.h"
#include <stdlib.h>
#include "clock.h"

#define BT_UUID_CONFIGURATION_S 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x00, 0x10, 0x2f, 0xe5
#define BT_UUID_CONFIGURATION_SERVICE   BT_UUID_DECLARE_128(BT_UUID_CONFIGURATION_S)

#define BT_UUID_FACTORY_RESET_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x01, 0x10, 0x2f, 0xe5
#define BT_UUID_FACTORY_RESET_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_FACTORY_RESET_C)

#define BT_UUID_TIMESTAMP_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x02, 0x10, 0x2f, 0xe5
#define BT_UUID_TIMESTAMP_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_TIMESTAMP_C)

#define BT_UUID_MAC_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x03, 0x10, 0x2f, 0xe5
#define BT_UUID_MAC_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_MAC_C)

#define BT_UUID_SCAN_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x04, 0x10, 0x2f, 0xe5
#define BT_UUID_SCAN_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_SCAN_C)

#define BT_UUID_CONFIG_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x05, 0x10, 0x2f, 0xe5
#define BT_UUID_CONFIG_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_CONFIG_C)

#define BT_UUID_LISTING_S 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x00, 0x20, 0x2f, 0xe5
#define BT_UUID_LISTING_SERVICE   BT_UUID_DECLARE_128(BT_UUID_LISTING_S)

#define BT_UUID_LISTING_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x01, 0x20, 0x2f, 0xe5
#define BT_UUID_LISTING_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_LISTING_C)

#define BT_UUID_LISTING_LEN_C 0xfb, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x02, 0x20, 0x2f, 0xe5
#define BT_UUID_LISTING_LEN_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_LISTING_LEN_C)


#define MAX_WRITE_SIZE 20
#define MAX_READ_SIZE 20

// extern bool clock_initialized;

static uint8_t config_ct[10];
static uint8_t config_ct_update;
static uint8_t factory_ct_update;
static uint8_t listing_ct[20];
static uint8_t listing_num_ct[10];
static uint8_t timestamp_ct[10];
static uint8_t mac_ct[10];
static uint8_t scan_ct[10];
static uint8_t config_ct[10];
static uint8_t factory_ct[10];
uint8_t g_listing_data[MAX_LISTING_ARRAY_SIZE];
int g_listing_idx = 0;
uint16_t listing_write_len = 0x0000;
int read_offset = 0;
int write_offset = 0;
uint8_t * listing_read_total;
uint8_t * listing_write_total;
uint16_t listing_len = 0x0000;

extern struct k_work_delayable ble_scan_start_work;

static ssize_t factory_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(factory_ct));
}

void reset_listing_read(){
	//TODO check if some of this is redundant
	printk("resetting listing read");
	read_offset = 0;
	free(listing_read_total);
	listing_len = 0;
	nvs_delete_listings();
	

}

static ssize_t listing_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	// initial characteristic read request - read all listings
	if (!read_offset){
		printk("first char read\n");
		listing_len = nvs_read_listings_len();
		printk("len: %d\n", listing_len);
		listing_read_total = malloc(listing_len*sizeof(uint8_t));
		nvs_read_listings(&listing_read_total);
	}

	uint8_t chunk[MAX_READ_SIZE];

	// End of read handler
	// TODO: free memory
	// TODO: set proper condition for read end
	// TODO: test with mobile to see if condition works
	// TODO: test if delay is required between reads
	if (listing_len <=read_offset && read_offset != 0){
		// TODO verify this is correct
		printk("finished reading\n");
		printk("read_offset: %d\n", read_offset);
		printk("listin_len: %d\n", listing_len);
		printk("read_offset >= listing_len\n");
		nvs_delete_listings();//TODO make this smarter incase user loses all data
		read_offset = 0;
		free(listing_read_total);
		listing_len = 0;
		return 0;
	}
	if (!listing_len){
		printk("listing_len is 0\n");
		//set chunk to no values
		return;
	}
	int i=0;
	for (i=0; i < MAX_READ_SIZE; i++){
		if (read_offset < listing_len){
			chunk[i] = listing_read_total[read_offset];
			read_offset++;
		}
		else{
			printk("end of chunk: %d", i);
			break;
		}
	}
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &chunk,
				i);
}

static ssize_t listing_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{	

	printk("here again\n");
	uint8_t *p = (uint8_t *)buf;
	if (!write_offset){
		printk("first char write\n");
		printk("len: %d\n", listing_write_len);
		listing_write_total = malloc(listing_write_len*sizeof(uint8_t));
	}
	
	for (int i = 0; i < MAX_WRITE_SIZE && write_offset <listing_write_len ; i++){
		listing_write_total[write_offset] = p[i];
		write_offset++;
	}

	if (write_offset >= listing_write_len){
		printk("write_offset: %d\n", write_offset);
		printk("listing_write_len: %d\n", listing_write_len);
		printk("write_offset >= listing_write_len\n");
		// for (int i = 30; i < listing_write_len; i++){
		// 	printk("%02x ", listing_write_total[i]);
		// }
		printk("\n");
		switch (listing_write_len)
		{
		case DGC_LISTING_LENGTH:
			handle_dgc(listing_write_total, listing_write_len);
			break;
		case SHORT_TREATMENT_LISTING_LENGTH:
			send_treatment(listing_write_total);
			break;
		default:
			break;
		}
		
		// save_listings(listing_write_total, listing_write_len);
		free(listing_write_total);
		write_offset = 0;
	}
}

static ssize_t gateway_listing_len_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	uint16_t read_len = 0;
	read_len = nvs_read_listings_len();
	printk("read_len: %d\n", read_len);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &read_len,
				sizeof(read_len));
}


static ssize_t factory_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(factory_ct)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);
	factory_ct_update = 1U;
	//get value from buffer
	uint8_t *p = (uint8_t *)buf;
	//print all bytes received in p 
	for (int i = 0; i < len; i++) {
		printk("%02x ", *p++);
	}
	// //print characteristic name
	// printk("config_write_ct: %d\n", p[0]);
	// //print value
	printk("char handle: %d\n",attr->handle);

	//switch case handle type
	switch (attr->handle) {
	case 0x0000:
		factory_reset();
		break;
	default:
		break;
	}
	return len;
}

static ssize_t gateway_listing_len_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	uint8_t *p = (uint8_t *)buf;
	listing_write_len = p[0] << 8 | p[1];
	printk("listing_len_write: %d\n", listing_write_len);
	// nvs_write_listings_len(listing_write_len);
	// TODO: add write to memory
}


static ssize_t timestamp_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	uint8_t timestamp[4];
	memcpy(timestamp,nvs_read_timestamp(),4);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, timestamp,
				4);

	// return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
	// 			 sizeof(timestamp_ct));
}

static ssize_t timestamp_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	uint8_t *p = (uint8_t *)buf;
	uint32_t timestamp = p[0] << 24 | p[1] << 16 |  p[2] << 8 | p[3];
	printk("timestamp: %d\n", timestamp);
	//write to memory
	nvs_write_timestamp(timestamp);
	update_timestamp(timestamp);

	// if (!clock_initialized){
	// 	clock_init();
	// }
	// TODO: add write to memory
}


static ssize_t mac_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	uint8_t mac[6];
	memcpy(mac,nvs_read_mac(),6);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, mac,
				6);

	// return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
	// 			 sizeof(timestamp_ct));
}

static ssize_t mac_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	uint8_t *p = (uint8_t *)buf;
	uint8_t mac[6] = {p[0], p[1], p[2], p[3], p[4], p[5]};
	printk("mac: %d\n", mac);
	//write to memory
	nvs_write_mac(mac);
}


static ssize_t scan_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;
	uint8_t scan[1];
	return bt_gatt_attr_read(conn, attr, buf, len, offset, scan,
				1);
}

static ssize_t scan_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	printk("scan write\n");
	printk("scanning site");
	long_beep();
	k_work_schedule(&ble_scan_start_work, Z_TIMEOUT_MS(500));
	printk("calendar paused");
	pause_clock();
}

static ssize_t config_read_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	uint8_t config_data[14];
	uint16_t biz_id = nvs_read_business_id();
	uint8_t time[4];
	memcpy(time,nvs_read_timestamp(),4) ;
	uint8_t loc[8];
	memcpy(loc,nvs_read_location(),8) ;
	uint8_t biz_arr[] = {biz_id >> 8, biz_id};
	//printk all the values being copied
	printk("\nbiz: ");
	for (size_t i = 0; i < 2; i++)
	{
		printk("%02X", biz_arr[i]);
	}
	printk("\n");
	printk("time: ");
	for (size_t i = 0; i < 4; i++)
	{
		printk(" %02X", time[i]);
	}
	printk("\n");
	printk("location: ");
	for (size_t i = 0; i < 8; i++)
	{
		printk("%02X", loc[i]);
	}
	printk("\n");

	
	memcpy(config_data,biz_arr ,2);
	memcpy(config_data+2,time,4);
	memcpy(config_data+6,loc,8);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, config_data,
			14);
}

static ssize_t config_write_ct(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	printk("config write\n");
}

static void config_ct_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	/* TODO: Handle value */
}


/* Current Time Service Declaration */
BT_GATT_SERVICE_DEFINE(cts_cvs_list,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_LISTING_SERVICE),
	BT_GATT_CHARACTERISTIC(BT_UUID_LISTING_CHARACTERISTIC, BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       listing_read_ct, listing_write_ct, listing_ct),
	BT_GATT_CHARACTERISTIC(BT_UUID_LISTING_LEN_CHARACTERISTIC, BT_GATT_CHRC_READ |
			      BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       gateway_listing_len_read_ct, gateway_listing_len_write_ct, listing_num_ct),
);



BT_GATT_SERVICE_DEFINE(cts_cvs_config,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_CONFIGURATION_SERVICE),
	BT_GATT_CHARACTERISTIC(BT_UUID_FACTORY_RESET_CHARACTERISTIC, BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       factory_read_ct, factory_write_ct, factory_ct),
	BT_GATT_CHARACTERISTIC(BT_UUID_TIMESTAMP_CHARACTERISTIC, BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       timestamp_read_ct, timestamp_write_ct, timestamp_ct),
	BT_GATT_CHARACTERISTIC(BT_UUID_MAC_CHARACTERISTIC, BT_GATT_CHRC_READ |
				BT_GATT_CHRC_WRITE,
				BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
				mac_read_ct, mac_write_ct, mac_ct),
	BT_GATT_CHARACTERISTIC(BT_UUID_SCAN_CHARACTERISTIC, BT_GATT_CHRC_READ |
				BT_GATT_CHRC_WRITE,
				BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
				scan_read_ct, scan_write_ct, scan_ct),
	BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_CHARACTERISTIC, BT_GATT_CHRC_READ |
				BT_GATT_CHRC_WRITE,
				BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
				config_read_ct, config_write_ct, config_ct),
	BT_GATT_CCC(config_ct_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);
// void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
// {
// 	printk("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
// }

// static struct bt_gatt_cb gatt_callbacks = {
// 	.att_mtu_updated = mtu_updated
// };

