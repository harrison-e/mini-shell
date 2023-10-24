#ifndef _VECT_H
#define _VECT_H

#include <limits.h>

/** Main data structure for the vector. */
typedef struct vect {
  char **data;           /* Array containing the actual data. */
  unsigned int size;     /* Number of items currently in the vector. */
  unsigned int capacity; /* Maximum number of items the vector can hold before
                            growing. */
} vect_t;

/** Construct a new empty vector. */
vect_t *vect_new();

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v);

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx);

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx);

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt);

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt);

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v);

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v);

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v);

/* Vector configuration. */
#define VECT_INITIAL_CAPACITY 2
#define VECT_GROWTH_FACTOR 2

#define VECT_MAX_CAPACITY UINT_MAX

#endif /* ifndef _VECT_H */
