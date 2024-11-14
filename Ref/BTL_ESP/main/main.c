/*********************
 *      INCLUDES
 *********************/

#include "main.h"

/**********************
 *     VARIABLES
 **********************/

QueueHandle_t mqtt_topic_queue;

char ip_ssid_connected[32];

/**********************
 *  STATIC VARIABLES
 **********************/
static char buffer_uart_rx[1024 + 1];
char buffer_uart_tx[1024 + 1];
static char buffer_ssid_scan[200 + 1];

static uint8_t ssid[32];
static uint8_t pass[32];
static uint8_t flag_connect_success = 0;

static char url_mqtt[30] = "mqtt://172.16.0.181:1883";
static MQTT_Client_Data_t mqtt_client_0;
static uint8_t is_publish = 0;
static char ip_config[32];
char state_led[10] = "OFF";
char state_auto_nodered[10] = "OFF";

static char  ssid1[32];
static char  ssid2[32];
static char  ssid3[32];
static char  ssid4[32];
static char  ssid5[32];

// Task Handle

static TaskHandle_t uart_rx_task;
static TaskHandle_t uart_tx_task;
static TaskHandle_t wifiScan_task;
static TaskHandle_t wifiConnect_task;
static TaskHandle_t mqttConnect_task;
static TaskHandle_t mqttControlData_task;

// EventGroup Handle

EventGroupHandle_t event_uart_rx_heading;
EventGroupHandle_t event_uart_tx_heading;

// Timer Handle

static TimerHandle_t mqtt_subscribe_timer;

// Queue Handle

static QueueHandle_t mqtt_queue;

/******************************
 *  STATIC PROTOTYPE FUNCTION
 ******************************/

static void MQTT_timer_cb(TimerHandle_t xTimer);
static uint8_t getNumPage(uint8_t quantitySSID);
static uint8_t processingSSID(char *src, uint8_t numSSID, uint8_t page);
static void configIP_MQTT(char * ip_ssid, char *ip_mqtt, char * replace);

static void startUartRxTask(void *arg);
static void startUartTxTask(void *arg);
static void startWifiScan(void *arg);
static void startWifiConnectTask(void *arg);
static void startMQTTConnectTask(void *arg);
static void startMQTTControlDataTask(void *arg);

/********************
 *  MAIN FUNCTION
 ********************/

void app_main(void)
{
    printf("Main task start\n");

    printf("init module in main\n");
    NVS_Init();
    WIFI_StaInit();
    uartDriverInit(UART_NUM_1, TXD_PIN, RXD_PIN,
                   115200, UART_DATA_8_BITS,
                   UART_PARITY_DISABLE, UART_HW_FLOWCTRL_DISABLE,
                   UART_STOP_BITS_1);

    printf("init service in main\n");

    event_uart_rx_heading = xEventGroupCreate();
    event_uart_tx_heading = xEventGroupCreate();

    mqtt_subscribe_timer = xTimerCreate("time mqtt subscribe",
                                        TIME_MQTT_SUBSCRIBE / portTICK_PERIOD_MS,
                                        pdTRUE, (void *)0, MQTT_timer_cb);

    mqtt_queue = xQueueCreate(2, sizeof(uint8_t));
    mqtt_topic_queue = xQueueCreate(2, sizeof(uint8_t));

    printf("Task start initialization\n");

    xTaskCreate(startUartRxTask,
                "uart_rx_task",
                MIN_STACK_SIZE * 5,
                NULL,
                2,
                &uart_rx_task);

    xTaskCreate(startUartTxTask,
                "uart_tx_task",
                MIN_STACK_SIZE * 5,
                NULL,
                2,
                &uart_tx_task);

    xTaskCreate(startWifiScan,
                "Wifi scan",
                MIN_STACK_SIZE * 40,
                NULL,
                3,
                &wifiScan_task);

    xTaskCreate(startWifiConnectTask,
                "Wifi connect",
                MIN_STACK_SIZE * 3,
                NULL,
                4,
                &wifiConnect_task);

    xTaskCreate(startMQTTConnectTask,
                "Mqtt connect",
                MIN_STACK_SIZE * 3,
                NULL,
                5,
                &mqttConnect_task);

    xTaskCreate(startMQTTControlDataTask,
                "Mqtt control",
                MIN_STACK_SIZE * 3,
                NULL,
                6,
                &mqttControlData_task);

    printf("Task done initialization\n");
}

/********************
 *  STATIC FUNCTION
 ********************/

static void MQTT_timer_cb(TimerHandle_t xTimer)
{
    mqtt_command_t mqtt_command = MQTT_SUBSCRIBE;
    xQueueSend(mqtt_queue, &mqtt_command, 0);
}

/**
 * The function `getNumPage` calculates the number of pages needed based on the quantity of SSIDs.
 * 
 * @param quantitySSID The `quantitySSID` parameter represents the total quantity of SSIDs (Service Set
 * Identifiers) that need to be displayed on multiple pages.
 * 
 * @return The function `getNumPage` returns the number of pages needed to display a given quantity of
 * SSIDs, based on a predefined constant `SSID_IN_PAGE`.
 */
static uint8_t getNumPage(uint8_t quantitySSID)
{
	uint8_t quantity = 0;
	if (quantitySSID == 0)
	{
		return 0;
	}
	else
	{
		quantity = quantitySSID / SSID_IN_PAGE;
		if ((quantitySSID % SSID_IN_PAGE) > 0)
			quantity++;
		return quantity;
	}
}

/**
 * The function `processingSSID` processes a given source string to extract and store up to five SSIDs
 * based on the specified page number.
 * 
 * @param src The `src` parameter is a pointer to a character array containing the input data to be
 * processed. It seems like this function is designed to extract and process SSID information from the
 * input data.
 * @param numSSID The `numSSID` parameter in the `processingSSID` function represents the total number
 * of SSIDs that need to be processed. This value is used to determine how many SSIDs can be displayed
 * on a single page based on the `SSID_IN_PAGE` constant and the current page number.
 * @param page The `page` parameter in the `processingSSID` function represents the current page number
 * being processed. It is used to determine which subset of SSIDs should be extracted from the source
 * string. The function extracts SSIDs from the source string based on the current page number and the
 * number of SSIDs per
 * 
 * @return The function `processingSSID` returns the number of SSIDs to process on the current page
 * based on the total number of SSIDs (`numSSID`) and the SSID count per page (`SSID_IN_PAGE`).
 */
static uint8_t processingSSID(char *src, uint8_t numSSID, uint8_t page)
{
	char buffer[200 + 1];
    memcpy(buffer, src, strlen((char *) src));
    uint8_t arg_position = 0;

    memset((void *)ssid1, '\0', sizeof(ssid1));
    memset((void *)ssid2, '\0', sizeof(ssid2));
    memset((void *)ssid3, '\0', sizeof(ssid3));
    memset((void *)ssid4, '\0', sizeof(ssid4));
    memset((void *)ssid5, '\0', sizeof(ssid5));

	// cut string
	char *temp_token = strtok(buffer, "\r");
	while (temp_token != NULL)
	{
        if (arg_position == (4 + (page - 1) * 5))
        {
            memcpy(ssid5, temp_token, strlen((char *)temp_token) + 1);
        }

        if (arg_position == (3 + (page - 1) * 5))
        {
            memcpy(ssid4, temp_token, strlen((char *)temp_token) + 1);
        }

        if (arg_position == (2 + (page - 1) * 5))
        {
            memcpy(ssid3, temp_token, strlen((char *)temp_token) + 1);
        }

        if (arg_position == (1 + (page - 1) * 5))
        {
            memcpy(ssid2, temp_token, strlen((char *)temp_token) + 1);
        }

        if (arg_position == (0 + (page - 1) * 5))
        {
            memcpy(ssid1, temp_token, strlen((char *)temp_token) + 1);
        }
		arg_position++;
        
		temp_token = strtok(NULL, "\r");
	}

    return (numSSID - (page - 1) * SSID_IN_PAGE) > 
                SSID_IN_PAGE ? SSID_IN_PAGE : (numSSID - (page - 1) * SSID_IN_PAGE);
}

/**
 * The function `configIP_MQTT` extracts the IP address prefix from the provided SSID and appends a
 * specified string to it to configure the MQTT IP address.
 * 
 * @param ip_ssid The `ip_ssid` parameter is a string representing an IP address.
 * @param ip_mqtt The `ip_mqtt` parameter is a character array where the function `configIP_MQTT` will
 * store the modified IP address.
 * @param replace The `replace` parameter in the `configIP_MQTT` function is a string that will be
 * appended to the modified `ip_buffer` before being copied to the `ip_mqtt` parameter.
 */
static void configIP_MQTT(char * ip_ssid, char *ip_mqtt, char * replace)
{
    char ip_buffer[20];
    memset(ip_buffer, 0, 20);
    strcpy(ip_buffer, ip_ssid);
    uint8_t size_ip = strlen(ip_buffer);
    for (uint8_t i = size_ip; i > 0; i--)
    {
        if (ip_buffer[i] == '.')
        {
            break;
        }
        ip_buffer[i] = '\0';
        
    }
    
    sprintf(ip_mqtt, "%s%s", ip_buffer, replace);
}

static void startUartRxTask(void *arg)
{
    uint8_t buffer_temp[RX_BUF_SIZE + 1];
    uint16_t pos_buffer_uart_rx = 0;
    uint8_t enable_bit = 0;
    int8_t data_heading = -1;
    uint16_t i;

    memset((void *)buffer_uart_rx, '\0', sizeof(buffer_uart_rx));

    while (1)
    {
        uint16_t rxBytes = uart_read_bytes(UART_NUM_1, &buffer_temp,
                                           RX_BUF_SIZE, 10 / portTICK_PERIOD_MS);
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

            data_heading = buffer_uart_rx[0];
            uint8_t size_buffer = strlen(buffer_uart_rx);

            for (i = 0; i < size_buffer; i++)
            {
                buffer_uart_rx[i] = buffer_uart_rx[i + 1];
            }

            switch (data_heading)
            {
            case HEADING_WIFI:
                if (memcmp(buffer_uart_rx, "OFF", strlen(buffer_uart_rx) + 1) == 0)
                {
                    xEventGroupSetBits(event_uart_rx_heading, OFF_WIFI_BIT);
                }
                else if (memcmp(buffer_uart_rx, "ON", strlen(buffer_uart_rx) + 1) == 0)
                {
                    xEventGroupSetBits(event_uart_rx_heading, ON_WIFI_BIT);
                }
                
                break;
            case HEADING_TAB_PAGE:
                if (memcmp(buffer_uart_rx, "NEXT", strlen(buffer_uart_rx) + 1) == 0)
                {
                    xEventGroupSetBits(event_uart_rx_heading, NEXT_PAGE_BIT);
                }
                else if (memcmp(buffer_uart_rx, "BACK", strlen(buffer_uart_rx) + 1) == 0)
                {
                    xEventGroupSetBits(event_uart_rx_heading, BACK_PAGE_BIT);
                }
                break;

            case HEADING_CONNECT_WIFI:
                xEventGroupSetBits(event_uart_rx_heading, CONNECT_WIFI_RX_BIT);
                flag_connect_success = 0;
                break;

            case HEADING_CONNECT_MQTT:
                xEventGroupSetBits(event_uart_rx_heading, CONNECT_MQTT_BIT);
                break;

            case HEADING_MQTT_PUBLISH:
                mqtt_command_t mqtt_command = MQTT_PUBLISH;
                xQueueSend(mqtt_queue, &mqtt_command, 0);
                break;

            default:
                enable_bit = 0;
                memset((void *)buffer_uart_rx, '\0', sizeof(buffer_uart_rx));
                break;

                memset((void *)buffer_uart_rx, '\0', sizeof(buffer_uart_rx));
            }

            enable_bit = 0;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

static void startUartTxTask(void *arg)
{
    while (1)
    {
        EventBits_t uxBits = xEventGroupWaitBits(event_uart_tx_heading,
                                                 SCAN_WIFI_BIT |
                                                     SEND_CONNECT_WIFI_SUCCESSFUL_BIT |
                                                     SEND_CONNECT_WIFI_UNSUCCESSFUL_BIT |
                                                     SEND_REFUSE_CONNECT_MQTT_BIT |
                                                     SEND_CONNECT_MQTT_SUCCESSFUL_BIT |
                                                     SEND_CONNECT_MQTT_UNSUCCESSFUL_BIT |
                                                     SEND_MQTT_SUBSCRIBE,
                                                 pdTRUE, pdFALSE,
                                                 portMAX_DELAY);

        if (uxBits & SCAN_WIFI_BIT)
        {
            transmissionFrameData(HEADING_SEND_SCAN_WIFI, buffer_uart_tx);
        }

        if (uxBits & SEND_CONNECT_WIFI_SUCCESSFUL_BIT)
        {
            transmissionFrameData(HEADING_SEND_CONNECT_WIFI, buffer_uart_tx);
        }

        if (uxBits & SEND_CONNECT_WIFI_UNSUCCESSFUL_BIT)
        {
            transmissionFrameData(HEADING_SEND_CONNECT_WIFI, buffer_uart_tx);
        }

        if (uxBits & SEND_REFUSE_CONNECT_MQTT_BIT)
        {
            transmissionFrameData(HEADING_SEND_CONNECT_MQTT, buffer_uart_tx);
        }

        if (uxBits & SEND_CONNECT_MQTT_SUCCESSFUL_BIT)
        {
            strcpy(buffer_uart_tx, "TRUE");
            transmissionFrameData(HEADING_SEND_CONNECT_MQTT, buffer_uart_tx);
        }

        if (uxBits & SEND_CONNECT_MQTT_UNSUCCESSFUL_BIT)
        {
            strcpy(buffer_uart_tx, "FAILED");
            transmissionFrameData(HEADING_SEND_CONNECT_MQTT, buffer_uart_tx);
        }

        if (uxBits & SEND_MQTT_SUBSCRIBE)
        {
            transmissionFrameData(HEADING_MQTT_SUBSCRIBE, buffer_uart_tx);
        }

        memset((void *)buffer_uart_tx, '\0', sizeof(buffer_uart_tx));
    }
}

static void startWifiScan(void *arg)
{
    uint8_t num_wifi_scan = 0;
    uint8_t limitNumPage = 1; 
    uint8_t numSSIDofPage;
    uint8_t page = 0;

    while (1)
    {
        EventBits_t uxBits = xEventGroupWaitBits(event_uart_rx_heading,
                                                 ON_WIFI_BIT |
                                                 NEXT_PAGE_BIT |
                                                 BACK_PAGE_BIT |
                                                     OFF_WIFI_BIT,
                                                 pdTRUE, pdFALSE,
                                                 portMAX_DELAY);

        if (uxBits & ON_WIFI_BIT)
        {  
            /*
                - Store all ssid and pass scan to array buffer_uart_tx.
                - Check ssid & pass
                    - Matching ssid & pass in nvs ->
                    set bit CONNECT_WIFI_SCAN_BIT.
                    - Don't matching ssid & pass in nvs ->
                    store ssid & pass in nvs.
            */
            page = 1;
            num_wifi_scan = WIFI_Scan(buffer_ssid_scan);
            while (num_wifi_scan == 0)
            {
                ets_delay_us(500000);
                num_wifi_scan = WIFI_Scan(buffer_ssid_scan);
            }
            

            // if (WIFI_state_connect() == CONNECT_OK)
            // {
            //     xEventGroupSetBits(event_uart_tx_heading,
            //                        SEND_CONNECT_WIFI_UNSUCCESSFUL_BIT);
            //     continue;
            // }

            if (matchingWIFIScan(buffer_ssid_scan, (uint8_t *)ssid, (uint8_t *)pass) != -1)
            {
                xEventGroupSetBits(event_uart_rx_heading,
                                   CONNECT_WIFI_SCAN_BIT);
            }

            limitNumPage = getNumPage(num_wifi_scan);
            numSSIDofPage = processingSSID(buffer_ssid_scan, num_wifi_scan, page);
            if (numSSIDofPage == 5)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r%s\r%s\r", "START", numSSIDofPage,ssid1, 
                                    ssid2, ssid3, ssid4, ssid5);

            else if (numSSIDofPage == 4)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r%s\r", "START", numSSIDofPage,ssid1, 
                                    ssid2, ssid3, ssid4);

            else if (numSSIDofPage == 3)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r", "START", numSSIDofPage,ssid1, 
                                    ssid2, ssid3);

            else if (numSSIDofPage == 2)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r", "START", numSSIDofPage,ssid1, 
                                    ssid2);

            else if (numSSIDofPage == 1)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r", "START", numSSIDofPage,ssid1);
            
            memset((void *)ssid1, '\0', sizeof(ssid1));
            memset((void *)ssid2, '\0', sizeof(ssid2));
            memset((void *)ssid3, '\0', sizeof(ssid3));
            memset((void *)ssid4, '\0', sizeof(ssid4));
            memset((void *)ssid5, '\0', sizeof(ssid5));

            printf("%d\n", num_wifi_scan);
            printf("%d\n", limitNumPage);

            xEventGroupSetBits(event_uart_tx_heading,
                               SCAN_WIFI_BIT);

        }

        if (uxBits & NEXT_PAGE_BIT)
        {
            char state_page[10];
            if(page < limitNumPage) page++;

            if (page == 1) strcpy(state_page, "START");
            else if (page == limitNumPage) strcpy(state_page, "END");
            else strcpy(state_page, "MID");

            numSSIDofPage = processingSSID(buffer_ssid_scan, num_wifi_scan, page);
            if (numSSIDofPage == 5)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2, ssid3, ssid4, ssid5);

            else if (numSSIDofPage == 4)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2, ssid3, ssid4);

            else if (numSSIDofPage == 3)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2, ssid3);

            else if (numSSIDofPage == 2)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2);

            else if (numSSIDofPage == 1)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r", state_page, numSSIDofPage,ssid1);
            
            memset((void *)ssid1, '\0', sizeof(ssid1));
            memset((void *)ssid2, '\0', sizeof(ssid2));
            memset((void *)ssid3, '\0', sizeof(ssid3));
            memset((void *)ssid4, '\0', sizeof(ssid4));
            memset((void *)ssid5, '\0', sizeof(ssid5));
            printf("next: %s: %d\n",state_page, page);

            xEventGroupSetBits(event_uart_tx_heading,
                               SCAN_WIFI_BIT);
        }

        if (uxBits & BACK_PAGE_BIT)
        {
            char state_page[10];
            if(page > 1) page--;
            
            if (page == 1) strcpy(state_page, "START");
            else if (page == limitNumPage) strcpy(state_page, "END");
            else strcpy(state_page, "MID");

            numSSIDofPage = processingSSID(buffer_ssid_scan, num_wifi_scan, page);

            if (numSSIDofPage == 5)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2, ssid3, ssid4, ssid5);

            else if (numSSIDofPage == 4)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2, ssid3, ssid4);

            else if (numSSIDofPage == 3)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2, ssid3);

            else if (numSSIDofPage == 2)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r%s\r", state_page, numSSIDofPage,ssid1, 
                                    ssid2);

            else if (numSSIDofPage == 1)
                sprintf(buffer_uart_tx,"%s\r%d\r%s\r", state_page, numSSIDofPage,ssid1);

            memset((void *)ssid1, '\0', sizeof(ssid1));
            memset((void *)ssid2, '\0', sizeof(ssid2));
            memset((void *)ssid3, '\0', sizeof(ssid3));
            memset((void *)ssid4, '\0', sizeof(ssid4));
            memset((void *)ssid5, '\0', sizeof(ssid5));
            printf("back: %s: %d\n",state_page, page);

            xEventGroupSetBits(event_uart_tx_heading,
                               SCAN_WIFI_BIT);
        }

        if (uxBits & OFF_WIFI_BIT)
        {
            esp_wifi_stop();
        }
    }
}

static void startWifiConnectTask(void *arg)
{
    while (1)
    {
        EventBits_t uxBits = xEventGroupWaitBits(event_uart_rx_heading,
                                                 CONNECT_WIFI_RX_BIT |
                                                     CONNECT_WIFI_SCAN_BIT,
                                                 pdTRUE, pdFALSE,
                                                 portMAX_DELAY);
        if (uxBits & CONNECT_WIFI_RX_BIT)
        {
            /*
                - Connect successful -> set bit SEND_CONNECT_WIFI_SUCCESSFUL
                                    -> store ssid & pass in nvs
                - Connect unsuccessful -> set bit SEND_CONNECT_WIFI_UNSUCCESSFUL
            */
            getSSID_PASS((uint8_t *)buffer_uart_rx, (uint8_t *)ssid, (uint8_t *)pass);

            if (WIFI_Connect((uint8_t *)ssid, (uint8_t *)pass) == CONNECT_OK)
            {
                if (flag_connect_success == 0)
                {
                    WIFI_StoreNVS((uint8_t *)ssid, (uint8_t *)pass);
                    flag_connect_success = 1;
                }

                strcpy(buffer_uart_tx, (const char *)ssid);
                printf("connect\n");
                xEventGroupSetBits(event_uart_tx_heading,
                                   SEND_CONNECT_WIFI_SUCCESSFUL_BIT);
            }
            else
            {
                strcpy(buffer_uart_tx, "FAILED");
                xEventGroupSetBits(event_uart_tx_heading,
                                   SEND_CONNECT_WIFI_UNSUCCESSFUL_BIT);
            }
        }

        if (uxBits & CONNECT_WIFI_SCAN_BIT)
        {
            if (WIFI_state_connect() == CONNECT_OK)
            {
                continue;
            }

            if (WIFI_Connect((uint8_t *)ssid, (uint8_t *)pass) == CONNECT_OK)
            {
                strcpy(buffer_uart_tx, (const char *)ssid);
                xEventGroupSetBits(event_uart_tx_heading,
                                   SEND_CONNECT_WIFI_SUCCESSFUL_BIT);
            }
        }
    }
}

static void startMQTTConnectTask(void *arg)
{
    while (1)
    {
        EventBits_t uxBits = xEventGroupWaitBits(event_uart_rx_heading,
                                                 CONNECT_MQTT_BIT,
                                                 pdTRUE, pdFALSE,
                                                 portMAX_DELAY);
        if (uxBits & CONNECT_MQTT_BIT)
        {
            /*
                If the system is connected to wifi, mqtt connection is allowed
                If the system is not connected to wifi, mqtt connection is not allowed
            */

            if (WIFI_state_connect() == CONNECT_OK)
            {
                char buffer_connect[4];
                printf("%s\n",buffer_uart_rx);
                sprintf(buffer_connect, "%.*s", strlen(buffer_uart_rx), buffer_uart_rx);
                configIP_MQTT(ip_ssid_connected, ip_config, buffer_connect);
                sprintf(url_mqtt, "mqtt://%.*s:1883", strlen(ip_config), ip_config);
                printf("%s\n", url_mqtt);
                MQTT_app_start(&mqtt_client_0, url_mqtt);
            }
            else
            {
                strcpy(buffer_uart_tx, "REFUSE");
                xEventGroupSetBits(event_uart_tx_heading,
                                   SEND_REFUSE_CONNECT_MQTT_BIT);
            }
        }
    }
}

static void startMQTTControlDataTask(void *arg)
{
    uint8_t buffer;

    while (1)
    {
        if (xQueueReceive(mqtt_queue, &buffer, portMAX_DELAY) == pdPASS)
        {
            if (WIFI_state_connect() != CONNECT_OK)
            {
                is_publish = 0;
                strcpy(buffer_uart_tx, "FAILED");
                xTimerStop(mqtt_subscribe_timer, 0);
                xEventGroupSetBits(event_uart_tx_heading,
                                   SEND_CONNECT_WIFI_UNSUCCESSFUL_BIT);
                continue;
            }

            if (MQTT_app_state_connect() != 1)
            {
                is_publish = 0;
                xTimerStop(mqtt_subscribe_timer, 0);
                
                xEventGroupSetBits(event_uart_tx_heading,
                                   SEND_CONNECT_MQTT_UNSUCCESSFUL_BIT);
                continue;
            }

            if (buffer == MQTT_PUBLISH)
            {
                char state_led[4];
                char state_auto[4];
                char lux[5];
                char temperature[10];

                processingDataMQTTPublish(buffer_uart_rx, state_led, state_auto, lux, temperature);

                esp_mqtt_client_publish(mqtt_client_0.client, "state_led", (const char *)&state_led, 0, 1, 0);
                esp_mqtt_client_publish(mqtt_client_0.client, "state_auto_stm32", (const char *)&state_auto, 0, 1, 0);
                esp_mqtt_client_publish(mqtt_client_0.client, "lux", (const char *)&lux, 0, 1, 0);
                esp_mqtt_client_publish(mqtt_client_0.client, "temperature_stm32", (const char *)&temperature, 0, 1, 0);
                
                if (is_publish == 0)
                {
                    xTimerStart(mqtt_subscribe_timer, 0);
                    is_publish = 1;
                }
            }
            else if (buffer == MQTT_SUBSCRIBE)
            {

                BaseType_t ret_val;
                MQTT_Topic_t mqtt_topic_command;

                esp_mqtt_client_subscribe_single(mqtt_client_0.client, "state_auto_nodered", 0);
                esp_mqtt_client_subscribe_single(mqtt_client_0.client, "led", 0);
                
                ret_val = xQueueReceive(mqtt_topic_queue, &mqtt_topic_command, 1000/ portTICK_PERIOD_MS);
               
                if (ret_val == pdTRUE)
                {
                    if (mqtt_topic_command == TOPIC_LED)
                    {
                        // strcpy(state_led, data_mqtt);
                        printf("%s\n", state_led);
                    }

                    if (mqtt_topic_command == TOPIC_AUTO)
                    {
                        // strcpy(state_auto_nodered, data_mqtt);
                        printf("%s\n", state_auto_nodered);
                    }
                    sprintf(buffer_uart_tx, "%s\r%s\r", state_led, state_auto_nodered);
                    xEventGroupSetBits(event_uart_tx_heading,
                                    SEND_MQTT_SUBSCRIBE);
                }
            }
        }
    }
}