/*
 * app_data_transmission.h
 *
 *  Created on: Nov 18, 2024
 *      Author: dongkhoa
 */

#ifndef DATA_TRANSMISSION_APP_DATA_TRANSMISSION_H_
#define DATA_TRANSMISSION_APP_DATA_TRANSMISSION_H_

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

  void APP_DATA_TRANSMISSION_CreateTask(void);
  void APP_DATA_TRANSMISSION_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* DATA_TRANSMISSION_APP_DATA_TRANSMISSION_H_ */
