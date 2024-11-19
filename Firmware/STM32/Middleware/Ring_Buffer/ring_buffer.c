/*
 * ring_buffer.c
 *
 *  Created on: Jul 22, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "ring_buffer.h"

/*********************
 *   PUBLIC FUNCTION
 *********************/

/**
 * The function `RING_BUFFER_Init` initializes a ring buffer by setting the
 * input and output indices to 0 and the count to 0.
 *
 * @param rb The parameter `rb` is a pointer to a `ring_buffer_t` structure,
 * which is used to implement a queue data structure.
 */
void
RING_BUFFER_Init (ring_buffer_t *rb)
{
  rb->u32_in    = 0;
  rb->u32_out   = 0;
  rb->u32_count = 0;
}

/**
 * @brief Resets the ring buffer to an empty state.
 *
 * This function resets the indices and count of the given ring buffer,
 * making it empty.
 *
 * @param rb Pointer to the ring buffer to reset.
 */
void
RING_BUFFER_Reset (ring_buffer_t *rb)
{
  rb->u32_in    = 0;
  rb->u32_out   = 0;
  rb->u32_count = 0;
}

/**
 * The function `RING_BUFFER_Is_Empty` checks if a ring buffer is empty.
 *
 * @param rb The parameter `rb` is a pointer to a `ring_buffer_t` structure.
 *
 * @return The function `RING_BUFFER_Is_Empty` returns a boolean value
 * indicating whether the ring buffer `rb` is empty or not. It returns `true` if
 * the count of elements in the ring buffer is 0, indicating that the buffer is
 * empty, and `false` otherwise.
 */
bool
RING_BUFFER_Is_Empty (ring_buffer_t *rb)
{
  return (rb->u32_count == 0);
}

/**
 * The function checks if a ring buffer is full based on its count and size.
 *
 * @param rb The `rb` parameter is a pointer to a `ring_buffer_t` structure,
 * which is likely used to implement a ring buffer data structure for managing
 * data in a circular buffer fashion.
 *
 * @return The function `RING_BUFFER_Is_Full` is returning a boolean value,
 * either `true` or `false`, based on whether the count of elements in the ring
 * buffer `rb` is equal to the predefined size `RING_BUFFER_SIZE`.
 */
bool
RING_BUFFER_Is_Full (ring_buffer_t *rb)
{
  return (rb->u32_count == (uint32_t)RING_BUFFER_SIZE);
}

/**
 * The function `RING_BUFFER_Push_Data` adds an element to a ring buffer and
 * updates the buffer's internal variables.
 *
 * @param rb The parameter `rb` is a pointer to a structure of type
 * `ring_buffer_t`.
 * @param element The `element` parameter in the `RING_BUFFER_Push_Data`
 * function represents the data element that needs to be added to the ring
 * buffer. It is of type `char`, which means it can store a single character (1
 * byte) of data.
 */
void
RING_BUFFER_Push_Data (ring_buffer_t *rb, char element)
{
  if (RING_BUFFER_Is_Full(rb))
  {
    RING_BUFFER_Reset(rb);
  }
  *(rb->u8_buffer + rb->u32_in) = element;

  rb->u32_in = (rb->u32_in + 1) & ((uint32_t)RING_BUFFER_SIZE - 1);
  rb->u32_count++;
}

/**
 * The function `RING_BUFFER_Pull_Data` pulls data from a ring buffer and
 * updates the buffer's out index and count.
 *
 * @param rb The parameter `rb` is a pointer to a structure of type
 * `ring_buffer_t`.
 *
 * @return The function `RING_BUFFER_Pull_Data` is returning a `uint8_t` data
 * value from the ring buffer pointed to by the `rb` parameter.
 */
uint8_t
RING_BUFFER_Pull_Data (ring_buffer_t *rb)
{
  if (RING_BUFFER_Is_Empty(rb))
  {
    return 0xFF;
  }
  register uint8_t data = *(rb->u8_buffer + rb->u32_out);

  rb->u32_out = (rb->u32_out + 1) & ((uint32_t)RING_BUFFER_SIZE - 1);
  rb->u32_count--;

  return data;
}
