/*
 * app_data_trans_rec.c
 *
 *  Created on: Nov 18, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_trans_rec.h"
#include "app_data.h"
#include "scheduler.h"
#include "ring_buffer.h"
#include "uart.h"
#include <string.h>

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define FLAG_UPDATED     1
#define FLAG_NOT_UPDATED 0

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct _Control_TaskContextTypedef_
{
  SCH_TASK_HANDLE         taskHandle;
  SCH_TaskPropertyTypedef taskProperty;
} Control_TaskContextTypedef;

typedef struct _DATA_TRANS_REC_T
{
  volatile ring_buffer_t *p_PF_buffer;
  volatile ring_buffer_t *p_vol_cur_buffer;
  volatile uart_cfg_t    *p_uart_data_trans_rec;
  volatile uint8_t        u8_data_transmission[16];
  volatile uint8_t        u8_data_rec[16];
  volatile uint8_t        u8_index_data_rec : 4;
  volatile uint8_t        flag_update_PF : 1;
  volatile uint8_t        flag_update_vol_cur : 1;
} DATA_TRANS_REC_T;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_DATA_TRANS_REC_TaskUpdate(void);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static DATA_TRANS_REC_T           s_data_trans_rec;
static Control_TaskContextTypedef s_ControlTaskContext
    = { SCH_INVALID_TASK_HANDLE, // Will be updated by Scheduler
        {
            SCH_TASK_SYNC,                // taskType;
            5,                            // taskPeriodInMS;
            APP_DATA_TRANS_REC_TaskUpdate // taskFunction;
        } };

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function APP_DATA_TRANS_REC_CreateTask creates a task using the
 * SCH_TASK_CreateTask function.
 */
void
APP_DATA_TRANS_REC_CreateTask (void)
{
  SCH_TASK_CreateTask(&s_ControlTaskContext.taskHandle,
                      &s_ControlTaskContext.taskProperty);
}

void
APP_DATA_TRANS_REC_Init (void)
{
  // Link pointer to variable
  s_data_trans_rec.p_PF_buffer           = &s_data_system.s_PF_buffer;
  s_data_trans_rec.p_vol_cur_buffer      = &s_data_system.s_vol_cur_buffer;
  s_data_trans_rec.p_uart_data_trans_rec = &s_data_system.s_uart_data_trans_rec;

  // Reset data filed of variable s_data_trans_rec
  s_data_trans_rec.flag_update_PF      = FLAG_NOT_UPDATED;
  s_data_trans_rec.flag_update_vol_cur = FLAG_NOT_UPDATED;
  s_data_trans_rec.u8_index_data_rec   = 0;
  memset((uint8_t *)s_data_trans_rec.u8_data_transmission,
         0,
         sizeof(s_data_trans_rec.u8_data_transmission));
  memset((uint8_t *)s_data_trans_rec.u8_data_rec,
         0,
         sizeof(s_data_trans_rec.u8_data_rec));

  // Config UART
  BSP_UART_Config((uart_cfg_t *)s_data_trans_rec.p_uart_data_trans_rec,
                  USART1,
                  USART1_IRQn);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

/**
 * @brief Handles data transmission and reception for voltage, current, and
 * power factor (PF).
 *
 * This function performs two main tasks:
 * 1. Constructs a transmission packet with VOLTAGE + '\r' + CURRENT + '\r' + PF
 * + '\r', and sends it via UART when data is ready.
 * 2. Receives data from UART, processes the received packet, and updates the
 * system's power value.
 */
static void
APP_DATA_TRANS_REC_TaskUpdate (void)
{
  /**
   * @details
   * - Check if the voltage and current buffer is not empty.
   * - Read 4 bytes of voltage data and append '\r'.
   * - Read 4 bytes of current data and append '\r'.
   */
  if (!RING_BUFFER_Is_Empty((ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer))
  {
    // Read voltage
    s_data_trans_rec.u8_data_transmission[0] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[1] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[2] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[3] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[4] = '\r';

    // Read current
    s_data_trans_rec.u8_data_transmission[5] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[6] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[7] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[8] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[9] = '\r';
    s_data_trans_rec.flag_update_vol_cur     = FLAG_UPDATED;
  }

  /**
   * @details
   * - Check if the power factor (PF) buffer is not empty.
   * - Read 4 bytes of PF data and append '\r'.
   */
  if (!RING_BUFFER_Is_Empty((ring_buffer_t *)s_data_trans_rec.p_PF_buffer))
  {
    // Read PF
    s_data_trans_rec.u8_data_transmission[10] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[11] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[12] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[13] = RING_BUFFER_Pull_Data(
        (ring_buffer_t *)s_data_trans_rec.p_vol_cur_buffer);
    s_data_trans_rec.u8_data_transmission[14] = '\r';
    s_data_trans_rec.flag_update_PF           = FLAG_UPDATED;
  }

  /**
   * @details
   * - If both voltage/current and PF data have been updated,
   *   transmit the combined data packet via UART.
   * - Reset the update flags after transmission.
   */
  if ((s_data_trans_rec.flag_update_PF == FLAG_UPDATED)
      && (s_data_trans_rec.flag_update_vol_cur == FLAG_UPDATED))
  {
    BSP_UART_SendString((uart_cfg_t *)s_data_trans_rec.p_uart_data_trans_rec,
                        (char *)s_data_trans_rec.u8_data_transmission);
    s_data_trans_rec.flag_update_PF      = FLAG_NOT_UPDATED;
    s_data_trans_rec.flag_update_vol_cur = FLAG_NOT_UPDATED;
  }

  /**
   * @details
   * - Check if UART has received new data.
   * - Store received data in a buffer.
   * - If a complete data packet (terminated by '\r') is received,
   *   process the packet and update the power value in the system structure.
   */
  if (BSP_UART_IsAvailableDataReceive(
          (uart_cfg_t *)s_data_trans_rec.p_uart_data_trans_rec))
  {
    s_data_trans_rec.u8_data_rec[s_data_trans_rec.u8_index_data_rec]
        = BSP_UART_ReadChar(
            (uart_cfg_t *)s_data_trans_rec.p_uart_data_trans_rec);

    s_data_trans_rec.u8_index_data_rec++;

    if (s_data_trans_rec.u8_data_rec[s_data_trans_rec.u8_index_data_rec - 1]
        == '\r')
    {
      // Convert the received 4 bytes to a float power value
      uint32_t *p_val;
      p_val  = (uint32_t *)&s_data_system.f_power;
      *p_val = (uint32_t)((s_data_trans_rec.u8_data_rec[0] << 24)
                          | (s_data_trans_rec.u8_data_rec[1] << 16)
                          | (s_data_trans_rec.u8_data_rec[2] << 8)
                          | (s_data_trans_rec.u8_data_rec[3] << 0));
      s_data_trans_rec.u8_index_data_rec = 0; // Reset index after processing
    }
  }
}
