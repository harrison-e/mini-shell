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
    vect_t *tokens = readTokens();

    pid_t child = fork();
    assert(child != -1);

    if (child == 0) {
      // child process
      char **args = vect_to_array(tokens);
      if (strcmp(args[0], "") == 0) {
        exit(0);
      }
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
        int signal = WTERMSIG(status);
        if (signal == 11) {
          printf("  \nBye bye.\n");
        } else {
          printf("[%s]: terminated by signal %d\n", tokens->data[0], signal);
        }
        exit(0);
      }
    }
  }
  printf("Bye bye.\n");
  return 0;
}

vect_t *readTokens() {
  char buf[256];
  fflush(stdout);
  size_t count = read(0, buf, 255);
  buf[count] = '\0';

  vect_t *tokens = tokenize(buf);
  return tokens;
}
