#include "dynamic_array.h"

#include <stdlib.h>

#include "sizelims.h"


#define GROW_ARRAY_SIZE(SIZE) ((SIZE) + (SIZE)/2)
#define CAN_GROW_SIZE(SIZE, MAXSIZE) ((SIZE) < (MAXSIZE) - (SIZE)/2)


void *init_dynamic_array(struct dynamic_array *darray, size_t size) {
  void *elements = malloc(size);
  darray->elements = elements;
  darray->size = (elements) ? size : 0;
  return elements;
}

void destroy_dynamic_array(struct dynamic_array *darray) {
  free(darray->elements);
  darray->elements = NULL;
  darray->size = 0;
}

enum da_grow_rc grow_dynamic_array(struct dynamic_array *darray, size_t max_size) {
  size_t old_size = darray->size;
  if (!CAN_GROW_SIZE(old_size, max_size)) return GROW_SIZE_ERROR;
  
  size_t new_size = GROW_ARRAY_SIZE(old_size);
  void *new_elements = realloc(darray->elements, new_size);
  if (!new_elements) return GROW_ALLOC_ERROR;

  darray->elements = new_elements;
  darray->size = new_size;

  return GROW_SUCCESS;
}
