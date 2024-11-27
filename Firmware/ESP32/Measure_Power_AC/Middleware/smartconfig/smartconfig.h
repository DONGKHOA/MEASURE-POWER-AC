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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void initialise_smartconfig (void);

#endif
