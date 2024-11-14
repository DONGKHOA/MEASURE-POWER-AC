#ifndef GPIO_H
#define GPIO_H

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include "driver/gpio.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
    GPIO_PULL_UP,
    GPIO_PULL_DOWN,
    GPIO_NO_PULL,
}GPIO_INIT_PULL_t;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void GPIO_Input_Init(gpio_num_t io_pin, GPIO_INIT_PULL_t gpio_init_pull);
void GPIO_Output_Init(gpio_num_t io_pin);
void GPIO_SetState(gpio_num_t io_pin, uint8_t state);
uint8_t GPIO_GetState(gpio_num_t io_pin);

#endif /* GPIO_H */ 