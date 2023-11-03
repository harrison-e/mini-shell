#pragma once

#include "shell_grammer.h"
#include "vect.h"
#include <assert.h>
#include <string.h>

typedef enum ShellStatus { EXIT = 0, RUN = 1, ERROR = 2 } status_t;

const char *const BUILT_IN_COMMANDS[] = {"cd", "source", "prev", "help", NULL};

#define BUILT_IN_COMMANDS_COUNT (sizeof(BUILT_IN_COMMANDS) / sizeof(char *) - 1)

int handleCdCommand(vect_t *tokens);

int handlePrevCommand(vect_t *previousCommand);

void handleHelpCommand();

int executeBuiltInCommand(vect_t *tokens, vect_t *previousCommand);

vect_t *readTokens();