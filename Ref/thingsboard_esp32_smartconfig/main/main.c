#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
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
#include "uart_driver.h"
#include "mqtt_client_tcp.h"
#include "tbc_mqtt_helper.h"

// static uint8_t 			ssid[32] = "MrAT";
// static uint8_t 			pass[32] = "14042003";

uint32_t te_lux;

// static TimerHandle_t mqtt_timer;
static QueueHandle_t mqtt_queue;


#define TXD_PIN                 (GPIO_NUM_18)
#define RXD_PIN                 (GPIO_NUM_19)
#define MIN_STACK_SIZE          1024
#define TELEMETRY_LUX         	"lux"

// #define TIME_MQTT                  2000

// static void MQTT_timer_cb(TimerHandle_t xTimer)
// {
//     xQueueSend(mqtt_queue, &te_lux, 0);
// }


tbcmh_value_t* te_get_lux(void)
{
     // ESP_LOGI(TAG, "Get temperature");
    cJSON* lux = cJSON_CreateNumber(te_lux);
    return lux;
}

void tb_telemetry_send(tbcmh_handle_t client)
{

    cJSON *object = cJSON_CreateObject();
    cJSON_AddItemToObject(object, TELEMETRY_LUX, te_get_lux());
    tbcmh_telemetry_upload_ex(client, object, 1/*qos*/, 0/*retain*/);
    cJSON_Delete(object);
}


void tb_on_connected(tbcmh_handle_t client, void *context){}

void tb_on_disconnected(tbcmh_handle_t client, void *context){}

static void mqtt_main(void *arg)
{
    // WIFI_StaInit();
    // WIFI_Connect(ssid, pass);

    initialise_wifi();

    const char *access_token = "hwpgkyx3ozahl2ysklay";
    const char *uri = "mqtt://thingsboard.cloud";

    tbcmh_handle_t client = tbcmh_init();

    tbc_transport_config_esay_t config = {
        .uri = uri,                     /*!< Complete ThingsBoard MQTT broker URI */
        .access_token = access_token,   /*!< ThingsBoard Access Token */
        .log_rxtx_package = true                /*!< print Rx/Tx MQTT package */
     };
    tbcmh_connect_using_url(client, &config, 
                        NULL, tb_on_connected, tb_on_disconnected);
    uint32_t lux;
    while(1) 
    {
        if ( xQueueReceive(mqtt_queue, &lux, portMAX_DELAY))
        {
            if (tbcmh_has_events(client)) tbcmh_run(client);
            if (tbcmh_is_connected(client)) tb_telemetry_send(client);   
        }
    }
}

static void echo_task(void *arg)
{
    uint8_t buffer_temp[10 + 1];
    static char buffer_uart_rx[10 + 1];
    uint16_t pos_buffer_uart_rx = 0;
    uint8_t enable_bit = 0;
    uint16_t i;

    memset((void *)buffer_uart_rx, '\0', sizeof(buffer_uart_rx));

    while (1)
    {
        uint16_t rxBytes = uart_read_bytes(UART_NUM_1, &buffer_temp,
                                           10, 10 / portTICK_PERIOD_MS);
        if (rxBytes > 0)
        {
            for (i = 0; i < rxBytes; i++)
            {
                buffer_uart_rx[pos_buffer_uart_rx] = buffer_temp[i];
                if (buffer_uart_rx[pos_buffer_uart_rx] == '\n')
                {
                    enable_bit = 1;
                    buffer_uart_rx[pos_buffer_uart_rx] = '\0';
                    pos_buffer_uart_rx = 0;
                    break;
                }
                pos_buffer_uart_rx++;
            }

            if (enable_bit == 0)
            {
                continue;
            }

            te_lux = atoi(buffer_uart_rx);
            xQueueSend(mqtt_queue, &te_lux, 0);

            enable_bit = 0;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    
}

void app_main(void)
{
    printf("Main task start\n");

    printf("init module in main\n");
    NVS_Init();

    uartDriverInit(UART_NUM_1, TXD_PIN, RXD_PIN,
                   115200, UART_DATA_8_BITS,
                   UART_PARITY_DISABLE, UART_HW_FLOWCTRL_DISABLE,
                   UART_STOP_BITS_1);

    // mqtt_timer = xTimerCreate("time mqtt subscribe",
    //                                     TIME_MQTT / portTICK_PERIOD_MS,
    //                                     pdTRUE, (void *)0, MQTT_timer_cb);
    mqtt_queue = xQueueCreate(2, sizeof(uint32_t));
    // xTimerStart(mqtt_timer, 0);
    printf("init service in main\n");
    xTaskCreate(echo_task, "uart_echo_task", 1024*10, NULL, 10, NULL);
    xTaskCreate(mqtt_main, "mqtt_maintask", 1024*10, NULL, 11, NULL);
}