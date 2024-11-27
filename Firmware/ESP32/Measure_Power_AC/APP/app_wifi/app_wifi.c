/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "smartconfig.h"
#include "app_wifi.h"
#include "nvs_rw.h"
#include "wifi.h"
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include <string.h>

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define TIME_SCAN_WIFI 60000

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct wifi_data
{
  TimerHandle_t      s_timer_scan_wifi;
  uint8_t           *p_ssid;
  uint8_t           *p_pass;
  SemaphoreHandle_t *p_semphr_smart_wifi;
  SemaphoreHandle_t  s_semphr_scan_wifi;
} wifi_data_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_WIFI_task(void *arg);
static void APP_WIFI_Timer_cb(TimerHandle_t xTimer);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static wifi_data_t s_wifi;
static char        buffer_ssid_scan[200 + 1];

/****************************************************************************
 *   PUBLIC FUNCTION
 ***************************************************************************/

void
APP_WIFI_CreateTask (void)
{
  xTaskCreate(APP_WIFI_task, "WIFI_maintask", 1024 * 40, NULL, 10, NULL);
}

void
APP_WIFI_CreateTimer (void)
{
  // s_wifi.s_timer_scan_wifi = xTimerCreate("time",
  //                                         TIME_SCAN_WIFI / portTICK_PERIOD_MS,
  //                                         pdTRUE,
  //                                         (void *)0,
  //                                         APP_WIFI_Timer_cb);
}

void
APP_WIFI_Init (void)
{
  // Link pointer to variable
  s_wifi.p_ssid = (uint8_t *)&s_data_system.u8_ssid;
  s_wifi.p_pass = (uint8_t *)&s_data_system.u8_pass;
  s_wifi.p_semphr_smart_wifi
      = (SemaphoreHandle_t *)s_data_system.s_semphr_smart_wifi;

  NVS_Init();
  WIFI_StaInit();

  // s_wifi.s_semphr_scan_wifi = xSemaphoreCreateBinary();
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static int8_t
matchingWIFIScan (char *data, uint8_t *ssid, uint8_t *pass)
{
  char *arg_list[50];
  char  buffer[1024];
  memcpy(buffer, data, strlen((char *)data));
  uint8_t arg_position = 0;

  // cut string
  char *temp_token = strtok(buffer, "\r");
  while (temp_token != NULL)
  {
    arg_list[arg_position] = temp_token;
    arg_position++;
    temp_token = strtok(NULL, "\r");
  }

  // check matching ssid in NVS
  for (uint8_t i = 0; i < arg_position; i++)
  {
    if (WIFI_ScanNVS((uint8_t *)arg_list[i], pass) != -1)
    {
      memcpy(ssid, arg_list[i], strlen((char *)arg_list[i]) + 1);
      puts((char *)ssid);
      return 1;
    }
  }
  return -1;
}

static void
APP_WIFI_task (void *arg)
{
  uint8_t buff_queue;
  WIFI_Scan(buffer_ssid_scan);

  while (1)
  {
    vTaskDelay(1000);
    if (xQueueReceive(s_data_system.s_semphr_smart_wifi, (void *)&buff_queue, ( TickType_t ) 10 ) == pdPASS)
    {
      // Store ssid - pass of wifi
      // WIFI_StoreNVS(s_wifi.p_ssid, s_wifi.p_pass);
    }
  }
}
