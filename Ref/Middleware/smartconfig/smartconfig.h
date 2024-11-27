#ifndef SMARTCONFIG_H_
#define SMARTCONFIG_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_smartconfig.h"
#include <stdint.h>
#include "esp_wifi.h"

/*********************
 *      DEFINES
 *********************/

#define ESPTOUCH_DONE_BIT (1 << 0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void initialise_smartconfig(uint8_t *p_ssid, uint8_t *p_pass);
// void get_smartconfig_event_group(EventGroupHandle_t *p_wifi_event_group);

#endif
