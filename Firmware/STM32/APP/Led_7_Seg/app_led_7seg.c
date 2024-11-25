/*
 * app_led_7seg.c
 *
 *  Created on: Nov 22, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_data.h"
#include "app_led_7seg.h"
#include "gpio.h"
#include "scheduler.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define NUMBER_OF_LED 5

#define PORT_LED_A  GPIOB
#define PORT_LED_B  GPIOB
#define PORT_LED_C  GPIOB
#define PORT_LED_D  GPIOB
#define PORT_LED_E  GPIOB
#define PORT_LED_F  GPIOB
#define PORT_LED_G  GPIOB
#define PORT_LED_DP GPIOB

#define PIN_LED_A  LL_GPIO_PIN_0
#define PIN_LED_B  LL_GPIO_PIN_0
#define PIN_LED_C  LL_GPIO_PIN_0
#define PIN_LED_D  LL_GPIO_PIN_0
#define PIN_LED_E  LL_GPIO_PIN_0
#define PIN_LED_F  LL_GPIO_PIN_0
#define PIN_LED_G  LL_GPIO_PIN_0
#define PIN_LED_DP LL_GPIO_PIN_0

#define PORT_CONTROL_0 GPIOB
#define PORT_CONTROL_1 GPIOB
#define PORT_CONTROL_2 GPIOB
#define PORT_CONTROL_3 GPIOB
#define PORT_CONTROL_4 GPIOB

#define PIN_CONTROL_0 LL_GPIO_PIN_0
#define PIN_CONTROL_1 LL_GPIO_PIN_0
#define PIN_CONTROL_2 LL_GPIO_PIN_0
#define PIN_CONTROL_3 LL_GPIO_PIN_0
#define PIN_CONTROL_4 LL_GPIO_PIN_0

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct _LED_7SEG_t
{
  volatile uint32_t      u32_pin_led[8];
  volatile uint32_t      u32_pin_control[5];
  volatile GPIO_TypeDef *p_port_led[8];
  volatile GPIO_TypeDef *p_port_control[5];
  volatile float        *p_power;
  volatile uint8_t       u8_position_led;
} LED_7SEG_t;

typedef struct _Control_TaskContextTypedef_
{
  SCH_TASK_HANDLE         taskHandle;
  SCH_TaskPropertyTypedef taskProperty;
} Control_TaskContextTypedef;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void     APP_LED_7SEG_TaskUpdate(void);
static void     APP_LED_7SEG_DisplayLed(uint8_t u8_number);
static void     APP_LED_7SEG_ScanLed(void);
static uint32_t APP_LED_7_SEG_Pow(uint8_t u8_x, uint8_t u8_y);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

// Data of table don't consist of dp
static uint8_t u8_data_mask_led[10]
    = { 0xC0, 0XF9, 0XA4, 0XB0, 0X99, 0X92, 0X82, 0X8F, 0X80, 0X90 };

static LED_7SEG_t                 s_LED_7SEG;
static Control_TaskContextTypedef s_ControlTaskContext
    = { SCH_INVALID_TASK_HANDLE, // Will be updated by Scheduler
        {
            SCH_TASK_SYNC,          // taskType;
            5,                      // taskPeriodInMS;
            APP_LED_7SEG_TaskUpdate // taskFunction;
        } };

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function initializes the LED 7-segment display by assigning port and pin
 * values and linking a pointer to a system power variable.
 */
void
APP_LED_7_SEG_Init (void)
{
  // Link pointer to variable
  s_LED_7SEG.p_power = &s_data_system.f_power;

  // Reset data filed of s_LED_7SEG
  s_LED_7SEG.p_port_led[0] = PORT_LED_A;
  s_LED_7SEG.p_port_led[1] = PORT_LED_B;
  s_LED_7SEG.p_port_led[2] = PORT_LED_C;
  s_LED_7SEG.p_port_led[3] = PORT_LED_D;
  s_LED_7SEG.p_port_led[4] = PORT_LED_E;
  s_LED_7SEG.p_port_led[5] = PORT_LED_F;
  s_LED_7SEG.p_port_led[6] = PORT_LED_G;
  s_LED_7SEG.p_port_led[7] = PORT_LED_DP;

  s_LED_7SEG.u32_pin_led[0] = PIN_LED_A;
  s_LED_7SEG.u32_pin_led[1] = PIN_LED_B;
  s_LED_7SEG.u32_pin_led[2] = PIN_LED_C;
  s_LED_7SEG.u32_pin_led[3] = PIN_LED_D;
  s_LED_7SEG.u32_pin_led[4] = PIN_LED_E;
  s_LED_7SEG.u32_pin_led[5] = PIN_LED_F;
  s_LED_7SEG.u32_pin_led[6] = PIN_LED_G;
  s_LED_7SEG.u32_pin_led[7] = PIN_LED_DP;

  s_LED_7SEG.p_port_control[0] = PORT_CONTROL_0;
  s_LED_7SEG.p_port_control[1] = PORT_CONTROL_0;
  s_LED_7SEG.p_port_control[2] = PORT_CONTROL_0;
  s_LED_7SEG.p_port_control[3] = PORT_CONTROL_0;
  s_LED_7SEG.p_port_control[4] = PORT_CONTROL_0;

  s_LED_7SEG.u32_pin_control[0] = PIN_CONTROL_0;
  s_LED_7SEG.u32_pin_control[1] = PIN_CONTROL_0;
  s_LED_7SEG.u32_pin_control[2] = PIN_CONTROL_0;
  s_LED_7SEG.u32_pin_control[3] = PIN_CONTROL_0;
  s_LED_7SEG.u32_pin_control[4] = PIN_CONTROL_0;

  s_LED_7SEG.u8_position_led = 0;
  *s_LED_7SEG.p_power = 0;
}

/**
 * The function `APP_LED_7_SEG_CreateTask` creates a task using the
 * `SCH_TASK_CreateTask` function.
 */
void
APP_LED_7_SEG_CreateTask (void)
{
  SCH_TASK_CreateTask(&s_ControlTaskContext.taskHandle,
                      &s_ControlTaskContext.taskProperty);
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

static void
APP_LED_7SEG_TaskUpdate (void)
{
  if (s_LED_7SEG.u8_position_led >= NUMBER_OF_LED)
  {
    s_LED_7SEG.u8_position_led = 0;
  }

  if (*s_LED_7SEG.p_power >= 1000)
  {
    uint16_t u16_integer = (uint16_t)(*s_LED_7SEG.p_power);
    uint8_t  u8_decimal  = (uint8_t)((*s_LED_7SEG.p_power - u16_integer) * 100);
    uint8_t  u8_number   = 0;

    if (s_LED_7SEG.u8_position_led < 1)
    {
      u8_number
          = (u8_decimal / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led))
            % 10;
    }
    else
    {
      u8_number = (u16_integer
                   / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led - 1))
                  % 10;
    }

    APP_LED_7SEG_DisplayLed(u8_number);

    // Turn on DP
    if (s_LED_7SEG.u8_position_led == 1)
    {
      BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_led[7],
                        s_LED_7SEG.u32_pin_led[7],
                        (u8_data_mask_led[u8_number] >> 7) & 0x01);
    }
  }
  else if (*s_LED_7SEG.p_power >= 100)
  {
    uint16_t u16_integer = (uint16_t)(*s_LED_7SEG.p_power);
    uint8_t  u8_decimal  = (uint8_t)((*s_LED_7SEG.p_power - u16_integer) * 100);
    uint8_t  u8_number   = 0;

    if (s_LED_7SEG.u8_position_led < 2)
    {
      u8_number
          = (u8_decimal / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led))
            % 10;
    }
    else
    {
      u8_number = (u16_integer
                   / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led - 2))
                  % 10;
    }

    APP_LED_7SEG_DisplayLed(u8_number);

    // Turn on DP
    if (s_LED_7SEG.u8_position_led == 2)
    {
      BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_led[7],
                        s_LED_7SEG.u32_pin_led[7],
                        (u8_data_mask_led[u8_number] >> 7) & 0x01);
    }
  }
  else if (*s_LED_7SEG.p_power >= 10)
  {
    uint8_t  u8_integer  = (uint8_t)(*s_LED_7SEG.p_power);
    uint16_t u16_decimal = (uint16_t)((*s_LED_7SEG.p_power - u8_integer) * 100);
    uint8_t  u8_number   = 0;

    if (s_LED_7SEG.u8_position_led < 3)
    {
      u8_number
          = (u16_decimal / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led))
            % 10;
    }
    else
    {
      u8_number
          = (u8_integer / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led - 2))
            % 10;
    }

    APP_LED_7SEG_DisplayLed(u8_number);

    // Turn on DP
    if (s_LED_7SEG.u8_position_led == 3)
    {
      BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_led[7],
                        s_LED_7SEG.u32_pin_led[7],
                        (u8_data_mask_led[u8_number] >> 7) & 0x01);
    }
  }
  else
  {
    uint8_t  u8_integer  = (uint8_t)(*s_LED_7SEG.p_power);
    uint16_t u16_decimal = (uint16_t)((*s_LED_7SEG.p_power - u8_integer) * 100);
    uint8_t  u8_number   = 0;

    if (s_LED_7SEG.u8_position_led < 4)
    {
      u8_number
          = (u16_decimal / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led))
            % 10;
    }
    else
    {
      u8_number
          = (u8_integer / APP_LED_7_SEG_Pow(10, s_LED_7SEG.u8_position_led - 2))
            % 10;
    }

    APP_LED_7SEG_DisplayLed(u8_number);

    // Turn on DP
    if (s_LED_7SEG.u8_position_led == 4)
    {
      BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_led[7],
                        s_LED_7SEG.u32_pin_led[7],
                        (u8_data_mask_led[u8_number] >> 7) & 0x01);
    }
  }
  
  APP_LED_7SEG_ScanLed();
  s_LED_7SEG.u8_position_led++;
}

static void
APP_LED_7SEG_DisplayLed (uint8_t u8_number)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_led[i],
                      s_LED_7SEG.u32_pin_led[i],
                      (u8_data_mask_led[u8_number] >> i) & 0x01);
  }
}

static void
APP_LED_7SEG_ScanLed (void)
{
  for (uint8_t i = 0; i < NUMBER_OF_LED; i++)
  {
    if (s_LED_7SEG.u8_position_led == i)
    {
      BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_control[i],
                        s_LED_7SEG.u32_pin_control[i],
                        1);
      continue;
    }
    BSP_GPIO_SetState((GPIO_TypeDef *)s_LED_7SEG.p_port_control[i],
                      s_LED_7SEG.u32_pin_control[i],
                      0);
  }
}

static uint32_t
APP_LED_7_SEG_Pow (uint8_t u8_x, uint8_t u8_y)
{
  uint32_t u32_ret = 1;
  for (uint8_t i = 1; i <= u8_y; i++)
  {
    u32_ret *= u8_x;
  }
  return u32_ret;
}
