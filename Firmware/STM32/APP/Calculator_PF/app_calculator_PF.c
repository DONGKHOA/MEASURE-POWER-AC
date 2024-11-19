/*
 * app_calculator_PF.c
 *
 *  Created on: Nov 17, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx_ll_exti.h"
#include "app_calculator_PF.h"
#include "app_data.h"
#include "ring_buffer.h"
#include "scheduler.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define START_COUNTER(x)   \
  LL_TIM_SetCounter(x, 0); \
  LL_TIM_EnableCounter(x);

#define FREQUENCY_TIMER  56.0 // Mhz

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct _Control_TaskContextTypedef_
{
  SCH_TASK_HANDLE         taskHandle;
  SCH_TaskPropertyTypedef taskProperty;
} Control_TaskContextTypedef;

typedef struct _CALCULATOR_PF_t
{
  volatile ring_buffer_t *p_PF_buffer_irq;
  volatile ring_buffer_t *p_PF_buffer;
  volatile uint8_t        value_temp_irq[2];
  volatile float *p_delta_T;
} CALCULATOR_PF_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_CALCULATOR_PF_TaskUpdate(void);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static CALCULATOR_PF_t            s_calculator_pf;
static Control_TaskContextTypedef s_ControlTaskContext
    = { SCH_INVALID_TASK_HANDLE, // Will be updated by Scheduler
        {
            SCH_TASK_SYNC,               // taskType;
            1,                           // taskPeriodInMS;
            APP_CALCULATOR_PF_TaskUpdate // taskFunction;
        } };

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function APP_CALCULATOR_PF_CreateTask creates a task using the
 * SCH_TASK_CreateTask function.
 */
void
APP_CALCULATOR_PF_CreateTask (void)
{
  SCH_TASK_CreateTask(&s_ControlTaskContext.taskHandle,
                      &s_ControlTaskContext.taskProperty);
}

/**
 * The function initializes buffers for a calculator application in C.
 */
void
APP_CALCULATOR_PF_Init (void)
{
  // Link pointer to variable
  s_calculator_pf.p_PF_buffer     = &s_data_system.s_PF_buffer;
  s_calculator_pf.p_PF_buffer_irq = &s_data_system.s_PF_buffer_irq;
  s_calculator_pf.p_delta_T = &s_data_system.f_delta_T_PF;

  // Initialize buffer
  RING_BUFFER_Init((ring_buffer_t *)s_calculator_pf.p_PF_buffer);
  RING_BUFFER_Init((ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq);
}

/**
 * The function `APP_CALCULATOR_PF_EXTI_IRQHandler` handles EXTI interrupts for
 * lines 5 and 6, updating a buffer with timer values.
 */
void
APP_CALCULATOR_PF_EXTI_IRQHandler (void)
{
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    if (LL_TIM_IsEnabledCounter(TIM2))
    {
      LL_TIM_DisableCounter(TIM2);
      s_calculator_pf.value_temp_irq[0]
          = (uint8_t)(LL_TIM_GetCounter(TIM2) >> 8);
      s_calculator_pf.value_temp_irq[1] = (uint8_t)LL_TIM_GetCounter(TIM2);

      RING_BUFFER_Push_Data((ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq,
                            s_calculator_pf.value_temp_irq[0]);
      RING_BUFFER_Push_Data((ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq,
                            s_calculator_pf.value_temp_irq[1]);
    }
    else
    {
      START_COUNTER(TIM2)
    }
  }
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    if (LL_TIM_IsEnabledCounter(TIM2))
    {
      LL_TIM_DisableCounter(TIM2);
      s_calculator_pf.value_temp_irq[0]
          = (uint8_t)(LL_TIM_GetCounter(TIM2) >> 8);
      s_calculator_pf.value_temp_irq[1] = (uint8_t)LL_TIM_GetCounter(TIM2);

      RING_BUFFER_Push_Data((ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq,
                            s_calculator_pf.value_temp_irq[0]);
      RING_BUFFER_Push_Data((ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq,
                            s_calculator_pf.value_temp_irq[1]);
    }
    else
    {
      START_COUNTER(TIM2)
    }
  }
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_CALCULATOR_PF_TaskUpdate (void)
{
  if (RING_BUFFER_Is_Empty((ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq))
  {
    return;
  }

  uint16_t u16_value_temp;
  uint8_t  u8_value_adc[2];

  // u8_value_adc[0] 8 bit high of delta_T
  u8_value_adc[0] = RING_BUFFER_Pull_Data(
      (ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq);

  // u8_value_adc[1] 8 bit low of delta_T
  u8_value_adc[1] = RING_BUFFER_Pull_Data(
      (ring_buffer_t *)s_calculator_pf.p_PF_buffer_irq);

  u16_value_temp = (uint16_t)((u8_value_adc[0] << 8) | (u8_value_adc[1]));

  *s_calculator_pf.p_delta_T = (float) u16_value_temp / FREQUENCY_TIMER;

  // Transmission Data to app_data_transmission
}
