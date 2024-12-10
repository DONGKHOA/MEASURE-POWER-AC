#ifndef APP_RTC_H_
#define APP_RTC_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

  void APP_Rtc_CreateTask(void);
  void APP_Rtc_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_RTC_H_ */