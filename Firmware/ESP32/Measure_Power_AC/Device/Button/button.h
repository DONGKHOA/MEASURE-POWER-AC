#ifndef BUTTON_H
#define BUTTON_H

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include "driver/gpio.h"

#include "exti.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC TYPEDEFS
   ***************************************************************************/

  typedef void (*BUTTON_CALLBACK_FUNCTION_t)(int pin);

  /****************************************************************************
   *   PUBLIC DATA
   ***************************************************************************/

  typedef struct
  {
    uint32_t gpio_pin;
    uint64_t debouncing_timer;
    uint64_t pressing_timer;
    uint8_t  is_pressing;
  } BUTTON_HANDLE_t;

  void BUTTON_Init(BUTTON_HANDLE_t *button, uint32_t pin);
  void BUTTON_SetCallbackFunction(BUTTON_CALLBACK_FUNCTION_t PressingCallback,
                                  BUTTON_CALLBACK_FUNCTION_t ReleasingCallback);
  void BUTTON_EXTI_Handle(BUTTON_HANDLE_t *button, uint32_t pin);
#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H*/