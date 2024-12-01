/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "app_smart_config.h"
#include "app_things_board.h"
#include "app_data_ESP32.h"
#include "app_data_rec.h"
#include "app_data_trans.h"
#include "app_process_data.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_15)

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
  s_data_system.s_flag_mqtt_event  = xEventGroupCreate();
  s_data_system.s_data_rec_queue   = xQueueCreate(10, sizeof(float));
  s_data_system.s_data_trans_queue = xQueueCreate(10, sizeof(float));

  // Init Application
  APP_SmartConfig_Init();
  APP_Data_rec_Init();
  APP_Process_data_Init();
  APP_Things_board_Init();

  // Create Task
  APP_SmartConfig_CreateTask();
  APP_Process_data_CreateTask();
  APP_Data_rec_CreateTask();
  APP_Things_board_CreateTask();
}

/******************************************************************************
 *    PRIVATE FUNCTION
 *****************************************************************************/
