#pragma once

#include "shell_grammer.h"
#include "vect.h"
#include <assert.h>
#include <string.h>

typedef enum ShellStatus { EXIT = 0, RUN = 1, ERROR = 2 } status_t;

vect_t *readTokens();