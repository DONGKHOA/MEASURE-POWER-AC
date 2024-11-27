#ifndef APP_SMART_CONFIG_H_
#define APP_SMART_CONFIG_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/
#include "smartconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

void APP_SmartConfig_CreateTask(void);
void APP_SmartConfig_Init(void);

#ifdef __cplusplus
}
#endif

#endif  /* APP_SMART_CONFIG_H_ */