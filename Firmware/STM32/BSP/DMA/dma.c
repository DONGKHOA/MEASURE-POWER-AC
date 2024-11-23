/*
 * dma.c
 *
 *  Created on: Nov 15, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "dma.h"

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

/**
 * The function BSP_DMA_Start_IT initializes and starts a DMA transfer with
 * interrupt handling enabled.
 *
 * @param p_DMA The parameter `p_DMA` is a pointer to the DMA controller
 * (DMA_TypeDef) that you want to configure for data transfer.
 * @param Channel The `Channel` parameter specifies the DMA channel to be used
 * for the transfer. It is typically a number that corresponds to a specific DMA
 * channel on the microcontroller.
 * @param SrcAddress The source address where the data transfer will start.
 * @param DstAddress The `DstAddress` parameter in the `BSP_DMA_Start_IT`
 * function represents the destination address where the data will be
 * transferred. It is the memory address where the data will be written to
 * during the DMA transfer operation.
 * @param DataLength DataLength is the length of the data to be transferred in
 * the DMA operation. It specifies the number of data items to be transferred
 * from the source address to the destination address.
 */
void
BSP_DMA_Start_IT (DMA_TypeDef *p_DMA,
                  uint32_t     Channel,
                  uint32_t     SrcAddress,
                  uint32_t     DstAddress,
                  uint32_t     DataLength)
{
  LL_DMA_DisableChannel(p_DMA, Channel);

  LL_DMA_ConfigAddresses(p_DMA,
                         Channel,
                         SrcAddress,
                         (uint32_t)DstAddress,
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetDataLength(p_DMA, Channel, DataLength);

//  LL_DMA_EnableIT_HT(p_DMA, Channel);
  LL_DMA_EnableIT_TC(p_DMA, Channel);
  LL_DMA_EnableIT_TE(p_DMA, Channel);

  LL_DMA_EnableChannel(p_DMA, Channel);
}

/**
 * The function `BSP_DMA_IRQ_Channel1_Handler` handles interrupts for a DMA
 * channel, managing half transfer, transfer complete, and transfer error
 * events.
 *
 * @param p_DMA The parameter `p_DMA` is a pointer to a structure of type
 * `DMA_TypeDef`, which is likely a data structure representing a Direct Memory
 * Access (DMA) controller in a micro-controller or embedded system.
 */
flagIRQ_dma_t
BSP_DMA_IRQ_Channel1_Handler (DMA_TypeDef *p_DMA)
{
  /* Half Transfer Complete Interrupt management ******************************/
  if (LL_DMA_IsActiveFlag_HT1(p_DMA))
  {
    // Disable the half transfer interrupt if the DMA mode is not CIRCULAR
    if (LL_DMA_GetMode(p_DMA, LL_DMA_CHANNEL_1) == LL_DMA_MODE_NORMAL)
    {
      LL_DMA_DisableIT_HT(p_DMA, LL_DMA_CHANNEL_1);
    }
    LL_DMA_ClearFlag_HT1(p_DMA);
    return FLAG_HALF_TRANSFER_COMPLETE;
  }
  /* Transfer Complete Interrupt management ***********************************/
  else if (LL_DMA_IsActiveFlag_TC1(p_DMA))
  {
    // Disable the half transfer interrupt if the DMA mode is not CIRCULAR
    if (LL_DMA_GetMode(p_DMA, LL_DMA_CHANNEL_1) == LL_DMA_MODE_NORMAL)
    {
      LL_DMA_DisableIT_TC(p_DMA, LL_DMA_CHANNEL_1);
    }
    LL_DMA_ClearFlag_TC1(p_DMA);
    return FLAG_TRANSFER_COMPLETE;
  }
  /* Transfer Error Interrupt management **************************************/
  else if (LL_DMA_IsActiveFlag_TE1(p_DMA))
  {
    // Disable the half transfer interrupt if the DMA mode is not CIRCULAR
    if (LL_DMA_GetMode(p_DMA, LL_DMA_CHANNEL_1) == LL_DMA_MODE_NORMAL)
    {
      LL_DMA_DisableIT_TE(p_DMA, LL_DMA_CHANNEL_1);
    }
    LL_DMA_ClearFlag_TE1(p_DMA);
    return FLAG_TRANSFER_ERROR;
  }
  return FLAG_NONE;
}
