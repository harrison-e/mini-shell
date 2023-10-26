#include "shell.h"
#include "tokenizer.h"
#include "vect.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {

  printf("Welcome to mini-shell.\n");
  status_t status = RUN;

  while (status == RUN) {
    printf("shell $ ");
    fflush(stdout);
    vect_t *tokens = readTokens();

    pid_t child = fork();
    assert(child != -1);

    if (child == 0) {
      // child process
      char **args = vect_to_array(tokens);
      execvp(args[0], args);
      exit(1);
    } else {
      // parent process
      int status;
      waitpid(child, &status, 0);
      if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 1) {
          printf("[%s]: command not found\n", tokens->data[0]);
        }
      } else if (WIFSIGNALED(status)) {
        printf("  \nBye bye.\n");
        exit(0);
      }
    }
  }
  printf("Bye bye.\n");
  return 0;
}

vect_t *readTokens() {
  char buf[256];
  size_t count = read(0, buf, 256);
  if (count > 0) {
    buf[count - 1] = '\0';
  } else {
    buf[count] = '\0';
  }

  vect_t *tokens = tokenize(buf);
  return tokens;
}
