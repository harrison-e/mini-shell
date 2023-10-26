#include "shell_grammer.h"
#include "vect.h"
#include <assert.h>
#include <string.h>

// TOdo: write downstream deletion fns
//   - delete_cmdln deletes its pipes, which delete their redirs, which delete
//   their simple cmds
//   - in shell,

simplcmd_t *new_simplcmd(vect_t *tokens) {
  simplcmd_t *simplcmd = malloc(sizeof(simplcmd_t));
  simplcmd->tokens = tokens;
  return simplcmd;
}

redir_t *new_redir(vect_t *tokens, int start, int end) {
  redir_t *redir = malloc(sizeof(redir_t));

  // assert that the first and last tokens are not a redirection operators
  assert(strcmp(vect_get(tokens, start), ">") != 0);
  assert(strcmp(vect_get(tokens, start), "<") != 0);
  assert(strcmp(vect_get(tokens, end), ">") != 0);
  assert(strcmp(vect_get(tokens, end), "<") != 0);

  for (int i = start + 1; i <= end - 1; i++) {
    int tokenIsGreaterThan = strcmp(vect_get(tokens, i), ">") == 0;
    int tokenIsLessThan = strcmp(vect_get(tokens, i), "<") == 0;
    if (tokenIsGreaterThan || tokenIsLessThan) {
      redir->SimpleCommand = new_simplcmd(vect_subset(tokens, start, i - 1));
      strcpy(redir->fileName, vect_get(tokens, i + 1));
    }

    if (tokenIsGreaterThan) {
      redir->redirectionType = OUTPUT;
    } else if (tokenIsLessThan) {
      redir->redirectionType = INPUT;
    }
  }

  return redir;
}

pipe_t *new_pipe(vect_t *tokens, int start, int end) {
  pipe_t *pipe = malloc(sizeof(pipe_t));

  pipe->redirCount = 1;
  for (int i = start; i <= end; i++) {
    if (strcmp("|", vect_get(tokens, i)) == 0) {
      pipe->redirCount++;
    }
  }
  pipe->redirects = malloc(sizeof(redir_t) * (pipe->redirCount));

  int redirIndex = 0;
  int segmentStart = start;
  for (int i = start; i <= end; i++) {
    if (strcmp("|", vect_get(tokens, i)) == 0) {
      pipe->redirects[redirIndex] = *new_redir(tokens, segmentStart, i - 1);
      redirIndex++;
      segmentStart = i + 1;
    }
  }

  if (segmentStart < vect_size(tokens)) {
    pipe->redirects[redirIndex] =
        *new_redir(tokens, segmentStart, vect_size(tokens) - 1);
    redirIndex++;
  }

  assert(redirIndex == pipe->redirCount);

  return pipe;
}

cmdln_t *new_cmdln(vect_t *tokens) {
  assert(tokens != NULL);

  cmdln_t *cmdln = malloc(sizeof(cmdln_t));

  cmdln->pipeCmdCount = 1;
  for (int i = 0; i < vect_size(tokens); i++) {
    if (strcmp(";", vect_get(tokens, i)) == 0) {
      cmdln->pipeCmdCount++;
    }
  }
  cmdln->pipes = malloc(sizeof(pipe_t) * (cmdln->pipeCmdCount));

  int pipeIndex = 0;
  int segmentStart = 0;
  for (int i = 0; i < vect_size(tokens); i++) {
    if (strcmp(";", vect_get(tokens, i)) == 0) {
      cmdln->pipes[pipeIndex] = *new_pipe(tokens, segmentStart, i - 1);
      pipeIndex++;
      segmentStart = i + 1;
    }
  }

  if (segmentStart < vect_size(tokens)) {
    cmdln->pipes[pipeIndex] =
        *new_pipe(tokens, segmentStart, vect_size(tokens) - 1);
    pipeIndex++;
  }

  assert(pipeIndex == cmdln->pipeCmdCount);

  return cmdln;
}

/**

1. Sequencing : command1; command2
  a) Split the token list on semicolon.
  b) Fork child A & execute command1 (recursively).
  c) In parent: wait for child A to finish.
  d) Fork child B & execute command2 (recursively).
  e) In parent: wait for child B to finish.
  f) Note, that you may have success processing a sequence of commands using
an ordinary loop too.

2. Pipe: command1 | command2
  a) Fork child A.
  b) In child A: create a pipe.
  c) In child A: fork child B.
  d) In child B: hook pipe to stdout, close other side.
  e) In child B: execute command1.
  f) In child A: hook pipe to stdin, close other side.
  g) In child A: execute command2.
  h) In child A: wait for child B.
  i) In parent: wait for child A.
  j) Redirection: command <OP> file

3. Fork a child.
  a) In child: replace the appropriate file descriptor to accomplish the
redirect.
  b) In child: execute command (recursively).
  c) In parent: wait for child to finish.

 */