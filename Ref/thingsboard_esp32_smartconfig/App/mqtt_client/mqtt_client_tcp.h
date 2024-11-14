#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct 
{
    esp_mqtt_client_handle_t client;
}MQTT_Client_Data_t;

typedef enum
{
    TOPIC_LED,
    TOPIC_AUTO,
}MQTT_Topic_t;

/**********************
 *  EXTERN VARIABLES
 **********************/

extern char data_mqtt [10];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

void MQTT_app_start(MQTT_Client_Data_t *MQTT_Client, char *url);
int8_t MQTT_app_state_connect(void);

#endif