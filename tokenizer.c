#include "tokenizer.h"
#include "token.h"
#include "vect.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

vect_t *tokenize(char *input) {
  state_t state = WAIT;
  vect_t *tokens = vect_new();
  token_t *currentToken = token_new();

  for (int i = 0; i < strlen(input); i++) {
    state = tokenizeHelper(tokens, currentToken, input[i], state);
    if (state == -1) {
      return NULL;
    }
  }

  // if we're in a quote, then a closing quote is missing and we throw an error
  if (state == QUOTE) {
    return NULL;
  }

  addTokenToVect(tokens, currentToken);

  return tokens;
}

void addTokenToVect(vect_t *tokens, token_t *token) {
  assert(tokens != NULL);
  assert(token != NULL);
  if (!tokenIsEmpty(token)) {
    vect_add(tokens, token_get(token));
    clearToken(token);
  }
}

int tokenizeHelper(vect_t *tokens, token_t *tokenRef, char currentChar,
                   state_t state) {
  switch (state) {
  case WAIT:
    return handleWait(tokens, tokenRef, currentChar);
  case BUILD:
    return handleBuild(tokens, tokenRef, currentChar);
  case QUOTE:
    return handleQuote(tokens, tokenRef, currentChar);
  default:
    return -1;
  }
}

int specialTokenHelper(vect_t *tokens, token_t *tokenRef, char special) {
  assert(tokens != NULL);
  assert(tokenRef != NULL);

  if (!tokenIsEmpty(tokenRef)) {
    addTokenToVect(tokens, tokenRef);
    clearToken(tokenRef);
  }
  char *special_token = malloc(2);
  special_token[0] = special;
  special_token[1] = '\0';
  vect_add(tokens, special_token);
  free(special_token);
  return 0;
}

state_t handleWait(vect_t *tokens, token_t *tokenRef, char input) {
  switch (input) {
  case ' ':
    return WAIT;
  case '\"':
    return QUOTE;
  case '(':
  case ')':
  case '<':
  case '>':
  case '|':
  case ';':
  case '\n':
    specialTokenHelper(tokens, tokenRef, input);
    return BUILD;
  default:
    addCharToToken(tokenRef, input);
    return BUILD;
  }
}

state_t handleBuild(vect_t *tokens, token_t *tokenRef, char input) {
  switch (input) {
  case ' ':
    addTokenToVect(tokens, tokenRef);
    return WAIT;
  case '\"':
    return QUOTE;
  case '(':
  case ')':
  case '<':
  case '>':
  case '|':
  case ';':
  case '\n':
    specialTokenHelper(tokens, tokenRef, input);
    return BUILD;
  default:
    addCharToToken(tokenRef, input);
    return BUILD;
  }
}

state_t handleQuote(vect_t *tokens, token_t *tokenRef, char input) {
  switch (input) {
  case '\"':
    return BUILD;
  default:
    addCharToToken(tokenRef, input);
    return QUOTE;
  }
}