/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

#include "uart_driver.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * The function initializes a UART driver with specified configurations such as baud rate, data bits,
 * parity, flow control, and stop bits.
 * 
 * @param uart_port The `uart_port` parameter specifies which UART port to initialize. It could be
 * something like `UART_NUM_0` or `UART_NUM_1` depending on the specific UART hardware you are working
 * with.
 * @param tx_pin The `tx_pin` parameter in the `uartDriverInit` function refers to the GPIO pin number
 * that is used for transmitting data in UART communication. This pin is connected to the transmit (TX)
 * line of the UART interface and is responsible for sending data out from the microcontroller to
 * another device.
 * @param rx_pin The `rx_pin` parameter in the `uartDriverInit` function is the GPIO pin number to
 * which the receive (RX) pin of the UART interface is connected. This pin is used for receiving data
 * from an external device or another UART transmitter.
 * @param baudrate Baudrate refers to the number of symbols or signal changes transmitted per second in
 * a communication channel. It is typically measured in bits per second (bps) or baud. The baudrate
 * parameter in the function `uartDriverInit` specifies the desired transmission speed for the UART
 * communication.
 * @param data_bit The `data_bit` parameter in the `uartDriverInit` function specifies the number of
 * data bits per frame. It determines the size of the data being transmitted or received over the UART
 * communication. The options for `data_bit` typically include values like `UART_DATA_5_BITS`,
 * `UART_DATA
 * @param parity_bit The `parity_bit` parameter in the `uartDriverInit` function specifies the type of
 * parity used for error-checking in UART communication. It can be one of the following values:
 * @param flow_control Flow control is a mechanism used in communication protocols to manage the flow
 * of data between two devices. It can be either hardware flow control or software flow control.
 * @param stop_bit The `stop_bit` parameter in the `uartDriverInit` function refers to the number of
 * stop bits used in UART communication. It specifies the number of bits that signal the end of a data
 * frame. Common values for `stop_bit` include `UART_STOP_BITS_1` for one stop bit
 */
void uartDriverInit(uart_port_t uart_port, gpio_num_t tx_pin, gpio_num_t rx_pin,
                    uint32_t baudrate,
                    uart_word_length_t data_bit, 
                    uart_parity_t parity_bit, 
                    uart_hw_flowcontrol_t flow_control,
                    uart_stop_bits_t stop_bit)
{
    const uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = data_bit,
        .parity = parity_bit,
        .stop_bits = stop_bit,
        .flow_ctrl = flow_control,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // using buffer in rx data
    uart_driver_install(uart_port, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(uart_port, &uart_config);
    uart_set_pin(uart_port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

/**
 * The function `uartSendData` sends data over a UART port by writing the specified data bytes.
 * 
 * @param uart_port The `uart_port` parameter is the UART port or interface through which the data will
 * be sent. It is typically a hardware-specific identifier that specifies which UART peripheral to use
 * for communication.
 * @param data The `data` parameter in the `uartSendData` function is a pointer to a character array
 * (string) that contains the data to be sent via UART (Universal Asynchronous Receiver-Transmitter)
 * communication.
 */
void uartSendData(uart_port_t uart_port, char* data)
{
    const int len = strlen(data);
    uart_write_bytes(uart_port, data, len);
}