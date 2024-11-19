/*
 * timer_irq.h
 *
 *  Created on: Oct 16, 2024
 *      Author: dongkhoa
 */

#ifndef TIMER_IRQ_TIMER_IRQ_H_
#define TIMER_IRQ_TIMER_IRQ_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

static inline void BSP_TIM_Stop_IT (TIM_TypeDef *p_tim)
{
	p_tim->DIER &= ~TIM_DIER_UIE;
	p_tim->CR1 &= ~TIM_CR1_CEN;
}

static inline void BSP_TIM_Start_IT (TIM_TypeDef *p_tim)
{
	p_tim->DIER |= TIM_DIER_UIE;
	p_tim->CR1 |= TIM_CR1_CEN;
}

static inline void BSP_TIM_IRQ_Handler (TIM_TypeDef *p_tim)
{
	p_tim->DIER &= ~TIM_DIER_UIE;
}


#ifdef __cplusplus
}
#endif


#endif /* TIMER_IRQ_TIMER_IRQ_H_ */
