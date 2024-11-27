/*
 * app_data_trans_rec.h
 *
 *  Created on: Nov 18, 2024
 *      Author: dongkhoa
 */

#ifndef DATA_TRANS_REC_APP_DATA_TRANS_REC_H_
#define DATA_TRANS_REC_APP_DATA_TRANS_REC_H_

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

  void APP_DATA_TRANS_REC_CreateTask(void);
  void APP_DATA_TRANS_REC_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* DATA_TRANS_REC_APP_DATA_TRANS_REC_H_ */
