/*
 * app_calculator_PF.h
 *
 *  Created on: Nov 17, 2024
 *      Author: dongkhoa
 */

#ifndef CALCULATOR_PF_APP_CALCULATOR_PF_H_
#define CALCULATOR_PF_APP_CALCULATOR_PF_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"
#include "dma.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

  void APP_CALCULATOR_PF_CreateTask(void);
  void APP_CALCULATOR_PF_Init(void);

  void APP_CALCULATOR_PF_EXTI_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* CALCULATOR_PF_APP_CALCULATOR_PF_H_ */
