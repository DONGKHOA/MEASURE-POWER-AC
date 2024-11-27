/*
 * app_status_led.h
 *
 *  Created on: Nov 25, 2024
 *      Author: dongkhoa
 */

#ifndef STATUS_LED_APP_STATUS_LED_H_
#define STATUS_LED_APP_STATUS_LED_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

  void APP_STATUS_LED_Init(void);
  void APP_STATUS_LED_CreateTimer(void);
  void APP_STATUS_LED_CreateTask(void);

#ifdef __cplusplus
}
#endif

#endif /* STATUS_LED_APP_STATUS_LED_H_ */
