/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "ds3231.h"
#include "driver/i2c.h"
#include "driver/i2c_master.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define I2C_MASTER_NUM            0
#define I2C_MASTER_FREQ_HZ        400000 /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0      /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0      /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS     1000

#define DS3231_ADDRESS       (0x68)
#define SECOND_VALUE_ADDRESS 0x00

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static uint8_t   DEC_To_BCD(uint8_t u8_DEC_value);
static uint8_t   BCD_To_DEC(uint8_t u8_BCD_value);
static esp_err_t i2c_master_init(gpio_num_t e_scl_io, gpio_num_t e_sda_io);

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
DS3231_Init (ds3231_data_t *p_ds3231_data, gpio_num_t e_scl_io, gpio_num_t e_sda_io)
{
  ds3231_data_t realtime = { .u8_hour   = 9,
                             .u8_minute = 55,
                             .u8_second = 0,
                             .u8_day    = 1,
                             .u8_date   = 31,
                             .u8_month  = 7,
                             .u8_year   = 23 };

  i2c_master_init(e_scl_io, e_sda_io);

  DS3231_Register_Write(&realtime);
}

esp_err_t
DS3231_Register_Read (ds3231_data_t *p_ds3231_data)
{
  uint8_t reg_addr = SECOND_VALUE_ADDRESS;
  int8_t  ret      = i2c_master_write_read_device(I2C_MASTER_NUM,
                                            DS3231_ADDRESS,
                                            &reg_addr,
                                            1,
                                            (uint8_t *)p_ds3231_data,
                                            7,
                                            I2C_MASTER_TIMEOUT_MS
                                                / portTICK_PERIOD_MS);

  for (uint8_t i = 0; i < sizeof(ds3231_data_t); i++)
  {
    *((uint8_t *)p_ds3231_data + i) = BCD_To_DEC(*((uint8_t *)p_ds3231_data + i));
  }
  return ret;
}

esp_err_t
DS3231_Register_Write (ds3231_data_t *p_ds3231_data)
{
  int     ret;
  uint8_t tx_buf[8] = {};
  for (uint8_t i = 0; i < sizeof(ds3231_data_t); i++)
  {
    tx_buf[i + 1] = DEC_To_BCD(*((uint8_t *)p_ds3231_data + i));
  }
  tx_buf[0] = SECOND_VALUE_ADDRESS;
  ret       = i2c_master_write_to_device(I2C_MASTER_NUM,
                                   DS3231_ADDRESS,
                                   tx_buf,
                                   sizeof(tx_buf),
                                   I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

  return ret;
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static uint8_t
BCD_To_DEC (uint8_t u8_BCD_value)
{
  return ((u8_BCD_value >> 4) * 10) + (u8_BCD_value & 0x0F);
}

static uint8_t
DEC_To_BCD (uint8_t u8_DEC_value)
{
  return ((u8_DEC_value / 10) << 4) | (u8_DEC_value % 10);
}

static esp_err_t
i2c_master_init (gpio_num_t e_scl_io, gpio_num_t e_sda_io)
{
  int i2c_master_port = I2C_MASTER_NUM;

  i2c_config_t conf = {
    .mode             = I2C_MODE_MASTER,
    .sda_io_num       = e_sda_io,
    .scl_io_num       = e_scl_io,
    .sda_pullup_en    = GPIO_PULLUP_ENABLE,
    .scl_pullup_en    = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  i2c_param_config(i2c_master_port, &conf);

  return i2c_driver_install(i2c_master_port,
                            conf.mode,
                            I2C_MASTER_RX_BUF_DISABLE,
                            I2C_MASTER_TX_BUF_DISABLE,
                            0);
}