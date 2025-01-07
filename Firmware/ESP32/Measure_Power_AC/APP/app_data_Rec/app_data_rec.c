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

  /******************************************************************************
   *   PUBLIC FUNCTION
   *****************************************************************************/

void
APP_Data_rec_CreateTask (void)
{
  xTaskCreate(APP_Data_rec_task, "data_rec", 1024 * 10, NULL, 9, NULL);
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
  uart_event_t event;
  while (1)
  {
    if (xQueueReceive(uart_queue, &event, portMAX_DELAY))
    {
      switch (event.type)
      {
        case UART_DATA:
          int Bytes = uart_read_bytes(
              UART_NUM_2,
              &s_process_rec_data
                   .u8_data_rec[s_process_rec_data.u8_index_data_rec],
              event.size,
              pdMS_TO_TICKS(10));

          s_process_rec_data.u8_index_data_rec += Bytes;

          if (Bytes > 0)
          {
            for (int i = 0; i < s_process_rec_data.u8_index_data_rec; i++)
            {
              if (s_process_rec_data.u8_data_rec[i] == '\r')
              {
                if (i >= 4)
                {
                  // Convert data from string to float
                  uint32_t *p_val;
                  p_val = (uint32_t *)&s_process_rec_data.received_data;
                  *p_val
                      = (uint32_t)((s_process_rec_data.u8_data_rec[0] << 24)
                                   | (s_process_rec_data.u8_data_rec[1] << 16)
                                   | (s_process_rec_data.u8_data_rec[2] << 8)
                                   | (s_process_rec_data.u8_data_rec[3] << 0));

                  // printf("Received data: %f\n",
                  //        s_process_rec_data.received_data);

                  xQueueSend(*s_process_rec_data.p_data_rec_queue,
                             &s_process_rec_data.received_data,
                             0);
                }

                // Move remaining data to the beginning of the buffer
                int remaining_bytes
                    = s_process_rec_data.u8_index_data_rec - (i + 1);
                for (int j = 0; j < remaining_bytes; j++)
                {
                  s_process_rec_data.u8_data_rec[j]
                      = s_process_rec_data.u8_data_rec[i + 1 + j];
                }

                // Update index
                s_process_rec_data.u8_index_data_rec = remaining_bytes;
                i                                    = 0; // Reset i
              }
            }
          }
          break;

        case UART_FIFO_OVF: // FIFO Overflow
          printf("UART FIFO Overflow\n");
          uart_flush_input(UART_NUM_2); // Delete data in FIFO
          xQueueReset(uart_queue);      // Reset queue
          break;

        case UART_BUFFER_FULL:
          printf("Ring Buffer Full\n");
          uart_flush_input(UART_NUM_2);
          xQueueReset(uart_queue);
          break;

        case UART_PARITY_ERR:
          printf("UART Parity Error\n");
          break;

        case UART_FRAME_ERR:
          printf("UART Frame Error\n");
          break;

        default:
          printf("Unknown UART event type: %d\n", event.type);
          break;
      }
    }
  }
}