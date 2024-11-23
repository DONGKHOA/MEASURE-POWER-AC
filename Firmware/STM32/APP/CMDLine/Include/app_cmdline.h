/*
 * app_cmdline.h
 *
 *  Created on: Jul 8, 2024
 *      Author: dongkhoa
 */

#ifndef CMDLINE_INCLUDE_CMDLINE_H_
#define CMDLINE_INCLUDE_CMDLINE_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 *    PUBLIC DEFINES
 *****************************************************************************/

/**
 * @brief Defines the value that is returned if the command is OK.
 */
#define CMDLINE_OK (0)

/**
 * @brief Defines the value that is returned if the command is not found.
 */
#define CMDLINE_BAD_CMD (1)

/**
 * @brief Defines the value that is returned if there are too many arguments.
 */
#define CMDLINE_TOO_MANY_ARGS (2)

/**
 * @brief Defines the value that is returned if there are too few arguments.
 */
#define CMDLINE_TOO_FEW_ARGS (3)

/**
 * @brief Defines the value that is returned if an argument is invalid.
 */
#define CMDLINE_INVALID_ARG (4)

  /****************************************************************************
   *   PUBLIC TYPEDEFS
   ***************************************************************************/

  /**
   * @brief Command line function callback type.
   *
   * This typedef defines the prototype for a command line function that
   * will be called with a set of arguments.
   *
   * @param argc The number of arguments.
   * @param argv An array of pointers to the arguments as strings.
   *
   * @return An integer status code from the command.
   */
  typedef int (*pfnCmdLine)(int argc, char *argv[]);

  /**
   * @brief Structure for an entry in the command list table.
   */
  typedef struct
  {
    const char *pcCmd;  /**< @brief A pointer to a string containing the name
                            of the command.*/
    pfnCmdLine pfnCmd;  /**< @brief A function pointer to the implementation
                            of the command.*/
    const char *pcHelp; /**< @brief A pointer to a string of brief help text
                            for the command. */
  } tCmdLineEntry;

  /****************************************************************************
   *   PUBLIC DATA
   ***************************************************************************/

  /**
   * @brief Command table provided by the application.
   *
   * The last element of the array must be a structure whose pcCmd field
   * contains a NULL pointer.
   *
   * @note This is an external variable from file @file command.c
   */
  extern tCmdLineEntry g_psCmdTable[];

  /****************************************************************************
   *     PUBLIC FUNCTIONS
   ***************************************************************************/

  extern uint8_t CmdLineProcess(char *pcCmdLine);

#ifdef __cplusplus
}
#endif

#endif /* CMDLINE_INCLUDE_CMDLINE_H_ */
