/*********************
 *      INCLUDES
 *********************/

#include "http_client_request.h"

#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

/*********************
 *      DEFINES
 *********************/

#define WEB_SERVER  "api.openweathermap.org"
#define WEB_PORT    "80"

static char REQUEST[512];
static const char *TAG = "http request";
char *data_http;

static uint8_t http_request()
{
    const struct addrinfo hints = 
    {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];
    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) 
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return HTTP_REQUEST_FAIL;
    }

    /* Code to print the resolved IP.

        Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) 
    {
        ESP_LOGE(TAG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        return HTTP_REQUEST_FAIL;
    }
    ESP_LOGI(TAG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        return HTTP_REQUEST_FAIL;
    }

    ESP_LOGI(TAG, "... connected");
    freeaddrinfo(res);
    sprintf(REQUEST, "GET http://api.openweathermap.org/data_http/2.5/weather?q=Thanh%%20pho%%20Ho%%20Chi%%20Minh&appid=a5b16100ee019b826c6a7e22902d14e5\n\n");
    if (write(s, REQUEST, strlen(REQUEST)) < 0) 
    {
        ESP_LOGE(TAG, "... socket send failed");
        close(s);
        return HTTP_REQUEST_FAIL;
    }
    ESP_LOGI(TAG, "... socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) 
    {
        ESP_LOGE(TAG, "... failed to set socket receiving timeout");
        close(s);
        return HTTP_REQUEST_FAIL;
    }
    ESP_LOGI(TAG, "... set socket receiving timeout success");

    /* Read HTTP response */
    uint16_t count = 0;
    uint8_t i;
    do {
        bzero(recv_buf, 64);
        r = read(s, recv_buf, 64-1);
        for(i = 0; i < r; i++) 
        {
            *(data_http + i + count) = recv_buf[i];
        }
        count += i;
    } while(r > 0);
    *(data_http + i + count) = '\0';

    ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);
    ESP_LOGI(TAG, "Starting again!");
    return HTTP_REQUEST_OK;
}

char *WIFI_HTTP_Request()
{
    data_http = (char *)calloc(1024, 1);
    if (data_http == NULL)
    {
      printf("Unable to allocate memory.\n");
      exit(1);
    }
    if (http_request() == HTTP_REQUEST_OK)
    {
        return data_http;
    }
    free(data_http);   
    return NULL;
}