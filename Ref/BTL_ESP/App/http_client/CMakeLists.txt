set(component_srcs "http_client_request.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "freertos" "lwip" "esp_wifi" nvs
                       )