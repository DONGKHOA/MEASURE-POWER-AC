/*
 * schedular.c
 *
 *  Created on: Jul 9, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <string.h>
#include "scheduler.h"
#include "systick.h"
#include "stm32f1xx.h"

/*********************
 *    PRIVATE TYPEDEF
 *********************/

/**
 * @brief Structure defining the context of a task in the scheduler.
 */
typedef struct SCH_TaskContextTypedef
{
  SCH_TaskStateTypedef taskState;               /**< @brief State of the
                                                      task. */
  uint16_t taskFlag;                            /**< @brief Flag
                                                      associated with the
                                                      task. */
  uint32_t taskTick;                            /**< @brief Tick count
                                                      related to the task. */
  const SCH_TaskPropertyTypedef *pTaskProperty; /**< @brief Pointer to the
                                                        task's properties. */
} SCH_TaskContextTypedef;

/**
 * @brief Structure defining the context of a timer in the scheduler.
 */
typedef struct SCH_TimerContextTypedef
{
  SCH_TimerStateTypedef timerState;         /**< @brief State of the
                                                timer. */
  uint16_t timerFlag;                       /**< @brief Flag associated
                                                with the timer. */
  uint32_t timerTick;                       /**< @brief Tick count
                                                related to the timer. */
  SCH_TimerPropertyTypedef *pTimerProperty; /**< @brief Pointer to the
                                                timer's properties. */
} SCH_TimerContextTypedef;

/*********************
 *    PRIVATE DEFINE
 *********************/

#define MAX_TASK   5
#define MAX_TIMERS 2

/*********************
 *    PRIVATE MACRO
 *********************/

// START STOP SYS_TICK?
#define SCH_START BSP_SYSTICK_TimerStart()
#define SCH_STOP  BSP_SYSTICK_TimerStop()

static SCH_TaskContextTypedef  s_TaskContext[MAX_TASK];
static uint8_t                 s_NumOfTaskScheduled;
static SCH_TimerContextTypedef s_TimerContext[MAX_TIMERS];
static uint8_t                 s_NumOfTimers;

volatile uint32_t s_SystemTick;
volatile uint32_t s_SoftTimers[SCH_TIM_LAST];

/******************************************************************************
 *     PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief Initialize the scheduler.
 *
 * This function resets various scheduler variables and initializes the
 * scheduler context, including task and timer contexts, as well as soft timers.
 * It also initializes the SysTick timer for scheduler operations.
 */
void
SCH_Initialize (void)
{
  s_SystemTick         = RESET; /**< @brief Reset system tick counter. */
  s_NumOfTaskScheduled = RESET; /**< @brief Reset number of scheduled tasks. */
  s_NumOfTimers        = RESET; /**< @brief Reset number of timers. */

  /**< @brief Initialize Scheduler Context */

  /**< @brief Clear task context array. */
  memset((uint8_t *)&s_TaskContext[0],
         RESET,
         (sizeof(SCH_TaskContextTypedef) * MAX_TASK));

  /**< @brief Clear timer context array. */
  memset((uint8_t *)&s_TimerContext[0],
         RESET,
         (sizeof(SCH_TimerContextTypedef) * MAX_TIMERS));

  /**< @brief Clear soft timers array. */
  memset((uint8_t *)&s_SoftTimers[0], RESET, (sizeof(uint32_t) * SCH_TIM_LAST));

  /**< @brief Initialize SysTick timer. */
  BSP_SYSTICK_TimerInit();
}

/**
 * @brief Start or reset a software timer.
 *
 * This function sets or resets the specified software timer with the given time
 * in milliseconds.
 *
 * @param timer Identifier of the software timer to start or reset.
 * @param timeInMs Time duration in milliseconds to set the software timer to.
 */
void
SCH_TIM_Start (const SCH_SoftTimerTypedef timer, const uint32_t timeInMs)
{
  if (timer < SCH_TIM_LAST)
  {
    /**< @brief Set the specified software timer. */
    s_SoftTimers[timer] = timeInMs;
  }
}

/**
 * @brief Check if a software timer has completed.
 *
 * This function checks if the specified software timer has completed its
 * countdown (reached zero).
 *
 * @param timer Identifier of the software timer to check.
 * @return Returns 1 if the timer has completed (time remaining is zero),
 *         otherwise returns 0.
 */
uint16_t
SCH_TIM_HasCompleted (const SCH_SoftTimerTypedef timer)
{
  /**< @brief Check if the timer has completed. */
  return (s_SoftTimers[timer] == 0 ? 1 : 0);
}

/**
 * @brief Resume a suspended task in the scheduler.
 *
 * This function changes the state of the specified task to TASK_StateReady,
 * indicating that the task can now be executed.
 *
 * @param taskIndex Index of the task to resume.
 * @return Returns STS_DONE if the task was successfully resumed, otherwise
 *         returns STS_ERROR.
 */
status_t
SCH_TASK_ResumeTask (SCH_TASK_HANDLE taskIndex)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  if (taskIndex < s_NumOfTaskScheduled)
  {
    /**< @brief Get Task Context */
    SCH_TaskContextTypedef *pTaskContext = &s_TaskContext[taskIndex];
    pTaskContext->taskState              = TASK_StateReady;
    status                               = STS_DONE;
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * @brief Stop a running task in the scheduler.
 *
 * This function changes the state of the specified task to TASK_StateHold,
 * indicating that the task should pause or stop its execution.
 *
 * @param taskIndex Index of the task to stop.
 * @return Returns STS_DONE if the task was successfully stopped, otherwise
 *         returns STS_ERROR.
 */
status_t
SCH_TASK_StopTask (SCH_TASK_HANDLE taskIndex)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  if (taskIndex < s_NumOfTaskScheduled)
  {
    /**< @brief Get Task Context */
    SCH_TaskContextTypedef *pTaskContext = &s_TaskContext[taskIndex];

    /**< @brief Set task state to hold. */
    pTaskContext->taskState = TASK_StateHold;

    /**< @brief Update status to indicate success. */
    status = STS_DONE;
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * The function `SCH_TASK_EnableTask` enables a task identified by `taskIndex`
 * if it is within the range of scheduled tasks.
 *
 * @param taskIndex The `taskIndex` parameter is the index of the task that you
 * want to enable in the task scheduler. It is used to identify the specific
 * task within the scheduler's task list.
 *
 * @return The function `SCH_TASK_EnableTask` returns the status of the
 * operation, which can be either `STS_ERROR` if the task index is out of range
 * or `STS_DONE` if the task is successfully enabled.
 */
status_t
SCH_TASK_EnableTask (SCH_TASK_HANDLE taskIndex)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  if (taskIndex < s_NumOfTaskScheduled)
  {
    /**< @brief Get Task Context */
    SCH_TaskContextTypedef *pTaskContext = &s_TaskContext[taskIndex];
    pTaskContext->taskFlag               = TRUE;
    status                               = STS_DONE;
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * @brief Create a new task in the scheduler.
 *
 * This function creates a new task using the provided task properties and
 * registers it with the scheduler.
 *
 * @param pHandle Pointer to store the handle of the created task.
 * @param pTaskProperty Pointer to the task properties defining the new task.
 * @return Returns STS_DONE if the task was successfully created and registered,
 *         otherwise returns STS_ERROR.
 */
status_t
SCH_TASK_CreateTask (SCH_TASK_HANDLE         *pHandle,
                     SCH_TaskPropertyTypedef *pTaskProperty)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  /**< @brief Ensure valid parameters are provided */
  if (pHandle && pTaskProperty)
  {
    /**< @brief Check if there's room for more tasks */
    if (s_NumOfTaskScheduled < (MAX_TASK - 1))
    {
      SCH_TaskContextTypedef *pTaskContext
          = &s_TaskContext[s_NumOfTaskScheduled];

      /**< @brief Set task properties in task context */
      pTaskContext->pTaskProperty = pTaskProperty;
      pTaskContext->taskFlag      = FALSE;
      pTaskContext->taskTick      = pTaskProperty->taskTick;
      pTaskContext->taskState     = TASK_StateReady;

      /**< @brief Provide task handle to caller */
      *pHandle = s_NumOfTaskScheduled;

      /**< @brief Increment task count */
      s_NumOfTaskScheduled++;

      /**< @brief Task registered successfully */
      status = STS_DONE;
    }
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * @brief Create a new timer in the scheduler.
 *
 * This function creates a new timer using the provided timer properties and
 * registers it with the scheduler.
 *
 * @param pHandle Pointer to store the handle of the created timer.
 * @param pTimerProperty Pointer to the timer properties defining the new timer.
 * @return Returns STS_DONE if the timer was successfully created and
 * registered, otherwise returns STS_ERROR.
 */
status_t
SCH_TIM_CreateTimer (SCH_TIMER_HANDLE         *pHandle,
                     SCH_TimerPropertyTypedef *pTimerProperty)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  /**< @brief Ensure valid parameters are provided */
  if (pHandle && pTimerProperty)
  {
    /**< @brief Check if there's room for more timers */
    if (s_NumOfTimers < (MAX_TIMERS - 1))
    {
      SCH_TimerContextTypedef *pTimerContext = &s_TimerContext[s_NumOfTimers];

      /**< @brief Set timer properties in timer context */
      pTimerContext->pTimerProperty = pTimerProperty;
      pTimerContext->timerState     = TIM_StateStop;
      pTimerContext->timerFlag      = FALSE;
      pTimerContext->timerTick      = RESET;

      /**< @brief Provide timer handle to caller */
      *pHandle = s_NumOfTimers;

      /**< @brief Increment timer count */
      s_NumOfTimers++;

      /**< @brief Timer registered successfully */
      status = STS_DONE;
    }
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * @brief Restart a timer in the scheduler.
 *
 * This function restarts the specified timer by resetting its tick count
 * and setting its state to TIM_StateRun, indicating that the timer is running.
 *
 * @param timerIndex Index of the timer to restart.
 * @return Returns STS_DONE if the timer was successfully restarted,
 *         otherwise returns STS_ERROR.
 */
status_t
SCH_TIM_RestartTimer (SCH_TIMER_HANDLE timerIndex)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  if (timerIndex < s_NumOfTimers)
  {
    /**< @brief Get Timer Context */
    SCH_TimerContextTypedef *pTimerContext = &s_TimerContext[timerIndex];
    pTimerContext->timerTick               = RESET;
    pTimerContext->timerState              = TIM_StateRun;
    status                                 = STS_DONE;
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * @brief Stop a timer in the scheduler.
 *
 * This function stops the specified timer by setting its state to
 * TIM_StateStop, indicating that the timer should halt its countdown or
 * operation.
 *
 * @param timerIndex Index of the timer to stop.
 * @return Returns STS_DONE if the timer was successfully stopped,
 *         otherwise returns STS_ERROR.
 */
status_t
SCH_TIM_StopTimer (SCH_TIMER_HANDLE timerIndex)
{
  status_t status
      = STS_ERROR; /**< @brief Initialize status to indicate error. */

  if (timerIndex < s_NumOfTimers)
  {
    /**< @brief Get Timer Context */
    SCH_TimerContextTypedef *pTimerContext = &s_TimerContext[timerIndex];
    pTimerContext->timerState              = TIM_StateStop;
    status                                 = STS_DONE;
  }

  /**< @brief Return the status of the operation. */
  return status;
}

/**
 * @brief Run the system tick timer to manage tasks and timers.
 *
 * This function increments the system tick counter and manages both synchronous
 * tasks and running timers based on their respective periods. It updates task
 * and timer ticks, checks if their periods have elapsed, and updates their
 * flags accordingly. Additionally, it decrements software timers.
 */
void
SCH_RunSystemTickTimer (void)
{
  uint8_t                 taskIndex;    /**< @brief Index variable for tasks. */
  SCH_TaskContextTypedef *pTaskContext; /**< @brief Pointer to task context. */
  uint8_t                 timerIndex; /**< @brief Index variable for timers. */
  SCH_TimerContextTypedef
      *pTimerContext; /**< @brief Pointer to timer context. */

  /**< @brief Increment System Tick counter */
  s_SystemTick++;

  /**< @brief Check Status of periodic tasks */
  for (taskIndex = 0; taskIndex < s_NumOfTaskScheduled; taskIndex++)
  {
    /**< @brief Get Task Context */
    pTaskContext = &s_TaskContext[taskIndex];

    /**< @brief Check type and State of the task */
    if ((SCH_TASK_SYNC == pTaskContext->pTaskProperty->taskType)
        && (TASK_StateReady == pTaskContext->taskState))
    {
      /**< @brief Increment task tick */
      pTaskContext->taskTick += 1;

      /**< @brief Check if we reached task period */
      if (pTaskContext->taskTick >= pTaskContext->pTaskProperty->taskPeriodInMS)
      {
        /**< @brief Reset Task tick timer */
        pTaskContext->taskTick = RESET;
        /**< @brief Enable Flag */
        pTaskContext->taskFlag = TRUE;
      }
    }
  }

  /**< @brief Check Status of timers */
  for (timerIndex = 0; timerIndex < s_NumOfTimers; timerIndex++)
  {
    /**< @brief Get Timer Context */
    pTimerContext = &s_TimerContext[timerIndex];

    /**< @brief Check type and State of the timer */
    if (TIM_StateRun == pTimerContext->timerState)
    {
      /**< @brief Increment timer tick */
      pTimerContext->timerTick += 1;

      /**< @brief Check if we reached timer period */
      if (pTimerContext->timerTick
          >= pTimerContext->pTimerProperty->timerPeriodInMS)
      {
        /**< @brief Enable Flag */
        pTimerContext->timerFlag = TRUE;
        /**< @brief Reset tick timer */
        pTimerContext->timerTick = RESET;
        /**< @brief Check timer type and change the state */
        pTimerContext->timerState
            = (SCH_TIMER_PERIODIC == pTimerContext->pTimerProperty->timerType)
                  ? TIM_StateRun
                  : TIM_StateStop;
      }
    }
  }

  /**< @brief Update software timers */
  for (timerIndex = 0; timerIndex < SCH_TIM_LAST; timerIndex++)
  {
    if (s_SoftTimers[timerIndex] > 0)
    {
      s_SoftTimers[timerIndex]--;
    }
  }
}

/**
 * @brief Start the scheduler by initiating the system tick timer.
 *
 * This function starts the scheduler by invoking SCH_START, which initiates
 * the system tick timer or performs any necessary operations to begin the
 * scheduling of tasks and timers.
 */
void
SCH_StartScheduler (void)
{
  /**< @brief Scheduler by initiating the system tick timer */
  SCH_START;
}

/**
 * @brief Stop the scheduler and reset scheduler context.
 *
 * This function stops the scheduler by invoking SCH_STOP, which halts the
 * system tick timer or performs any necessary operations to cease scheduling
 * tasks and timers. Additionally, it initializes the scheduler context,
 * resetting all task contexts, timer contexts, and software timers.
 */
void
SCH_StopScheduler (void)
{
  /**< @brief Stop Scheduler by stopping the system tick timer */
  SCH_STOP;

  /**< @brief Initialize Scheduler Context */
  memset((uint8_t *)&s_TaskContext[0],
         RESET,
         (sizeof(SCH_TaskContextTypedef) * MAX_TASK));
  memset((uint8_t *)&s_TimerContext[0],
         RESET,
         (sizeof(SCH_TimerContextTypedef) * MAX_TIMERS));
  memset((uint8_t *)&s_SoftTimers[0], RESET, (sizeof(uint32_t) * SCH_TIM_LAST));
}

/**
 * @brief Handle scheduled tasks and timers.
 *
 * This function iterates through scheduled tasks and timers, checking their
 * respective flags to determine if they need to be executed. If a task's flag
 * is set and it is in the ready state, its associated function is called.
 * Similarly, if a timer's flag is set, its callback function is invoked.
 */
void
SCH_HandleScheduledTask (void)
{
  uint8_t                 taskIndex;    /**< @brief Index variable for tasks. */
  SCH_TaskContextTypedef *pTaskContext; /**< @brief Pointer to task context. */
  uint8_t                 timerIndex; /**< @brief Index variable for timers. */
  SCH_TimerContextTypedef
      *pTimerContext; /**< @brief Pointer to timer context. */

  /**< @brief Handle scheduled tasks */
  for (taskIndex = 0; taskIndex < s_NumOfTaskScheduled; taskIndex++)
  {
    /**< @brief Get Task Context */
    pTaskContext = &s_TaskContext[taskIndex];

    /**< @brief Check type and State of the task */
    if ((TRUE == pTaskContext->taskFlag)
        && (TASK_StateReady == pTaskContext->taskState))
    {
      pTaskContext->taskFlag = FALSE;
      /**< @brief Call task function if defined */
      if (pTaskContext->pTaskProperty->taskFunction)
      {
        pTaskContext->pTaskProperty->taskFunction();
      }
    }
  }

  /**< @brief Handle scheduled timers */
  for (timerIndex = 0; timerIndex < s_NumOfTimers; timerIndex++)
  {
    /**< @brief Get Timer Context */
    pTimerContext = &s_TimerContext[timerIndex];

    /**< @brief Check timer flag */
    if (TRUE == pTimerContext->timerFlag)
    {
      pTimerContext->timerFlag = FALSE;
      /**< @brief Call timer callback function if defined */
      if (pTimerContext->pTimerProperty->timerCallbackFunction)
      {
        pTimerContext->pTimerProperty->timerCallbackFunction();
      }
    }
  }
}

/**
 * @brief Get the current system tick count.
 *
 * This function returns the current system tick count, which represents the
 * number of times the system tick timer has incremented since startup or
 * initialization.
 *
 * @return uint32_t Current system tick count.
 */
uint32_t
SCH_SystemTick (void)
{
  return s_SystemTick;
}
