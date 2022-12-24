/**
 * @file command.h
 * @author Jeongu Kim (jeongukim@kaist.ac.kr)
 * @brief Command handler.
 *
 * @copyright (c) 2022 Jeongu Kim <jeongukim@kaist.ac.kr>
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "protocol.h"

/**
 * @brief Execute a command.
 *
 * @param cmd Command ID.
 * @param arg Arguments.
 * @param arglen Length of arguments in bytes.
 * @return Error code.
 */
uint8_t command_execute(uint8_t cmd, const uint8_t arg[], uint8_t arglen);

#endif
