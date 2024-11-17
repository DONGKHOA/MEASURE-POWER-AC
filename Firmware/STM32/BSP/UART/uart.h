/*
 * uart.h
 *
 *  Created on: 24-June-2024
 *      Author: dongkhoa
 */

#ifndef UART_UART_H_
#define UART_UART_H_

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx.h"
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /**********************
   *    PUBLIC TYPEDEFS
   **********************/

  /**
   * @brief Structure to hold UART configuration data.
   */
  typedef struct uart_cfg
  {
    volatile ring_buffer_t s_rx_buffer; /**< @brief Receive ring buffer. */
    volatile ring_buffer_t s_tx_buffer; /**< @brief Transmit ring buffer. */
    USART_TypeDef         *p_uart;      /**< @brief Pointer to UART
                                           peripheral. */
    IRQn_Type e_uart_irqn;              /**< @brief UART interrupt number. */
  } uart_cfg_t;

  /*********************
   *   PUBLIC FUNCTION
   *********************/

  void    BSP_UART_Config(uart_cfg_t    *uartstdio_device,
                          USART_TypeDef *uart,
                          IRQn_Type      uart_irqn);
  uint8_t BSP_UART_IsAvailableDataReceive(uart_cfg_t *uartstdio_device);
  void    BSP_UART_SendChar(uart_cfg_t *uartstdio_device, char c);
  void    BSP_UART_SendString(uart_cfg_t *uartstdio_device, const char *s);
  char    BSP_UART_ReadChar(uart_cfg_t *uartstdio_device);

  // Call Function "BSP_UART_TimeOut" in ISR SysTick
  void BSP_UART_TimeOut(void);

  // Call Function "BSP_UART_ISR" in ISR UART
  void BSP_UART_ISR(uart_cfg_t *uartstdio_device);

#ifdef __cplusplus
}
#endif

#endif /* UART_UART_H_ */
