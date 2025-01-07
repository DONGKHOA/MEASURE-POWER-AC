#ifndef WIFI_H_
#define WIFI_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdio.h>

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
  CONNECT_OK = 1,
  CONNECT_FAIL,
  UNEXPECTED_EVENT,
} WIFI_Status_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void          WIFI_StaInit(void);
uint8_t       WIFI_Scan(char *data_name);
int8_t        WIFI_ScanNVS(uint8_t *ssid, uint8_t *pass);
void          WIFI_StoreNVS(uint8_t *ssid, uint8_t *password);
void          WIFI_ClearNVS(uint8_t *ssid);
WIFI_Status_t WIFI_Connect(uint8_t *ssid, uint8_t *password);
void          WIFI_SmartConfig(void);
int8_t        matchingWIFIScan(char *data, uint8_t *ssid, uint8_t *pass);
#endif