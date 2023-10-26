#pragma once

#include "token.h"
#include "vect.h"
#include <string.h>

typedef enum TokenState { WAIT = 0, BUILD = 1, QUOTE = 2 } state_t;

#define INPUT_MAX 255

vect_t *tokenize(char *input);

void addTokenToVect(vect_t *tokens, token_t *token);

int tokenizeHelper(vect_t *tokens, token_t *tokenRef, char currentChar,
                   state_t state);

int specialTokenHelper(vect_t *tokens, token_t *tokenRef, char special);

state_t handleWait(vect_t *tokens, token_t *tokenRef, char input);

state_t handleBuild(vect_t *tokens, token_t *tokenRef, char input);

state_t handleQuote(vect_t *tokens, token_t *tokenRef, char input);