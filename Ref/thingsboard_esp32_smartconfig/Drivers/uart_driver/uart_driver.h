#ifndef UART_H_
#define UART_H_

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include "driver/uart.h"
#include "driver/gpio.h"

/*********************
 *      DEFINES
 *********************/

#define RX_BUF_SIZE 1024

void uartDriverInit(uart_port_t uart_port, gpio_num_t tx_pin, gpio_num_t rx_pin,
                    uint32_t baudrate,
                    uart_word_length_t data_bit, 
                    uart_parity_t parity_bit, 
                    uart_hw_flowcontrol_t flow_control,
                    uart_stop_bits_t stop_bit);

void uartSendData(uart_port_t uart_port, char * data);

#endif