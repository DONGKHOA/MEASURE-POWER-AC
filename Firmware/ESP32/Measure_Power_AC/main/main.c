/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "rom/ets_sys.h"

#include "wifi.h"
#include "gpio.h"
#include "nvs_rw.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_15)

#define MIN_STACK_SIZE 1024

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static QueueHandle_t mqtt_queue;

/******************************************************************************
 *    PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

tbcmh_value_t *te_get_lux(void);
void           tb_telemetry_send(tbcmh_handle_t client);
void           tb_on_connected(tbcmh_handle_t client, void *context);
void           tb_on_disconnected(tbcmh_handle_t client, void *context);

static void mqtt_main(void *arg);

/******************************************************************************
 *       MAIN FUNCTION
 *****************************************************************************/

void
app_main (void)
{
  NVS_Init();

  uartDriverInit(UART_NUM_1,
                 TXD_PIN,
                 RXD_PIN,
                 115200,
                 UART_DATA_8_BITS,
                 UART_PARITY_DISABLE,
                 UART_HW_FLOWCTRL_DISABLE,
                 UART_STOP_BITS_1);

  xTaskCreate(mqtt_main, "mqtt_maintask", 1024 * 10, NULL, 11, NULL);

  mqtt_queue = xQueueCreate(2, sizeof(uint32_t));
}

/******************************************************************************
 *    PRIVATE FUNCTION
 *****************************************************************************/

tbcmh_value_t *
te_get_lux (void)
{
  cJSON *lux = cJSON_CreateNumber(te_lux);
  return lux;
}

void
tb_telemetry_send (tbcmh_handle_t client)
{

  cJSON *object = cJSON_CreateObject();
  cJSON_AddItemToObject(object, TELEMETRY_LUX, te_get_lux());
  tbcmh_telemetry_upload_ex(client, object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(object);
}

void
tb_on_connected (tbcmh_handle_t client, void *context)
{
}

void
tb_on_disconnected (tbcmh_handle_t client, void *context)
{
}

static void
mqtt_main (void *arg)
{
  // WIFI_StaInit();
  // WIFI_Connect(ssid, pass);

  initialise_wifi();

  const char *access_token = "hwpgkyx3ozahl2ysklay";
  const char *uri          = "mqtt://thingsboard.cloud";

  tbcmh_handle_t client = tbcmh_init();

  tbc_transport_config_esay_t config = {
    .uri              = uri, /*!< Complete ThingsBoard MQTT broker URI */
    .access_token     = access_token, /*!< ThingsBoard Access Token */
    .log_rxtx_package = true          /*!< print Rx/Tx MQTT package */
  };
  tbcmh_connect_using_url(
      client, &config, NULL, tb_on_connected, tb_on_disconnected);
  uint32_t lux;
  while (1)
  {
    if (xQueueReceive(mqtt_queue, &lux, portMAX_DELAY))
    {
      if (tbcmh_has_events(client))
      {
        tbcmh_run(client);
      }
      if (tbcmh_is_connected(client))
      {
        tb_telemetry_send(client);
      }
    }
  }
}