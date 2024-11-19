/*
 * app_data.h
 *
 *  Created on: Nov 17, 2024
 *      Author: dongkhoa
 */

#ifndef DATA_APP_DATA_H_
#define DATA_APP_DATA_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"
#include "uart.h"
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC TYPEDEFS
   ***************************************************************************/

  typedef struct _DATA_READ_DataTypedef
  {
    ring_buffer_t s_PF_buffer_irq;
    ring_buffer_t s_PF_buffer;
    ring_buffer_t s_vol_cur_buffer_irq;
    ring_buffer_t s_vol_cur_buffer;
    uart_cfg_t    s_uart_data_trans;
    uart_cfg_t    s_uart_cmd;
    float         f_voltage; // amplitude
    float         f_current; // amplitude
    float         f_delta_T_PF;
    uint16_t      u16_adc_value[2];
  } DATA_READ_DataTypedef;

  /****************************************************************************
   *   PUBLIC DATA
   ***************************************************************************/

  extern volatile DATA_READ_DataTypedef s_data_system;

#ifdef __cplusplus
}
#endif

#endif /* DATA_APP_DATA_H_ */
