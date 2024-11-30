// smartconfig.c
#include "smartconfig.h"
#include "esp_event.h"
#include "wifi.h"

/**********************
 *   STATIC FUNCTIONS
 **********************/

void
initialise_smartconfig (void)
{
  ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
  smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
}
