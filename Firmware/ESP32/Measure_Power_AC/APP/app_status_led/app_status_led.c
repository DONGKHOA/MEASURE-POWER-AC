/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_status_led.h"
#include "app_data_ESP32.h"

#include "gpio.h"

/******************************************************************************
 *    PUBLIC DEFINES
 *****************************************************************************/

#define LED_PIN GPIO_NUM_2

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct status_data
{
  STATE_t *p_state;
} status_data_t;

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

status_data_t s_status_data;
static int    count = 0;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_Status_led_Task(void *pvParameters);
static void toggle_led(void);
static void led_on(void);
static void led_off(void);
void        timer_callback(TimerHandle_t xTimer);

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Status_led_CreateTask (void)
{
  xTaskCreate(APP_Status_led_Task, "Status_Led_task", 1024 * 2, NULL, 4, NULL);
}

void
APP_Status_led_Init (void)
{
  // Link pointer to variable
  s_status_data.p_state = &s_data_system.s_state;

  // Init status led
  GPIO_Output_Init(LED_PIN);

  TimerHandle_t timer = xTimerCreate("Timer",
                                     pdMS_TO_TICKS(100), // Period 100ms
                                     pdTRUE,             // Auto reload
                                     NULL,
                                     APP_Timer_Callback // Callback function
  );

  xTimerStart(timer, 0);
}

static void
APP_Timer_Callback (TimerHandle_t xTimer)
{
  count += 100; // Increase count every 100ms
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Status_led_Task (void *pvParameters)
{
  while (1)
  {
    if (s_status_data.p_state != NULL)
    {
      printf("State: %d\n", *s_status_data.p_state);

      switch (*s_status_data.p_state)
      {
        case STATE_SCAN_WIFI:
          if (count % 500 == 0)
          {
            toggle_led();
          }
          break;

        case STATE_WIFI_CONNECTED:
          if (count % 1000 == 0)
          {
            toggle_led();
          }
          break;

        case STATE_SMART_CONFIG:
          if (count % 2000 == 0)
          {
            toggle_led();
          }
          break;

        case STATE_THINGSBOARD_SUCCESS:
          if (count % 3000 == 0)
          {
            toggle_led();
          }
          break;

        case STATE_THINGSBOARD_UNSUCCESSFUL:
          if (count % 1000 == 0)
          {
            led_on();
          }
          break;

        case STATE_RESET_FULL_DATA:
          if (count % 5000 == 0)
          {
            led_on();
          }
          if (count % 3000 == 0)
          {
            led_off();
          }
          break;
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

static void
toggle_led (void)
{
  static bool led_status = false;
  led_status             = !led_status;
  gpio_set_level(LED_PIN, led_status);
  printf("LED Status: %s\n", led_status ? "ON" : "OFF");
}

static void
led_on (void)
{
  gpio_set_level(LED_PIN, 1);
}

static void
led_off (void)
{
  gpio_set_level(LED_PIN, 0);
}