/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <string.h>

#include "nvs_rw.h"
#include "uart.h"
#include "wifi.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "app_things_board.h"
#include "app_data_ESP32.h"
#include "app_data_rec.h"
#include "app_data_trans.h"
#include "app_process_data.h"
#include "app_rtc.h"
#include "app_button.h"
#include "app_status_led.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_15)

uint8_t ssid_test[]     = "ThangHo_1";
uint8_t password_test[] = "testsmartconfig";

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

/******************************************************************************
 *    PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/
static inline void
APP_MAIN_ResetDataSystem (void)
{
  s_data_system.f_power        = 0;
  s_data_system.f_energy       = 0;
  s_data_system.f_energy_min   = 0;
  s_data_system.f_energy_hour  = 0;
  s_data_system.f_energy_day   = 0;
  s_data_system.f_energy_week  = 0;
  s_data_system.f_energy_month = 0;
  s_data_system.f_energy_year  = 0;

  s_data_system.s_flag_enable.u8_second = 0;
  s_data_system.s_flag_enable.u8_minute = 0;
  s_data_system.s_flag_enable.u8_hour   = 0;
  s_data_system.s_flag_enable.u8_day    = 0;
  s_data_system.s_flag_enable.u8_month  = 0;
  s_data_system.s_flag_enable.u8_year   = 0;

  memset(s_data_system.u8_ssid, 0, sizeof(s_data_system.u8_ssid));
  memset(s_data_system.u8_pass, 0, sizeof(s_data_system.u8_pass));
}

/******************************************************************************
 *       MAIN FUNCTION
 *****************************************************************************/

void
app_main (void)
{
  // Reset Data System
  APP_MAIN_ResetDataSystem();

  // Create Service
  s_data_system.s_flag_wifi_event   = xEventGroupCreate();
  s_data_system.s_flag_mqtt_event   = xEventGroupCreate();
  s_data_system.s_flag_time_event   = xEventGroupCreate();
  s_data_system.s_flag_button_event = xEventGroupCreate();
  s_data_system.s_data_rec_queue    = xQueueCreate(32, sizeof(float));
  s_data_system.s_data_trans_queue  = xQueueCreate(32, sizeof(float));

  NVS_Init();

  // Init Application
  APP_Rtc_Init();
  APP_Things_board_Init();
  APP_Data_Trans_Init();
  APP_Data_rec_Init();
  APP_Process_data_Init();
  APP_Button_Init();
  APP_Status_led_Init();
  uartDriverInit(UART_NUM_2,
                 TXD_PIN,
                 RXD_PIN,
                 115200,
                 UART_DATA_8_BITS,
                 UART_PARITY_DISABLE,
                 UART_HW_FLOWCTRL_DISABLE,
                 UART_STOP_BITS_1);

  // Create Task
  APP_Status_led_CreateTask();
  APP_Rtc_CreateTask();
  // APP_Data_Trans_CreateTask();
  APP_Things_board_CreateTask();
  APP_Data_rec_CreateTask();
  APP_Process_data_CreateTask();
  APP_Button_CreateTask();
}

/******************************************************************************
 *    PRIVATE FUNCTION
 *****************************************************************************/