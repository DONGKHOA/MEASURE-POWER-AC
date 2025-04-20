#ifndef EXTI_H_
#define EXTI_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

/*********************
 *      DEFINES
 *********************/

typedef void (*EXTI_CALLBACK_FUNCTION_t)(uint32_t pin);

typedef enum
{
  EXTI_EDGE_RISING  = 1,
  EXTI_EDGE_FALLING = 2,
  EXTI_EDGE_ANY     = 3,
} EXTI_EDGE_t;

void BSP_EXTI_Init(uint32_t pin, EXTI_EDGE_t edge);
void BSP_EXTI_SetCallbackFunction(EXTI_CALLBACK_FUNCTION_t CallbackFunction);

#endif