// smartconfig.c
#include "smartconfig.h"
#include "esp_event.h"
#include <string.h>
#include "wifi.h"

/**********************
 *  PRIVATE DATA
 **********************/

static uint8_t *p_ssid_midd;
static uint8_t *p_pass_midd;

EventGroupHandle_t s_wifi_event_group;

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void
event_handler (void            *arg,
               esp_event_base_t event_base,
               int32_t          event_id,
               void            *event_data)
{
  if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
  {
    smartconfig_event_got_ssid_pswd_t *evt
        = (smartconfig_event_got_ssid_pswd_t *)event_data;

    memcpy(p_ssid_midd, evt->ssid, sizeof(evt->ssid));
    memcpy(p_pass_midd, evt->password, sizeof(evt->password));
    // WIFI_Connect(p_ssid_midd, p_pass_midd);
    // xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
  }
}

// void
// get_smartconfig_event_group (EventGroupHandle_t *p_wifi_event_group)
// {
//   p_wifi_event_group = &s_wifi_event_group;
// }

void
initialise_smartconfig (uint8_t *p_ssid, uint8_t *p_pass)
{
  p_ssid_midd = p_ssid;
  p_pass_midd = p_pass;

  // Start wifi
  esp_wifi_start();

  // smartconfig event group init
  s_wifi_event_group = xEventGroupCreate();

  // smartconfig event register
  // esp_event_loop_create_default();
  esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
}
