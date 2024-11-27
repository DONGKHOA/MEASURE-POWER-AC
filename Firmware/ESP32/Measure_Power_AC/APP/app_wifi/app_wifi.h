#ifndef APP_WIFI_APP_WIFI_H_
#define APP_WIFI_APP_WIFI_H_

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

  void APP_WIFI_CreateTask(void);
  void APP_WIFI_CreateTimer(void);
  void APP_WIFI_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_WIFI_APP_WIFI_H_ */
