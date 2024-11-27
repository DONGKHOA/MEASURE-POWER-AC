#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds3231.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"

#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define DS3231_ADDRESS              (0x68)
#define SECOND_VALUE_ADDRESS        0x00

static gpio_num_t i2c_master_scl_io;
static gpio_num_t i2c_master_sda_io;

#define SCL_IO_PIN GPIO_NUM_19
#define SDA_IO_PIN GPIO_NUM_18

static const char *RTC_TAG = "I2C";

static uint8_t BCD_To_DEC(uint8_t p_BCD_value)
{
	return ((p_BCD_value >> 4) * 10) + (p_BCD_value & 0x0F);
}

static uint8_t DEC_To_BCD(uint8_t p_DEC_value)
{
	return ((p_DEC_value / 10) << 4) | (p_DEC_value % 10);
}

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_master_sda_io,
        .scl_io_num = i2c_master_scl_io,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 
                                I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void DS3231_Init(rtc_data_t *rtc_data, gpio_num_t scl_io, gpio_num_t sda_io)
{
    rtc_data_t realtime = 
    {
        .hour = 9,
        .minute = 55,
        .second = 0,
        .day = 1,
        .date = 31,
        .month = 7,
        .year = 23
    };
    i2c_master_sda_io = sda_io;
    i2c_master_scl_io = scl_io;
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(RTC_TAG, "I2C initialized successfully");

    DS3231_Register_Write(&realtime);

    // ESP_ERROR_CHECK(DS3231_Register_Write(&realtime));
}

esp_err_t DS3231_Register_Read(rtc_data_t *rtc_data)
{
    uint8_t reg_addr = SECOND_VALUE_ADDRESS;
    int8_t ret = i2c_master_write_read_device(I2C_MASTER_NUM, DS3231_ADDRESS, 
                &reg_addr, 1, (uint8_t *)rtc_data, 7, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    
    for (uint8_t i = 0; i < sizeof(rtc_data_t); i++)
    {
        *((uint8_t *)rtc_data + i) = BCD_To_DEC(*((uint8_t *)rtc_data + i));
    }
    return ret;
}

esp_err_t DS3231_Register_Write(rtc_data_t *rtc_data)
{
    int ret;
    uint8_t tx_buf[8] = {};
    for(uint8_t i = 0; i < sizeof(rtc_data_t); i++)
    {   
        tx_buf[i + 1] = DEC_To_BCD(*((uint8_t *)rtc_data + i));
    }
    tx_buf[0] = SECOND_VALUE_ADDRESS;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, DS3231_ADDRESS, tx_buf, 
            sizeof(tx_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}