/*********************
 *      INCLUDES
 *********************/

#include "gpio.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * The function `GPIO_Input_Init` initializes a GPIO pin as an input with optional pull-up or pull-down
 * resistor configuration.
 * 
 * @param pin The `pin` parameter is of type `gpio_num_t`, which is typically an enumeration
 * representing the GPIO pin number on a microcontroller. It is used to specify the GPIO pin for which
 * the input configuration needs to be initialized.
 * @param gpio_res_pull The `gpio_res_pull` parameter is used to specify the pull configuration for the
 * GPIO pin. It can have two values:
 */
void GPIO_Input_Init(gpio_num_t pin, GPIO_INIT_PULL_t gpio_res_pull)
{
    if(gpio_res_pull == GPIO_NO_PULL)
    {
        gpio_config_t io_conf = 
        {
            .pin_bit_mask   =   (1 << pin),
            .mode           =   GPIO_MODE_INPUT,
            .pull_up_en     =   !gpio_res_pull,
            .pull_down_en   =   !gpio_res_pull,
            .intr_type      =   GPIO_INTR_DISABLE,
        };
        gpio_config(&io_conf);
    }
    else
    {
        gpio_config_t io_conf = 
        {
            .pin_bit_mask   =   (1 << pin),
            .mode           =   GPIO_MODE_INPUT,
            .pull_up_en     =   !gpio_res_pull,
            .pull_down_en   =   gpio_res_pull,
            .intr_type      =   GPIO_INTR_DISABLE,
        };
        gpio_config(&io_conf);
    }
}

/**
 * The function `GPIO_Output_Init` initializes a GPIO pin as an output with specific configuration
 * settings.
 * 
 * @param io_pin The `io_pin` parameter is the GPIO pin number that you want to initialize as an output
 * pin.
 */
void GPIO_Output_Init(gpio_num_t io_pin)
{
    gpio_config_t io_config = 
    {
        .mode           = GPIO_MODE_OUTPUT,
        .pin_bit_mask   = (1 << io_pin),
        .pull_up_en     = GPIO_PULLUP_DISABLE,
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .intr_type      = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_config);
}

/**
 * The function GPIO_SetState sets the state of a GPIO pin to a specified level.
 * 
 * @param io_pin The `io_pin` parameter represents the GPIO pin number that you want to set the state
 * for.
 * @param state The `state` parameter in the `GPIO_SetState` function is a uint8_t type variable that
 * represents the desired state to set for the specified GPIO pin. It can have a value of either 0 or
 * 1, where 0 typically represents a low state (logic 0) and
 */
void GPIO_SetState(gpio_num_t io_pin, uint8_t state)
{
    gpio_set_level(io_pin, state);
}

/**
 * The function `GPIO_GetState` returns the state of a specified GPIO pin.
 * 
 * @param io_pin The `io_pin` parameter is of type `gpio_num_t`, which is typically used to represent a
 * GPIO pin number in embedded systems programming. It is an enumeration or typedef that maps to the
 * physical GPIO pin numbers on the microcontroller or development board you are working with. When you
 * call `GPIO
 * 
 * @return The function `GPIO_GetState` is returning the state of the GPIO pin specified by `io_pin`.
 * It is using the `gpio_get_level` function to get the level of the GPIO pin and returning it as a
 * `uint8_t` value.
 */
uint8_t GPIO_GetState(gpio_num_t io_pin)
{
    return gpio_get_level(io_pin);
}