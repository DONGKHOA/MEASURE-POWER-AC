/*
 * systick.c
 *
 *  Created on: Jul 9, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "systick.h"
#include "stm32f1xx.h"

/******************************************************************************
 *     PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief Start the SysTick timer.
 *
 * This function enables the SysTick timer.
 */
void
BSP_SYSTICK_TimerStart (void)
{

  SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

/**
 * @brief Stop the SysTick timer.
 *
 * This function disables the SysTick timer.
 */
void
BSP_SYSTICK_TimerStop (void)
{
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief Initialize the SysTick timer to trigger interrupts every 1
 * millisecond.
 *
 * This function configures the SysTick timer to generate interrupts at a rate
 * of 1 millisecond, based on the system core clock.
 */
void
BSP_SYSTICK_TimerInit (void)
{
  SysTick_Config(SystemCoreClock / 1000);
}
