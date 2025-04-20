/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_things_board.h"
#include "app_data_ESP32.h"
#include "tbc_mqtt_helper.h"
#include "wifi.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define TELEMETRY_ENERGY       "energy"
#define TELEMETRY_ENERGY_HOUR  "energy_hour"
#define TELEMETRY_ENERGY_DAY   "energy_day"
#define TELEMETRY_ENERGY_WEEK  "energy_week"
#define TELEMETRY_ENERGY_MONTH "energy_month"
#define TELEMETRY_ENERGY_YEAR  "energy_year"

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct things_board_data
{
  EventGroupHandle_t *p_flag_mqtt_event;
  EventGroupHandle_t *p_flag_wifi_event;
  float              *p_energy;
  float              *p_energy_min;
  float              *p_energy_hour;
  float              *p_energy_day;
  float              *p_energy_week;
  float              *p_energy_month;
  float              *p_energy_year;
  STATE_t            *p_state;
} things_board_data_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Things_board_task(void *arg);

static tbcmh_value_t *te_get_energy(void);
static void           tb_telemetry_send_energy(tbcmh_handle_t client);

static tbcmh_value_t *te_get_energy_hour(void);
static void           tb_telemetry_send_energy_hour(tbcmh_handle_t client);

static tbcmh_value_t *te_get_energy_day(void);
static void           tb_telemetry_send_energy_day(tbcmh_handle_t client);

static tbcmh_value_t *te_get_energy_week(void);
static void           tb_telemetry_send_energy_week(tbcmh_handle_t client);

static tbcmh_value_t *te_get_energy_month(void);
static void           tb_telemetry_send_energy_month(tbcmh_handle_t client);

static tbcmh_value_t *te_get_energy_year(void);
static void           tb_telemetry_send_energy_year(tbcmh_handle_t client);

static void tb_on_connected(tbcmh_handle_t client, void *context);
static void tb_on_disconnected(tbcmh_handle_t client, void *context);

static void startWifiScan(void);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static things_board_data_t s_things_board_data;

static char    buffer_ssid_scan[200 + 1];
static uint8_t ssid[32];
static uint8_t pass[32];

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Things_board_CreateTask (void)
{
  xTaskCreate(APP_Things_board_task, "things_board", 1024 * 20, NULL, 10, NULL);
}

void
APP_Things_board_Init (void)
{
  // Link pointer to variable
  s_things_board_data.p_flag_mqtt_event = &s_data_system.s_flag_mqtt_event;
  s_things_board_data.p_flag_wifi_event = &s_data_system.s_flag_wifi_event;

  s_things_board_data.p_energy       = &s_data_system.f_energy;
  s_things_board_data.p_energy_min   = &s_data_system.f_energy_min;
  s_things_board_data.p_energy_hour  = &s_data_system.f_energy_hour;
  s_things_board_data.p_energy_day   = &s_data_system.f_energy_day;
  s_things_board_data.p_energy_week  = &s_data_system.f_energy_week;
  s_things_board_data.p_energy_month = &s_data_system.f_energy_month;
  s_things_board_data.p_energy_year  = &s_data_system.f_energy_year;
  s_things_board_data.p_state        = &s_data_system.s_state;
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Things_board_task (void *arg)
{
  WIFI_StaInit();

  // s_things_board_data.p_state = STATE_SCAN_WIFI;
  // printf("%d\n", *s_things_board_data.p_state);

  startWifiScan();

  const char *access_token = "CRP3LkcD9jjhyFbSvAl1";
  const char *uri          = "mqtt://demo.thingsboard.io";

  tbcmh_handle_t client = tbcmh_init();

  tbc_transport_config_esay_t config = {
    .uri              = uri, /*!< Complete ThingsBoard MQTT broker URI */
    .access_token     = access_token, /*!< ThingsBoard Access Token */
    .log_rxtx_package = true          /*!< print Rx/Tx MQTT package */
  };

  tbcmh_connect_using_url(
      client, &config, NULL, tb_on_connected, tb_on_disconnected);

  EventBits_t uxBits;
  // EventBits_t bits;

  while (1)
  {
    uxBits = xEventGroupWaitBits(
        *s_things_board_data.p_flag_mqtt_event,
        BIT_SEND_ENERGY | BIT_SEND_ENERGY_HOUR | BIT_SEND_ENERGY_DAY
            | BIT_SEND_ENERGY_WEEK | BIT_SEND_ENERGY_MONTH
            | BIT_SEND_ENERGY_YEAR,
        pdTRUE,
        pdFALSE,
        portMAX_DELAY);

    // bits = xEventGroupWaitBits(*s_things_board_data.p_flag_wifi_event,
    //                            WIFI_FAIL_BIT,
    //                            pdFALSE,
    //                            pdFALSE,
    //                            portMAX_DELAY);

    if (uxBits != 0)
    {
      if (tbcmh_has_events(client))
      {
        tbcmh_run(client);
      }
    }

    if ((uxBits & BIT_SEND_ENERGY))
    {
      if (tbcmh_is_connected(client))
      {
        tb_telemetry_send_energy(client);

        // Set bit flag to know send energy successfully
        *s_things_board_data.p_state = STATE_THINGSBOARD_SUCCESS;
      }
    }

    if ((uxBits & BIT_SEND_ENERGY_HOUR))
    {
      if (tbcmh_is_connected(client))
      {
        tb_telemetry_send_energy_hour(client);
      }
    }

    if ((uxBits & BIT_SEND_ENERGY_DAY))
    {
      if (tbcmh_is_connected(client))
      {
        tb_telemetry_send_energy_day(client);
      }
    }

    if ((uxBits & BIT_SEND_ENERGY_MONTH))
    {
      if (tbcmh_is_connected(client))
      {
        tb_telemetry_send_energy_month(client);
      }
    }

    if ((uxBits & BIT_SEND_ENERGY_YEAR))
    {
      if (tbcmh_is_connected(client))
      {
        tb_telemetry_send_energy_year(client);
      }
    }

    // if (bits & WIFI_FAIL_BIT)
    // {
    //   WIFI_SmartConfig();
    // }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

/************TELEMETRY ENERGY*************************************************/

tbcmh_value_t *
te_get_energy (void)
{
  cJSON *p_energy = cJSON_CreateNumber(*s_things_board_data.p_energy);
  return p_energy;
}

void
tb_telemetry_send_energy (tbcmh_handle_t client)
{
  cJSON *p_object = cJSON_CreateObject();
  cJSON_AddItemToObject(p_object, TELEMETRY_ENERGY, te_get_energy());
  tbcmh_telemetry_upload_ex(client, p_object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(p_object);
}

/************TELEMETRY HOUR***************************************************/

static tbcmh_value_t *
te_get_energy_hour (void)
{
  cJSON *p_energy = cJSON_CreateNumber(*s_things_board_data.p_energy_min);
  return p_energy;
}

static void
tb_telemetry_send_energy_hour (tbcmh_handle_t client)
{
  cJSON *p_object = cJSON_CreateObject();
  cJSON_AddItemToObject(p_object, TELEMETRY_ENERGY_HOUR, te_get_energy_hour());
  tbcmh_telemetry_upload_ex(client, p_object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(p_object);
}

/************TELEMETRY DAY****************************************************/

static tbcmh_value_t *
te_get_energy_day (void)
{
  cJSON *p_energy_day = cJSON_CreateNumber(*s_things_board_data.p_energy_hour);
  return p_energy_day;
}

static void
tb_telemetry_send_energy_day (tbcmh_handle_t client)
{
  cJSON *p_object = cJSON_CreateObject();
  cJSON_AddItemToObject(p_object, TELEMETRY_ENERGY_DAY, te_get_energy_day());
  tbcmh_telemetry_upload_ex(client, p_object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(p_object);
}

/************TELEMETRY WEEK***************************************************/

static tbcmh_value_t *
te_get_energy_week (void)
{
  cJSON *p_energy_week = cJSON_CreateNumber(*s_things_board_data.p_energy_day);
  return p_energy_week;
}

static void
tb_telemetry_send_energy_week (tbcmh_handle_t client)
{
  cJSON *p_object = cJSON_CreateObject();
  cJSON_AddItemToObject(p_object, TELEMETRY_ENERGY_WEEK, te_get_energy_week());
  tbcmh_telemetry_upload_ex(client, p_object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(p_object);
}

/************TELEMETRY MONTH**************************************************/

static tbcmh_value_t *
te_get_energy_month (void)
{
  cJSON *p_energy_month = cJSON_CreateNumber(*s_things_board_data.p_energy_day);
  return p_energy_month;
}

static void
tb_telemetry_send_energy_month (tbcmh_handle_t client)
{
  cJSON *p_object = cJSON_CreateObject();
  cJSON_AddItemToObject(
      p_object, TELEMETRY_ENERGY_MONTH, te_get_energy_month());
  tbcmh_telemetry_upload_ex(client, p_object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(p_object);
}

/************TELEMETRY YEAR***************************************************/

static tbcmh_value_t *
te_get_energy_year (void)
{
  cJSON *p_energy_year
      = cJSON_CreateNumber(*s_things_board_data.p_energy_month);
  return p_energy_year;
}

static void
tb_telemetry_send_energy_year (tbcmh_handle_t client)
{
  cJSON *p_object = cJSON_CreateObject();
  cJSON_AddItemToObject(p_object, TELEMETRY_ENERGY_YEAR, te_get_energy_year());
  tbcmh_telemetry_upload_ex(client, p_object, 1 /*qos*/, 0 /*retain*/);
  cJSON_Delete(p_object);
}

/************CONNECTED - DISCONNECTED*****************************************/

static void
tb_on_connected (tbcmh_handle_t client, void *context)
{
}
static void
tb_on_disconnected (tbcmh_handle_t client, void *context)
{
}

/************INIT WIFI*****************************************/

static void
startWifiScan (void)
{
  *s_things_board_data.p_state = STATE_SCAN_WIFI;

  uint8_t num_wifi_scan = 0;

  /*
      - Store all ssid and pass scan to array buffer.
      - Check ssid & pass
          - Matching ssid & pass in nvs ->
          set bit CONNECT_WIFI_SCAN_BIT.
          - Don't matching ssid & pass in nvs ->
          store ssid & pass in nvs.
  */
  num_wifi_scan = WIFI_Scan(buffer_ssid_scan);
  while (num_wifi_scan == 0)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    num_wifi_scan = WIFI_Scan(buffer_ssid_scan);
  }
  // Check matching ssid and pass in nvs
  if (matchingWIFIScan(buffer_ssid_scan, (uint8_t *)ssid, (uint8_t *)pass)
      != -1)
  {
    printf("Found Matching SSID and Password\r\n");
    if (WIFI_Connect((uint8_t *)ssid, (uint8_t *)pass) == CONNECT_OK)
    {
      printf("Connect Wifi\r\n");

      *s_things_board_data.p_state = STATE_WIFI_CONNECTED;
    }
    else
    {
      WIFI_ClearNVS((uint8_t *)ssid);

      *s_things_board_data.p_state = STATE_SMART_CONFIG;

      WIFI_SmartConfig();

      *s_things_board_data.p_state = STATE_WIFI_CONNECTED;

      printf("Connect Wifi through smart config\r\n");
    }
  }
  else
  {
    *s_things_board_data.p_state = STATE_SMART_CONFIG;

    // Execute smart config and store ssid and pass to nvs
    WIFI_SmartConfig();

    *s_things_board_data.p_state = STATE_WIFI_CONNECTED;

    printf("Connect Wifi through smart config\r\n");
  }
}
