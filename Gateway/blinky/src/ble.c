
/** @file
 *  @brief BLE logic
 */

#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt_dm.h>
#include <bluetooth/scan.h>

#include <dk_buttons_and_leds.h>
#include <sys/byteorder.h>
#include <stdlib.h>
#include "aggregator.h"
#include "fs.h"
#include "ble.h"
#include "services.h"
#include "listing.h"
#include "clock.h"
#include "led.h"
#include "lte.h"
#include "button.h"

#include <net/net_pkt.h>
#include <net/net_if.h>
// extern uint8_t g_listing_data[MAX_LISTING_ARRAY_SIZE];
// int g_listing_idx;



static treatment m_treatment;

static ble_conn_handler m_ble_conn_handler;

struct k_timer scan_stop_timer;

extern bool time_paused;

bool peripheral = true;



#define MFG_DATA_SIZE 15



static uint8_t mfg_data[MFG_DATA_SIZE] = { 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, GATEWAY_SERIAL_ID & 0xff, (GATEWAY_SERIAL_ID >> 8) & 0xff , 0x00, 0x00, 0x00, 0x00, 0x00};

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE,
		(CONFIG_BT_DEVICE_APPEARANCE >> 0) & 0xff,
		(CONFIG_BT_DEVICE_APPEARANCE >> 8) & 0xff),
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_HRS_VAL)),
			  
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, BT_PER_COMP_NAME),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, MFG_DATA_SIZE),
};

#define BT_UUID_BUZZER 0xfc, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x00, 0x30, 0x2f, 0xe6
#define BT_UUID_BUZZER_SERVICE   BT_UUID_DECLARE_128(BT_UUID_BUZZER)

#define BT_UUID_CONFIG 0xfc, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x00, 0x10, 0x2f, 0xe6
#define BT_UUID_CONFIG_SERVICE   BT_UUID_DECLARE_128(BT_UUID_CONFIG)

#define BT_UUID_BUZZER_char 0xfc, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x01, 0x30, 0x2f, 0xe6
#define BT_UUID_BUZZER_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_BUZZER_char)

#define BT_UUID_BACKOFF 0xfc, 0xc8, 0x51, 0x60, 0xc9, 0xc9, 0x52, 0x44,    \
			    0xad, 0x88, 0x87, 0xac, 0x04, 0x10, 0x2f, 0xe6
#define BT_UUID_BACKOFF_CHARACTERISTIC   BT_UUID_DECLARE_128(BT_UUID_BACKOFF)


#define BUZZER_CONTROL_CHAR_HANDLE 0x2A
#define BT_BACKOFF_CHAR_HANDLE 0x13
#define TREATMENT_DELAY_CHAR_HANDLE 0x15
#define TREATMENT_TYPE_CHAR_HANDLE 0x13
#define BT_UUID_TOBE_VAL 0xeeee
/** @def BT_UUID_HIDS
 *  @brief HID Service
 */
#define BT_UUID_TOBE \
	BT_UUID_DECLARE_16(BT_UUID_TOBE_VAL)

#define BT_TOBE_DFU_VAL 0x4001
/** @def BT_DFU
 *  @brief DFUService
 */

#define BT_TOBE_DFU \
	BT_UUID_DECLARE_16(BT_TOBE_DFU_VAL)
/** @def BT_UUID_BT_TOBE_DFU
 *  @brief DFU Service UUID value
 */

#define STOP_SCAN_INTERVAL 120000 // 30000//120000 //Time needed to scan and retrieve all data from devices
#define START_SCAN_INTERVAL 15000 //15000//86400000 //Daily interval 
#define STOP_TREATMENT_SCAN_INTERVAL 5000 //Time needed to scan and send treatment to all devices
#define HTTP_DELAY_TIME 5000
#define ADVERTISING_START_TIME 35000 //Time needed to start advertising
#define MAX_ADV_DEVICES 200

static struct bt_conn *default_conn;

devices devices_init[] ={
	{
		.device = NULL,
		.devices_len = 0
	}

};


struct k_work_delayable ble_scan_start_work;
struct k_work_delayable ble_scan_stop_work;
struct k_work_delayable reboot_work;
struct k_work_delayable send_post_work;
struct k_work_delayable advertising_start_work;
struct k_work_delayable stop_lte_work;
extern struct k_work_delayable clock_work;
struct k_work_delayable treatment_send_work;
struct k_work_delayable adv_data_flag_work;


void add_data_to_ignition_adv(uint8_t * new_data, uint8_t ind);

void ble_restart_adv(void);

static volatile uint8_t adv_count;
static volatile uint16_t listings_count;
bool compiling_adv = false;;
static uint32_t GetFreeMemorySize()
{
	uint32_t  i;
	uint32_t  len;
	uint8_t*  ptr;
	
	for(i=1;;i++)
	{
		len = i * 256;
		ptr = (uint8_t*)malloc(len);
		if (!ptr)
		break;
		free(ptr);
	}

	len -= 256; //(i-1)*256 is aviliable size
	
	return len;
}

uint8_t * listings_adv_data;
uint8_t ignition_adv_data[MAX_ADV_DEVICES*IGNITION_DATA_LISTING_BODY_LENGTH];
uint8_t periodic_adv_data[MAX_ADV_DEVICES*PERIODIC_SENSOR_DATA_LISTING_LENGTH];
uint8_t diagnostics_adv_data[MAX_ADV_DEVICES*DIAGNOSTICS_LISTING_BODY_LENGTH];
uint8_t cartridge_adv_data[MAX_ADV_DEVICES*CARTRIDGE_LISTING_BODY_LENGTH];


device device_adv[MAX_ADV_DEVICES];



#include <bluetooth/addr.h>

void get_mac_address(uint8_t *mac_addr)
{	
    bt_addr_le_t addr;
    size_t id_count = 1;

    bt_id_get(&addr, &id_count);

    size_t len = sizeof(addr.a.val);
    for (int i = 0; i < len / 2; i++) {
        uint8_t tmp = addr.a.val[i];
        mac_addr[i] = addr.a.val[len - i - 1];
        mac_addr[len - i - 1] = tmp;
    }
}

static void discover_bzs_completed(struct bt_gatt_dm *dm, void *ctx)
{
	bt_gatt_dm_data_print(dm);	
	int err;
	
	static uint8_t data_backoff[2] = {0x3A,0x00};
	err = bt_gatt_write_without_response(default_conn,TREATMENT_DELAY_CHAR_HANDLE,&m_treatment.delay,sizeof(m_treatment.delay),false);
	err = bt_gatt_write_without_response(default_conn,TREATMENT_TYPE_CHAR_HANDLE,&m_treatment.type,sizeof(m_treatment.type),false);
	

	err = bt_gatt_write_without_response(default_conn,BT_BACKOFF_CHAR_HANDLE,&data_backoff,sizeof(data_backoff),false);

		// err = bt_gatt_write(default_conn,&params);

	// if (err) {
	// 	printk("Failed to write characteristic (err %d)\n", err);
	// }

	err = bt_gatt_dm_data_release(dm);
	if (err) {
		printk("Could not release the discovery data (err %d)\n", err);
	}
	
	bt_conn_disconnect(default_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);//No need if adv back off is triggered? 
	
}

static void discover_bzs_service_not_found(struct bt_conn *conn, void *ctx)
{
	printk("No more services\n");
}

static void discover_bzs_error_found(struct bt_conn *conn, int err, void *ctx)
{
	printk("The discovery procedure failed, err %d\n", err);
}

static struct bt_gatt_dm_cb discover_bzs_cb = {
	.completed = discover_bzs_completed,
	.service_not_found = discover_bzs_service_not_found,
	.error_found = discover_bzs_error_found,
};


int data_compare(uint8_t * a, uint8_t *b, int len){
	int i;
	for (i = 0; i < len; i++) {
		if (a[i] != b[i]) {
			return 1;
		}
	}
	printk("Data is the same\n");
	return 0;
}

void add_data_to_periodic_morning_adv(uint8_t * new_data, uint8_t ind){
	uint8_t hive_id[8] = {new_data[26], new_data[27],new_data[28], new_data[29], new_data[30], new_data[31], new_data[32], new_data[33]};
	uint8_t t_arr[4] = {new_data[45] , new_data[46], new_data[47] , new_data[48]};
	uint8_t hive_temp[2] = {new_data[50] , new_data[51]};	
	uint8_t chip_temp[2] = {0x00,0x00};
	uint8_t chamber_temp[2] = {new_data[52] , new_data[53]};	
	uint8_t board_temp[2] = {new_data[54] , new_data[55]};	
	uint8_t battery = new_data[49];
	uint8_t battery_deprecated[4] = {0x00, 0x00, 0x00, 0x00};
	uint8_t charge_status = {0};
	uint8_t charge_trend = {0};
	
	//firmware version is index 17 to 20 
	//then ddc flag is 21 
	//so device id is 22 to 25
	//then hive id is 26 to 33
	

	uint8_t error = {0};
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind, hive_id, 8);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+8, t_arr, 4);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+12, hive_temp, 2);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+14, chip_temp, 2);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+16, chamber_temp, 2);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+18, board_temp, 2);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+20, &battery, 1);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+21, battery_deprecated, 4);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+25, &charge_status, 1);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+26, &charge_trend, 1);
	memcpy(periodic_adv_data+PERIODIC_SENSOR_DATA_LISTING_LENGTH*ind+27, &error, 1);
	
	// free(temp);
	return;
}

void add_data_to_ignition_adv(uint8_t * new_data, uint8_t ind){

	uint32_t curr_t = get_timestamp_secs()/60;
	uint8_t t_arr[4] = {new_data[94] , new_data[95], new_data[96] , new_data[97]};
	//firmware version is index 17 to 20 
	//then ddc flag is 21 
	//so device id is 22 to 25
	//then hive id is 26 to 33
	uint8_t hive_id[8] = {new_data[26], new_data[27],new_data[28], new_data[29], new_data[30], new_data[31], new_data[32], new_data[33]};
	uint8_t batteries[4] = {new_data[98],new_data[100],new_data[102],new_data[104]};
	uint8_t deltas[4] = {new_data[99],new_data[101],new_data[103],new_data[105]};
	uint8_t success_rate = new_data[36];
	uint8_t correction_tot = new_data[37];
	uint8_t events_tot = new_data[38];
	uint8_t error = {0};
	uint8_t other_data[12] = {0};
	

	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind, t_arr, 4);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+4, hive_id, 8);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+12, other_data, 12);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+24, batteries, 4);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+28, deltas, 4);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+32, &success_rate, 1);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+33, &correction_tot, 1);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+34, &events_tot, 1);
	memcpy(ignition_adv_data+IGNITION_DATA_LISTING_BODY_LENGTH*ind+35, &error, 1);
	// free(temp);
	return;

}

void add_data_to_cartridge_adv(uint8_t * new_data, uint8_t ind){
	// offset = INITIAL_IGNITION_DATA_OFFSET
	//conctaenate new device adv data to current ignition_adv_data
	
	// "timestamp": Number, // 4 bytes
	// "status": Number, // 1 byte

	uint32_t curr_t = get_timestamp_secs()/60;
	uint8_t t_arr[4] = {new_data[118] , new_data[119], new_data[120] , new_data[121]};
	//firmware version is index 17 to 20 
	//then ddc flag is 21 
	//so device id is 22 to 25
	//then hive id is 26 to 33
	uint8_t status = new_data[34];
	

	memcpy(cartridge_adv_data+CARTRIDGE_LISTING_BODY_LENGTH*ind, t_arr, 4);
	memcpy(cartridge_adv_data+CARTRIDGE_LISTING_BODY_LENGTH*ind+4, &status, 1);
	return;

}

void add_data_to_diagnostics_adv(uint8_t * new_data, uint8_t ind){
	// offset = INITIAL_IGNITION_DATA_OFFSET
	//conctaenate new device adv data to current ignition_adv_data


	uint32_t curr_t = get_timestamp_secs()/60;
	uint8_t t_arr[4] = {new_data[81] , new_data[82], new_data[83] , new_data[84]};
	//firmware version is index 17 to 20 
	//then ddc flag is 21 
	//so device id is 22 to 25
	//then hive id is 26 to 33
	uint8_t big_fan_rps = new_data[85];
	uint8_t big_fan_valid = new_data[86];
	uint8_t small_fan_rps = new_data[87];
	uint8_t small_fan_valid = new_data[88];
	uint8_t solar_panel = new_data[89];
	uint8_t battery = new_data[90];
	uint8_t ignition = new_data[91];
	uint8_t sensors = new_data[92];
	uint8_t device_valid = 0x00;
	if (big_fan_valid && small_fan_valid && (battery >= 50) && ignition && sensors) device_valid = 0x01;
	else device_valid = 0x00;
	
	

	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind, t_arr, 4);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+4, &big_fan_rps, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+5, &big_fan_valid, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+6, &small_fan_rps, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+7, &small_fan_valid, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+8, &solar_panel, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+9, &battery, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+10, &ignition, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+11, &sensors, 1);
	memcpy(diagnostics_adv_data+DIAGNOSTICS_LISTING_BODY_LENGTH*ind+12, &device_valid, 1);

	// free(temp);
	return;

}

void add_to_payload(struct bt_scan_device_info *device_info, uint8_t * addr, char *p_addr){

	
	bool device_exists = false;
	    //search for device which already exists in the payload
		for (int i = 0; i < adv_count; i++) {
			if (memcmp(device_adv[i].mac_addr, addr,6U)==0) {
				device_exists = true;			
				break;
			}
		}
		
		if(device_exists == true) return;


		//replace with memcpy
		for (int i=0; i<6; i++){
				device_adv[adv_count].mac_addr[i] = addr[i];
			}

		if (m_treatment.send) connect_and_write(device_info, &p_addr);
		else{
			add_data_to_ignition_adv(device_info->adv_data->data,adv_count);
			listings_count++;
			add_data_to_periodic_morning_adv(device_info->adv_data->data,adv_count);
			listings_count++;
			add_data_to_diagnostics_adv(device_info->adv_data->data,adv_count);
			listings_count++;
			add_data_to_cartridge_adv(device_info->adv_data->data,adv_count);
			listings_count++;
		}
		
		adv_count++;


}

void connect_and_write(struct bt_scan_device_info *device_info, char * addr)
{
	int err;
	printk("Connecting to device\n");

		struct bt_conn_le_create_param *conn_params;


		conn_params = BT_CONN_LE_CREATE_PARAM(
				BT_CONN_LE_OPT_CODED | BT_CONN_LE_OPT_NO_1M,
				BT_GAP_SCAN_FAST_INTERVAL,
				BT_GAP_SCAN_FAST_INTERVAL);

		err = bt_conn_le_create(device_info->recv_info->addr, conn_params,
					BT_LE_CONN_PARAM_DEFAULT,
					&default_conn);

		if (err) {
			printk("Create conn failed (err %d)\n", err);

			err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
			if (err) {
				printk("Scanning failed to start (err %d)\n", err);
			}
		}

		printk("Connection pending to: %s\n",addr);
}

void adv_data_flag(struct k_work *work)
{
	if (peripheral) ble_set_adv_data_flag(0x00);
}

void treatment_work_handler(struct k_work *work){
	m_treatment.send = true;
	int err;
	err = bt_le_adv_stop();
    if (err!=0) {
      printk("adv cannot be stopped (err %d)\n", err);
    }
	exit_low_power();
	printk("Starting treatment scan\n");
	// scan_init();
	// k_timer_start(&scan_stop_timer, K_SECONDS(10), K_NO_WAIT);
	k_work_schedule(&ble_scan_stop_work, Z_TIMEOUT_MS(STOP_TREATMENT_SCAN_INTERVAL));

	err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
	if (err!=0) {
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}
}

void update_treatment_params(uint16_t delay, uint8_t type){
	m_treatment.delay = delay;
	m_treatment.type = type;
}
void stop_send_treatment(){
	m_treatment.send = false;
	

}

static void scan_filter_match(struct bt_scan_device_info *device_info,
			      struct bt_scan_filter_match *filter_match,
			      bool connectable)
{
	
		int err;
		char addr[BT_ADDR_LE_STR_LEN];

		bt_addr_le_to_str(device_info->recv_info->addr, addr, sizeof(addr));
		if (!compiling_adv){
			compiling_adv = true;
			add_to_payload(device_info, device_info->recv_info->addr->a.val, addr);
			compiling_adv = false;
		}
	

}

static void connected(struct bt_conn *conn, uint8_t conn_err)
{
	// g_listing_idx = 0;
	block_button();
	int err;
	struct bt_conn_info info;
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	if (peripheral){
		if (conn_err) {
		printk("Connection failed (err %d)\n", conn_err);
		release_button();
		return;
		}

		err = bt_conn_get_info(conn, &info);

		if (err) {
			printk("Failed to get connection info\n");
			release_button();
		} else {
			const struct bt_conn_le_phy_info *phy_info;
			phy_info = info.le.phy;

			printk("Connected: %s, tx_phy %u, rx_phy %u\n",
				addr, phy_info->tx_phy, phy_info->rx_phy);
			m_ble_conn_handler.connected = true;
			
		}
	}
	else{

	if (conn_err) {
		release_button();
		printk("Failed to connect to %s (%u)\n", addr, conn_err);

		bt_conn_unref(default_conn);
		default_conn = NULL;
		
		err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
		if (err) {
			printk("Scanning failed to start (err %d)\n", err);
		}

		return;
	}


	err = bt_conn_get_info(conn, &info);

	if (err) {
		release_button();
		printk("Failed to get connection info\n");
	} else {
		const struct bt_conn_le_phy_info *phy_info;

		/*************************************************************************************************************
		Changed conn.h in sdk, added const struct bt_conn_le_phy_info  *phy without CONFIG_BT_USER_PHY_UPDATE condition
		**************************************************************************************************************/

		phy_info = info.le.phy;
		printk("Connected: %s, tx_phy %u, rx_phy %u\n",
				addr, phy_info->tx_phy, phy_info->rx_phy);
		m_ble_conn_handler.connected = true;
	}

	if (conn == default_conn) {
		printk("discovering %s\n", addr);
		err = bt_gatt_dm_start(conn, NULL, &discover_bzs_cb, NULL);
		if (err) {
			release_button();
			printk("Failed to start discovery (err %d)\n", err);
		}
	}
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	int err;
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	release_button();
	printk("Disconnected: %s (reason 0x%02x)\n", addr, reason);
	reset_listing_read();
	m_ble_conn_handler.connected = false;
	//print g_listing_data in for loop
	// int g_listing_data_len = sizeof(g_listing_data_len)/sizeof(uint8_t);
	// printk("index length: %d\n", g_listing_idx);
	// for(int i = 0; i < g_listing_idx; i++){
	// 	printk("%02x ", g_listing_data[i]);
	// }
	if (default_conn != conn) {
		printk("default_conn != conn\n");
		k_work_schedule(&adv_data_flag_work, Z_TIMEOUT_MS(3*1000));
		return;
	}

	bt_conn_unref(default_conn);
	default_conn = NULL;
	if (!peripheral){
		err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
		if (err) {
			printk("Scanning failed to start (err %d)\n", err);
		}
	}
	if (m_ble_conn_handler.is_pm){
		adv_normal_mode();
	}
	if (peripheral) ble_set_adv_data_flag(0x00);
	
	

	
}

BT_SCAN_CB_INIT(scan_cb, scan_filter_match, NULL, NULL, NULL);
BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void scan_init(void)
{
	int err;

	/* Use active scanning and disable duplicate filtering to handle any
	 * devices that might update their advertising data at runtime. */
	struct bt_le_scan_param scan_param = {
		.type     = BT_LE_SCAN_TYPE_ACTIVE,
		.interval = BT_GAP_SCAN_FAST_INTERVAL,
		.window   = BT_GAP_SCAN_FAST_WINDOW,
		.options  = BT_LE_SCAN_OPT_CODED | BT_LE_SCAN_OPT_NO_1M
	};

	struct bt_scan_init_param scan_init = {
		.connect_if_match = 0,
		.scan_param = &scan_param,
		.conn_param = NULL
	};

	bt_scan_init(&scan_init);
	bt_scan_cb_register(&scan_cb);
// #define DEVICE_NAME 0x54


	err = bt_scan_filter_add(BT_SCAN_FILTER_TYPE_UUID, BT_TOBE_DFU);
	if (err) {
		printk("Scanning filters cannot be set (err %d)\n", err);

		return;
	}

	err = bt_scan_filter_enable(BT_SCAN_UUID_FILTER, false);
	if (err) {
		printk("Filters cannot be turned on (err %d)\n", err);
	}
}



void adv_work_handler(void){
	peripheral = true;
	printk("adv_work_handler\n");
	int err;
	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		// return;
	}
	
	enter_low_power();
	resume_clock();
	release_button();
	k_work_schedule(&clock_work, Z_TIMEOUT_MS(SEC_INTERVAL));
}

void set_ble_adv_pm(){
	m_ble_conn_handler.is_pm = true;
}

void adv_normal_mode(){
	if (m_ble_conn_handler.connected){
		printk("cant revert to normal adv, still connected\n");
		return;
	}
	printk("adv_normal mode\n");
	int err;
	 err = bt_le_adv_stop();
    if (err!=0) {
      printk("adv cannot be stopped (err %d)\n", err);
    }
	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}
	m_ble_conn_handler.is_pm = false;
}



void ble_scan_stop(void){
	printk("adv count %d: ",adv_count);
	release_button();
	compiling_adv = false;
	leds_off();
	int err;
	printk("Stopping scan\n");
	err = bt_le_scan_stop();
	if (err!=0) {
		printk("Scanning cannot be stopped (err %d)\n", err);
	}
	/* send abrv treatment functionality
	if (m_treatment.send){
		adv_count=0;
		k_work_schedule(&advertising_start_work, Z_TIMEOUT_MS((10)*1000));
	    stop_send_treatment();
		printk("Enter low power\n");
		return;
	}
	*/
	
	
	printk("data stored: \n");
	compile_ignition_listings(ignition_adv_data, adv_count, device_adv);
	compile_periodic_listings(periodic_adv_data, adv_count, device_adv);
	compile_diagnostics_listings(diagnostics_adv_data, adv_count, device_adv);
	compile_cartridge_listings(cartridge_adv_data, adv_count, device_adv);
	compile_gateway_status_listing(adv_count);
	listings_count++;
	uint16_t curr_num = nvs_read_listings_num();
	uint16_t new_num = curr_num + listings_count;
	nvs_write_listings_num(new_num);
	printk("adv count: %d\n",adv_count);
	adv_count=0;
	listings_count = 0;
	
	// TODO reset adv_data arrays

	
	save_listings_from_inst();
	nvs_write_post_flag(1);
	nvs_write_timestamp(get_timestamp_mins()+1);
	k_work_schedule(&reboot_work, Z_TIMEOUT_MS(5*SEC_INTERVAL));
}

void reboot_work_handler(void){
	printk("rebooting\n");
	sys_reboot(SYS_REBOOT_WARM);
}


void ble_scan_start(void)

{
	exit_low_power();
	led_yellow();
	peripheral = false;
	adv_count = 0;
	listings_count = 0;
	bt_le_adv_stop();
	printk("Starting scan\n");
	// scan_init();
	k_work_schedule(&ble_scan_stop_work, Z_TIMEOUT_MS(STOP_SCAN_INTERVAL));

	int err;
	err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
	if (err!=0) {
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}
	
}

void ble_advertise_time(uint32_t adv_time){
	static uint8_t new_mfg_data[MFG_DATA_SIZE];
	memcpy(new_mfg_data, mfg_data,sizeof(new_mfg_data));

	new_mfg_data[4] = (uint8_t)(adv_time >> 24);
	new_mfg_data[5] = (uint8_t)(adv_time >> 16);
	new_mfg_data[6] = (uint8_t)(adv_time >> 8);
	new_mfg_data[7] = (uint8_t)(adv_time & 0xFF);
	memcpy(mfg_data, new_mfg_data,sizeof(new_mfg_data));
	memcpy(sd[1].data, new_mfg_data,sizeof(new_mfg_data));

	printk("adv update time mode\n");
	ble_restart_adv();
}

void ble_advertise_battery(void){
	static uint8_t new_mfg_data[MFG_DATA_SIZE];
	memcpy(new_mfg_data, mfg_data,sizeof(new_mfg_data));
	
	int bat_lvl = get_battery();
	uint8_t bat_hundreds = bat_lvl/100;
	uint8_t bat_tens = (bat_lvl - bat_hundreds*100)/10;
	new_mfg_data[10] = bat_hundreds;
	new_mfg_data[11] = bat_tens;
	memcpy(mfg_data, new_mfg_data,sizeof(new_mfg_data));
	memcpy(sd[1].data, new_mfg_data,sizeof(new_mfg_data));
	printk("adv update battery mode\n");
	ble_restart_adv();
}

void ble_advertise_lte_vals(void){
	uint8_t *data = get_signal_vals();
    static uint8_t new_mfg_data[MFG_DATA_SIZE];
    memcpy(new_mfg_data, mfg_data,sizeof(new_mfg_data));
    new_mfg_data[12] = data[0];
    new_mfg_data[13] = data[1];
	memcpy(mfg_data, new_mfg_data,sizeof(new_mfg_data));
    memcpy(sd[1].data, new_mfg_data,sizeof(new_mfg_data));
    printk("adv update lte mode\n");
    ble_restart_adv();
    free(data);
}

void ble_set_adv_data_flag(uint8_t flag){
	static uint8_t new_mfg_data[MFG_DATA_SIZE];
	memcpy(new_mfg_data, mfg_data,sizeof(new_mfg_data));
	new_mfg_data[14] = flag;
	memcpy(mfg_data, new_mfg_data,sizeof(new_mfg_data));
	memcpy(sd[1].data, new_mfg_data,sizeof(new_mfg_data));
	printk("adv update data flag\n");
	ble_restart_adv();
}

void ble_restart_adv(void){
	int err;
	 err = bt_le_adv_stop();
    if (err!=0) {
      printk("adv cannot be stopped (err %d)\n", err);
    }
	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}
}

void ble_init(void)
{
	compiling_adv = false;
	m_treatment.send = false;
	// peripheral = false;
	m_ble_conn_handler.connected = false;
	m_ble_conn_handler.is_pm = false;
	int err;
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	if (err) {
		printk("Blueooth failed to initialize (err %d)\n", err);
		return;
	}
	printk("Bluetooth initialized\n");
	 uint8_t mac_address[6];
    get_mac_address(mac_address);
	//save mac to nvs
	nvs_write_mac(mac_address);

	printk("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac_address[0], mac_address[1], mac_address[2],
           mac_address[3], mac_address[4], mac_address[5]);
	scan_init();


	//TODO retrieve registration state from flash.
	// if registered, advertise registered 
	// if not registered, advertise unregistered

	uint8_t post_flag = nvs_read_post_flag();
	if (post_flag){
		pause_clock();
		k_work_schedule(&send_post_work, Z_TIMEOUT_MS(HTTP_DELAY_TIME));
	}
	else{
		// start BLE 4 adv
		err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
					sd, ARRAY_SIZE(sd));
		if (err) {
			printk("Advertising failed to start (err %d)\n", err);
			// return;
		}
		enter_low_power();
		//read listings length
		uint16_t len = nvs_read_listings_len();
		if (len) {
			printk("data exists, setting adv data flag\n");
			ble_set_adv_data_flag(0x01);
		}

		
	}

	k_work_init_delayable(&adv_data_flag_work, adv_data_flag);
	k_work_init_delayable(&treatment_send_work, treatment_work_handler);
}

