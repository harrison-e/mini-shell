#pragma once

#include "vect.h"

typedef enum ShellStatus { EXIT = 0, RUN = 1, ERROR = 2 } status_t;

vect_t *readTokens();