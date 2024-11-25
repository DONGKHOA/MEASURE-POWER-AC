/*
 * app_command.h
 *
 *  Created on: Nov 19, 2024
 *      Author: dongkhoa
 */

#ifndef CMDLINE_COMMAND_H_
#define CMDLINE_COMMAND_H_

/******************************************************************************
 *      INCLUDES
 *****************************************************************************/

#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /****************************************************************************
   *   PUBLIC FUNCTION
   ***************************************************************************/

  void APP_COMMAND_Init(void);
  void APP_COMMAND_CreateTask(void);

  /**
   * @brief List of command
   */
  int APP_COMMAND_Help(int argc, char *argv[]);
  int APP_COMMAND_ReadVoltage(int argc, char *argv[]);
  int APP_COMMAND_ReadCurrent(int argc, char *argv[]);
  int APP_COMMAND_ReadPF(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* CMDLINE_COMMAND_H_ */
