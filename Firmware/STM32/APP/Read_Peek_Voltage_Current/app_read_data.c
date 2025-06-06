/*
 * app_read_data.c
 *
 *  Created on: Nov 18, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_read_data.h"
#include "app_data.h"
#include "stm32f1xx_ll_adc.h"
#include "acs712.h"
#include "gpio.h"
#include "ring_buffer.h"
#include "scheduler.h"

#include "math.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define ADC_VOLTAGE_CHANNEL 1
#define ADC_CURRENT_CHANNEL 2

/******************************************************************************
 *    PRIVATE TYPEDEFS
 *****************************************************************************/

typedef struct _Control_TaskContextTypedef_
{
  SCH_TASK_HANDLE         taskHandle;
  SCH_TaskPropertyTypedef taskProperty;
} Control_TaskContextTypedef;

typedef struct _READ_DATA_t
{
  volatile ring_buffer_t *p_vol_cur_buffer_irq;
  volatile ring_buffer_t *p_vol_cur_buffer;
  volatile float         *p_voltage;
  volatile float         *p_current;
} READ_DATA_t;

/******************************************************************************
 *  PRIVATE PROTOTYPE FUNCTION
 *****************************************************************************/

static void APP_READ_DATA_TaskUpdate(void);
static void APP_READ_DATA_ConvertVoltage(uint16_t u16_adc_value_voltage);
static void APP_READ_DATA_ConvertCurrent(uint16_t u16_adc_value_current);

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/
float _err_measure = 2;
float _err_estimate = 2;
float _q = 0.001;
float _current_estimate = 0;
float _last_estimate = 0;
float _kalman_gain = 0;

static READ_DATA_t                s_read_data;
static Control_TaskContextTypedef s_ControlTaskContext
    = { SCH_INVALID_TASK_HANDLE, // Will be updated by Scheduler
        {
            SCH_TASK_SYNC,           // taskType;
            1,                      // taskPeriodInMS;
            APP_READ_DATA_TaskUpdate // taskFunction;
        } };

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function APP_READ_DATA_CreateTask creates a task using the
 * SCH_TASK_CreateTask function.
 */
void
APP_READ_DATA_CreateTask (void)
{
  SCH_TASK_CreateTask(&s_ControlTaskContext.taskHandle,
                      &s_ControlTaskContext.taskProperty);
}

/**
 * The function initializes buffers for a calculator application in C.
 */
void
APP_READ_DATA_Init (void)
{
  // Link pointer to variable
  s_read_data.p_vol_cur_buffer     = &s_data_system.s_vol_cur_buffer;
  s_read_data.p_vol_cur_buffer_irq = &s_data_system.s_vol_cur_buffer_irq;
  s_read_data.p_voltage            = &s_data_system.f_voltage;
  s_read_data.p_current            = &s_data_system.f_current;

  // Initialize buffer
  RING_BUFFER_Init((ring_buffer_t *)s_read_data.p_vol_cur_buffer);
  RING_BUFFER_Init((ring_buffer_t *)s_read_data.p_vol_cur_buffer_irq);
}

void
APP_READ_DATA_HandleDMA_IRQ (flagIRQ_dma_t status)
{
  if (status == FLAG_TRANSFER_COMPLETE)
  {
    RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer_irq, 1);
  }
}

/******************************************************************************
 *  PRIVATE FUNCTION
 *****************************************************************************/

float Kalman_GetValue(float mea) {
  _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  _err_estimate = (1.0f - _kalman_gain) * _err_estimate + fabsf(_last_estimate - _current_estimate) * _q;
  _last_estimate = _current_estimate;

  return _current_estimate;
}

/**
 * The function `APP_READ_DATA_TaskUpdate` reads data from a ring buffer,
 * processes the data, converts voltage and current values, transmits the data
 * to another buffer, and changes the current range based on a threshold value.
 *
 * @return If the ring buffer is empty, the function `APP_READ_DATA_TaskUpdate`
 * will return without performing any further operations.
 */
static void
APP_READ_DATA_TaskUpdate (void)
{
  uint32_t *p_value_temp;
  uint16_t  u16_value_temp;
  uint8_t   u8_value[4];
  uint16_t test;
  if (!RING_BUFFER_Is_Empty((ring_buffer_t *)s_read_data.p_vol_cur_buffer_irq))
  {
	  u8_value[0] = RING_BUFFER_Pull_Data(
		      (ring_buffer_t *)s_read_data.p_vol_cur_buffer_irq);
	  u16_value_temp = (uint16_t)(s_data_system.u16_adc_value[0]);
	  APP_READ_DATA_ConvertVoltage(u16_value_temp);

	  u16_value_temp = (uint16_t)(s_data_system.u16_adc_value[1]);
	  test = (uint16_t)Kalman_GetValue((float)u16_value_temp);
	  APP_READ_DATA_ConvertCurrent(test);

	  // Transmission Data to app_data_transmission
	  p_value_temp = (uint32_t *)s_read_data.p_voltage;

	  u8_value[0] = (uint8_t)(*p_value_temp >> 24);
	  u8_value[1] = (uint8_t)(*p_value_temp >> 16);
	  u8_value[2] = (uint8_t)(*p_value_temp >> 8);
	  u8_value[3] = (uint8_t)(*p_value_temp >> 0);

	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[0]);
	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[1]);
	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[2]);
	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[3]);

	  p_value_temp = (uint32_t *)s_read_data.p_current;

	  u8_value[0] = (uint8_t)(*p_value_temp >> 24);
	  u8_value[1] = (uint8_t)(*p_value_temp >> 16);
	  u8_value[2] = (uint8_t)(*p_value_temp >> 8);
	  u8_value[3] = (uint8_t)(*p_value_temp >> 0);

	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[0]);
	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[1]);
	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[2]);
	  RING_BUFFER_Push_Data((ring_buffer_t *)s_read_data.p_vol_cur_buffer,
	                        u8_value[3]);
  }
}

/**
 * The function `APP_READ_DATA_ConvertVoltage` converts an ADC value
 * representing voltage to millivolts and then calculates the corresponding
 * voltage value.
 *
 * @param u16_adc_value_voltage The parameter `u16_adc_value_voltage` is a
 * 16-bit unsigned integer representing the ADC value of the voltage being read.
 */
static void
APP_READ_DATA_ConvertVoltage (uint16_t u16_adc_value_voltage)
{
  float f_mVolt;
  f_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(
      3300, u16_adc_value_voltage, LL_ADC_RESOLUTION_12B);

  *s_read_data.p_voltage = f_mVolt * 11.0 / 100.0;
}

static void
APP_READ_DATA_ConvertCurrent (uint16_t u16_adc_value_current)
{
  float f_mVolt;
  f_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(
      3300, u16_adc_value_current, LL_ADC_RESOLUTION_12B);

  f_mVolt = f_mVolt / 1.6;

  *s_read_data.p_current = ACS712_VoltageConverterCurrent(f_mVolt);
}
