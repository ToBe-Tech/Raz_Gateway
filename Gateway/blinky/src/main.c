/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <drivers/gps.h>
#include <drivers/sensor.h>
#include <console/console.h>
#include <net/nrf_cloud.h>
#include <dk_buttons_and_leds.h>
#include <modem/lte_lc.h>
#include <sys/reboot.h>
#include <drivers/gpio.h>


#include <modem/nrf_modem_lib.h>
#include <modem/modem_info.h>
#if defined(CONFIG_NRF_MODEM_LIB)
#include <nrf_modem.h>
#endif


#include "aggregator.h"

#include "ble.h"
#include "alarm.h"
#include "led.h"
#include <net/socket.h>
#include "button.h"
#include "fs.h"
#include "clock.h"
#include "modemSetup.h"
#include "low_power.h"
#include "listing.h"
#include "lte.h"
#include "battery.h"

/* Interval in milliseconds after the device will retry cloud connection
 * if the event NRF_CLOUD_EVT_TRANSPORT_CONNECTED is not received.
 */

#define BAT_MIN 3900

#define MAX_MTU_SIZE     1000
#define RECV_BUF_SIZE    4096
#define SEND_BUF_SIZE    MAX_MTU_SIZE

#define LED_RED 29
#define LED_GREEN 30
#define LED_BLUE 31

#define __ONLINE

const struct device *dev;

void short_beep(void);

void long_beep(void);

void tone_b(void);

void tone_a(void);

//////////https://41x9s3uqe5.execute-api.us-east-1.amazonaws.com/prod/listing ##original prod endpoint, doesn't work

/*########UNCOMMNENT FOR PRODUCTION############*/
#define PROD
/*########UNCOMMNENT FOR PRODUCTION############*/

#define Z_HTTP_HOST_DEV "l2eldm9fh1.execute-api.us-east-1.amazonaws.com"
#define Z_HTTP_HOST_PROD "f9v5raedt4.execute-api.us-east-1.amazonaws.com"
#define Z_HTTP_PORT 443
#define Z_PATH_DEV "/dev/listing"
#define Z_PATH_PROD "/prod2/listing"

#ifdef PROD
#define Z_HTTP_HOST Z_HTTP_HOST_PROD
#define Z_PATH Z_PATH_PROD
#else
#define Z_HTTP_HOST Z_HTTP_HOST_DEV
#define Z_PATH Z_PATH_DEV
#endif

#define STRINGIFY(x) #x

#define HTTP_HDR_END "\r\n\r\n"
#define LISTINGS_GROUP_SIZE 20
#define TEST_STRING	"21AAAAAABBBBBB096DACA701636c6f75640001A5DC5C000000000000000000010017000007d001A5DC5C1B101B111C111B12042063050101FE"

#define POST_TEMPLATE "POST %s? HTTP/1.1\r\n"\
                      "Host: %s\r\n"\
                      "Connection: close\r\n"\
                      "Content-Type: text/plain\r\n"\
                      "Content-length: %d\r\n\r\n"\
                      "%s"

bool connectToLTE(void);

bool app_http_start_zypher(char * post_buf,int send_data_len, int *listings_post_count, uint16_t ** del_listing_ind, uint16_t current_ind, int num_curr_listings);

extern bool time_paused;

#define RETRY_CONNECT_WAIT K_MSEC(90000)


/* Sensor data */
int countDigits(int num);

/* Structures for work */
static struct k_work_delayable connect_work;
struct k_work_delayable aggregated_work;
extern struct k_work_delayable send_post_work;
extern struct k_work_delayable ble_scan_start_work;
extern struct k_work_delayable ble_scan_stop_work;
extern struct k_work_delayable reboot_work;
extern struct k_work_delayable advertising_start_work;
extern struct k_work_delayable advertising_normal_work;
extern struct k_work_delayable clock_work;
extern struct k_work_delayable stop_lte_work;
extern struct k_work_delayable button_time_count;
extern struct k_work_delayable adv_data_flag_work;


/* Forward declaration of functions */
static void work_init(void);

void sensor_data_send(struct nrf_cloud_sensor_data *data);

static uint32_t GetFreeMemorySizeA()
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

static void stop_lte_work_handler(struct k_work *work){
	int err;
	//COBY NO LED
	leds_off();
	err = lte_lc_deinit();
	
	if (err){
		LOG_INF("lte_lc_deinit failed: %d\n", err);
	}
	else{
		LOG_INF("lte_lc_deinit success: %d\n", err);
	}
	// write time and schedule restart
	nvs_write_timestamp(get_timestamp_mins()+1);
	k_work_schedule(&reboot_work, Z_TIMEOUT_MS(5*SEC_INTERVAL));
	// k_work_schedule(&advertising_start_work, Z_TIMEOUT_MS(5*1000));
}

int countDigits(int num){
    int count=(num==0)?1:log10(num)+1; 
    return count;
}

/**@brief Update LEDs state. */
static void send_post(struct k_work *work)
{
	// LOG_INF("send_post\n");
	//COBY DO LED
	// block_button();
	exit_low_power();
	led_red();

	// uint32_t free_memory = GetFreeMemorySizeA();
	// LOG_INF("Free memory: %d\n", free_memory);
	// if (free_memory < 5000){
	// 	LOG_INF("Free memory too low: %d\n", free_memory);
	// 	nvs_delete_listings();
	// 	k_work_schedule(&reboot_work, Z_TIMEOUT_MS(5*1000));
	// 	return;
	// }
	int err;
	// LOG_INF("Sending post.\n");
	
	int listings_post_count = 0;
	uint16_t listings_length = nvs_read_listings_num();
	uint16_t list_size = nvs_read_listings_len();
	LOG_INF("listings length 1: %d",listings_length );
	uint16_t * del_listing_ind = malloc(listings_length*sizeof(uint16_t));
	uint8_t * listings_hex_arr = malloc(list_size*sizeof(uint8_t));
	nvs_read_listings(&listings_hex_arr);
	uint16_t current_ind = 0;
	if (list_size == 0){
		LOG_INF("No listings to send, exiting\n");
		goto exit;
	}
	bool conn = connectToLTE();
	if (!conn){
		LOG_INF("Failed to connect to LTE, exiting\n");
		goto exit;
	}
	err = lte_lc_psm_req(true);
	if (err) {
		LOG_INF("lte_lc_psm_req failed: %d\n", err);
		goto exit;
	}
	else{
		LOG_INF("lte_lc_psm_req success: %d\n", err);
	}

	/////////////////////////////////////////
	#ifdef __ONLINE
	for (int i = 0; i < list_size; i++){
		// uint32_t free_memory = GetFreeMemorySizeA();
		// LOG_INF("Free memory: %d\n", free_memory);
		char q_mark = '\'';
		char * final_body_data = malloc(2*MAX_LISTING_SIZE*LISTINGS_GROUP_SIZE);
		memcpy(final_body_data, &q_mark,1);
		int final_listing_size = 1;
		//1. copy bytes into one string containing three listings
		int m;
		for (m = 0; (m < LISTINGS_GROUP_SIZE && i < list_size); m++, i++){
			
			int header_len = listings_hex_arr[i]; 
			// LOG_INF("header_len: %d\n", header_len);
			int body_ind = header_len + i; 
			// LOG_INF("body_ind: %d\n", body_ind);
			uint8_t current_listing_size = header_len +  listings_hex_arr[body_ind] + 1; 
			// LOG_INF("hex at body bytes: %02x\n", listings_hex_arr[body_ind]);
			// LOG_INF("current_listing_size: %d\n", current_listing_size);
			uint8_t * current_listing_hex = malloc(current_listing_size*sizeof(uint8_t));
			memcpy(current_listing_hex, listings_hex_arr + i, current_listing_size);
			i+=current_listing_size-1; 

			//3. convert temp listing array to string
			char * body_data;
			body_data = hex_to_http_string(current_listing_hex, current_listing_size);
			int body_len = strlen(body_data);
			LOG_INF("body_len: %d\n", body_len);
			memcpy(final_body_data + final_listing_size,body_data,body_len);
			final_listing_size += body_len;
			free(current_listing_hex);
			free(body_data);
		}

		if (i<list_size) i--;

		LOG_INF("Sending listings, size: %d\n", final_listing_size);
		// char temp_buf[strlen(final_body_data)];
		// sprintf(temp_buf, "%c%s",'\'', final_body_data);
		// sprintf(final_body_data, "%s%c", temp_buf,'\'');

		
		memcpy(final_body_data+final_listing_size,&q_mark,1 );
	    
		char pre_buf[final_listing_size+1];
		memcpy(pre_buf, final_body_data,final_listing_size+1);
		int pre_buf_len = strlen(pre_buf);

		// final_listing_size+=2;
		char send_buf[] =
			"POST " Z_PATH " HTTP/1.1\r\n" ///1/1
			"Host: " Z_HTTP_HOST":443" "\r\n"
			"Content-length: %d\r\n"
			"Connection: close" HTTP_HDR_END
			"%s";
		
		int num_digits = countDigits(pre_buf_len);
		
		int send_len = (sizeof(send_buf)-2) + final_listing_size+1 +(num_digits-2);
		char * post_buf = malloc(send_len);
		int ret_len = snprintf(post_buf, send_len, send_buf, final_listing_size+1, pre_buf);
		free(final_body_data);
		LOG_INF("send_len: %d, ret_len: %d\n", send_len, ret_len);

		int send_data_len = strlen(post_buf);
		//4. send string
		bool is_posting = app_http_start_zypher(post_buf,ret_len, &listings_post_count,&del_listing_ind, current_ind, m);	
		free(post_buf);
		current_ind+=m;
		if (!is_posting){
			LOG_INF("Failed to send listing, exiting\n");
			goto exit;
		}
		LOG_INF("after app_http_start_zypher\n");

	}
	#endif
	/////////////////////////////////////
	
	exit:
	    LOG_INF("exit\n");
		//5. free array
		if (!listings_post_count && !listings_length) LOG_INF("no listings\n");
		else{
			// if all listings were sent, delete from memory
			if (listings_post_count == listings_length){
				LOG_INF("posted all listings\n");
				LOG_INF("listings length 2: %d",listings_length );
				nvs_delete_listings();	
			}
			else{
				LOG_INF("posted %d listings\n", listings_post_count);
				remove_posted_listings(listings_hex_arr, del_listing_ind,list_size,listings_post_count,listings_length);
			}
			free(listings_hex_arr);	
		}
		nvs_write_post_flag(0);	//TODO add check if write succesful
		k_work_schedule(&stop_lte_work, Z_TIMEOUT_MS(5*1000));
		
	}




/**@brief Initializes and submits delayed work. */
static void work_init(void)
{
	k_work_init_delayable(&send_post_work, send_post);
	k_work_init_delayable(&ble_scan_start_work, ble_scan_start);
	k_work_init_delayable(&ble_scan_stop_work, ble_scan_stop);
	//adv work init
	k_work_init_delayable(&advertising_start_work, adv_work_handler);
	//adv work init
	k_work_init_delayable(&advertising_normal_work, adv_normal_mode_handler);
	k_work_init_delayable(&clock_work, clock_work_handler);
	k_work_init_delayable(&stop_lte_work, stop_lte_work_handler);
	k_work_init_delayable(&reboot_work, reboot_work_handler);
	k_work_init_delayable(&button_time_count, button_time_count_handler);

}


bool connectToLTE(void){
	int err;
	LOG_INF("Waiting for network.. \n");
	err = cert_provision();
	if (err) {
        LOG_INF("cert_provision error\n");
		return false;
	}
	err = lte_lc_init_and_connect();

	// err = lte_lc_connect();
	if (err) {
		LOG_INF("Failed to connect to the LTE network, err %d\n", err);
		return false;
	}
	LOG_INF("OK\n");
	//WARNING: this is used to test offline listings savings, changed to true for production
	return true; // true
	
}

static char z_recv_buf[RECV_BUF_SIZE];
static char chunk[128];

int findPosOfChar(char ch) {
	char qolon = '{';//character to search
	char *quotPtr = strchr(z_recv_buf, qolon);
	if(quotPtr == NULL)
	{
		// Handle error
		return -1;
	}
	int positionStart = quotPtr - z_recv_buf;
	return positionStart;
}

int blocking_send(int fd, uint8_t *buf, uint32_t size, uint32_t flags)
{
	int err;

	do {
		err = send(fd, buf, size, flags);
	} while (err < 0 && (errno == EAGAIN));

	return err;
}
static char recv_buf[RECV_BUF_SIZE];

bool find_response_code(char * buf){
	char *s;
	s = strstr(buf, "200");
	if (s != NULL) {
		LOG_INF("200 OK\n");
		return true;
	}
	else{
		LOG_INF("NOT 200 OK\n");
		return false;
	}
}

bool app_http_start_zypher(char * post_buf,int send_data_len, int *listings_post_count, uint16_t ** del_listing_ind, uint16_t current_ind, int num_curr_listings)
{
	// LOG_INF("inside http start\n");
	// uint32_t free_memory = GetFreeMemorySizeA();
	// LOG_INF("Free memory: %d\n", free_memory);
	struct sockaddr_in local_addr;
	struct addrinfo *res;
	struct addrinfo hints;

	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(Z_HTTP_PORT);
	local_addr.sin_addr.s_addr = 0;

	// LOG_INF("POST Request: "Z_HTTP_HOST"\n\r");

	int err = getaddrinfo(Z_HTTP_HOST, NULL, &hints, &res);
	if (err) {
		LOG_INF("getaddrinfo errno %d\n", errno);
		/* No clean up needed, just return */
		return false;
	}

	((struct sockaddr_in *)res->ai_addr)->sin_port = htons(Z_HTTP_PORT);

	int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
	// int client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NATIVE_TLS, IPPROTO_TLS_1_2);
	// uint32_t free_memory2 = GetFreeMemorySizeA();
	// LOG_INF("\nFree memory: %d\n", free_memory2);
	enum {
		NONE = 0,
		OPTIONAL = 1,
		REQUIRED = 2,
	};

	/* Setup TLS socket options */

	err = tls_setup(client_fd, Z_HTTP_HOST);
	if (err) {
		goto clean_up;
	}

	err = connect(client_fd, (struct sockaddr *)res->ai_addr,
		      sizeof(struct sockaddr_in));
	if (err > 0) {
		LOG_INF("connect err: %d\n", errno);
		goto clean_up;
	} else {
		LOG_INF("Connected to " Z_HTTP_HOST "\n");
	}
	//get listings byte length
		
		int num_bytes = send(client_fd, post_buf, send_data_len, 0);
		if (num_bytes < 0) {
			LOG_INF("send errno: %d\n", errno);
			goto clean_up;
		}
		// else LOG_INF("send is fine\n");

		int tot_num_bytes = 0;
		// LOG_INF("sent post request\n");

		do {
			// LOG_INF("doing\n");
			/* TODO: make a proper timeout *
			* Current solution will just hang 
			* until remote side closes connection */
			// LOG_INF("receiving\n-----\n");
			num_bytes = recv(client_fd, &z_recv_buf[tot_num_bytes], RECV_BUF_SIZE - tot_num_bytes, 0);
			//print num_bytes
			// LOG_INF("num_bytes: %d\n", num_bytes);
			tot_num_bytes += num_bytes;
			
			if (num_bytes <= 0) {
				LOG_INF("\nrecv errno: %d\n", errno);
				break;
			}
			
			// LOG_INF("GOT buffer size: %d \n",num_bytes);
			
			// LOG_INF("%s\n", chunk);
			// LOG_INF("\n^^^^^\n");
			// off += num_bytes;
		} while (num_bytes != 0);
		
		// LOG_INF("z_recv_buf:\n %s\n", z_recv_buf);
		bool res_code = find_response_code(z_recv_buf);
		if (res_code){
			led_green();
			for (int i = 0; i < num_curr_listings; i++){
				memcpy(*del_listing_ind+(*listings_post_count), &current_ind, sizeof(uint16_t));
				current_ind++;
				(*listings_post_count)++;
			}

			
		} 
		else{
			led_magenta();
		}
		
		// int length = 200;
		// LOG_INF("%*.*s", length, length, z_recv_buf);
		
	

	// LOG_INF("\n\rFinished. Closing socket\n");
	
	clean_up:
		LOG_INF("cleaning up\n");
		freeaddrinfo(res);
		err = close(client_fd);
		return true;

	}

void battery_low_beep(void){
	tone_a();
	tone_b();
	tone_a();
	tone_b();
	tone_a();
	tone_b();
}


void tone_a(void){
	for (int i = 1; i < 500; ++i)
  {
	gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
	k_usleep(500);
	gpio_pin_set(dev, 28,0);
	k_usleep(500);
  }
}

void tone_b(void){
	for (int i = 1; i < 500; ++i)
  {
	gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
	k_usleep(800);
	gpio_pin_set(dev, 28,0);
	k_usleep(800);
  }
}

void fault_beep(void){
	for (int i = 0; i < 3; ++i)
  {
	short_beep();
	k_msleep(100);
  }
}

void short_beep(void){
	for (int i = 1; i < 30; ++i)
  {
	gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
	k_msleep(1);
	gpio_pin_set(dev, 28,0);
	k_msleep(1);
  }
}

void long_beep(void){
	for (int i = 1; i < 150; ++i)
  {
	gpio_pin_set(dev, 28,1);	//GPIO p0.28 == buzzer
	k_msleep(1);
	gpio_pin_set(dev, 28,0);
	k_msleep(1);
  }
}


void main(void)

{		
	int err;
	dev = device_get_binding("GPIO_0");

    // gpio_pin_configure(dev,28, GPIO_OUTPUT); //GPIO P0.28 // buzzer
	init_led();
	// gpio_pin_set(dev, 2,1);	//GPIO p0.03 == LED2
	led_cyan();
	
	// k_msleep(500);

	// startup beep
	long_beep();

  	// gpio_pin_set(dev, 2,0);	//GPIO p0.03 == LED2
	leds_off();

	#if defined(CONFIG_DK_LIBRARY)
  	dk_buttons_init(button_handler);
	#endif
	 config_t * config;
	LOG_INF("ToBe Gateway Application started\n");

	// /* add to pre-configure modem on first use to fix SSL issues
	// --------------------------------------------------
	// err = lte_lc_deinit();
	
	// __ASSERT(err == 0, "lte_lc_power_off failed: %d", err);
	// nrf_modem_lib_shutdown();
	// nrf_modem_lib_init(NORMAL_MODE);
	// err = cert_provision();
	
	// if (err) {
    //     LOG_INF("cert_provision error\n");
	// 	return;
	// }
	// -----------------------------------------------------
	// */
	
	 work_init(); // timers

	low_power_init();
	init_nvs();
	config = nvs_read_config();
	if (config == NULL) {
		LOG_INF("Failed to read config\n");
		return;
	}
	free(config);

	//***Incase of memory errors***//
	// factory_reset();
	int bat_raw = get_battery();
	LOG_INF("Battery: %d\n", bat_raw);
	if (bat_raw < BAT_MIN){
		LOG_INF("Battery too low\n");
		battery_low_beep();
	}
	char buf[40];
	get_signal_buf();
	clock_init();
	
	ble_init();

	
	
	while(1) {
		k_sleep(K_SECONDS(60));
	}
}

