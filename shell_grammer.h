#include "vect.h"
#include <stdlib.h>
// SHELL GRAMMAR

typedef enum RedirectionType { NONE = 0, INPUT = 1, OUTPUT = 2 } redir_a;

typedef struct SimpleCommand {
  vect_t *tokens;
} simplcmd_t;

typedef struct Redirection {
  redir_a redirectionType;
  simplcmd_t *SimpleCommand;
  char *fileName;
} redir_t;

typedef struct PipeCommand {
  int redirCount;
  redir_t *redirects;
} pipe_t;

typedef struct CommandLine {
  int pipeCmdCount;
  pipe_t *pipes;
} cmdln_t;