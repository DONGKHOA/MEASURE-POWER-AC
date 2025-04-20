/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "app_data_trans.h"
#include "uart.h"
#include "freertos/queue.h"

#include <stdio.h>
#include <string.h>

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct process_trans_data
{
  QueueHandle_t   *p_data_trans_queue;
  float            f_power;
  volatile uint8_t u8_data_transmission[8];
  volatile uint8_t flag_update_power : 1;
} process_trans_data_t;

/******************************************************************************
 *  PRIVATE DEFINES
 ******************************************************************************/

#define FLAG_UPDATED     1
#define FLAG_NOT_UPDATED 0

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Data_trans_task(void *arg);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static process_trans_data_t s_process_trans_data;

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Data_Trans_CreateTask (void)
{
  xTaskCreate(APP_Data_trans_task, "data_trans", 1024 * 10, NULL, 11, NULL);
}

void
APP_Data_Trans_Init (void)
{
  s_process_trans_data.p_data_trans_queue = &s_data_system.s_data_trans_queue;
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Data_trans_task (void *arg)
{

  while (1)
  {
    /**
     * @details
     * - Check if the queue is not empty.
     * - Read and store 4 bytes of power data and append '\r' to
     *   u8_data_transmission array.
     */
    float value_to_send; // Giá trị nhận từ queue

    // Chờ dữ liệu từ hàng đợi với timeout 10ms
    if (xQueueReceive(*s_process_trans_data.p_data_trans_queue,
                      &value_to_send,
                      portMAX_DELAY))

    {
      uint32_t *p_val;
      p_val = (uint32_t *)&value_to_send;

      s_process_trans_data.u8_data_transmission[0] = (uint8_t)((*p_val >> 24));
      s_process_trans_data.u8_data_transmission[1] = (uint8_t)((*p_val >> 16));
      s_process_trans_data.u8_data_transmission[2] = (uint8_t)((*p_val >> 8));
      s_process_trans_data.u8_data_transmission[3] = (uint8_t)((*p_val >> 0));
      s_process_trans_data.u8_data_transmission[4] = '\r';
      // s_process_trans_data.flag_update_power       = FLAG_UPDATED;

      // printf("%f\r\n", value_to_send);
      // uint8_t len = strlen(s_process_trans_data.u8_data_transmission);
      uart_write_bytes(
          UART_NUM_2, (char *)s_process_trans_data.u8_data_transmission, 5);
    }
  }
}