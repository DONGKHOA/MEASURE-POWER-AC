/*
 * scheduler.h
 *
 *  Created on: Jul 9, 2024
 *      Author: dongkhoa
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "basetypedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*********************
   *    PUBLIC DEFINES
   *********************/

#define SCH_TASK_HANDLE          uint8_t
#define SCH_INVALID_TASK_HANDLE  0xFF
#define SCH_TIMER_HANDLE         uint8_t
#define SCH_INVALID_TIMER_HANDLE 0xFF

  /**********************
   *    PUBLIC TYPEDEFS
   **********************/

  /**
   * @brief Function pointer for a task.
   *
   * This typedef defines a function pointer type for tasks that are
   * scheduled to run in the system. The function does not take any
   * parameters and does not return any value.
   */
  typedef void (*SCH_TASK)(void);

  /**
   * @brief Function pointer for a timer callback function.
   *
   * This typedef defines a function pointer type for timer callback functions.
   * These functions are called when a timer event occurs. The function does not
   * take any parameters and does not return any value.
   */
  typedef void (*SCH_TimerCallback)(void);

  /**
   * @brief Enumeration for task types in the scheduler.
   */
  typedef enum _SCH_TASK_TYPE
  {
    SCH_TASK_NONE, /**< @brief No specific task type. */
    SCH_TASK_SYNC, /**< @brief Synchronous task. */
    SCH_TASK_ASYNC /**< @brief Asynchronous task. */
  } SCH_TASK_TYPE;

  /**
   * @brief Enumeration for timer types in the scheduler.
   */
  typedef enum _SCH_TIMER_TYPE
  {
    SCH_TIMER_NONE = 0, /**< @brief No timer specified. */
    SCH_TIMER_MONO,     /**< @brief One-shot timer. */
    SCH_TIMER_PERIODIC  /**< @brief Periodic timer. */
  } SCH_TIMER_TYPE;

  /**
   * @brief Structure for defining properties of a task.
   */
  typedef struct _SCH_TaskPropertyTypedef
  {
    SCH_TASK_TYPE     taskType;       /**< @brief Type of the task. */
    uint16_t          taskPeriodInMS; /**< @brief Period of the task in
                                             milliseconds. */
    SCH_TASK taskFunction;            /**< @brief Function pointer to the
                                             task function. */
    uint32_t taskTick;                /**< @brief Tick count for the task. */
  } SCH_TaskPropertyTypedef;

  /**
   * @brief Structure for defining properties of a timer.
   */
  typedef struct _SCH_TimerPropertyTypedef
  {
    SCH_TIMER_TYPE timerType;                /**< @brief Type of the timer. */
    uint16_t       timerPeriodInMS;          /**< @brief Period of the timer
                                                  in milliseconds. */
    SCH_TimerCallback timerCallbackFunction; /**< @brief Function pointer to
                                                  the timer callback
                                                  function. */
  } SCH_TimerPropertyTypedef;

  /**
   * @brief Enumeration for task states in the scheduler.
   */
  typedef enum _SCH_TaskStateTypedef
  {
    TASK_StateHold = 0, /**< @brief Task is on hold. */
    TASK_StateReady     /**< @brief Task is ready to run. */
  } SCH_TaskStateTypedef;

  /**
   * @brief Enumeration for timer states in the scheduler.
   */
  typedef enum _SCH_TimerStateTypedef
  {
    TIM_StateStop = 0, /**< @brief Timer is stopped. */
    TIM_StateRun       /**< @brief Timer is running. */
  } SCH_TimerStateTypedef;

  /**
   * @brief Enumeration for software timer identifiers.
   */
  typedef enum _SCH_SoftTimerTypedef
  {
    SCH_TIM_FIRST = 0,             /**< @brief First timer identifier. */
    SCH_TIM_LED   = SCH_TIM_FIRST, /**< @brief LED timer. */
    SCH_TIM_TEMPERATURE_LOG,       /**< @brief Temperature log timer. */
    SCH_TIM_LAST                   /**< @brief Last timer identifier. */
  } SCH_SoftTimerTypedef;

  /**********************
   *   PUBLIC FUNCTIONS
   **********************/

  void SCH_Initialize(void);
  status_t SCH_TASK_CreateTask(SCH_TASK_HANDLE         *pHandle,
                               SCH_TaskPropertyTypedef *pTaskProperty);
  status_t SCH_TASK_ResumeTask(SCH_TASK_HANDLE taskIndex);
  status_t SCH_TASK_StopTask(SCH_TASK_HANDLE taskIndex);
  status_t SCH_TASK_EnableTask(SCH_TASK_HANDLE taskIndex);
  void     SCH_RunSystemTickTimer(void);
  void     SCH_StartScheduler(void);
  void     SCH_StopScheduler(void);
  void     SCH_HandleScheduledTask(void);
  uint32_t SCH_SystemTick(void);
  status_t SCH_TIM_CreateTimer(SCH_TIMER_HANDLE         *pHandle,
                               SCH_TimerPropertyTypedef *pTimerProperty);
  status_t SCH_TIM_StopTimer(SCH_TIMER_HANDLE timerIndex);
  status_t SCH_TIM_RestartTimer(SCH_TIMER_HANDLE timerIndex);
  void SCH_TIM_Start(const SCH_SoftTimerTypedef timer, const uint32_t timeInMs);
  uint16_t SCH_TIM_HasCompleted(const SCH_SoftTimerTypedef timer);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_H_ */
