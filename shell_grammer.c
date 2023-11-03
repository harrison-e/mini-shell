#include "shell_grammer.h"
#include "vect.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void handleChildStatus(int status, const char *cmd) {
  if (WIFEXITED(status)) {
    if (WEXITSTATUS(status) == 1) {
      printf("[%s]: command not found\n", cmd);
    }
  } else if (WIFSIGNALED(status)) {
    int signal = WTERMSIG(status);
    printf("[%s]: terminated by signal %d\n", cmd, signal);
  }
}

//==========================
//------SIMPLE COMMAND------
//==========================

// construct a new simplcmd
simplcmd_t *simplcmd_new(vect_t *tokens) {
  simplcmd_t *simplcmd = malloc(sizeof(simplcmd_t));
  simplcmd->tokens = tokens;
  return simplcmd;
}

// delete a simplcmd
void simplcmd_delete(simplcmd_t *s) {
  assert(s != NULL);

  vect_delete(s->tokens);
  free(s);
}

// execute a simplcmd
// return exit status
int simplcmd_exec(simplcmd_t *s) {
  // child process
  char **args = vect_to_array(s->tokens);
  // if the command is empty, exit
  if (strcmp(args[0], "") == 0) {
    exit(0);
  }

  // execute tokens
  execvp(args[0], args);
  exit(0);
}

//==========================
//------REDIRECTION---------
//==========================
redir_t *redir_new(vect_t *tokens, int start, int end) {
  redir_t *redir = malloc(sizeof(redir_t));

  // assert that the first and last tokens are not a redirection operators
  assert(strcmp(vect_get(tokens, start), ">") != 0);
  assert(strcmp(vect_get(tokens, start), "<") != 0);
  assert(strcmp(vect_get(tokens, end), ">") != 0);
  assert(strcmp(vect_get(tokens, end), "<") != 0);

  int simpleCmdEnd = end;
  for (int i = start + 1; i <= end - 1; i++) {
    int tokenIsGreaterThan = strcmp(vect_get(tokens, i), ">") == 0;
    int tokenIsLessThan = strcmp(vect_get(tokens, i), "<") == 0;
    if (tokenIsGreaterThan || tokenIsLessThan) {
      redir->fileName = malloc(strlen(vect_get(tokens, i + 1)));
      strcpy(redir->fileName, vect_get(tokens, i + 1));
      simpleCmdEnd = i - 1;
    }

    if (tokenIsGreaterThan) {
      redir->redirectionType = OUTPUT;
      break;
    } else if (tokenIsLessThan) {
      redir->redirectionType = INPUT;
      break;
    }
  }
  redir->SimpleCommand = simplcmd_new(vect_subset(tokens, start, simpleCmdEnd));

  return redir;
}

// delete a redir, deleting its filename and simplcmd
void redir_delete(redir_t *r) {
  assert(r != NULL);

  simplcmd_delete(r->SimpleCommand);
  free(r->fileName);
}

// execute a redir
int redir_exec(redir_t *r) {
  assert(r != NULL);

  pid_t redir_child;
  int fd;
  assert(-1 != (redir_child = fork()));
  if (redir_child == 0) {
    // in child
    switch (r->redirectionType) {
    case NONE:
      simplcmd_exec(r->SimpleCommand);
      break;

    case INPUT: // <
      close(0);
      fd = open(r->fileName, O_RDONLY);
      assert(fd == 0);
      simplcmd_exec(r->SimpleCommand);
      break;

    case OUTPUT: // >
      close(1);
      fd = open(r->fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      assert(fd == 1);
      simplcmd_exec(r->SimpleCommand);
      break;

    default:
      exit(-1);
      break;
    }
  } else {
    // in parent
    int status;
    waitpid(redir_child, &status, 0);
    const char *cmd = vect_get(r->SimpleCommand->tokens, 0);
    handleChildStatus(status, cmd);
    return status;
  }
}

//==========================
//-------PIPE COMMAND-------
//==========================

// construct a new pipe_t, with some subset of tokens
pipe_t *pipe_new(vect_t *tokens, int start, int end) {
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
      pipe->redirects[redirIndex] = redir_new(tokens, segmentStart, i - 1);
      redirIndex++;
      segmentStart = i + 1;
    }
  }

  if (segmentStart < vect_size(tokens)) {
    pipe->redirects[redirIndex] = redir_new(tokens, segmentStart, end);
    redirIndex++;
  }

  assert(redirIndex == pipe->redirCount);

  return pipe;
}

// delete a pipe_t, deleting its redirs
void pipe_delete(pipe_t *p) {
  assert(p != NULL);

  for (int i = 0; i < p->redirCount; i++) {
    redir_delete(p->redirects[i]);
  }

  free(p);
}

// execute a pipe, set up pipes and exec redirs

// EXAMPLE OF MULTIPLE PIPES:
// https://gist.github.com/aspatic/93e197083b65678a132b9ecee53cfe86

int pipe_exec(pipe_t *p) {
  assert(p != NULL);

  if (p->redirCount <= 1) {
    // single command, make no pipes
    assert(p->redirects != NULL);
    return redir_exec(p->redirects[0]);
  } else {
    // first redir is @0, last redir is @redirCount-1
    // kill stdout for first, kill stdin for last
    // kill stdout and stdin for in between
    // pipe between consecutive redirs

    // # fds = 2*(redirCount-1)
    int fd_count = 2 * (p->redirCount - 1);
    int pipe_fds[fd_count];

    // ex with redirCount = 2
    int pipe_fd[2];
    assert(pipe(pipe_fd) != -1); // create a pipe
    int write_fd = pipe_fd[1];
    int read_fd = pipe_fd[0];

    pid_t write_child = fork();
    assert(write_child != -1);

    if (write_child == 0) {
      close(read_fd);
      assert(1 == dup2(write_fd, 1)); // replace stdout with w_fd
      redir_exec(p->redirects[0]);
      exit(0);
    }

    pid_t read_child = fork();
    assert(read_child != -1);

    if (read_child == 0) {
      close(write_fd);
      assert(0 == dup2(read_fd, 0)); // replace stdin with r_fd
      redir_exec(p->redirects[1]);
      exit(0);
    }

    close(read_fd);
    close(write_fd);
    wait(NULL);
    wait(NULL);
    //

    // for loop
    return 0;
  }
}

//==========================
//-------COMMAND LINE-------
//==========================

// construct a new cmdln_t
cmdln_t *cmdln_new(vect_t *tokens) {
  assert(tokens != NULL);

  cmdln_t *cmdln = malloc(sizeof(cmdln_t));

  cmdln->pipeCmdCount = 1;
  for (int i = 0; i < vect_size(tokens); i++) {
    if ((strcmp(";", vect_get(tokens, i)) == 0) ||
        (strcmp("\n", vect_get(tokens, i)) == 0)) {
      cmdln->pipeCmdCount++;
    }
  }
  cmdln->pipes = malloc(sizeof(pipe_t) * (cmdln->pipeCmdCount));

  int pipeIndex = 0;
  int segmentStart = 0;
  for (int i = 0; i < vect_size(tokens); i++) {
    if ((strcmp(";", vect_get(tokens, i)) == 0) ||
        (strcmp("\n", vect_get(tokens, i)) == 0)) {
      cmdln->pipes[pipeIndex] = pipe_new(tokens, segmentStart, i - 1);
      pipeIndex++;
      segmentStart = i + 1;
    }
  }

  if (segmentStart < vect_size(tokens)) {
    cmdln->pipes[pipeIndex] =
        pipe_new(tokens, segmentStart, vect_size(tokens) - 1);
    pipeIndex++;
  }

  return cmdln;
}

// delete a cmdln, deleting its pipe_t's
void cmdln_delete(cmdln_t *c) {
  assert(c != NULL);

  for (int i = 0; i < c->pipeCmdCount; i++) {
    pipe_delete(c->pipes[i]);
  }

  free(c);
}

// execute a cmdln, executing all of its pipes
// return exit status
int cmdln_exec(cmdln_t *c) {
  assert(c != NULL);

  for (int i = 0; i < c->pipeCmdCount; i++) {
    if (c->pipes[i] == NULL) {
      continue;
    }
    pid_t child_i = fork();
    assert(-1 != child_i);
    if (child_i == 0) {
      int pipe_status;
      if ((pipe_status = pipe_exec(c->pipes[i]))) {
        return pipe_status;
        exit(1);
      }
      exit(0);
    } else {
      int status;
      waitpid(child_i, &status, 0);
      handleChildStatus(status, NULL);
    }
  }
  return 0;
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
