#include "token.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

token_t *token_new() {
  token_t *t = malloc(sizeof(token_t));
  if (!t)
    return NULL;

  t->data = malloc(MAX_TOKEN_SIZE);
  if (!t->data)
    return NULL;

  t->size = 0;
  memset(t->data, '\0', MAX_TOKEN_SIZE);

  return t;
}

int addCharToToken(token_t *token, char c) {
  assert(token != NULL);
  assert(token->data != NULL);
  assert(token->size < MAX_TOKEN_SIZE);

  token->data[token->size] = c;
  token->size++;
  return 0;
}

int clearToken(token_t *token) {
  assert(token != NULL);
  assert(token->data != NULL);

  memset(token->data, '\0', MAX_TOKEN_SIZE);
  token->size = 0;
  return 0;
}

void token_delete(token_t *token) {
  assert(token != NULL);
  assert(token->data != NULL);

  free(token->data);
  free(token);
}

int tokenIsEmpty(token_t *token) {
  assert(token != NULL);
  assert(token->data != NULL);

  if (token->data[0] == '\0')
    return 1;
  else
    return 0;
}

const char *token_get(token_t *token) {
  assert(token != NULL);
  assert(token->data != NULL);

  return token->data;
}