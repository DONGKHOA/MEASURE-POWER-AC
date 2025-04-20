/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_timer.h"
#include "app_data_ESP32.h"

#include "button.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define PRESSING_TIMEOUT 2000 // ms
#define GET_TICK         (xTaskGetTickCount() * portTICK_PERIOD_MS)

#define BUTTON_PRESSING_BIT       (1 << 0)
#define BUTTON_RELEASING_BIT      (1 << 1)

/******************************************************************************
 *  PRIVATE TYPEDEF
 *****************************************************************************/

static BUTTON_CALLBACK_FUNCTION_t BUTTON_PressingCallback      = NULL;
static BUTTON_CALLBACK_FUNCTION_t BUTTON_ReleasingCallback     = NULL;

static QueueHandle_t      button_event_queue           = NULL;
static TimerHandle_t      button_soft_timer            = NULL;
static TaskHandle_t       button_debouncing_task       = NULL;
static TaskHandle_t       button_executing_task        = NULL;
static EventGroupHandle_t button_executing_event_group = NULL;
static uint8_t            is_initialized               = 0;
static BUTTON_HANDLE_t   *current_button               = NULL;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/
static void
button_debouncing_task_handler (void *arg)
{
  BUTTON_HANDLE_t *button = NULL;
  while (1)
  {
    if (xQueueReceive(button_event_queue, &button, portMAX_DELAY))
    {
      current_button = button;
      if (GET_TICK - button->debouncing_timer < 20)
      {
        continue;
      }
      button->debouncing_timer = GET_TICK;

      if (gpio_get_level(button->gpio_pin) == 0)
      {
        button->pressing_timer = GET_TICK;
        xTimerReset(button_soft_timer, 0);
        xEventGroupSetBits(button_executing_event_group, BUTTON_PRESSING_BIT);
      }
      else
      {
        xTimerStop(button_soft_timer, 0);
        xEventGroupSetBits(button_executing_event_group, BUTTON_RELEASING_BIT);
      }
    }
  }
}

static void
button_executing_task_handler (void *arg)
{
  while (1)
  {
    EventBits_t uxBits
        = xEventGroupWaitBits(button_executing_event_group,
                              BUTTON_PRESSING_BIT | BUTTON_RELEASING_BIT,
                              pdTRUE,
                              pdFALSE,
                              portMAX_DELAY);

    if (uxBits & BUTTON_PRESSING_BIT)
    {
      if (BUTTON_PressingCallback != NULL && current_button != NULL)
      {
        BUTTON_PressingCallback(current_button->gpio_pin);
      }
    }
    if (uxBits & BUTTON_RELEASING_BIT)
    {
      if (BUTTON_ReleasingCallback != NULL && current_button != NULL)
      {
        BUTTON_ReleasingCallback(current_button->gpio_pin);
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

static void
soft_timer_callback ()
{
  xTimerStop(button_soft_timer, 0);
}

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/
void
BUTTON_Init (BUTTON_HANDLE_t *button, uint32_t pin)
{
  if (is_initialized == 0)
  {
    button_event_queue = xQueueCreate(20, sizeof(BUTTON_HANDLE_t *));
    if (button_event_queue == NULL)
    {
      printf("Failed to create button event queue");
      return;
    }

    button_soft_timer = xTimerCreate("button_soft_timer",
                                     PRESSING_TIMEOUT / portTICK_PERIOD_MS,
                                     pdFALSE,
                                     (void *)0,
                                     soft_timer_callback);
    if (button_soft_timer == NULL)
    {
      printf("Failed to create button software timer");
      return;
    }

    button_executing_event_group = xEventGroupCreate();
    if (button_executing_event_group == NULL)
    {
      printf("Failed to create button executing event group");
      return;
    }

    xTaskCreate(button_debouncing_task_handler,
                "button_debouncing_task",
                2048,
                NULL,
                10,
                &button_debouncing_task);
    xTaskCreate(button_executing_task_handler,
                "button_executing_task",
                2048,
                NULL,
                11,
                &button_executing_task);
    is_initialized = 1;
  }

  button->gpio_pin         = pin;
  button->debouncing_timer = 0;
  button->pressing_timer   = 0;
  button->is_pressing      = 0;

  BSP_EXTI_Init(pin, EXTI_EDGE_ANY);
}

void
BUTTON_SetCallbackFunction (BUTTON_CALLBACK_FUNCTION_t PressingCallback,
                            BUTTON_CALLBACK_FUNCTION_t ReleasingCallback)
{
  BUTTON_PressingCallback  = PressingCallback;
  BUTTON_ReleasingCallback = ReleasingCallback;
}

void
BUTTON_EXTI_Handle (BUTTON_HANDLE_t *button, uint32_t pin)
{
  if (button->gpio_pin == pin)
  {
    xQueueSend(button_event_queue, (void *)&button, 0);
  }
}
