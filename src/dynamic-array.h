#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#define GROW_ARRAY_SIZE(SIZE) ((SIZE) + (SIZE)/2)
#define CAN_GROW_SIZE(SIZE, MAXSIZE) ((SIZE) < (MAXSIZE) - (SIZE)/2)

struct dynamic_array {
  size_t size;
  size_t count;
  void *elements;
};



/*
enum da_result_type {
    OK,
    INDEX_ERROR,
    SIZE_ERROR,
    ALLOCATION_ERROR,
};

struct da_result {
  enum da_result_type type;
  void *value;
};

enum da_result_type da_grow(struct dynamic_array *array);


enum da_result_type da_init(struct dynamic_array *array);
void da_destroy(struct dynamic_array *array);

struct da_result da_set(struct dynamic_array *array, void *element, size_t element_size, size_t index);
struct da_result da_get(struct dynamic_array *array, size_t element_size, size_t index);

const char *da_result_type_to_string(enum da_result_type type);
*/


#endif
