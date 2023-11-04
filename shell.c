#include "shell.h"
#include "shell_grammer.h"
#include "tokenizer.h"
#include "vect.h"
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
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

bool shouldExit(vect_t *tokens) {
  if (tokens == NULL) {
    printf("\n");
    return true;
  }
  return tokens->size == 1 && (strcmp(vect_get(tokens, 0), "exit") == 0);
}

bool isBuiltInCommand(const char *command) {
  for (size_t i = 0; i < BUILT_IN_COMMANDS_COUNT; i++) {
    if (strcmp(command, BUILT_IN_COMMANDS[i]) == 0) {
      return 1;
    }
  }
  return false;
}

int handleCdCommand(vect_t *tokens) {
  if (tokens->size == 1) {
    chdir(getenv("HOME"));
  } else {
    chdir(vect_get(tokens, 1));
  }
  return 0;
}

int handlePrevCommand(vect_t *previousCommand) {
  if (previousCommand == NULL) {
    printf("No previous command.\n");
    return 0;
  }
  if (isBuiltInCommand(vect_get(previousCommand, 0))) {
    if (strcmp(vect_get(previousCommand, 0), "prev") != 0) {
      return executeBuiltInCommand(previousCommand, previousCommand);
    }
    return 1;
  } else {
    cmdln_t *commandLine = cmdln_new(previousCommand);
    int processStatus = cmdln_exec(commandLine);
    if (processStatus != 0) {
      // TODO: handle error
    }
    return processStatus;
  }
}

int handleSourceCommand(vect_t *tokens) {
  FILE *file = fopen(vect_get(tokens, 1), "r");
  if (file == NULL) {
    printf("Failed to open file\n");
    return 1;
  }

  vect_t *prevCommand = NULL;
  char line[READ_MAX];
  while (fgets(line, sizeof(line), file)) {
    vect_t *tokens = tokenize(line);
    if (strcmp(vect_get(tokens, vect_size(tokens) - 1), "\n") == 0) {
      vect_remove_last(tokens);
    }

    if (isBuiltInCommand(vect_get(tokens, 0))) {
      executeBuiltInCommand(tokens, prevCommand);
      if (strcmp(vect_get(tokens, 0), "prev") != 0) {
        free(prevCommand);
        prevCommand = tokens;
      }
    } else {
      cmdln_t *commandLine = cmdln_new(tokens);
      int processStatus = cmdln_exec(commandLine);
      if (processStatus == 0) {
        free(prevCommand);
        prevCommand = tokens;
      } else {
        // TODO: handle error
      }
    }
  }
  fclose(file);
  return 0;
}

void handleHelpCommand() {
  printf("cd [dir]\n");
  printf("  - This command should change the current working directory of the "
         "    shell to the path specified as the argument. E.g., cd /etc/, cd "
         "bin, cd .., etc.\n\n");

  printf("source [file]\n");
  printf("  - Execute a script.\nTakes a filename as an argument and "
         "processes each line of the file as a command, including built-ins. "
         "In other words, each line should be processed as if it was entered "
         "by the user at the prompt.\n\n");

  printf("prev\n");
  printf("  - Prints the previous command line and executes it again, without "
         "becoming the new command line.\n\n");

  printf("help\n");
  printf("  - Explains all the built-in commands available in your "
         "shell.\n\n");

  printf("exit\n");
  printf("  - Exits the shell.\n");
}

int executeBuiltInCommand(vect_t *tokens, vect_t *previousCommand) {
  const char *command = vect_get(tokens, 0);
  if (strcmp(command, "cd") == 0) {
    return handleCdCommand(tokens);
  } else if (strcmp(command, "source") == 0) {
    return handleSourceCommand(tokens);
  } else if (strcmp(command, "prev") == 0) {
    return handlePrevCommand(previousCommand);
  } else if (strcmp(command, "help") == 0) {
    handleHelpCommand();
    return 0;
  } else {
    return 0;
  }
}

int main(int argc, char **argv) {
  printf("Welcome to mini-shell.\n");
  status_t status = RUN;
  vect_t *previousCommand = NULL;

  while (status == RUN) {
    printf("shell $ ");
    vect_t *tokens = readTokens();

    if (shouldExit(tokens)) {
      status = EXIT;
    } else if (isBuiltInCommand(vect_get(tokens, 0))) {
      executeBuiltInCommand(tokens, previousCommand);
      if (strcmp(vect_get(tokens, 0), "prev") != 0) {
        if (previousCommand != NULL)
          vect_delete(previousCommand);
        previousCommand = vect_subset(tokens, 0, tokens->size - 1);
      }
    } else {
      cmdln_t *commandLine = cmdln_new(tokens);
      int processStatus = cmdln_exec(commandLine);
      cmdln_delete(commandLine);
      if (processStatus == 0) {
        if (previousCommand != NULL)
          vect_delete(previousCommand);
        previousCommand = vect_subset(tokens, 0, tokens->size - 1);
      } else {
        // TODO: handle error
      }
    }
    if (tokens != NULL)
      vect_delete(tokens);
  }
  if (previousCommand != NULL)
    vect_delete(previousCommand);
  printf("Bye bye.\n");
  return 0;
}

vect_t *readTokens() {
  char buf[READ_MAX];
  fflush(stdout);
  size_t count = read(0, buf, READ_MAX - 1);
  if (count == 0) {
    return NULL;

    buf[count] = '\0';

    vect_t *tokens = tokenize(buf);
    if (strcmp(vect_get(tokens, tokens->size - 1), "\n") == 0)
      vect_remove_last(tokens);

    return tokens;
  }
