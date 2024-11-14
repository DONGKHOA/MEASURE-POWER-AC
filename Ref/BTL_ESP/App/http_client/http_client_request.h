#ifndef APP_HTTP_CLIENT_H_
#define APP_HTTP_CLIENT_H_

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
    HTTP_REQUEST_OK,
    HTTP_REQUEST_FAIL,
}http_client_request_t;

char *WIFI_HTTP_Request();

#endif