idf_component_register(SRCS "app_button.c"
                       INCLUDE_DIRS "."
                       REQUIRES app_data gpio Button nvs wifi
                       )