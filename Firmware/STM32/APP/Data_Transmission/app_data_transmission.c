/*
 * app_data_transmission.c
 *
 *  Created on: Nov 18, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_transmission.h"
#include "app_data.h"
#include "scheduler.h"
#include "ring_buffer.h"
#include "uart.h"

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

typedef struct _DATA_TRANSMISSION_T
{
  volatile ring_buffer_t *p_PF_buffer;
  volatile ring_buffer_t *p_vol_cur_buffer;
  volatile uart_cfg_t    *p_uart_data_trans;
  volatile uint8_t        u8_delta_T[10];
  volatile uint8_t        u8_voltage[7];
  volatile uint8_t        u8_current[6];
  volatile uint8_t        flag_update_PF : 1;
  volatile uint8_t        flag_update_vol_cur : 1;
} DATA_TRANSMISSION_T;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_DATA_TRANSMISSION_TaskUpdate(void);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static DATA_TRANSMISSION_T        s_data_trans;
static Control_TaskContextTypedef s_ControlTaskContext
    = { SCH_INVALID_TASK_HANDLE, // Will be updated by Scheduler
        {
            SCH_TASK_SYNC,                   // taskType;
            1,                               // taskPeriodInMS;
            APP_DATA_TRANSMISSION_TaskUpdate // taskFunction;
        } };

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function APP_DATA_TRANSMISSION_CreateTask creates a task using the
 * SCH_TASK_CreateTask function.
 */
void
APP_DATA_TRANSMISSION_CreateTask (void)
{
  SCH_TASK_CreateTask(&s_ControlTaskContext.taskHandle,
                      &s_ControlTaskContext.taskProperty);
}

void
APP_DATA_TRANSMISSION_Init (void)
{
  // Link pointer to variable
  s_data_trans.p_PF_buffer       = &s_data_system.s_PF_buffer;
  s_data_trans.p_vol_cur_buffer  = &s_data_system.s_vol_cur_buffer;
  s_data_trans.p_uart_data_trans = &s_data_system.s_uart_data_trans;

  // Reset data filed of variable s_data_trans
  s_data_trans.flag_update_PF      = FLAG_NOT_UPDATED;
  s_data_trans.flag_update_vol_cur = FLAG_NOT_UPDATED;

  // Config UART
  BSP_UART_Config(
      (uart_cfg_t *)s_data_trans.p_uart_data_trans, USART1, USART1_IRQn);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_DATA_TRANSMISSION_TaskUpdate (void)
{
  if (!RING_BUFFER_Is_Empty((ring_buffer_t *)s_data_trans.p_PF_buffer))
  {

    s_data_trans.flag_update_PF = FLAG_UPDATED;
  }

  if (!RING_BUFFER_Is_Empty((ring_buffer_t *)s_data_trans.p_vol_cur_buffer))
  {

    s_data_trans.flag_update_vol_cur = FLAG_UPDATED;
  }

  if ((s_data_trans.flag_update_PF == FLAG_UPDATED)
      && (s_data_trans.flag_update_vol_cur == FLAG_UPDATED))
  {
    s_data_trans.flag_update_PF      = FLAG_NOT_UPDATED;
    s_data_trans.flag_update_vol_cur = FLAG_NOT_UPDATED;
  }
}
