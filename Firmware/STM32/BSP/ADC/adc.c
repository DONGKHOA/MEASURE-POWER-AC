/*
 * adc.c
 *
 *  Created on: Nov 15, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "adc.h"
#include "stm32f1xx_ll_utils.h"
#include "dma.h"

/******************************************************************************
 *    PRIVATE DEFINES
 *****************************************************************************/

#define LIMIT_WAIT_ADC_OPERATION 1000UL // ms

/******************************************************************************
 *  STATIC VARIABLES
 *****************************************************************************/

static volatile uint32_t u32_adc_timeout = 0;

/******************************************************************************
 *   INLINE PRIVATE FUNCTION
 *****************************************************************************/

static inline void
BSP_ADC_ConversionStop_Disable (ADC_TypeDef *p_ADC)
{
  // Disable the ADC peripheral
  LL_ADC_Disable(p_ADC);

  // Wait for ADC effectively disabled
  u32_adc_timeout = LIMIT_WAIT_ADC_OPERATION;
  while (LL_ADC_IsEnabled(p_ADC) != 0)
  {
    if (u32_adc_timeout == 0)
    {
      return;
    }
  }
}

static inline void
BSP_ADC_Enable (ADC_TypeDef *p_ADC)
{
  if (LL_ADC_IsEnabled(p_ADC) == RESET)
  {
    LL_ADC_Enable(p_ADC);
    // Wait for ADC effectively enabled
    u32_adc_timeout = LIMIT_WAIT_ADC_OPERATION;
    while (LL_ADC_IsEnabled(p_ADC) == RESET)
    {
      if (u32_adc_timeout == 0)
      {
        return;
      }
    }
  }
}

/******************************************************************************
 *   PROTOTYPE PRIVATE FUNCTION
 *****************************************************************************/

/******************************************************************************
 *   PUBLIC FUNCTION
 *****************************************************************************/

void
BSP_ADC_Start_Calibration (ADC_TypeDef *p_ADC)
{
  // 1. Disable ADC peripheral
  BSP_ADC_ConversionStop_Disable(p_ADC);

  // 2. Calibration prerequisite delay before starting the calibration.
  // - ADC must be enabled for at least two ADC clock cycles
  BSP_ADC_Enable(p_ADC);

  // Hardware prerequisite: delay before starting the calibration.
  //  - Computation of CPU clock cycles corresponding to ADC clock cycles.
  //  - Wait for the expected ADC clock cycles delay
  LL_mDelay(10);

  // 3. Resets ADC calibration registers
  p_ADC->CR2 |= ADC_CR2_RSTCAL;

  // Wait for calibration reset completion
  u32_adc_timeout = LIMIT_WAIT_ADC_OPERATION;
  while ((p_ADC->CR2 & ADC_CR2_RSTCAL) != 0)
  {
    if (u32_adc_timeout == 0)
    {
      return;
    }
  }

  // 4. Start ADC calibration
  LL_ADC_StartCalibration(p_ADC);

  // Wait for calibration reset completion
  u32_adc_timeout = LIMIT_WAIT_ADC_OPERATION;
  while ((p_ADC->CR2 & ADC_CR2_CAL) != 0)
  {
    if (u32_adc_timeout == 0)
    {
      return;
    }
  }
}

// Using with >= 2 Channel in ADC
void
BSP_ADC_Start_DMA (ADC_TypeDef *p_ADC, uint16_t *pData, uint32_t Length)
{
  // Enable the ADC peripheral
  BSP_ADC_Enable(p_ADC);

  // Clear previous flags (EOC)
  LL_ADC_ClearFlag_EOS(p_ADC);

  LL_ADC_REG_SetDMATransfer(p_ADC, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

  BSP_DMA_Start_IT(DMA1,
                   LL_DMA_CHANNEL_1,
                   LL_ADC_DMA_GetRegAddr(p_ADC, LL_ADC_DMA_REG_REGULAR_DATA),
                   (uint32_t)pData,
                   Length);

  // Start ADC conversion
  if (LL_ADC_REG_IsTriggerSourceSWStart(p_ADC))
  {
    // If software start is selected
    LL_ADC_REG_StartConversionSWStart(p_ADC);
  }
  else
  {
    // If external trigger is selected
    LL_ADC_REG_StartConversionExtTrig(p_ADC, LL_ADC_REG_TRIG_EXT_RISING);
  }
}

void
BSP_ADC_TimeOut (void)
{
  if (u32_adc_timeout != 0)
  {
    u32_adc_timeout--;
  }
}

/******************************************************************************
 *   PRIVATE FUNCTION
 *****************************************************************************/
