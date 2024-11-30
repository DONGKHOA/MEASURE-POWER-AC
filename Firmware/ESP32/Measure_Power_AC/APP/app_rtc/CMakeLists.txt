idf_component_register(SRCS "app_rtc.c"
                       INCLUDE_DIRS "."
                       REQUIRES app_data DS3231
                       )