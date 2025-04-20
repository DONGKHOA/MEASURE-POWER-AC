/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "app_data_rec.h"
#include "uart.h"
#include "freertos/queue.h"

#include <stdio.h>
#include <string.h>

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/
typedef enum state_rec_data
{
  STATE_HEADING = 0,
  STATE_DATA,
} state_rec_data_t;

typedef struct process_data
{
  QueueHandle_t *p_data_rec_queue;
  float          received_data;
  uint8_t        u8_data_rec[16];
  uint8_t        u8_index_data_rec : 4;
} process_rec_data_t;

/******************************************************************************
 *  PRIVATE DEFINE
 ******************************************************************************/

#define MAX_DATA_TYPES 3

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Data_rec_task(void *arg);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static process_rec_data_t s_process_rec_data;

extern QueueHandle_t uart_queue; // UART Queue
state_rec_data_t     e_state_data = STATE_HEADING;

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Data_rec_CreateTask (void)
{
  xTaskCreate(APP_Data_rec_task, "data_rec", 1024 * 10, NULL, 11, NULL);
}

void
APP_Data_rec_Init (void)
{
  s_process_rec_data.p_data_rec_queue = &s_data_system.s_data_rec_queue;
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Data_rec_task (void *arg)
{
  uint8_t count = 0;
  uint8_t array[4];
  int     i     = 0;
  int     Bytes = 0;
  while (1)
  {
    switch (e_state_data)
    {
      case STATE_HEADING:

        Bytes = uart_read_bytes(
            UART_NUM_2, &s_process_rec_data.u8_data_rec, 1, pdMS_TO_TICKS(1));

        if (Bytes > 0)
        {
          if (s_process_rec_data.u8_data_rec[0] == 0xaa)
          {
            count++;
          }
          else
          {
            if (count > 0)
            {
              count--;
            }
          }

          if (count == 5)
          {
            count        = 0;
            e_state_data = STATE_DATA;
          }
          Bytes = 0;
        }
        break;

      case STATE_DATA:
        Bytes = uart_read_bytes(
            UART_NUM_2, &s_process_rec_data.u8_data_rec, 1, pdMS_TO_TICKS(1));
        if (Bytes > 0)
        {

          if (s_process_rec_data.u8_data_rec[0] == 0xdd)
          {

            count++;
            i = 0;
            // Convert data from string to float
            uint32_t *p_val;
            p_val  = (uint32_t *)&s_process_rec_data.received_data;
            *p_val = (uint32_t)((array[0] << 24) | (array[1] << 16)
                                | (array[2] << 8) | (array[3] << 0));

            xQueueSend(*s_process_rec_data.p_data_rec_queue,
                       &s_process_rec_data.received_data,
                       0);
            goto abc;
          }
          array[i] = s_process_rec_data.u8_data_rec[0];
          i++;
        abc:
          if (count == 3)
          {
            count        = 0;
            e_state_data = STATE_HEADING;
          }
          Bytes = 0;
        }

        break;

      default:
        break;
    }
  }
}