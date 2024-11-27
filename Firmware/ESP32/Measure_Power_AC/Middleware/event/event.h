#ifndef EVENT_H_
#define EVENT_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "esp_smartconfig.h"
#include <stdint.h>
#include "esp_wifi.h"

/*********************
 *      DEFINES
 *********************/

#define ESPTOUCH_DONE_BIT  (1 << 0)
#define WIFI_CONNECTED_BIT (1 << 1)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void event_init (uint8_t *p_ssid, uint8_t *p_pass);
void event_handler(void            *arg,
                   esp_event_base_t event_base,
                   int32_t          event_id,
                   void            *event_data);

#endif
