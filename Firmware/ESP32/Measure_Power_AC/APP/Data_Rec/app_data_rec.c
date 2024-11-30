/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "app_data_rec.h"

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct process_data
{
  QueueHandle_t *p_data_rec_queue;
  float          f_current;
  float          f_voltage;
  float          f_power_factor;
} process_data_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Data_rec_task(void *arg);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static process_data_t s_process_data;

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Data_rec_CreateTask (void)
{
  xTaskCreate(APP_Data_rec_task, "data_rec", 1024 * 10, NULL, 8, NULL);
}

void
APP_Data_rec_Init (void)
{
  s_process_data.p_data_rec_queue = &s_data_system.s_data_rec_queue;
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Data_rec_task (void *arg)
{
  s_process_data.f_voltage      = 310.1;
  s_process_data.f_current      = 20.9;
  s_process_data.f_power_factor = 1;
  while (1)
  {
    xQueueSend(*s_process_data.p_data_rec_queue, &s_process_data.f_voltage, 0);
    xQueueSend(*s_process_data.p_data_rec_queue, &s_process_data.f_current, 0);
    xQueueSend(
        *s_process_data.p_data_rec_queue, &s_process_data.f_power_factor, 0);

    vTaskDelay(20);

    s_process_data.f_current = 3.5;
    xQueueSend(*s_process_data.p_data_rec_queue, &s_process_data.f_voltage, 0);
    xQueueSend(*s_process_data.p_data_rec_queue, &s_process_data.f_current, 0);
    xQueueSend(
        *s_process_data.p_data_rec_queue, &s_process_data.f_power_factor, 0);

    vTaskDelay(20);

    s_process_data.f_current = 25.1;
    xQueueSend(*s_process_data.p_data_rec_queue, &s_process_data.f_voltage, 0);
    xQueueSend(*s_process_data.p_data_rec_queue, &s_process_data.f_current, 0);
    xQueueSend(
        *s_process_data.p_data_rec_queue, &s_process_data.f_power_factor, 0);

    vTaskDelay(20);
  }
}