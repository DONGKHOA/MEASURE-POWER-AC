/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "smartconfig.h"
#include "event.h"
#include <string.h>
/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/
extern EventGroupHandle_t s_wifi_event_group;
/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct smartconfig_data
{
  uint8_t           *p_ssid;
  uint8_t           *p_pass;
  SemaphoreHandle_t *p_semphr_smart_wifi;
} smartconfig_data_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_smartconfig_task(void *arg);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static smartconfig_data_t s_smartconfig;

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/
void
APP_SmartConfig_CreateTask (void)
{
  xTaskCreate(APP_smartconfig_task, "smart_maintask", 1024 * 10, NULL, 11, NULL);
}

void
APP_SmartConfig_Init (void)
{
  // Link pointer to variable
  s_smartconfig.p_ssid = (uint8_t *)&s_data_system.u8_ssid;
  s_smartconfig.p_pass = (uint8_t *)&s_data_system.u8_pass;
  // s_smartconfig.p_semphr_smart_wifi
  //     = (SemaphoreHandle_t *)s_data_system.s_semphr_smart_wifi;

  // Initialize smartconfig
  event_init(s_smartconfig.p_ssid, s_smartconfig.p_pass);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_smartconfig_task (void *arg)
{
  uint8_t count = 1;

  ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
  smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

  while (1)
  {
    EventBits_t uxBits
        = xEventGroupWaitBits(s_wifi_event_group,
                              ESPTOUCH_DONE_BIT | WIFI_CONNECTED_BIT,
                              true,
                              false,
                              portMAX_DELAY);
    if (uxBits & ESPTOUCH_DONE_BIT)
    {
      esp_smartconfig_stop();
    }
    if (uxBits & WIFI_CONNECTED_BIT)
    {
      xQueueSend(s_data_system.s_semphr_smart_wifi, (void *)&count, ( TickType_t ) 10);
      puts("1");
    }
  }
}