/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "app_smart_config.h"
#include "smartconfig.h"
#include "nvs_rw.h"
#include "wifi.h"
#include "rom/ets_sys.h"
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
  uint8_t *p_ssid;
  uint8_t *p_pass;
} smartconfig_data_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_smartconfig_task(void *arg);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static smartconfig_data_t s_smartconfig;
static char               buffer_ssid_scan[200 + 1];

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_SmartConfig_CreateTask (void)
{
  xTaskCreate(APP_smartconfig_task, "smart_cfg", 1024 * 10, NULL, 11, NULL);
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

  NVS_Init();
  WIFI_StaInit();

    // esp_wifi_stop();

    // wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    // esp_wifi_init(&config);

    // wifi_config_t wifi_config =
    // {
    //   .sta =
    //   {
    //       .threshold.authmode = WIFI_AUTH_WPA2_PSK,
    //       .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
    //   },
    // };

    // memcpy(wifi_config.sta.ssid,
    //         "Van Son",
    //         sizeof("Van Son"));
    // memcpy(wifi_config.sta.password,
    //         "26061975",
    //         sizeof("26061975"));

    // esp_wifi_set_mode(WIFI_MODE_STA);
    // esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    // esp_wifi_start();
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
      return 1;
    }
  }
  return -1;
}

static void
APP_smartconfig_task (void *arg)
{
  uint8_t u8_number_of_scan = 0;
  uint8_t u8_flag_connected = 0;

  while (u8_number_of_scan < 2)
  {
    ets_delay_us(500000);
    WIFI_Scan(buffer_ssid_scan);
    if (matchingWIFIScan(
            buffer_ssid_scan, s_smartconfig.p_ssid, s_smartconfig.p_pass)
        != -1)
    {
      esp_wifi_stop();

      wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
      esp_wifi_init(&config);

      wifi_config_t wifi_config =
      {
        .sta =
        {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
      };

      memcpy(wifi_config.sta.ssid,
             s_smartconfig.p_ssid,
             sizeof(s_smartconfig.p_ssid));
      memcpy(wifi_config.sta.password,
             s_smartconfig.p_pass,
             sizeof(s_smartconfig.p_pass));

      esp_wifi_set_mode(WIFI_MODE_STA);
      esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
      esp_wifi_start();

      u8_flag_connected++;
      goto non_init_smartcfg;
    }
    u8_number_of_scan++;
  }

  initialise_smartconfig();

non_init_smartcfg:

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
    if ((uxBits & WIFI_CONNECTED_BIT) && (u8_flag_connected == 0))
    {
      WIFI_StoreNVS(s_smartconfig.p_ssid, s_smartconfig.p_pass);
    }
  }
}