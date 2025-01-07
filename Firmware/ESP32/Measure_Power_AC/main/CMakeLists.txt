idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs esp_event app_data_Rec app_data_trans app_process_data app_rtc app_button wifi app_status_led
                    PRIV_REQUIRES app_things_board
                    )
