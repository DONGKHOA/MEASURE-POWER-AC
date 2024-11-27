#include "event.h"
#include <string.h>
/**********************
 *  PRIVATE DATA
 **********************/

static uint8_t *p_ssid_midd;
static uint8_t *p_pass_midd;

EventGroupHandle_t s_wifi_event_group;

void
event_init (uint8_t *p_ssid, uint8_t *p_pass)
{
  s_wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

  p_ssid_midd = p_ssid;
  p_pass_midd = p_pass;
}
void
event_handler (void            *arg,
               esp_event_base_t event_base,
               int32_t          event_id,
               void            *event_data)

{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    esp_wifi_connect();
  }
  else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
  {
    smartconfig_event_got_ssid_pswd_t *evt
        = (smartconfig_event_got_ssid_pswd_t *)event_data;
    wifi_config_t wifi_config =
        {
            .sta =
                {
                    /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (password len => 8).
                     * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
                     * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
                     * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
                     */
                    .threshold.authmode = WIFI_AUTH_WPA2_PSK,
                    .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
                },
        };

    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password,
           evt->password,
           sizeof(wifi_config.sta.password));
    
    memcpy(p_ssid_midd, evt->ssid, sizeof(evt->ssid));
    memcpy(p_pass_midd, evt->password, sizeof(evt->password));

    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_connect();
  }
  else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
  {
    xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}