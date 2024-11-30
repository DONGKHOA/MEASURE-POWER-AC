idf_component_register(SRCS "app_smart_config.c"
                       INCLUDE_DIRS "."
                       REQUIRES app_data smartconfig wifi nvs event
                       )