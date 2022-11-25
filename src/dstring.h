#ifndef DSTRING_H
#define DSTRING_H

#include "dynamic_array.h"

struct dstring {
    struct dynamic_array darray;
    size_t length;
};

void *init_dstring(struct dstring *dstring);
void destroy_dstring(struct dstring *dstring);

enum da_grow_rc grow_dstring(struct dstring *dstring);

char *get_dstring_chars(const struct dstring *dstring);
size_t get_dstring_size(const struct dstring *dstring);

#endif
