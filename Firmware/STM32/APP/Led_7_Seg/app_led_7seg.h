/*
 * app_led_7seg.h
 *
 *  Created on: Nov 22, 2024
 *      Author: dongkhoa
 */

#ifndef LED_7_SEG_APP_LED_7SEG_H_
#define LED_7_SEG_APP_LED_7SEG_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"
#include "uart.h"
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif


  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

void APP_LED_7_SEG_Init(void);
void APP_LED_7_SEG_CreateTask(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_7_SEG_APP_LED_7SEG_H_ */
