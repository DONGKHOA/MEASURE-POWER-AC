/*
 * app_status_led.c
 *
 *  Created on: Nov 25, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_status_led.h"
#include "app_data.h"
#include "gpio.h"
#include "scheduler.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define PORT_LED_STATUS GPIOB
#define PIN_LED_STATUS  LL_GPIO_PIN_4

#define THRESHOLD_CURRENT 5.0 // A

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct _Status_Led_t
{
  uint8_t u8_flag_enable_timer_100ms : 1;
  uint8_t u8_flag_enable_timer_500ms : 1;
} __attribute__((packed)) Status_Led_t;

typedef struct _StatusLed_TaskContextTypedef
{
  SCH_TASK_HANDLE         taskHandle;
  SCH_TaskPropertyTypedef taskProperty;
} StatusLed_TaskContextTypedef;

typedef struct STATUS_LED_TimerContextTypedef_
{
  SCH_TIMER_HANDLE         timerHandle;
  SCH_TimerPropertyTypedef timerProperty;
} STATUS_LED_TimerContextTypedef_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_STATUS_LED_TaskUpdate(void);
static void APP_STATUS_LED_100MS_TimerUpdate(void);
static void APP_STATUS_LED_500MS_TimerUpdate(void);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

static Status_Led_t                 s_status_led;
static StatusLed_TaskContextTypedef s_StatusLedTaskContext
    = { SCH_INVALID_TASK_HANDLE, // Will be updated by Scheduler
        {
            SCH_TASK_SYNC,            // taskType;
            500,                      // taskPeriodInMS;
            APP_STATUS_LED_TaskUpdate // taskFunction;
        } };

static STATUS_LED_TimerContextTypedef_t s_StatusLedTimerContext[2]
    = { { SCH_INVALID_TIMER_HANDLE, // Will be updated by Scheduler
          {
              SCH_TIMER_PERIODIC,              // timerType;
              100,                             // timerPeriodInMS;
              APP_STATUS_LED_100MS_TimerUpdate // timerCallbackFunction;
          } },

        { SCH_INVALID_TIMER_HANDLE, // Will be updated by Scheduler
          {
              SCH_TIMER_PERIODIC,              // timerType;
              500,                             // timerPeriodInMS;
              APP_STATUS_LED_500MS_TimerUpdate // timerCallbackFunction;
          } } };

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function initializes status LEDs by resetting GPIO pins, linking a
 * pointer to a variable, and resetting fields in a structure.
 */
void
APP_STATUS_LED_Init (void)
{
  // Reset GPIO
  LL_GPIO_SetOutputPin(PORT_LED_STATUS, PIN_LED_STATUS);

  // Reset field of s_status_led
  s_status_led.u8_flag_enable_timer_100ms = 0;
  s_status_led.u8_flag_enable_timer_500ms = 0;
}

/**
 * The function `APP_STATUS_LED_CreateTimer` creates timers for status LED
 * handling.
 */
void
APP_STATUS_LED_CreateTimer (void)
{
  SCH_TIM_CreateTimer(&s_StatusLedTimerContext[0].timerHandle,
                      &s_StatusLedTimerContext[0].timerProperty);
  SCH_TIM_CreateTimer(&s_StatusLedTimerContext[1].timerHandle,
                      &s_StatusLedTimerContext[1].timerProperty);
}

/**
 * The function APP_STATUS_LED_CreateTask creates a task for the status LED.
 */
void
APP_STATUS_LED_CreateTask (void)
{
  SCH_TASK_CreateTask(&s_StatusLedTaskContext.taskHandle,
                      &s_StatusLedTaskContext.taskProperty);
//  SCH_TIM_RestartTimer(s_StatusLedTimerContext[0].timerHandle);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_STATUS_LED_TaskUpdate (void)
{
  if (s_data_system.f_current < THRESHOLD_CURRENT)
  {
    SCH_TIM_StopTimer(s_StatusLedTimerContext[0].timerHandle);
    SCH_TIM_RestartTimer(s_StatusLedTimerContext[1].timerHandle);
  }
  else
  {
    SCH_TIM_StopTimer(s_StatusLedTimerContext[1].timerHandle);
    SCH_TIM_RestartTimer(s_StatusLedTimerContext[0].timerHandle);
  }
}

static void
APP_STATUS_LED_100MS_TimerUpdate (void)
{
  BSP_GPIO_Toggle(PORT_LED_STATUS, PIN_LED_STATUS);
}

static void
APP_STATUS_LED_500MS_TimerUpdate (void)
{
  BSP_GPIO_Toggle(PORT_LED_STATUS, PIN_LED_STATUS);
}
