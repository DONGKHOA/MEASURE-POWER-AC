/*
 * ring_buffer.h
 *
 *  Created on: Jul 22, 2024
 *      Author: dongkhoa
 */

#ifndef UART_BSP_RING_BUFFER_H_
#define UART_BSP_RING_BUFFER_H_

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /*********************
   *    PUBLIC DEFINES
   *********************/

#define RING_BUFFER_SIZE 256

/*********************
 *    PUBLIC TYPEDEFS
 *********************/

  /**
   * @brief Structure to hold ring buffer data.
   */
  typedef struct ring_buffer
  {
    volatile uint8_t u8_buffer[RING_BUFFER_SIZE]; /**< @brief Buffer array. */
    volatile uint32_t u32_in;    /**< @brief Input index. */
    volatile uint32_t u32_out;   /**< @brief Output index. */
    volatile uint32_t u32_count; /**< @brief Count of items in buffer. */
  } ring_buffer_t;

  /*********************
   *   PUBLIC FUNCTION
   *********************/

  void    RING_BUFFER_Init(ring_buffer_t *rb);
  void    RING_BUFFER_Reset(ring_buffer_t *rb);
  bool    RING_BUFFER_Is_Empty(ring_buffer_t *rb);
  bool    RING_BUFFER_Is_Full(ring_buffer_t *rb);
  void    RING_BUFFER_Push_Data(ring_buffer_t *rb, char element);
  uint8_t RING_BUFFER_Pull_Data(ring_buffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* UART_BSP_RING_BUFFER_H_ */
