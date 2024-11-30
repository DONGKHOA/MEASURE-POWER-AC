#ifndef DATA_APP_DATA_ESP32_H_
#define DATA_APP_DATA_ESP32_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC DEFINES
   ***************************************************************************/

#define BIT_SEND_ENERGY       (1 << 0)
#define BIT_SEND_ENERGY_HOUR  (1 << 1)
#define BIT_SEND_ENERGY_DAY   (1 << 2)
#define BIT_SEND_ENERGY_WEEK  (1 << 3)
#define BIT_SEND_ENERGY_MONTH (1 << 4)
#define BIT_SEND_ENERGY_YEAR  (1 << 5)

  /****************************************************************************
   *   PUBLIC TYPEDEFS
   ***************************************************************************/

  typedef struct _DATA_READ_DataTypedef
  {
    uint8_t            u8_ssid[32];
    uint8_t            u8_pass[32];
    EventGroupHandle_t s_flag_mqtt_event;
    QueueHandle_t      s_data_rec_queue;   // float
    QueueHandle_t      s_data_trans_queue; // float
    float              f_power;
    float              f_energy;
    float              f_energy_min;
    float              f_energy_hour;
    float              f_energy_day;
    float              f_energy_week;
    float              f_energy_month;
    float              f_energy_year;
  } DATA_READ_DataTypedef;

  /****************************************************************************
   *   PUBLIC DATA
   ***************************************************************************/

  extern DATA_READ_DataTypedef s_data_system;

#ifdef __cplusplus
}
#endif

#endif /* DATA_APP_DATA_ESP32_H_ */
