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
  if (!v)
    return NULL;

  v->size = 0;
  v->capacity = VECT_INITIAL_CAPACITY;

  v->data = calloc(v->capacity, sizeof(char *));
  if (!v->data) {
    free(v);
    return NULL;
  }

  for (int i = 0; i < v->capacity; i++)
    v->data[i] = NULL;

  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {
  assert(v != NULL);

  for (int i = 0; i < v->size; i++) 
    free(v->data[i]);

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
  assert(elt != NULL);
  
  if (!v->data[idx]) {
    v->size++;
  } else {
    free(v->data[idx]);
  }

  v->data[idx] = malloc(sizeof(char) * (strlen(elt) + 1));
  strcpy(v->data[idx], elt);
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);
  assert(elt != NULL);

  if (v->size == v->capacity) {
    v->capacity *= VECT_GROWTH_FACTOR;
    char **temp = malloc(v->capacity * sizeof(char *));
    for (int e = 0; e < v->size; e++) {
      temp[e] = vect_get_copy(v, e);
      free(v->data[e]);
    }
    free(v->data);
    v->data = temp;
  }

  v->data[v->size] = malloc(strlen(elt) + 1);
  strcpy(v->data[v->size], elt);
  v->size++;
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
    char* copy_i = vect_get_copy(v, i);
    vect_add(subset, copy_i);
    free(copy_i);
  }
  return subset;
}

/** Construct a new array and fill it with the items in the vector */
char **vect_to_array(vect_t *vect) {
  char **array = malloc((vect->size + 1) * sizeof(char *));
  for (int i = 0; i < vect->size; i++) {
    array[i] = vect->data[i];
  }
  array[vect_size(vect)] = NULL;
  return array;
}

