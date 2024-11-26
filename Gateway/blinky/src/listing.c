
#include "listing.h"
#include "fs.h"
#include <stdlib.h>
#include <string.h>

#include <clock.h>
#include <stdio.h>
#include "battery.h"
#include "led.h"
#include "buzzer.h"
#define TREATMENT_SEND_DELAY 60
#define REDUCED_LISTING_SIZE 0x0B
#define UUID_SIZE 0x0A 

static listing_instance_t m_listing_instance;

extern struct k_work_delayable treatment_send_work;

uint16_t hex_len=0;

void handle_dgc(uint8_t * listings,uint16_t listings_len){
    int err = 0;
    uint8_t mac[6] =  {listings[12],listings[13],listings[14],listings[15],listings[16],listings[17]};
    // for (int i = 0; i < 6; i++)
	// {
	// 	printk("mac: %02x\n", mac[i]);
	// }
    uint16_t serial = listings[34] <<8 | listings[35];
    uint32_t time = listings[36]<<24 | listings[37]<<16 | listings[38]<<8 | listings[39];

    uint8_t loc[8] = {listings[40],listings[41],listings[42],listings[43],listings[44],listings[45],listings[46],listings[47]};


    // err = nvs_write_mac(mac);
    err =  nvs_write_business_serial_id(serial);
    err = (err == SUCCESS) ? nvs_write_timestamp(time):FAILURE;
    err = (err == SUCCESS) ? nvs_write_status(0x02):FAILURE;
    update_timestamp(time);
    err = (err == SUCCESS) ? nvs_write_location(loc):FAILURE;
    if (err==SUCCESS){
        printk("DGC Success!\n");
    }
    else{
        printk("DGC Failed!\n");
    } 
}

void send_treatment(uint8_t * listings){
    uint16_t time = listings[35]<<8 | listings[36] ;

    uint8_t type = listings[37];

    //schedule treatment send in 1 minute
    k_work_schedule(&treatment_send_work, Z_TIMEOUT_MS(TREATMENT_SEND_DELAY));
    pause_clock();
    update_treatment_params(time,type);
    
}


void save_listings_from_inst(){

    printk("save_listings\n");
    //get all listings string and store in new_listings
    uint8_t * new_listings;
    new_listings = get_all_listings_hex();
   // read current listings
   uint16_t current_listings_len = nvs_read_listings_len();
   uint16_t new_len = hex_len;
   hex_len = 0;
   printk("new_len: %ld\n",new_len);
   printk("current_len: %ld\n",current_listings_len);
   if (!current_listings_len){
        printk("no current listings\n");
        
        nvs_write_listings_len(new_len);
        // write current listings to nvs
        nvs_write_listings(new_listings);
        //free memory allocated
        reset_listings();
        free(new_listings);
        return;
   }
   uint8_t * current_listings = malloc(current_listings_len*sizeof(uint8_t));
    nvs_read_listings(&current_listings);
    // realloc with new listings size
    current_listings = realloc(current_listings,(current_listings_len+new_len)*sizeof(uint8_t));
    // copy new listings to current listings
    memcpy(current_listings+current_listings_len,new_listings,new_len);
        //write new listings length
    nvs_write_listings_len(current_listings_len+new_len);
    // write current listings to nvs
    nvs_write_listings(current_listings);
    //free memory allocated

    // TODO free memory
    free(new_listings);
    free(current_listings);
    //reset listings
    reset_listings();
    
}



void update_listing_header(uint8_t * listing){
    //is first entity timestamp equal to 0x00000000? 
                //if yes
                //add current timestamp to first entity timestamp - bytes 18-21 of header
            //else do nothing
        // is target uuid equal to gateway mac? 
            // if yes
            //check if timestamp is not 0x00000000
                //if it isnt, add timestamp bytes 22-25 in header
                // switch between the data types
                // if 0x0c
                    // write bytes 33-44 in business id flash memory, bytes 45-48 in timestamp, bytes 49-51 in location lat, 51-53 location long.
}


void read_listing(){
    //read listing data raw from nvs
     read_listing_raw();

    //read number of listings from nvs 

    //save retrieved data in listing struct
    

    //return struct
}

void read_listing_raw(){
    //read listing data raw bytes from nvs
    

  
}
/*----------------------------------------------------------------
uint16_t header_len;
	uint8_t listing_uuid[MAX_UUID_SIZE];
	uint8_t target_uuid[MAX_TARGET_UUID_SIZE];
	uint32_t first_ent_timestamp;
	uint32_t target_rec_timestamp;
	uint32_t cloud_rec_timestamp;
	uint8_t cloud_sign;
	uint8_t data_id;
	uint16_t body_len;
	uint8_t body[MAX_LISTING_SIZE];
    */

/*
* Get all current listings from instance as hex array
*/
uint8_t * get_all_listings_hex(void){
     uint8_t *data=malloc(m_listing_instance.listing_arr_len*MAX_LISTING_BODY_SIZE*sizeof(uint8_t));

    //  uint8_t data_t[4*MAX_LISTING_BODY_SIZE];
    //  memcpy(data_t,&m_listing_instance.listing_arr[0].header_len,sizeof(&m_listing_instance.listing_arr[0].header_len));
    //  printk("data t strlen: %d\n",strlen(data_t));
    //loop through listings array
    
    for(int ind = 0; ind < m_listing_instance.listing_arr_len; ind++)
    {
 
        memcpy(data+hex_len, &m_listing_instance.listing_arr[ind].header_len,1);
        hex_len++;
        memcpy(data+hex_len,m_listing_instance.listing_arr[ind].listing_uuid,11);

        hex_len+=11;

        uint8_t first_ent[4] = {m_listing_instance.listing_arr[ind].first_ent_timestamp >> 24, m_listing_instance.listing_arr[ind].first_ent_timestamp >> 16, m_listing_instance.listing_arr[ind].first_ent_timestamp >> 8, m_listing_instance.listing_arr[ind].first_ent_timestamp};
        uint8_t target_ent[4] = {m_listing_instance.listing_arr[ind].target_rec_timestamp >> 24, m_listing_instance.listing_arr[ind].target_rec_timestamp >> 16, m_listing_instance.listing_arr[ind].target_rec_timestamp >> 8, m_listing_instance.listing_arr[ind].target_rec_timestamp};
        uint8_t cloud_ent[4] = {m_listing_instance.listing_arr[ind].cloud_rec_timestamp >> 24, m_listing_instance.listing_arr[ind].cloud_rec_timestamp >> 16, m_listing_instance.listing_arr[ind].cloud_rec_timestamp >> 8, m_listing_instance.listing_arr[ind].cloud_rec_timestamp};
       
        memcpy(data+hex_len,m_listing_instance.listing_arr[ind].target_uuid,6);
        hex_len+=6;
        memcpy(data+hex_len,first_ent,4);
        hex_len+=4;
        memcpy(data+hex_len,target_ent,4);
        hex_len+=4;
        uint8_t cloud_sign = 0x00;
        memcpy(data+hex_len,&cloud_sign,1);
        hex_len+=1;
        memcpy(data+hex_len,cloud_ent,4);
        hex_len+=4;
        memcpy(data+hex_len,&m_listing_instance.listing_arr[ind].data_id,1);
        hex_len++;
        uint8_t list_body_len[2] = {m_listing_instance.listing_arr[ind].body_len >>8, m_listing_instance.listing_arr[ind].body_len & 0xff};
        memcpy(data+hex_len,list_body_len,2);
        hex_len+=2;
        memcpy(data+hex_len,m_listing_instance.listing_arr[ind].body,m_listing_instance.listing_arr[ind].body_len);
        hex_len+=m_listing_instance.listing_arr[ind].body_len;
        // printk("data strlen end: %d\n", strlen(data));
    }
    printk("final hex_len is: %d\n",hex_len);
    return data;

}   

char * hex_to_http_string(uint8_t * arr, uint16_t len){
    char * hex_string = malloc(len*2*sizeof(char));
    for(int i = 0; i < len; i++){
        sprintf(hex_string+2*i,"%02x",arr[i]);
    }
  
    return hex_string;
}

char * get_single_listings_string(int len, int ind){
    char * data = malloc(len);
    //loop through listings array
    // for(int i = 0; i < m_listing_instance.listing_arr_len; i++)
    // {
        // printk("listing %d\n", ind);
        // printk("data before: %s",data);
        sprintf(data, "%02x", m_listing_instance.listing_arr[ind].header_len);
        // printk("data after: %s",data);
        for(int j = 0; j < 11; j++)
        {
            sprintf(data, "%s%02X",data,m_listing_instance.listing_arr[ind].listing_uuid[j]);
            // printk("data: %s",data);
            // nvs_concatenate_data(data);
        }
    
        uint8_t first_ent[4] = {m_listing_instance.listing_arr[ind].first_ent_timestamp >> 24, m_listing_instance.listing_arr[ind].first_ent_timestamp >> 16, m_listing_instance.listing_arr[ind].first_ent_timestamp >> 8, m_listing_instance.listing_arr[ind].first_ent_timestamp};
        uint8_t target_ent[4] = {m_listing_instance.listing_arr[ind].target_rec_timestamp >> 24, m_listing_instance.listing_arr[ind].target_rec_timestamp >> 16, m_listing_instance.listing_arr[ind].target_rec_timestamp >> 8, m_listing_instance.listing_arr[ind].target_rec_timestamp};
        uint8_t cloud_ent[4] = {m_listing_instance.listing_arr[ind].cloud_rec_timestamp >> 24, m_listing_instance.listing_arr[ind].cloud_rec_timestamp >> 16, m_listing_instance.listing_arr[ind].cloud_rec_timestamp >> 8, m_listing_instance.listing_arr[ind].cloud_rec_timestamp};
       
        for(int j = 0; j < 6; j++)
        {
            sprintf(data, "%s%02X",data,m_listing_instance.listing_arr[ind].target_uuid[j]);
        }
         for(int j = 0; j < 4; j++)
        {
            sprintf(data, "%s%02X",data,first_ent[j]);
        }
        for(int j = 0; j < 4; j++)
        {
            sprintf(data, "%s%02X",data,target_ent[j]);
        }
         sprintf(data, "%s%02X",data,0x00);
        for(int j = 0; j < 4; j++)
        {
            sprintf(data, "%s%02X",data,cloud_ent[j]);
        }
            // sprintf(data, "%s%02x", data, list.header[j]);
       
        sprintf(data, "%s%02X",data,m_listing_instance.listing_arr[ind].data_id);
        
        uint8_t list_body_len[2] = {m_listing_instance.listing_arr[ind].body_len >>8, m_listing_instance.listing_arr[ind].body_len & 0xff};
        for (int j=0; j < 2; j++)
        {
            sprintf(data, "%s%02X",data,list_body_len[j]);
        }
        // sprintf(data, "%s%02X",data,0x00);
        // sprintf(data, "%s%02X",data,0x24);
        //loop through body of each listing
        // printk("body len of listing: %d\n", m_listing_instance.listing_arr[ind].body_len);
        for(int j = 0; j < m_listing_instance.listing_arr[ind].body_len; j++)
        {
            // printk("body: %02x\n", m_listing_instance.listing_arr[i].body[j]);
            //convert each byte to hex string
            sprintf(data, "%s%02x",data, m_listing_instance.listing_arr[ind].body[j]);
        }
        
    // }

    char temp_buf[len];
    sprintf(temp_buf, "%c%s",'\'', data);
    sprintf(data, "%s%c", temp_buf,'\'');
    return data;

        //get listing string
        //add listing string to listings string
    //return listings string

  
}

uint16_t get_listings_length(){
    return m_listing_instance.listing_arr_len;
}

void reset_listings(){
    m_listing_instance.listing_arr_len = 0;
    m_listing_instance.listing_arr_pos =0;
    // m_listing_instance.listing_arr
}

void compile_ignition_listings(uint8_t * adv_data, uint8_t len, device * devices){
    uint8_t target[6] = {0x63,0x6c,0x6f,0x75,0x64,0x00};
    // uint8_t curr_t[4] = get_timestamp_mins();
    uint32_t zeros = 0x00000000;
    uint32_t current_seconds = get_timestamp_secs();
    // printk("length in listings compile: %d", len);
    uint8_t gateway_mac[6];
    memcpy(gateway_mac,nvs_read_mac(),6);
    
    for(uint8_t i=0; i<len; i++)
    {   
        // printk("again");
        uint8_t list_uuid[MAX_UUID_SIZE];
        for(uint8_t j=0; j<6 ; j++){
            list_uuid[j] = devices[i].mac_addr[5-j];
        }
        list_uuid[6] = 0x00;
        for(uint8_t j=7; j<11 ; j++){
            list_uuid[j] = current_seconds >> (8*(10-j));
        }
        //print uuid 
        // printk("uuid: ");
        // for(uint8_t j=0; j<11 ; j++){
        //     printk("%02x", list_uuid[j]);
        // }
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].header_len = FULL_LISTING_HEADER_LEN;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body_len = IGNITION_DATA_LISTING_BODY_LENGTH + 6;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].listing_uuid, list_uuid, sizeof(list_uuid)); // need to get mac address
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].data_id = 0x08;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_uuid,target, sizeof(target));
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].first_ent_timestamp = (uint32_t)current_seconds/60;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_rec_timestamp = zeros;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].cloud_rec_timestamp = zeros;
        for (int j=0; j<IGNITION_DATA_LISTING_BODY_LENGTH; j++){
            m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body[j] = adv_data[i*IGNITION_DATA_LISTING_BODY_LENGTH+j];
        }
        //add gateway mac to body
        for (int j=0; j<6; j++){
            m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body[IGNITION_DATA_LISTING_BODY_LENGTH+j] = gateway_mac[j];
        }

        
        m_listing_instance.listing_arr_pos++;
        m_listing_instance.listing_arr_len++;
    }
}

void compile_gateway_status_listing(uint8_t len){
    uint8_t target[6] = {0x63,0x6c,0x6f,0x75,0x64,0x00};
    // uint8_t curr_t[4] = get_timestamp_mins();
    uint32_t zeros = 0x00000000;
    uint32_t current_seconds = get_timestamp_secs();
    // printk("length in listings compile: %d", len);
    uint8_t status_body[GATEWAY_STATUS_LISTING_BODY_LENGTH];
    uint32_t gate_time = get_timestamp_mins();
    uint8_t gate_time_bytes[4] = {gate_time >> 24, gate_time >> 16, gate_time >> 8, gate_time};
    uint8_t status = nvs_read_status();
    uint8_t fwVersion[3] = FW_VERSION;
    uint8_t hwVersion[3] = HW_VERSION;
    uint16_t biz_id = nvs_read_business_id();
    uint8_t biz_arr[] = {biz_id >> 8, biz_id};
    int bat_lvl = get_battery();
	uint8_t bat_int = (bat_lvl - bat_lvl%1000)/1000;
	uint8_t bat_dec = (bat_lvl%1000)/10;
    uint8_t battery[2] = {bat_int, bat_dec};
    uint8_t location[8];
    memcpy(location, nvs_read_location(), sizeof(location));
    uint8_t hive_count = len;
    memcpy(status_body,gate_time_bytes,4);
    memcpy(status_body+4,&status,1);
    memcpy(status_body+5,fwVersion,3);
    memcpy(status_body+8,hwVersion,3);
    memcpy(status_body+11,biz_arr,2);
    memcpy(status_body+13,battery,2);
    memcpy(status_body+15,location,8);
    memcpy(status_body+23,&hive_count,1);
    //TODO
    // test fw/hw versions
    // parse biz id to 2 bytes
    // save all this to status body array



        // printk("again");
        //read mac from nvs
        uint8_t mac[6];
        memcpy(mac,nvs_read_mac(),6);
        uint8_t list_uuid[MAX_UUID_SIZE];
        for(uint8_t j=0; j<6 ; j++){
            list_uuid[j] = mac[j];
        }
        list_uuid[6] = 0x0B;
        for(uint8_t j=7; j<11 ; j++){
            list_uuid[j] = current_seconds >> (8*(10-j));
        }
        //print uuid 
        // printk("uuid: ");
        // for(uint8_t j=0; j<11 ; j++){
        //     printk("%02x", list_uuid[j]);
        // }
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].header_len = FULL_LISTING_HEADER_LEN;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body_len = GATEWAY_STATUS_LISTING_BODY_LENGTH;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].listing_uuid, list_uuid, sizeof(list_uuid)); // need to get mac address
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].data_id = 0x0B;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_uuid,target, sizeof(target));
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].first_ent_timestamp = (uint32_t)current_seconds/60;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_rec_timestamp = zeros;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].cloud_rec_timestamp = zeros;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body,status_body,GATEWAY_STATUS_LISTING_BODY_LENGTH);
        m_listing_instance.listing_arr_pos++;
        m_listing_instance.listing_arr_len++;
}


void compile_diagnostics_listings(uint8_t * adv_data, uint8_t len, device * devices){
    uint8_t target[6] = {0x63,0x6c,0x6f,0x75,0x64,0x00};
    // uint8_t curr_t[4] = get_timestamp_mins();
    uint32_t zeros = 0x00000000;
    uint32_t current_seconds = get_timestamp_secs();
    // printk("length in listings compile: %d", len);

    
    for(uint8_t i=0; i<len; i++)
    {   
        // printk("again");
        uint8_t list_uuid[MAX_UUID_SIZE];
        for(uint8_t j=0; j<6 ; j++){
            list_uuid[j] = devices[i].mac_addr[5-j];
        }
        list_uuid[6] = 0x0B;
        for(uint8_t j=7; j<11 ; j++){
            list_uuid[j] = current_seconds >> (8*(10-j));
        }
        //print uuid 
        // printk("uuid: ");
        // for(uint8_t j=0; j<11 ; j++){
        //     printk("%02x", list_uuid[j]);
        // }
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].header_len = FULL_LISTING_HEADER_LEN;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body_len = DIAGNOSTICS_LISTING_BODY_LENGTH;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].listing_uuid, list_uuid, sizeof(list_uuid)); // need to get mac address
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].data_id = 0x03;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_uuid,target, sizeof(target));
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].first_ent_timestamp = (uint32_t)current_seconds/60;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_rec_timestamp = zeros;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].cloud_rec_timestamp = zeros;
        for (int j=0; j<DIAGNOSTICS_LISTING_BODY_LENGTH; j++){
            m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body[j] = adv_data[i*DIAGNOSTICS_LISTING_BODY_LENGTH+j];
        }
        m_listing_instance.listing_arr_pos++;
        m_listing_instance.listing_arr_len++;
    }
}

void compile_cartridge_listings(uint8_t * adv_data, uint8_t len, device * devices){
    uint8_t target[6] = {0x63,0x6c,0x6f,0x75,0x64,0x00};
    // uint8_t curr_t[4] = get_timestamp_mins();
    uint32_t zeros = 0x00000000;
    uint32_t current_seconds = get_timestamp_secs();
    // printk("length in listings compile: %d", len);

    
    for(uint8_t i=0; i<len; i++)
    {   
        // printk("again");
        uint8_t list_uuid[MAX_UUID_SIZE];
        for(uint8_t j=0; j<6 ; j++){
            list_uuid[j] = devices[i].mac_addr[5-j];
        }
        list_uuid[6] = 0x1B;
        for(uint8_t j=7; j<11 ; j++){
            list_uuid[j] = current_seconds >> (8*(10-j));
        }
        //print uuid 
        // printk("uuid: ");
        // for(uint8_t j=0; j<11 ; j++){
        //     printk("%02x", list_uuid[j]);
        // }
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].header_len = FULL_LISTING_HEADER_LEN;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body_len = CARTRIDGE_LISTING_BODY_LENGTH;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].listing_uuid, list_uuid, sizeof(list_uuid)); // need to get mac address
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].data_id = 0x06;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_uuid,target, sizeof(target));
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].first_ent_timestamp = (uint32_t)current_seconds/60;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_rec_timestamp = zeros;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].cloud_rec_timestamp = zeros;
        for (int j=0; j<CARTRIDGE_LISTING_BODY_LENGTH; j++){
            m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body[j] = adv_data[i*CARTRIDGE_LISTING_BODY_LENGTH+j];
        }
        m_listing_instance.listing_arr_pos++;
        m_listing_instance.listing_arr_len++;
    }
}


void compile_periodic_listings(uint8_t * adv_data, uint8_t len, device * devices){
    uint8_t target[6] = {0x63,0x6c,0x6f,0x75,0x64,0x00};
    // uint8_t curr_t[4] = get_timestamp_mins();
    uint32_t zeros = 0x00000000;
    uint32_t current_seconds = get_timestamp_secs();
    // printk("length in periodic listings compile: %d", len);

    
    for(uint8_t i=0; i<len; i++)
    {   
        // printk("again");
        uint8_t list_uuid[MAX_UUID_SIZE];
        for(uint8_t j=0; j<6 ; j++){
            list_uuid[j] = devices[i].mac_addr[5-j];
        }
        list_uuid[6] = 0x02;
        for(uint8_t j=7; j<11 ; j++){
            list_uuid[j] = current_seconds >> (8*(10-j));

        }
        // print uuid 
        // printk("uuid: ");
        // for(uint8_t j=0; j<11 ; j++){
        //     printk("%02x\n", list_uuid[j]);
        // }
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].header_len = FULL_LISTING_HEADER_LEN;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body_len = PERIODIC_SENSOR_DATA_LISTING_LENGTH;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].listing_uuid, list_uuid, sizeof(list_uuid)); // need to get mac address
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].data_id = 0x01;
        memcpy(m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_uuid,target, sizeof(target));
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].first_ent_timestamp = (uint32_t)current_seconds/60;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].target_rec_timestamp = zeros;
        m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].cloud_rec_timestamp = zeros;
        for (int j=0; j<PERIODIC_SENSOR_DATA_LISTING_LENGTH; j++){
            m_listing_instance.listing_arr[m_listing_instance.listing_arr_pos].body[j] = adv_data[i*PERIODIC_SENSOR_DATA_LISTING_LENGTH+j];
        }
        m_listing_instance.listing_arr_pos++;
        m_listing_instance.listing_arr_len++;
    }
    
}

bool find_index(uint16_t * del_listing_ind, int listings_post_count, int cur_ind){
    // printk("finding index");
    for (int i=0; i<listings_post_count; i++){
        if (del_listing_ind[i] == cur_ind){
            return true;
        }
    }
    return false;
}
//data, non continuous indices, big size, 217, 397   
// (listings_hex_arr, del_listing_ind,list_size,listings_post_count,listings_length);
void remove_posted_listings(uint8_t * listings_hex_arr, uint16_t * del_listing_ind,uint16_t list_size,int listings_post_count,uint16_t listings_length){
    int cur_ind = 0;
    uint8_t * new_listings_hex = malloc(list_size);
    int prev_listing_size = 0;
    uint16_t new_listings_num = 0;
    //print del_listing_ind
    // printk("del_listing_ind: \n");
    // for (int i=0; i<listings_post_count; i++){
    //     printk("%d ", del_listing_ind[i]);
    // }
    for (int i = 0; i < list_size; i++){
		//1. copy bytes  into one listing array
		int header_len = listings_hex_arr[i]; 
		int body_ind = header_len + i; 
		uint8_t current_listing_size = header_len +  listings_hex_arr[body_ind] + 1; 
		uint8_t * current_listing_hex = malloc(current_listing_size*sizeof(uint8_t));
        // printk("memcpy1\n");
		memcpy(current_listing_hex, listings_hex_arr + i, current_listing_size);
        // if curr ind exists in del_listing_ind, concatenate current_listing_hex to new_listings_hex
        bool exists = find_index(del_listing_ind, listings_post_count, cur_ind);
        if (exists){
            // printk("listing should be deleted\n");
        }
        else{
            // printk("listing should not be deleted\n");
            memcpy(new_listings_hex + prev_listing_size, current_listing_hex, current_listing_size);
            
            // printk("memcpy2\n");
            new_listings_num++;
            prev_listing_size += current_listing_size;
        }
        free(current_listing_hex);

		i+=current_listing_size-1; 
        cur_ind++;
	}
    //save new listings to nvs
    nvs_delete_listings(); 
    nvs_write_listings_len((uint16_t)prev_listing_size);
    nvs_write_listings_num(new_listings_num);
    nvs_write_listings(new_listings_hex);
    free(new_listings_hex);
}

// compile_raw_listing_from_listings_array(uint8_t * listings_adv_data, uint16_t len){
//     //create listing buffer of length 51
//     uint8_t * raw_listings[]
//     //assign header data in bytes 0 to 32
//     //copy adv_data to listing buffer at bytes 33 to 51
//     //return listing buffer

// }


/* TEST CODE FOR REMOVE LISTING
/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*****************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


bool find_index(uint16_t * del_listing_ind, int listings_post_count, int cur_ind){
    for (int i=0; i<listings_post_count; i++){
        if (del_listing_ind[i] == cur_ind){
            return true;
        }
    }
    return false;
}


void del(uint16_t **del_listing_ind, int *listings_post_count, uint16_t current_ind){
    if (!(current_ind%2)){
        memcpy(*del_listing_ind+*listings_post_count, &current_ind, sizeof(uint16_t));
    	printf("listings_post_count: %d\n", *listings_post_count);
    	printf("current_ind: %d\n", current_ind);
    // 	printf("del_listing_ind: %d\n", *del_listing_ind[*listings_post_count]);
    	(*listings_post_count)++;
    }
}

void remove_posted_listings(uint8_t * listings_hex_arr, uint16_t * del_listing_ind,uint16_t list_size,int listings_post_count,uint16_t listings_length){
    int cur_ind = 0;
    uint8_t * new_listings_hex = malloc(list_size);
    uint16_t new_listings_size = 0;
    int prev_listing_size = 0;
    uint16_t new_listings_num = 0;
    for (int i = 0; i < list_size; i++){
		//1. copy bytes  into one listing array
		int header_len = listings_hex_arr[i]; 
		int body_ind = header_len + i; 
		uint8_t current_listing_size = header_len +  listings_hex_arr[body_ind] + 1; 
		uint8_t * current_listing_hex = malloc(current_listing_size*sizeof(uint8_t));
		memcpy(current_listing_hex, listings_hex_arr + i, current_listing_size);
        // if curr ind exists in del_listing_ind, concatenate current_listing_hex to new_listings_hex
        bool exists = find_index(del_listing_ind, listings_post_count, cur_ind);
        if (exists){
            printf("listing should be deleted\n");
        }
        else{
            printf("listing should not be deleted\n");
            printf("current size listing1: %d\n", prev_listing_size);
            memcpy(new_listings_hex + prev_listing_size, current_listing_hex, current_listing_size);
            new_listings_num++;
            prev_listing_size += current_listing_size;
            printf("current size listing2: %d\n", prev_listing_size);
        }
		i+=current_listing_size-1; 
        cur_ind++;
	}
    //save new listings to nvs
    for (int i=0; i< prev_listing_size; i++){
        printf("%02x", new_listings_hex[i]);
    }
    free(new_listings_hex);
}
//0x21,0xd9,0xd1,0x5d,0x25,0x9e,0x54,0x5c,0x05,0x4f,0xe8,0xf7,0x63,0x6c,0x6f,0x75,0x64,0x00,0x00,0x16,0xaa,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x0d,0x00,0x16,0xaa,0x34,0x15,0x00,0x16,0x00,0x00,0x41,0x01,0x00,0x01
uint8_t listings_hex_arr[4*47] = {0x21,0xd9,0xd1,0x5d,0x25,0x9e,0x54,0x5c,0x05,0x4f,0xe8,0xf7,0x63,0x6c,0x6f,0x75,0x64,0x00,0x00,0x16,0xaa,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x0d,0x00,0x16,0xaa,0x34,0x15,0x00,0x16,0x00,0x00,0x41,0x01,0x00,0x01,0x21,0xd9,0xd1,0x5d,0x25,0x9e,0x54,0x5c,0x05,0x4f,0xe8,0xf7,0x63,0x6c,0x6f,0x75,0x64,0x00,0x00,0x16,0xaa,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x0d,0x00,0x16,0xaa,0x34,0x15,0x00,0x16,0x00,0x00,0x41,0x01,0x00,0x01,0x21,0xd9,0xd1,0x5d,0x25,0x9e,0x54,0x5c,0x05,0x4f,0xe8,0xf7,0x63,0x6c,0x6f,0x75,0x64,0x00,0x00,0x16,0xaa,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x0d,0x00,0x16,0xaa,0x34,0x15,0x00,0x16,0x00,0x00,0x41,0x01,0x00,0x01,0x21,0xd9,0xd1,0x5d,0x25,0x9e,0x54,0x5c,0x05,0x4f,0xe8,0xf7,0x63,0x6c,0x6f,0x75,0x64,0x00,0x00,0x16,0xaa,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x0d,0x00,0x16,0xaa,0x34,0x15,0x00,0x16,0x00,0x00,0x41,0x01,0x00,0x01};
uint16_t list_size = 4*47;
uint16_t listings_length = 4;
int main()
{
    uint16_t * del_listing_ind = malloc(sizeof(uint16_t) * listings_length);
    int listings_post_count = 0;
    uint16_t current_ind = 0;
    for (uint16_t i=0; i<listings_length; i++){
        del(&del_listing_ind, &listings_post_count, i);
    }
    // for (int j=0; j<listings_post_count; j++){
    //     printf("%d\n",del_listing_ind[j]);
    // }
    remove_posted_listings(listings_hex_arr,del_listing_ind,list_size,listings_post_count,listings_length);
    return 0;
}



*/