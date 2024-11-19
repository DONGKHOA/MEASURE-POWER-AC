/*
 * app_read_data.h
 *
 *  Created on: Nov 18, 2024
 *      Author: dongkhoa
 */

#ifndef READ_PEEK_VOLTAGE_CURRENT_APP_READ_DATA_H_
#define READ_PEEK_VOLTAGE_CURRENT_APP_READ_DATA_H_

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
   *   PUBLIC DEFINES
   ***************************************************************************/

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

  void APP_READ_DATA_CreateTask(void);
  void APP_READ_DATA_Init(void);

  void APP_READ_DATA_HandleDMA_IRQ(flagIRQ_dma_t status);

#ifdef __cplusplus
}
#endif

#endif /* READ_PEEK_VOLTAGE_CURRENT_APP_READ_DATA_H_ */
