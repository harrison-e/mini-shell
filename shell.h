#pragma once

#include "shell_grammer.h"
#include "vect.h"
#include <assert.h>
#include <string.h>

#define READ_MAX 2048

typedef enum ShellStatus { EXIT = 0, RUN = 1, ERROR = 2 } status_t;

vect_t *readTokens();