#pragma once
#define MAX_TOKEN_SIZE 256

/**
 * Represents a Token
 * Essentially a char vector; has size, capacity, and data
 */
typedef struct Token {
  char *data;
  unsigned int size;
} token_t;

/**
 * Constructs a new Token
 */
token_t *token_new();

/**
 * Adds a char to the back of this token
 * Returns 0 upon success, non-zero upon failure
 */
int addCharToToken(token_t *token, char c);

/**
 * Clears the string data from this token
 * Returns 0 upon success, non-zero upon failure
 */
int clearToken(token_t *token);

/**
 * Delete this token, freeing its the memory it occupies (including its data)
 */
void token_delete(token_t *token);

/**
 * Checks if the given token is empty
 * Returns -1 if the token or its data point to NULL
 * Returns  0 if the token is not empty
 * Returns  1 if the token is empty
 */
int tokenIsEmpty(token_t *token);

/**
 * Return a const version of the data in the given token
 * Returns NULL if there is no token or no token data
 */
const char *token_get(token_t *token);