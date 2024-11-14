idf_component_register(SRCS "mqtt_client_tcp.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "freertos" "lwip" "esp_wifi" nvs "mqtt"
                       )