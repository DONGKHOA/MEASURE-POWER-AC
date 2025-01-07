/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_rtc.h"
#include "app_data_ESP32.h"
#include "ds3231.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct data_rtc_apt
{
  flag_enable_t      *p_flag_enable;
  EventGroupHandle_t *p_flag_time_event;
  ds3231_data_t       s_ds3231_data;
  ds3231_data_t       s_ds3231_predata;
} data_rtc_apt_t;

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static data_rtc_apt_t s_data_rtc_apt;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Rtc_task(void *arg);

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Rtc_CreateTask (void)
{
  xTaskCreate(APP_Rtc_task, "data_rtc", 1024 * 4, NULL, 7, NULL);
}

void
APP_Rtc_Init (void)
{
  s_data_rtc_apt.p_flag_enable     = &s_data_system.s_flag_enable;
  s_data_rtc_apt.p_flag_time_event = &s_data_system.s_flag_time_event;

  DS3231_Init(&s_data_rtc_apt.s_ds3231_data, GPIO_NUM_19, GPIO_NUM_18);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Rtc_task (void *arg)
{
  DS3231_Register_Read(&s_data_rtc_apt.s_ds3231_data);

  s_data_rtc_apt.s_ds3231_predata = s_data_rtc_apt.s_ds3231_data;

  while (1)
  {
    DS3231_Register_Read(&s_data_rtc_apt.s_ds3231_data);

    if (s_data_rtc_apt.s_ds3231_predata.u8_second
        != s_data_rtc_apt.s_ds3231_data.u8_second)
    {
      xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_SEC);
    }

    if (s_data_rtc_apt.s_ds3231_predata.u8_minute
        != s_data_rtc_apt.s_ds3231_data.u8_minute)
    {
      xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_MIN);
    }

    if (s_data_rtc_apt.s_ds3231_predata.u8_hour
        != s_data_rtc_apt.s_ds3231_data.u8_hour)
    {
      xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_HOUR);
    }

    if (s_data_rtc_apt.s_ds3231_predata.u8_day
        != s_data_rtc_apt.s_ds3231_data.u8_day)
    {
      xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_DAY);
    }

    // if (s_data_rtc_apt.s_ds3231_predata.u8_date
    //     != s_data_rtc_apt.s_ds3231_data.u8_date)
    // {
    //   xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_DATE);
    // }

    if (s_data_rtc_apt.s_ds3231_predata.u8_month
        != s_data_rtc_apt.s_ds3231_data.u8_month)
    {
      xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_MONTH);
    }

    if (s_data_rtc_apt.s_ds3231_predata.u8_year
        != s_data_rtc_apt.s_ds3231_data.u8_year)
    {
      xEventGroupSetBits(*s_data_rtc_apt.p_flag_time_event, BIT_FLAG_TIME_YEAR);
    }

    s_data_rtc_apt.s_ds3231_predata = s_data_rtc_apt.s_ds3231_data;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}