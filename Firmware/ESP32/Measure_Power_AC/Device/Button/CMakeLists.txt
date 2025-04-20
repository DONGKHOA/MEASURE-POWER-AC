set(component_srcs "button.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_timer exti app_data
                       )