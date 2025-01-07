/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "app_process_data.h"
#include <stdint.h>
#include "string.h"

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

#define SIZE_DATA_STORE_POWER_SEC   51
#define SIZE_DATA_STORE_POWER_MIN   61
#define SIZE_DATA_STORE_POWER_HOUR  61
#define SIZE_DATA_STORE_POWER_DAY   25
#define SIZE_DATA_STORE_POWER_WEEK  8
#define SIZE_DATA_STORE_POWER_MONTH 32
#define SIZE_DATA_STORE_POWER_YEAR  367

#define INDEX_LAST_DATA_STORE_POWER_SEC   (SIZE_DATA_STORE_POWER_SEC - 1)
#define INDEX_LAST_DATA_STORE_POWER_MIN   (SIZE_DATA_STORE_POWER_MIN - 1)
#define INDEX_LAST_DATA_STORE_POWER_HOUR  (SIZE_DATA_STORE_POWER_HOUR - 1)
#define INDEX_LAST_DATA_STORE_POWER_DAY   (SIZE_DATA_STORE_POWER_DAY - 1)
#define INDEX_LAST_DATA_STORE_POWER_WEEK  (SIZE_DATA_STORE_POWER_WEEK - 1)
#define INDEX_LAST_DATA_STORE_POWER_MONTH (SIZE_DATA_STORE_POWER_MONTH - 1)
#define INDEX_LAST_DATA_STORE_POWER_YEAR  (SIZE_DATA_STORE_POWER_YEAR - 1)

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct index_data
{
  uint16_t u16_index_year;
  uint8_t  u8_index_sec;
  uint8_t  u8_index_minute;
  uint8_t  u8_index_hour;
  uint8_t  u8_index_day;
  uint8_t  u8_index_week;
  uint8_t  u8_index_month;
} index_data_t;

typedef struct power_data
{
  float f_power_year[SIZE_DATA_STORE_POWER_YEAR];
  float f_power_minute[SIZE_DATA_STORE_POWER_MIN];
  float f_power_hour[SIZE_DATA_STORE_POWER_HOUR];
  float f_power_sec[SIZE_DATA_STORE_POWER_SEC];
  float f_power_day[SIZE_DATA_STORE_POWER_DAY];
  float f_power_month[SIZE_DATA_STORE_POWER_MONTH];
  float f_power_week[SIZE_DATA_STORE_POWER_WEEK];
} power_data_t;

typedef struct energy_data
{
  float *p_energy;
  float *p_energy_min;
  float *p_energy_hour;
  float *p_energy_day;
  float *p_energy_week;
  float *p_energy_month;
  float *p_energy_year;
} energy_data_t;

typedef struct process_data
{
  power_data_t        s_power_data;
  index_data_t        s_index_data;
  energy_data_t       s_energy_data;
  flag_enable_t      *p_flag_enable;
  EventGroupHandle_t *p_flag_mqtt_event;
  EventGroupHandle_t *p_flag_time_event;
  QueueHandle_t      *p_data_rec_queue;
  QueueHandle_t      *p_data_trans_queue;
  float              *p_power;
  float               f_current;
  float               f_voltage;
  float               f_power_factor;
} process_data_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Process_data_task(void *arg);

static inline void APP_Process_data_Calculate_Power(void);
static inline void APP_Process_data_Calculate_Power_Average_Sec(void);
static inline void APP_Process_data_Calculate_Energy_Sec(void);

static void APP_Process_data_Calculate_Energy(void);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static process_data_t s_process_data;
static float          f_sum;

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Process_data_CreateTask (void)
{
  xTaskCreate(APP_Process_data_task, "process_data", 1024 * 40, NULL, 8, NULL);
}

void
APP_Process_data_Init (void)
{
  // Link pointer to variable
  s_process_data.p_data_rec_queue   = &s_data_system.s_data_rec_queue;
  s_process_data.p_data_trans_queue = &s_data_system.s_data_trans_queue;
  s_process_data.p_power            = &s_data_system.f_power;
  s_process_data.p_flag_mqtt_event  = &s_data_system.s_flag_mqtt_event;
  s_process_data.p_flag_time_event  = &s_data_system.s_flag_time_event;

  s_process_data.s_energy_data.p_energy       = &s_data_system.f_energy;
  s_process_data.s_energy_data.p_energy_min   = &s_data_system.f_energy_min;
  s_process_data.s_energy_data.p_energy_hour  = &s_data_system.f_energy_hour;
  s_process_data.s_energy_data.p_energy_day   = &s_data_system.f_energy_day;
  s_process_data.s_energy_data.p_energy_week  = &s_data_system.f_energy_week;
  s_process_data.s_energy_data.p_energy_month = &s_data_system.f_energy_month;
  s_process_data.s_energy_data.p_energy_year  = &s_data_system.f_energy_year;

  s_process_data.p_flag_enable = &s_data_system.s_flag_enable;

  s_process_data.f_current                    = 0;
  s_process_data.f_voltage                    = 0;
  s_process_data.f_power_factor               = 0;
  s_process_data.s_index_data.u8_index_sec    = 0;
  s_process_data.s_index_data.u8_index_minute = 0;
  s_process_data.s_index_data.u8_index_hour   = 0;
  s_process_data.s_index_data.u8_index_day    = 0;
  s_process_data.s_index_data.u8_index_week   = 0;
  s_process_data.s_index_data.u8_index_month  = 0;
  s_process_data.s_index_data.u16_index_year  = 0;

  memset(s_process_data.s_power_data.f_power_sec,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_sec));

  memset(s_process_data.s_power_data.f_power_minute,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_minute));
  memset(s_process_data.s_power_data.f_power_hour,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_hour));
  memset(s_process_data.s_power_data.f_power_day,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_day));
  memset(s_process_data.s_power_data.f_power_week,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_week));
  memset(s_process_data.s_power_data.f_power_month,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_month));
  memset(s_process_data.s_power_data.f_power_year,
         0.0,
         sizeof(s_process_data.s_power_data.f_power_year));
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Process_data_task (void *arg)
{
  float   f_temp_value = 0;
  uint8_t u8_count     = 0;
  
  while (1)
  {
    if (xQueueReceive(
            *s_process_data.p_data_rec_queue, &f_temp_value, portMAX_DELAY))
    {
      if (u8_count == 0)
      {
        s_process_data.f_voltage = f_temp_value;
      }
      else if (u8_count == 1)
      {
        s_process_data.f_current = f_temp_value;
      }
      else if (u8_count == 2)
      {
        s_process_data.f_power_factor = f_temp_value;
        APP_Process_data_Calculate_Power();

        // Send power to data trans
        xQueueSend(
            *s_process_data.p_data_trans_queue, s_process_data.p_power, 0);
        // printf("Power: %f\n", *s_process_data.p_power);

        // Calculate energy
        APP_Process_data_Calculate_Energy();

        // Reset Count
        u8_count = 0xFF;
      }
      u8_count++;
    }
  }
}

static void
APP_Process_data_Calculate_Energy (void)
{

  EventBits_t uxBits = xEventGroupWaitBits(
      *s_process_data.p_flag_time_event,
      BIT_FLAG_TIME_SEC | BIT_FLAG_TIME_MIN | BIT_FLAG_TIME_HOUR
          | BIT_FLAG_TIME_DAY | BIT_FLAG_TIME_WEEK | BIT_FLAG_TIME_MONTH
          | BIT_FLAG_TIME_YEAR,
      pdTRUE,  // Erase bit after processed
      pdFALSE, // Wait for all bit
      0);     // No time out

  s_process_data.s_power_data
      .f_power_sec[s_process_data.s_index_data.u8_index_sec]
      = *s_process_data.p_power;

  if (s_process_data.s_index_data.u8_index_sec < 49)
  {
    s_process_data.s_index_data.u8_index_sec++;
    return;
  }

  if (uxBits & BIT_FLAG_TIME_YEAR)
  {
    *s_process_data.s_energy_data.p_energy_year = 0;
  }

  if (uxBits & BIT_FLAG_TIME_MONTH)
  {
    *s_process_data.s_energy_data.p_energy_month = 0;
  }

  if (uxBits & BIT_FLAG_TIME_DAY)
  {
    *s_process_data.s_energy_data.p_energy_day = 0;
  }

  if (uxBits & BIT_FLAG_TIME_HOUR)
  {
    *s_process_data.s_energy_data.p_energy_hour = 0;
  }

  if (uxBits & BIT_FLAG_TIME_MIN)
  {
    *s_process_data.s_energy_data.p_energy_min = 0;
  }

  if (uxBits & BIT_FLAG_TIME_SEC)
  {
    APP_Process_data_Calculate_Power_Average_Sec();
    APP_Process_data_Calculate_Energy_Sec();

    *s_process_data.s_energy_data.p_energy_min
        += *s_process_data.s_energy_data.p_energy;
    *s_process_data.s_energy_data.p_energy_hour
        += *s_process_data.s_energy_data.p_energy;
    *s_process_data.s_energy_data.p_energy_day
        += *s_process_data.s_energy_data.p_energy;
    *s_process_data.s_energy_data.p_energy_month
        += *s_process_data.s_energy_data.p_energy;
    *s_process_data.s_energy_data.p_energy_year
        += *s_process_data.s_energy_data.p_energy;

    s_process_data.s_index_data.u8_index_sec = 0;
    memset(s_process_data.s_power_data.f_power_sec,
           0.0,
           sizeof(s_process_data.s_power_data.f_power_sec));
  }

    EventBits_t bits = xEventGroupSetBits(
        *s_process_data.p_flag_mqtt_event,
        (BIT_SEND_ENERGY) | (BIT_SEND_ENERGY_HOUR) | (BIT_SEND_ENERGY_DAY)
            | (BIT_SEND_ENERGY_MONTH) | (BIT_SEND_ENERGY_YEAR));
}

static inline void
APP_Process_data_Calculate_Energy_Sec (void)
{
  *s_process_data.s_energy_data.p_energy
      = s_process_data.s_power_data.f_power_sec[INDEX_LAST_DATA_STORE_POWER_SEC]
        / (3600 * 1000);
}

static inline void
APP_Process_data_Calculate_Power (void)
{
  *s_process_data.p_power = (s_process_data.f_voltage * s_process_data.f_current
                             * s_process_data.f_power_factor)
                            / 2;
}

static inline void
APP_Process_data_Calculate_Power_Average_Sec (void)
{
  f_sum = 0;

  if (s_process_data.s_index_data.u8_index_sec == 0)
  {
    return;
  }

  for (uint8_t i = 0; i < INDEX_LAST_DATA_STORE_POWER_SEC; i++)
  {
    f_sum += s_process_data.s_power_data.f_power_sec[i];
  }

  s_process_data.s_power_data.f_power_sec[INDEX_LAST_DATA_STORE_POWER_SEC]
      = f_sum / INDEX_LAST_DATA_STORE_POWER_SEC;
}
