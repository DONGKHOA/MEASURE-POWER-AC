/*
 * adc.h
 *
 *  Created on: Nov 15, 2024
 *      Author: dongkhoa
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx_ll_adc.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /*********************
   *   PUBLIC FUNCTION
   *********************/

  void BSP_ADC_Start_Calibration(ADC_TypeDef *p_ADC);
  void BSP_ADC_Start_DMA(ADC_TypeDef *p_ADC, uint32_t *pData, uint32_t Length);

  // Call Function "BSP_ADC_TimeOut" in ISR SysTick
  void BSP_ADC_TimeOut(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_ADC_H_ */
