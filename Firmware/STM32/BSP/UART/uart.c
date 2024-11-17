/*
 * uart.c
 *
 *  Created on: 24-June-2024
 *      Author: dongkhoa
 */

/*********************
 *      INCLUDES
 *********************/

#include "uart.h"
#include "stm32f1xx_ll_usart.h"

/*********************
 *    PRIVATE DEFINES
 *********************/

#define ATOMIC_BLOCK_START(uart) LL_USART_DisableIT_RXNE(uart)
#define ATOMIC_BLOCK_END(uart)   LL_USART_EnableIT_RXNE(uart)
#define LIMIT_WAIT_BUFFER        1000UL

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * @brief Timeout value for waiting for the buffer to empty.
 */
static volatile uint32_t u32_uart_timeout = 0;

/*********************
 *   PUBLIC FUNCTION
 *********************/

/**
 * @brief Configures the UART and initializes its buffers.
 *
 * @param uartstdio_device Pointer to the UART configuration structure.
 * @param uart             Pointer to the UART peripheral.
 * @param uart_irqn        UART interrupt number.
 */
void
BSP_UART_Config (uart_cfg_t    *uartstdio_device,
                 USART_TypeDef *uart,
                 IRQn_Type      uart_irqn)
{
  uartstdio_device->p_uart      = uart;
  uartstdio_device->e_uart_irqn = uart_irqn;

  RING_BUFFER_Init((ring_buffer_t *)&uartstdio_device->s_tx_buffer);
  RING_BUFFER_Init((ring_buffer_t *)&uartstdio_device->s_rx_buffer);

  /**
   * Enable the UART Error Interrupt:
   * (Frame error, noise error, overrun error)
   */
  LL_USART_EnableIT_ERROR(uart);

  /* Enable the UART Data Register not empty Interrupt */
  LL_USART_EnableIT_RXNE(uart);
}

uint8_t
BSP_UART_IsAvailableDataReceive (uart_cfg_t *uartstdio_device)
{
  if (RING_BUFFER_Is_Empty((ring_buffer_t *)&uartstdio_device->s_rx_buffer))
  {
    return 0;
  }
  return 1;
}

/**
 * @brief Sends a character over UART with error handling for a full transmit
 * buffer.
 *
 * This function attempts to send a character via UART. If the transmit buffer
 * is full,
 * it waits until space is available or a u32_uart_timeout occurs.
 *
 * @param uartstdio_device Pointer to the UART configuration data structure.
 * @param c The character to be sent via UART communication.
 *
 * @return If the u32_uart_timeout reaches 0, the function will return without
 * sending the character.
 */
void
BSP_UART_SendChar (uart_cfg_t *uartstdio_device, char c)
{
  uint32_t u32_uart_timeout = LIMIT_WAIT_BUFFER;

  // Wait until there is space in the transmit buffer or u32_uart_timeout occurs
  while (RING_BUFFER_Is_Full((ring_buffer_t *)&uartstdio_device->s_tx_buffer))
  {
    if (u32_uart_timeout == 0)
    {
      return;
    }
    u32_uart_timeout--;
  }

  // Critical section to ensure atomic access to the buffer
  ATOMIC_BLOCK_START(uartstdio_device->p_uart);
  RING_BUFFER_Push_Data((ring_buffer_t *)&uartstdio_device->s_tx_buffer, c);
  ATOMIC_BLOCK_END(uartstdio_device->p_uart);

  // Enable Transmit Data Register Empty interrupt
  LL_USART_EnableIT_TXE(uartstdio_device->p_uart);
}

/**
 * @brief Sends a string over UART one character at a time.
 *
 * This function iterates over each character in the given string and sends it
 * via UART using the `UART_SendChar` function.
 *
 * @param uartstdio_device Pointer to the UART configuration data structure.
 * @param s Pointer to the constant character array (string) to be sent via
 * UART.
 */
void
BSP_UART_SendString (uart_cfg_t *uartstdio_device, const char *s)
{
  while (*s)
  {
    BSP_UART_SendChar(uartstdio_device, *s++);
  }
}

/**
 * The function `UART_ReadChar` reads a character from a UART device's receive
 * buffer if it is not empty.
 *
 * @param uartstdio_device The `uartstdio_device` parameter is a pointer to a
 * structure of type `uart_cfg_t`. This structure likely contains configuration
 * settings and information related to a UART (Universal Asynchronous
 * Receiver-Transmitter) device, such as baud rate, data bits, parity, stop
 * bits, and pointers to buffers
 *
 * @return The function `UARTReadChar` returns a character that is read from the
 * UART receive buffer. If the receive buffer is not empty, it pulls a character
 * from the buffer and returns it. If the buffer is empty, it returns the null
 * character '\0'.
 */
char
BSP_UART_ReadChar (uart_cfg_t *uartstdio_device)
{
  register char c = '\0';
  if (!RING_BUFFER_Is_Empty((ring_buffer_t *)&uartstdio_device->s_rx_buffer))
  {
    ATOMIC_BLOCK_START(uartstdio_device->p_uart);
    c = (char)RING_BUFFER_Pull_Data((ring_buffer_t *)&uartstdio_device->s_rx_buffer);
    ATOMIC_BLOCK_END(uartstdio_device->p_uart);
    return c;
  }
  return c;
}

/**
 * The function `UART_u32_uart_TimeOut` decrements the variable
 * `u32_uart_timeout` if it is not equal to 0.
 */
void
BSP_UART_TimeOut (void)
{
  if (u32_uart_timeout != 0)
  {
    u32_uart_timeout--;
  }
}

/********************
 *  PRIVATE FUNCTION
 ********************/

/*********************
 * INTERRUPT FUNCTION
 *********************/

/**
 * @brief UART interrupt service routine.
 *
 * This function handles UART interrupts for receiving and transmitting data.
 * It processes received data and stores it in a ring buffer or transmits data
 * from a ring buffer when the transmit data register is empty.
 *
 * @param uartstdio_device Pointer to the UART configuration data structure.
 */
void
BSP_UART_ISR (uart_cfg_t *uartstdio_device)
{
  // If DR is not empty and the Rx Int is enabled
  if ((LL_USART_IsActiveFlag_RXNE(uartstdio_device->p_uart) != RESET)
      && (LL_USART_IsEnabledIT_RXNE(uartstdio_device->p_uart) != RESET))
  {
    uint8_t c = LL_USART_ReceiveData8(uartstdio_device->p_uart);

    // If there are errors, clear the flags
    if ((LL_USART_IsActiveFlag_ORE(uartstdio_device->p_uart) != RESET)
        || (LL_USART_IsActiveFlag_FE(uartstdio_device->p_uart) != RESET)
        || (LL_USART_IsActiveFlag_NE(uartstdio_device->p_uart) != RESET))
    {
      LL_USART_ClearFlag_ORE(uartstdio_device->p_uart);
      LL_USART_ClearFlag_FE(uartstdio_device->p_uart);
      LL_USART_ClearFlag_NE(uartstdio_device->p_uart);
    }
    else
    {
      RING_BUFFER_Push_Data((ring_buffer_t *)&uartstdio_device->s_rx_buffer, c);
    }
    return;
  }

  // If interrupt is caused due to Transmit Data Register Empty
  if ((LL_USART_IsActiveFlag_TXE(uartstdio_device->p_uart) != RESET)
      && (LL_USART_IsEnabledIT_TXE(uartstdio_device->p_uart) != RESET))
  {
    if (RING_BUFFER_Is_Empty((ring_buffer_t *)&uartstdio_device->s_tx_buffer))
    {
      LL_USART_DisableIT_TXE(uartstdio_device->p_uart);
    }
    else
    {
      uint8_t c
          = RING_BUFFER_Pull_Data((ring_buffer_t *)&uartstdio_device->s_tx_buffer);
      LL_USART_TransmitData8(uartstdio_device->p_uart, c);
    }
    return;
  }
}
