#ifndef COMMAND_H
#define COMMAND_H

#include "protocol.h"

uint8_t command_execute(uint8_t cmd, const uint8_t *arg, uint8_t arglen);

#endif
