/*
 * basetypedef.h
 *
 *  Created on: Jul 9, 2024
 *      Author: dongkhoa
 */

#ifndef COMMON_INCLUDE_BASETYPEDEF_H_
#define COMMON_INCLUDE_BASETYPEDEF_H_

#ifdef __cplusplus
extern "C"
{
#endif

  /*********************
   *      INCLUDES
   *********************/

#include <stdint.h>

  /**********************
   *    PUBLIC TYPEDEFS
   **********************/

  /**
   * @brief Enumeration defining status codes.
   */
  typedef enum _status_t
  {
    STS_FIRST = 0,          /**< @brief First status code. */
    STS_DONE  = STS_FIRST,  /**< @brief Operation completed successfully. */
    STS_ERROR,              /**< @brief General error status. */
    STS_ITEMS_NOT_FOUND,    /**< @brief Items not found error. */
    STS_INVALID_PARAMETERS, /**< @brief Invalid parameters error. */
    STS_OUT_OF_HEAP,        /**< @brief Out of heap memory error. */
    STS_CRC_ERROR,          /**< @brief CRC error. */
    STS_OUT_OF_MEMORY,      /**< @brief Out of memory error. */
    STS_INVALID_RESPONSE,   /**< @brief Invalid response error. */
    STS_MEM_WRITE_FAILED,   /**< @brief Memory write operation failed error.
                             */
    STS_MEM_READ_FAILED,    /**< @brief Memory read operation failed error. */
    STS_NOT_ENABLED,        /**< @brief Feature or item not enabled error. */
    STS_LAST = STS_NOT_ENABLED /**< @brief Last status code. */
  } status_t;

  typedef enum
  {
    NO  = 0,
    YES = !NO
  } YesNoStatus;

  typedef enum
  {
    STOP  = 0,
    START = !STOP
  } StartStopStatus;

  typedef enum
  {
    CLOSE = 0,
    OPEN  = !CLOSE
  } OpenCloseStatus;

/*********************
 *    PUBLIC DEFINES
 *********************/
#define TRUE  (uint16_t)(1 == 1)
#define FALSE (uint16_t)(1 == 0)
#ifndef NULL
#define NULL 0
#endif

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

/* Exported macro ----------------------------------------------------------*/
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INCLUDE_BASETYPEDEF_H_ */
