idf_component_register(SRCS "app_rtc.c"
                       INCLUDE_DIRS "."
                       REQUIRES DS3231 app_data
                       )