#include "button.h"
#include "ble.h"
#include <pm/pm.h>
#include <pm/device.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>

LOG_MODULE_REGISTER(button, 4);
struct k_work_delayable advertising_normal_work;
struct k_work_delayable button_time_count;


extern struct k_work_delayable ble_scan_start_work;
#if defined(CONFIG_DK_LIBRARY)
uint8_t button_pressed = 0;
uint8_t button_count=0;
bool button_released = false;

bool ble_scan_flag = true;
bool button_block = false;
  uint8_t mfg_data_cpm[] = { 0xff, 0xfe, 0x01, 0x00 ,0x00, 0x00, 0x00, 0x00, GATEWAY_SERIAL_ID & 0xff, (GATEWAY_SERIAL_ID >> 8) & 0xff };
  const struct bt_data ad_cpm[] = {
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE,
		(CONFIG_BT_DEVICE_APPEARANCE >> 0) & 0xff,
		(CONFIG_BT_DEVICE_APPEARANCE >> 8) & 0xff),
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_HRS_VAL)),
			  
};

const struct bt_data sd_cpm[] = {
	BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, BT_PER_COMP_NAME),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data_cpm, 10),
};

void button_time_count_handler(struct k_work *work)
{
  if (button_released) return;
  k_work_schedule(&button_time_count, Z_TIMEOUT_MS(1000));
  button_count++;
  printk("button_count: %d\n", button_count);
}

void block_button(void){
  button_block = true;
}

void release_button(void){
  button_block = false;
}

void button_handler(uint32_t button_states, uint32_t has_changed) {
  int err;
  printk("button handler\n");
  if (button_block){
    printk("button blocked\n");
    fault_beep();
    return;
  }
  if (has_changed & button_states &
      BIT(0)) {
    button_released = false;

    button_count=0;
    // k_work_cancel(&button_time_count);
    // k_work_cancel_sync(&button_time_count, K_FOREVER);
    k_work_schedule(&button_time_count, Z_TIMEOUT_MS(1000));

    LOG_DBG("button pressed\n");
    
    // k_work_schedule(&button_time_handler, Z_TIMEOUT_MS(1000));
    //stop ble advertisement
   
    }
    else{
      button_released = true;
      block_button();
      printk("button released\n");

      if (button_count < 1){
         long_beep();
        
          err =bt_le_adv_stop();
          if (err!=0) {
            printk("adv cannot be stopped (err %d)\n", err);
          }
          // k_sleep(K_SECONDS(1));
          err = bt_le_adv_start(BT_LE_ADV_CONN, ad_cpm, ARRAY_SIZE(ad_cpm),
                    sd_cpm, ARRAY_SIZE(sd_cpm));
          if (err) {
            printk("Advertising failed to start (err %d)\n", err);
          }
          set_ble_adv_pm();
          k_work_schedule(&advertising_normal_work, Z_TIMEOUT_MS(ADV_CPM_TIMEOUT));
          button_count=0;

      }

      else if(button_count > 2 && button_count <10){
        button_count=0;
        printk("scanning site");
        long_beep();
        k_work_schedule(&ble_scan_start_work, Z_TIMEOUT_MS(50));
        printk("calendar paused");
        pause_clock();
      }

      else if(button_count>10){
        button_count=0;
        printk("resetting device");
        long_beep();
        long_beep();
        factory_reset();
        // reset_device();
      }
      else{
        button_count=0;
        printk("button count not in range");
        release_button();
      }
      
      //cancel

      // k_work_schedule(&button_time_handler, Z_TIMEOUT_MS(1000));


    }
}



void adv_normal_mode_handler(void){
  adv_normal_mode();
  release_button();
}
#endif