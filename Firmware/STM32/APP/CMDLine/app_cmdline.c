/*
 * app_cmdline.c
 *
 *  Created on: Jul 8, 2024
 *      Author: dongkhoa
 */

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "app_cmdline.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************
 *      PRIVATE DEFINES
 *****************************************************************************/

/**
 * @brief Defines the maximum number of arguments that can be parsed.
 *
 * If not defined previously, this macro sets the maximum number of arguments
 * that can be parsed to 8.
 */
#ifndef CMDLINE_MAX_ARGS
#define CMDLINE_MAX_ARGS 8
#endif

/******************************************************************************
 *      PRIVATE DATA
 *****************************************************************************/

/**
 * @brief An array to hold the pointers to the command line arguments.
 *
 * This array is used to store pointers to command line arguments.
 * The size is defined by CMDLINE_MAX_ARGS + 1.
 */
static char *g_ppcArgv[CMDLINE_MAX_ARGS + 1];

/******************************************************************************
 *     PUBLIC FUNCTIONS
 *****************************************************************************/

/**
 * @brief Process a command line string and execute the corresponding command.
 *
 * This function parses the provided command line string, splits it into
 * individual arguments, searches for a matching command in the command table,
 * and executes the corresponding command function if found.
 *
 * @param pcCmdLine Pointer to the command line string to process.
 * @return Returns CMDLINE_OK if successful, or an error code if an issue
 *         occurs (e.g., command not found, too many arguments).
 */
uint8_t
CmdLineProcess (char *pcCmdLine)
{
  char          *pcChar;
  uint_fast8_t   ui8Argc;
  bool           bFindArg = true;
  tCmdLineEntry *psCmdEntry;

  /**
   * Initialize the argument counter, and point to the beginning of the
   * command line string.
   */
  ui8Argc = 0;
  pcChar  = pcCmdLine;

  /**
   * Advance through the command line until a zero character is found.
   */
  while (*pcChar)
  {
    /**
     * If there is a space, then replace it with a zero, and set the flag
     * to search for the next argument.
     */
    if (*pcChar == ' ')
    {
      *pcChar  = 0;
      bFindArg = true;
    }
    /**
     * Otherwise it is not a space, so it must be a character that is part
     * of an argument.
     */
    else
    {
      /**
       * If bFindArg is set, then that means we are looking for the start
       * of the next argument.
       */
      if (bFindArg)
      {
        /**
         * As long as the maximum number of arguments has not been
         * reached, then save the pointer to the start of this new arg
         * in the argv array, and increment the count of args, argc.
         */
        if (ui8Argc < CMDLINE_MAX_ARGS)
        {
          g_ppcArgv[ui8Argc] = pcChar;
          ui8Argc++;
          bFindArg = false;
        }
        /**
         * The maximum number of arguments has been reached so return
         * the error.
         */
        else
        {
          return (CMDLINE_TOO_MANY_ARGS);
        }
      }
    }
    /**
     * Advance to the next character in the command line.
     */
    pcChar++;
  }

  /**
   * If one or more arguments was found, then process the command.
   */
  if (ui8Argc)
  {
    /**
     * Start at the beginning of the command table, to look for a matching
     * command.
     */
    psCmdEntry = &g_psCmdTable[0];

    /**
     * Search through the command table until a null command string is
     * found, which marks the end of the table.
     */
    while (psCmdEntry->pcCmd)
    {
      /**
       * If this command entry command string matches argv[0], then call
       * the function for this command, passing the command line
       * arguments.
       */
      if (!strcmp(g_ppcArgv[0], psCmdEntry->pcCmd))
      {
        return (psCmdEntry->pfnCmd(ui8Argc, g_ppcArgv));
      }
      /**
       * Not found, so advance to the next entry.
       */
      psCmdEntry++;
    }
  }

  /**
   * Fall through to here means that no matching command was found, so return
   * an error.
   */
  return (CMDLINE_BAD_CMD);
}
