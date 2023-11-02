#include "shell.h"
#include "shell_grammer.h"
#include "tokenizer.h"
#include "vect.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void printStructs(cmdln_t *commandLine) {
  printf("pipeCmdCount: %d\n", commandLine->pipeCmdCount);
  for (int i = 0; i < commandLine->pipeCmdCount; i++) {
    printf("  pipe %d:\n", i);
    printf("  redirCount: %d\n", commandLine->pipes[i]->redirCount);
    for (int j = 0; j < commandLine->pipes[i]->redirCount; j++) {
      printf("  redir %d:\n", j);
      printf("    file name: %s\n",
             commandLine->pipes[i]->redirects[j]->fileName);
      printf("    redirectionType: %d\n",
             commandLine->pipes[i]->redirects[j]->redirectionType);
      printf("    simple command:\n");
      printf("      token count: %d\n",
             commandLine->pipes[i]->redirects[j]->SimpleCommand->tokens->size);
      for (int k = 0;
           k < commandLine->pipes[i]->redirects[j]->SimpleCommand->tokens->size;
           k++) {
        printf(
            "      token %d: %s\n", k,
            vect_get(commandLine->pipes[i]->redirects[j]->SimpleCommand->tokens,
                     k));
      }
    }
  }
}

int shouldExit(vect_t *tokens) {
  if (tokens == NULL) {
    printf("\n");
    return 1;
  }
  return tokens->size == 1 && (strcmp(vect_get(tokens, 0), "exit") == 0);
}

int main(int argc, char **argv) {

  printf("Welcome to mini-shell.\n");
  status_t status = RUN;

  while (status == RUN) {
    printf("shell $ ");
    vect_t *tokens = readTokens();

    if (shouldExit(tokens)) {
      status = EXIT;
    } else {
      cmdln_t *commandLine = cmdln_new(tokens);
      printStructs(commandLine);
      cmdln_exec(commandLine);
    }
  }
  printf("Bye bye.\n");
  return 0;
}

vect_t *readTokens() {
  char buf[READ_MAX + 1];
  fflush(stdout);
  size_t count = read(0, buf, READ_MAX);
  if (count == 0) {
    return NULL;
  }
  buf[count] = '\0';

  vect_t *tokens = tokenize(buf);
  if (strcmp(vect_get(tokens, tokens->size - 1), "\n") == 0) {
    vect_remove_last(tokens);
  }
  return tokens;
}
