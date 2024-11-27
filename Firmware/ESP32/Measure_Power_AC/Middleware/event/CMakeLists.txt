idf_component_register(SRCS "event.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "freertos" "lwip" "wpa_supplicant"
                       REQUIRES "esp_wifi"
                       )