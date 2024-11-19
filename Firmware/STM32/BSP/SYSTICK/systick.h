/*
 * systick.h
 *
 *  Created on: Jul 9, 2024
 *      Author: dongkhoa
 */

#ifndef SYSTICK_SYSTICK_H_
#define SYSTICK_SYSTICK_H_

#ifdef __cplusplus
extern "C"
{
#endif

  /**********************
   *   PUBLIC FUNCTIONS
   **********************/
  void BSP_SYSTICK_TimerStart(void);
  void BSP_SYSTICK_TimerStop(void);
  void BSP_SYSTICK_TimerInit(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTICK_SYSTICK_H_ */
