#include "tokenizer.h"
#include "vect.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char buf[INPUT_MAX + 1]; // +1 for termination
  size_t count = read(0, buf, INPUT_MAX);
  buf[count] = '\0';

  vect_t *tokens = tokenize(buf);
  assert(tokens != NULL);

  // print out the tokens
  for (int i = 0; i < vect_size(tokens); i++) {
    printf("%s\n", vect_get(tokens, i));
  }

  return 0;
}