#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_log.h"

typedef struct 
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}__attribute__((packed))rtc_data_t;


void DS3231_Init(rtc_data_t *rtc_data, gpio_num_t scl_io, gpio_num_t sda_io);
esp_err_t DS3231_Register_Read(rtc_data_t *rtc_data);
esp_err_t DS3231_Register_Write(rtc_data_t *rtc_data);

#endif /* DS3231_H*/