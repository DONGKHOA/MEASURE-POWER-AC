/*
 * gpio.h
 *
 *  Created on: Oct 1, 2024
 *      Author: dongkhoa
 */

#ifndef GPIO_GPIO_H_
#define GPIO_GPIO_H_

/*********************
 *      INCLUDES
 *********************/

#include "stm32f1xx_ll_gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**********************
   *    PUBLIC TYPEDEFS
   **********************/
  typedef enum gpio_state
  {
    GPIO_LOW  = 0,
    GPIO_HIGH = 1,
  } gpio_state_t;

  /**********************
   *   PUBLIC FUNCTIONS
   **********************/

  /**
   * The function `BSP_GPIO_SetState` sets the state of a GPIO pin based on the
   * specified state value.
   *
   * @param gpio_port GPIO port to which the GPIO pin belongs (e.g., GPIOA,
   * GPIOB, etc.).
   * @param gpio_pin The `gpio_pin` parameter represents the specific pin on the
   * GPIO port that you want to set the state for.
   * @param state The `state` parameter in the `BSP_GPIO_SetState` function is
   * of type `gpio_state_t`, which is used to specify whether the GPIO pin
   * should be set to a low state (`GPIO_LOW`) or a high state (`GPIO_HIGH`).
   */
  static inline void BSP_GPIO_SetState (GPIO_TypeDef *gpio_port,
                                        uint32_t      gpio_pin,
                                        gpio_state_t  state)
  {
    if (state == GPIO_LOW)
    {
      gpio_port->BSRR = gpio_pin;
    }
    else
    {
      gpio_port->BSRR = gpio_pin << 16;
    }
  }

  static inline void BSP_GPIO_Toggle (GPIO_TypeDef *gpio_port,
                                      uint32_t      gpio_pin)
  {
    uint32_t odr     = gpio_port->ODR;
    uint32_t pinmask = ((gpio_pin >> GPIO_PIN_MASK_POS) & 0x0000FFFFU);
    gpio_port->BSRR  = ((odr & pinmask) << 16u) | (~odr & pinmask);
  }

  /**
   * The function `BSP_GPIO_GetState` returns the state of a GPIO pin as either
   * GPIO_HIGH or GPIO_LOW.
   *
   * @param gpio_port GPIO port pointer (e.g., GPIOA, GPIOB, etc.) where the pin
   * is located.
   * @param gpio_pin The `gpio_pin` parameter is the pin number of the GPIO port
   * for which you want to get the state. It is a 32-bit unsigned integer
   * representing the specific pin on the GPIO port.
   *
   * @return The function `BSP_GPIO_GetState` returns a value of type
   * `gpio_state_t`, which is either `GPIO_HIGH` or `GPIO_LOW` based on the
   * input pin state of the specified GPIO port and pin.
   */
  static inline gpio_state_t BSP_GPIO_GetState (GPIO_TypeDef *gpio_port,
                                                uint32_t      gpio_pin)
  {
    if ((gpio_port->IDR & gpio_pin) == gpio_pin)

    {
      return GPIO_HIGH;
    }
    return GPIO_LOW;
  }

#ifdef __cplusplus
}
#endif

#endif /* GPIO_GPIO_H_ */
