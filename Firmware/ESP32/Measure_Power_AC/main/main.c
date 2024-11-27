/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "rom/ets_sys.h"

// #include "wifi.h"
#include "gpio.h"
// #include "nvs_rw.h"
#include "uart.h"
#include "tbc_mqtt_helper.h"
#include "ds3231.h"
#include "app_smart_config.h"
#include "app_wifi.h"
#include "app_data_ESP32.h"
#include "event.h"

#define TELEMETRY_LUX "power"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_15)

#define MIN_STACK_SIZE 1024

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/


/******************************************************************************
 *    PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

/******************************************************************************
 *       MAIN FUNCTION
 *****************************************************************************/

void
app_main (void)
{
  s_data_system.s_semphr_smart_wifi = xQueueCreate(1, sizeof(uint8_t));
  APP_WIFI_CreateTimer();

  APP_SmartConfig_Init();
  APP_WIFI_Init();

  APP_WIFI_CreateTask();
  APP_SmartConfig_CreateTask();
}

/******************************************************************************
 *    PRIVATE FUNCTION
 *****************************************************************************/
