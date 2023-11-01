#pragma once

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
  redir_t **redirects;
} pipe_t;

typedef struct CommandLine {
  int pipeCmdCount;
  pipe_t **pipes;
} cmdln_t;

simplcmd_t *simplcmd_new(vect_t *tokens);

void simplcmd_delete(simplcmd_t *s);

int simplcmd_exec(simplcmd_t *s);

redir_t *redir_new(vect_t *tokens, int start, int end);

void redir_delete(redir_t *r);

int redir_exec(redir_t *r);

pipe_t *pipe_new(vect_t *tokens, int start, int end);

void pipe_delete(pipe_t *p);

int pipe_exec(pipe_t *p);

cmdln_t *cmdln_new(vect_t *tokens);

void cmdln_delete(cmdln_t *c);

int cmdln_exec(cmdln_t *c);