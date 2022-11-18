#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

struct dynamic_array {
  size_t size;
  void *elements;
};

enum da_grow_rc {
  GROW_SUCCESS,
  GROW_ALLOC_ERROR,
  GROW_SIZE_ERROR,
};

void *init_dynamic_array(struct dynamic_array *darray, size_t size);
void destroy_dynamic_array(struct dynamic_array *darray);
enum da_grow_rc grow_dynamic_array(struct dynamic_array *darray, size_t max_size);


#endif
