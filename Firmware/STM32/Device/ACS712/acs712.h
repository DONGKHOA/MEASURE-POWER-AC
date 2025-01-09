/*
 * acs712.h
 *
 *  Created on: Nov 21, 2024
 *      Author: dongkhoa
 */

#ifndef ACS712_ACS712_H_
#define ACS712_ACS712_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 *      DEFINES
 *****************************************************************************/

#define ACS712_0_30A 1
#define ACS712_0_20A 0
#define ACS712_0_5A 0

  /**********************
   *   PUBLIC FUNCTIONS
   **********************/

// Unit Voltage mV
  static inline float ACS712_VoltageConverterCurrent(float f_mVoltage)
  {
#if  ACS712_0_30A
	  return (f_mVoltage) / 64.37;
#endif
#if  ACS712_0_20A
	  return f_mVoltage / 100.0;
#endif
#if  ACS712_0_5A
	  return f_mVoltage / 185.0;
#endif
  }

#ifdef __cplusplus
}
#endif


#endif /* ACS712_ACS712_H_ */
