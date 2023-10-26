/**
 * Vector implementation.
 *
 * - Implement each of the functions to create a working growable array
 * (vector).
 * - Do not change any of the structs
 * - When submitting, You should not have any 'printf' statements in your vector
 *   functions.
 *
 * IMPORTANT: The initial capacity and the vector's growth factor should be
 * expressed in terms of the configuration constants in vect.h
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"

/** Construct a new empty vector. */
vect_t *vect_new() {
  vect_t *v = malloc(sizeof(vect_t));
  v->capacity = 2;
  v->data = calloc(v->capacity, sizeof(char *));
  v->size = 0;

  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {
  assert(v != NULL);

  for (int i = 0; i < v->size; i++) {
    free(v->data[i]);
  }
  free(v->data);
  free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  return v->data[idx];
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  char *dataCopy = malloc(sizeof(char) * (strlen(v->data[idx]) + 1));
  strcpy(dataCopy, v->data[idx]);
  return dataCopy;
}
/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  assert(idx < v->size);
  free(v->data[idx]);
  v->data[idx] = malloc(sizeof(char) * (strlen(elt) + 1));
  strcpy(v->data[idx], elt);
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);

  if (v->size == v->capacity) {
    v->capacity *= VECT_GROWTH_FACTOR;
    char **temp = realloc(v->data, v->capacity * sizeof(char *));
    if (temp != NULL) {
      v->data = temp;
    }
  }
  v->data[v->size] = malloc(sizeof(char) * (strlen(elt) + 1));
  strcpy(v->data[v->size], elt);
  v->size += 1;
}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);

  if (v->size == 0) {
    return;
  }
  v->size -= 1;
  free(v->data[v->size]);
  v->data[v->size] = NULL;
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);

  return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);

  return v->capacity;
}

/** Construct a new vector and fill it with a subset of data */
vect_t *vect_subset(vect_t *v, unsigned int start, unsigned int end) {
  assert(v != NULL);
  assert(start <= end);
  assert(end < v->size);
  assert(v->data != NULL);

  vect_t *subset = vect_new();
  for (int i = start; i <= end; i++) {
    vect_add(subset, vect_get_copy(v, i));
  }
  return subset;
}

/** Construct a new array and fill it with the items in the vector */
char **vect_to_array(vect_t *vect) {
  char **array = malloc(vect->size * sizeof(char *));
  for (int i = 0; i < vect->size; i++) {
    array[i] = vect->data[i];
  }
  return array;
}
