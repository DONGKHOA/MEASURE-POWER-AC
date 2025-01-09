/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "wifi.h"
#include "nvs_rw.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_ip_addr.h"
#include "esp_smartconfig.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "app_data_ESP32.h"

/*********************
 *      DEFINES
 *********************/

#define TAG1 "smartconfig"
static const char *TAG = "scan";

#define DEFAULT_SCAN_LIST_SIZE 10
#define MAXIMUM_RETRY          10

#define NUM_WIFI_NVS "Num_ssid_nvs"
#define NUM_WIFI_KEY "Num_ssid_key"
#define SSID_NVS     "ssid_nvs"
#define PASS_NVS     "pass_nvs"

#define MAX_AP_INFO 100 // Limit number of APs to store in NVS

/**********************
 *  STATIC VARIABLES
 **********************/

static uint8_t is_initialized = 0;

typedef struct wifi_data
{
  EventGroupHandle_t *p_flag_wifi_event;
  char                ssid_name[1024];
  uint8_t             s_retry_num;
  uint8_t volatile num_wifi;
} wifi_data_t;

wifi_data_t s_wifi_data;
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void
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
    if (s_wifi_data.s_retry_num < MAXIMUM_RETRY)
    {
      esp_wifi_connect();
      s_wifi_data.s_retry_num++;
      ESP_LOGI(TAG, "retry to connect to the AP");
    }
    else
    {
      xEventGroupSetBits(*s_wifi_data.p_flag_wifi_event, WIFI_FAIL_BIT);
    }
    ESP_LOGE(TAG, "connect to the AP fail");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    xEventGroupSetBits(*s_wifi_data.p_flag_wifi_event, WIFI_CONNECTED_BIT);
  }
}

static void
smart_config_event_handler (void            *arg,
                            esp_event_base_t event_base,
                            int32_t          event_id,
                            void            *event_data)
{

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    xTaskCreate(WIFI_SmartConfig, "smartconfig_task", 4096, NULL, 3, NULL);
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    esp_wifi_connect();
    xEventGroupClearBits(*s_wifi_data.p_flag_wifi_event,
                         WIFI_SMART_CONFIG_CONNECTED_BIT);
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    xEventGroupSetBits(*s_wifi_data.p_flag_wifi_event,
                       WIFI_SMART_CONFIG_CONNECTED_BIT);
  }
  else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
  {
    ESP_LOGI(TAG1, "Scan done");
  }
  else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
  {
    ESP_LOGI(TAG1, "Found channel");
  }
  else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
  {
    ESP_LOGI(TAG1, "Got SSID and password");

    smartconfig_event_got_ssid_pswd_t *evt
        = (smartconfig_event_got_ssid_pswd_t *)event_data;
    wifi_config_t wifi_config;

    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password,
           evt->password,
           sizeof(wifi_config.sta.password));

    uint8_t ssid[33]     = { 0 };
    uint8_t password[65] = { 0 };

    memcpy(ssid, evt->ssid, sizeof(evt->ssid));
    memcpy(password, evt->password, sizeof(evt->password));
    ESP_LOGI(TAG1, "SSID:%s", ssid);
    ESP_LOGI(TAG1, "PASSWORD:%s", password);

    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_connect();

    WIFI_StoreNVS(ssid, password);
    printf("Store SSID and password to NVS\n");
  }
  else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
  {
    xEventGroupSetBits(*s_wifi_data.p_flag_wifi_event, ESPTOUCH_DONE_BIT);
  }
}
static uint8_t
WIFI_GetNumSSID (void)
{
  uint8_t      num = 0;
  nvs_handle_t nvsHandle;
  nvs_open(NUM_WIFI_NVS, NVS_READWRITE, &nvsHandle);
  esp_err_t err = nvs_get_u8(nvsHandle, NUM_WIFI_KEY, &num);

  if (err == ESP_OK)
  {
    nvs_close(nvsHandle);
    return num;
  }
  else
  {
    nvs_set_u8(nvsHandle, NUM_WIFI_KEY, num);
    return 0;
  }
}

static esp_err_t
WIFI_ScanSSID (uint8_t *ssid, uint8_t id, uint8_t len)
{
  char ssid_key[32];
  sprintf(ssid_key, "%d ssid", id);
  return NVS_ReadString(SSID_NVS, (const char *)ssid_key, (char *)ssid, 32);
}

static esp_err_t
WIFI_ScanPass (uint8_t *pass, uint8_t id, uint8_t len)
{
  char pass_key[32];
  sprintf(pass_key, "%d pass", id);
  return NVS_ReadString(PASS_NVS, (const char *)pass_key, (char *)pass, 32);
}

static void
WIFI_SetNumSSID (uint8_t num)
{
  nvs_handle_t nvsHandle;
  nvs_open(NUM_WIFI_NVS, NVS_READWRITE, &nvsHandle);
  nvs_set_u8(nvsHandle, NUM_WIFI_KEY, num);
}

static esp_err_t
WIFI_SetSSID (uint8_t *ssid, uint8_t id)
{
  char ssid_key[32];
  sprintf(ssid_key, "%d ssid", id);
  return NVS_WriteString(SSID_NVS, (const char *)ssid_key, (const char *)ssid);
}

static esp_err_t
WIFI_SetPass (uint8_t *pass, uint8_t id)
{
  char pass_key[32];
  sprintf(pass_key, "%d pass", id);
  return NVS_WriteString(PASS_NVS, (const char *)pass_key, (const char *)pass);
}

static void
print_auth_mode (int authmode)
{
  switch (authmode)
  {
    case WIFI_AUTH_OPEN:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
      break;
    case WIFI_AUTH_OWE:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
      break;
    case WIFI_AUTH_WEP:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
      break;
    case WIFI_AUTH_WPA_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
      break;
    case WIFI_AUTH_WPA2_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
      break;
    case WIFI_AUTH_ENTERPRISE:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_ENTERPRISE");
      break;
    case WIFI_AUTH_WPA3_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
      break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
      break;
    case WIFI_AUTH_WPA3_ENT_192:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
      break;
    default:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
      break;
  }
}

static void
print_cipher_type (int pairwise_cipher, int group_cipher)
{
  switch (pairwise_cipher)
  {
    case WIFI_CIPHER_TYPE_NONE:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
      break;
    case WIFI_CIPHER_TYPE_WEP40:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
      break;
    case WIFI_CIPHER_TYPE_WEP104:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
      break;
    case WIFI_CIPHER_TYPE_TKIP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
      break;
    case WIFI_CIPHER_TYPE_CCMP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
      break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
      break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
      break;
    case WIFI_CIPHER_TYPE_SMS4:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
      break;
    case WIFI_CIPHER_TYPE_GCMP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
      break;
    case WIFI_CIPHER_TYPE_GCMP256:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
      break;
    default:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
      break;
  }

  switch (group_cipher)
  {
    case WIFI_CIPHER_TYPE_NONE:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
      break;
    case WIFI_CIPHER_TYPE_WEP40:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
      break;
    case WIFI_CIPHER_TYPE_WEP104:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
      break;
    case WIFI_CIPHER_TYPE_TKIP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
      break;
    case WIFI_CIPHER_TYPE_CCMP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
      break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
      break;
    case WIFI_CIPHER_TYPE_SMS4:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
      break;
    case WIFI_CIPHER_TYPE_GCMP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
      break;
    case WIFI_CIPHER_TYPE_GCMP256:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
      break;
    default:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
      break;
  }
}

static uint8_t
isDuplicate (char *str, wifi_ap_record_t ap_list[], int count)
{
  for (int i = 0; i < count; i++)
  {
    if (strcmp(str, (char *)ap_list[i].ssid) == 0)
    {
      return 1; // Duplicate
    }
  }
  return 0; // Not duplicate
}

static void
deleteDuplicateSubstrings (char *str, char *result)
{
  char *token;
  char *delim = "\n";
  char *substrings[100]; // Two-dimensional array to store non-duplicate
                         // substrings
  int count = 0;         // Number of substrings added to the array

  // Parse the original string into substrings and store them in the array
  token = strtok(str, delim);
  while (token != NULL)
  {
    if (!isDuplicate(token, substrings, count))
    {
      substrings[count++] = token;
    }
    token = strtok(NULL, delim);
  }

  // Reconstruct the new string from the non-duplicate substrings
  result[0] = '\0'; // Initialize the new string
  for (int i = 0; i < count; i++)
  {
    strcat(result,
           substrings[i]); // Concatenate the substring into the new string
    strcat(result, "\n");  // Add '\n' character after each substring
  }
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void
WIFI_StaInit (void)
{
  s_wifi_data.p_flag_wifi_event = &s_data_system.s_flag_wifi_event;
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
}

uint8_t
WIFI_Scan (char *data_name)
{
  esp_wifi_stop();

  int i;
  memset(s_wifi_data.ssid_name, '\0', sizeof(s_wifi_data.ssid_name));
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  uint16_t number         = 10; // Number of APs found in the scan
  uint16_t total_ap_count = 0;  // Total number of APs found

  wifi_ap_record_t temp_ap_info[10];
  wifi_ap_record_t all_ap_info[MAX_AP_INFO];

  memset(temp_ap_info, 0, sizeof(temp_ap_info));

  for (int scan_round = 0; scan_round < 3; scan_round++) // Scan 3 rounds
  {
    esp_wifi_scan_start(NULL, true);

    printf("Starting WiFi Scan Round %d\n", scan_round + 1);

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, temp_ap_info));

    for (i = 0; i < number; i++)
    {
      if (!isDuplicate(
              (char *)temp_ap_info[i].ssid, all_ap_info, total_ap_count))
      {
        if (total_ap_count < MAX_AP_INFO)
        {
          all_ap_info[total_ap_count++] = temp_ap_info[i];
        }
        else
        {
          printf("AP list is full. Cannot add more networks.\n");
          break;
        }
      }
    }
  }

  uint16_t ssid_name_pos = 0;
  uint8_t  buffer[32];
  uint16_t total_number_wifi = 0;

  for (i = 0; i < number; i++)
  {
    uint16_t temp_pos = 0;
    memset(buffer, '\0', sizeof(buffer));
    memcpy(
        buffer, all_ap_info[i].ssid, strlen((char *)all_ap_info[i].ssid) + 1);
    buffer[strlen((char *)buffer)] = '\n';

    while (buffer[temp_pos] != '\n')
    {
      s_wifi_data.ssid_name[ssid_name_pos] = buffer[temp_pos];
      ssid_name_pos++;
      temp_pos++;
    }
    s_wifi_data.ssid_name[ssid_name_pos] = '\n';
    ssid_name_pos++;

    ESP_LOGI(TAG, "SSID \t\t%s", all_ap_info[i].ssid);
    ESP_LOGI(TAG, "RSSI \t\t%d", all_ap_info[i].rssi);
    print_auth_mode(all_ap_info[i].authmode);

    if (all_ap_info[i].authmode != WIFI_AUTH_WEP)
    {
      print_cipher_type(all_ap_info[i].pairwise_cipher,
                        all_ap_info[i].group_cipher);
    }
    ESP_LOGI(TAG, "Channel \t\t%d\n", all_ap_info[i].primary);
  }

  deleteDuplicateSubstrings(s_wifi_data.ssid_name, data_name);

  for (i = 0; i < strlen(data_name); i++)
  {
    if (data_name[i] == '\n')
    {
      data_name[i] = '\r';
      total_number_wifi++;
    }
  }
  data_name[i] = '\0';

  esp_wifi_start();

  return total_number_wifi;
}

int8_t
WIFI_ScanNVS (uint8_t *ssid, uint8_t *pass)
{
  int8_t  i;
  uint8_t ssid_temp[32];
  uint8_t volatile num_scan = 0;

  num_scan = WIFI_GetNumSSID();
  if (num_scan == 0)
  {
    return -1;
  }

  for (i = 1; i <= num_scan; i++)
  {
    WIFI_ScanSSID(ssid_temp, i, 32);
    if (memcmp(ssid_temp, ssid, strlen((char *)ssid)) == 0)
    {
      WIFI_ScanPass(pass, i, 32);
      return i;
    }
  }
  return -1;
}

void
WIFI_StoreNVS (uint8_t *ssid, uint8_t *password)
{
  s_wifi_data.num_wifi = WIFI_GetNumSSID();
  s_wifi_data.num_wifi++;
  WIFI_SetNumSSID(s_wifi_data.num_wifi);
  WIFI_SetSSID(ssid, s_wifi_data.num_wifi);
  WIFI_SetPass(password, s_wifi_data.num_wifi);
}

void
WIFI_ClearNVS (uint8_t *ssid)
{
  nvs_handle_t nvsHandle;
  uint8_t      ssid_temp[32];
  uint8_t      num_wifi_temp = 0;
  char         key_ssid[16];
  char         key_pass[16];

  // Open NVS
  esp_err_t err = nvs_open(NUM_WIFI_NVS, NVS_READWRITE, &nvsHandle);
  if (err != ESP_OK)
  {
    printf("Error opening NVS: %s\n", esp_err_to_name(err));
    return;
  }

  // Take the number of SSID from NVS
  err = nvs_get_u8(nvsHandle, NUM_WIFI_KEY, &num_wifi_temp);
  if (err != ESP_OK || num_wifi_temp == 0)
  {
    printf("No WiFi entries to clear.\n");
    nvs_close(nvsHandle);
    return;
  }

  printf("num_wifi_temp = %d\n", num_wifi_temp);
  for (uint8_t i = 1; i <= num_wifi_temp; i++)
  {
    // Create key for SSID and pass
    snprintf(key_ssid, sizeof(key_ssid), "%d ssid", i);
    snprintf(key_pass, sizeof(key_pass), "%d pass", i);

    // Take SSID from NVS
    memset(ssid_temp, 0, sizeof(ssid_temp)); // Xóa dữ liệu cũ
    size_t ssid_len = sizeof(ssid_temp);

    err = NVS_ReadString(
        SSID_NVS, (const char *)key_ssid, (char *)ssid_temp, &ssid_len);

    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
      printf("SSID key not found: %s\n", key_ssid);
      continue; // Ignore if key not found
    }
    else if (err != ESP_OK)
    {
      printf("Error reading SSID key %s: %s\n", key_ssid, esp_err_to_name(err));
      continue;
    }

    // Check if SSID is the same
    if (strcmp((char *)ssid_temp, (char *)ssid) == 0)
    {
      // Erase SSID and pass if SSID is the same
      nvs_erase_key(nvsHandle, key_ssid);
      nvs_erase_key(nvsHandle, key_pass);

      printf("Removed SSID and pass for SSID: %s\n", ssid_temp);

      // Update number of SSID in NVS
      num_wifi_temp--;
      nvs_set_u8(nvsHandle, NUM_WIFI_KEY, num_wifi_temp);
      break;
    }
    printf("Fail to remove SSID and pass for SSID: %s\n", ssid_temp);
  }

  // Write changes to NVS
  nvs_commit(nvsHandle);
  nvs_close(nvsHandle);
}

/**
 * The function `WIFI_Connect` attempts to connect to a WiFi network using the
 * provided SSID and password, handling authentication modes and event
 * notifications.
 *
 * @param ssid The `ssid` parameter in the `WIFI_Connect` function is a pointer
 * to an array of characters representing the SSID (Service Set Identifier) of
 * the Wi-Fi network you want to connect to. The SSID is essentially the name of
 * the Wi-Fi network.
 * @param password The code you provided is a function `WIFI_Connect` that
 * attempts to connect to a WiFi network using the provided SSID and password.
 * The function initializes the WiFi configuration, sets up event handlers, and
 * then tries to connect to the network.
 *
 * @return The function `WIFI_Connect` returns a value of type `WIFI_Status_t`,
 * which is an enumeration type. The possible return values are:
 * - `CONNECT_OK` if the connection to the Wi-Fi network was successful.
 * - `CONNECT_FAIL` if the connection to the Wi-Fi network failed.
 * - `UNEXPECTED_EVENT` if an unexpected event occurred during the connection
 * process.
 */
WIFI_Status_t
WIFI_Connect (uint8_t *ssid, uint8_t *password)
{
  esp_wifi_stop();

  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);
  esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);

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

  for (uint8_t i = 0; i < 32; i++)
  {
    wifi_config.sta.ssid[i]     = *(ssid + i);
    wifi_config.sta.password[i] = *(password + i);
  }

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  esp_wifi_start();

  ESP_LOGI(TAG, "Wifi_init_station finished.");

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or
   * connection failed for the maximum number of re-tries (WIFI_FAIL_BIT). The
   * bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(*s_wifi_data.p_flag_wifi_event,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
   * can test which event actually happened. */
  if (bits & WIFI_CONNECTED_BIT)
  {
    s_wifi_data.s_retry_num = 0;
    ESP_LOGI(TAG,
             "Connected to ap SSID:%s password:%s",
             wifi_config.sta.ssid,
             wifi_config.sta.password);
    return CONNECT_OK;
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    s_wifi_data.s_retry_num = 0;
    ESP_LOGE(TAG,
             "Failed to connect to SSID:%s, password:%s",
             wifi_config.sta.ssid,
             wifi_config.sta.password);
    return CONNECT_FAIL;
  }
  else
  {
    s_wifi_data.s_retry_num = 0;
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
    return UNEXPECTED_EVENT;
  }
}

void
WIFI_SmartConfig (void)
{
  // *s_wifi_data.p_flag_wifi_event = STATE_SMART_CONFIG;

  if (is_initialized == 0)
  {
    // Initialize smartconfig event handler
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &smart_config_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &smart_config_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
        SC_EVENT, ESP_EVENT_ANY_ID, &smart_config_event_handler, NULL));

    is_initialized = 1;
  }

  esp_smartconfig_stop();

  EventBits_t uxBits;
  ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
  smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
  while (1)
  {
    uxBits = xEventGroupWaitBits(*s_wifi_data.p_flag_wifi_event,
                                 WIFI_SMART_CONFIG_CONNECTED_BIT
                                     | ESPTOUCH_DONE_BIT
                                     | WIFI_SMART_CONFIG_LOST_CONNECTED_BIT,
                                 true,
                                 false,
                                 portMAX_DELAY);
    if (uxBits & WIFI_SMART_CONFIG_CONNECTED_BIT)
    {
      ESP_LOGI(TAG1, "WiFi Connected to ap");
    }
    if (uxBits & ESPTOUCH_DONE_BIT)
    {
      ESP_LOGI(TAG1, "smartconfig over");
      esp_smartconfig_stop();
      vTaskDelete(NULL);
    }
    // if (uxBits & WIFI_SMART_CONFIG_LOST_CONNECTED_BIT)
    // {
    //   ESP_LOGI(TAG1, "Start esp smart config again");
    //   esp_smartconfig_start(&cfg);
    // }
  }
}

int8_t
matchingWIFIScan (char *data, uint8_t *ssid, uint8_t *pass)
{
  char *arg_list[50];
  char  buffer[1024];
  memcpy(buffer, data, strlen((char *)data));
  uint8_t arg_position = 0;

  // cut string
  char *temp_token = strtok(buffer, "\r");
  while (temp_token != NULL)
  {
    arg_list[arg_position] = temp_token;
    arg_position++;
    temp_token = strtok(NULL, "\r");
  }

  // check matching ssid in NVS
  for (uint8_t i = 0; i < arg_position; i++)
  {
    if (WIFI_ScanNVS((uint8_t *)arg_list[i], pass) != -1)
    {
      memcpy(ssid, arg_list[i], strlen((char *)arg_list[i]) + 1);
      return 1;
    }
  }
  return -1;
}