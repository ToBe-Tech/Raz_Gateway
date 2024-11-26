#include "clock.h"
#include <zephyr.h>
#include "fs.h"
#include <stdlib.h>
#include "ble.h"
struct k_work_delayable clock_work;
int seconds_counter = 0;
extern struct k_work_delayable ble_scan_start_work;
bool time_paused = false;

static clock_instance_t m_clock_instance;


void clock_event_handler(uint8_t h, uint8_t m);

void clock_init(){
    k_work_init_delayable(&clock_work, clock_work_handler);
    k_work_schedule(&clock_work,Z_TIMEOUT_MS(SEC_INTERVAL));
    uint8_t time[4];
    memcpy(time, nvs_read_timestamp(), 4);
    printk("time 0: %d\n", time[0]);
    printk("time 1: %d\n", time[1]);
    printk("time 2: %d\n", time[2]);
    printk("time 3: %d\n", time[3]);

    if (time[0] == 0xFF){
        m_clock_instance.timestamp_min = 0;
        m_clock_instance.timestamp_sec = 0;
        m_clock_instance.is_timestamp_valid = false;
    } else {
        m_clock_instance.timestamp_min = time[0] << 24 | time[1] << 16 | time[2] << 8 | time[3] & 0xFF;
        m_clock_instance.timestamp_sec = m_clock_instance.timestamp_min * 60;
        m_clock_instance.is_timestamp_valid = true;
    }
    m_clock_instance.last_timestamp_ms = 0;
    m_clock_instance.is_paused = false;
}


uint32_t get_timestamp_secs(){
    return m_clock_instance.timestamp_sec;
}

uint32_t get_timestamp_mins(){
    return m_clock_instance.timestamp_min;
}

void clock_work_handler(){
    // printk("clock_work_handler\n");
    // if (m_clock_instance.is_paused){
    //     return;
    // }
    k_work_schedule(&clock_work, Z_TIMEOUT_MS(SEC_INTERVAL));
    m_clock_instance.timestamp_sec++;
    seconds_counter++;
    //increase timestamp
    // create function for reading timestamp from here
    // save timestamp every T minutes
    if ((seconds_counter % 60) == 0){
        seconds_counter = 0;
        m_clock_instance.timestamp_min++;
        uint8_t h = (m_clock_instance.timestamp_min / 60) % 24;
        uint8_t m = m_clock_instance.timestamp_min % 60;
        printk("time is: %d : %d\n", h, m);
        if (!m_clock_instance.is_paused){
            clock_event_handler(h,m);
        }
    }
}


void update_timestamp(uint32_t timestamp){
    m_clock_instance.timestamp_min = timestamp;
    m_clock_instance.timestamp_sec = timestamp * 60;
    m_clock_instance.is_timestamp_valid = true;
}

void pause_clock(){
    m_clock_instance.is_paused = true;
}

void resume_clock(){
    m_clock_instance.is_paused = false;
}

void clock_event_handler(uint8_t h, uint8_t m){
    uint8_t hours[] = {0x01,0x06,0x08,0x15, 0x16};
    if (m==0){
        ble_advertise_battery();
        ble_advertise_lte_vals();
    }
    if (m==9){
            
            pause_clock();
            //save time to flash
            nvs_write_timestamp(m_clock_instance.timestamp_min);
            //advertise time
            ble_advertise_time(m_clock_instance.timestamp_min);
            resume_clock();
            //get battery and advertise battery
        }

    for (uint8_t i=0; i<sizeof(hours); i++){
        
        if (h == hours[i] && m == 5){
            printk("Time: %d:%d\n", h, m);
            k_work_schedule(&ble_scan_start_work, Z_TIMEOUT_MS(5));
            printk("calendar paused");
            pause_clock();
        }
    }
    
}