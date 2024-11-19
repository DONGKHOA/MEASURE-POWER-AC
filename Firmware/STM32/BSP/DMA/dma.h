/*
 * dma.h
 *
 *  Created on: Nov 15, 2024
 *      Author: dongkhoa
 */

#ifndef DMA_DMA_H_
#define DMA_DMA_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx_ll_dma.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC TYPEDEFS
   ***************************************************************************/

  typedef enum _flagIRQ_dma_t
  {
    FLAG_NONE = 0,
    FLAG_HALF_TRANSFER_COMPLETE,
    FLAG_TRANSFER_COMPLETE,
    FLAG_TRANSFER_ERROR
  } flagIRQ_dma_t;

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

  void          BSP_DMA_Start_IT(DMA_TypeDef *p_DMA,
                                 uint32_t     Channel,
                                 uint32_t     SrcAddress,
                                 uint32_t     DstAddress,
                                 uint32_t     DataLength);
  flagIRQ_dma_t BSP_DMA_IRQ_Channel1_Handler(DMA_TypeDef *p_DMA);

#ifdef __cplusplus
}
#endif

#endif /* DMA_DMA_H_ */
