idf_component_register(SRCS "app_wifi.c"
                       INCLUDE_DIRS "."
                       REQUIRES app_data wifi nvs smartconfig
                       )