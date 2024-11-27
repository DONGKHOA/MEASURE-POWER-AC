set(component_srcs)

idf_component_register(SRCS "smartconfig.c" "${component_srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "freertos" "lwip" "wpa_supplicant"
                       REQUIRES "esp_wifi" wifi
                       )