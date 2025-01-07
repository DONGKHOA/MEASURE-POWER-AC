/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data_ESP32.h"
#include "app_button.h"
#include "nvs_rw.h"

#include "button.h"
#include "gpio.h"
#include "wifi.h"

/******************************************************************************
 *  PRIVATE DEFINES
 *****************************************************************************/

#define GPIO_BUTTON_PIN 13
#define NUM_WIFI_NVS    "Num_ssid_nvs"

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void EXTI_CallbackFunction(uint32_t pin);
static void BUTTON_Pressing_Handle(int pin);
static void BUTTON_Releasing_Handle(int pin);

static void APP_Button_Task(void);
static void Button_ResetData(void);

/******************************************************************************
 *  PRIVATE DATA
 *****************************************************************************/

BUTTON_HANDLE_t button;

typedef struct app_button_data
{
  STATE_t *p_state;
} app_button_data_t;

app_button_data_t s_button_data;

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
APP_Button_CreateTask (void)
{
  xTaskCreate(APP_Button_Task, "button_task", 1024 * 4, NULL, 6, NULL);
}

void
APP_Button_Init (void)
{
  s_button_data.p_state = &s_data_system.s_state;

  BUTTON_Init(&button, GPIO_BUTTON_PIN);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_Button_Task (void)
{
  while (1)
  {
    BSP_EXTI_SetCallbackFunction(EXTI_CallbackFunction);

    BUTTON_SetCallbackFunction(BUTTON_Pressing_Handle, BUTTON_Releasing_Handle);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

static void
BUTTON_Pressing_Handle (int pin)
{
  if (pin == GPIO_BUTTON_PIN)
  {
    printf("Pressing\n");
    NVS_Erase_Flash(NUM_WIFI_NVS);
    Button_ResetData();
    *s_button_data.p_state = STATE_RESET_FULL_DATA;
  }
}

static void
BUTTON_Releasing_Handle (int pin)
{
  if (pin == GPIO_BUTTON_PIN)
  {
    printf("Releasing\n");
  }
}

static void
EXTI_CallbackFunction (uint32_t pin)
{
  BUTTON_EXTI_Handle(&button, pin);
}

static void
Button_ResetData (void)
{
  s_data_system.f_power        = 0;
  s_data_system.f_energy       = 0;
  s_data_system.f_energy_min   = 0;
  s_data_system.f_energy_hour  = 0;
  s_data_system.f_energy_day   = 0;
  s_data_system.f_energy_month = 0;
  s_data_system.f_energy_year  = 0;

  s_data_system.s_flag_enable.u8_second = 0;
  s_data_system.s_flag_enable.u8_minute = 0;
  s_data_system.s_flag_enable.u8_hour   = 0;
  s_data_system.s_flag_enable.u8_day    = 0;
  s_data_system.s_flag_enable.u8_month  = 0;
  s_data_system.s_flag_enable.u8_year   = 0;
}