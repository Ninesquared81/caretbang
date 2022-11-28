#include "dstring.h"
#include "sizelims.h"


void *init_dstring(struct dstring *dstring) {
    dstring->length = 0;
    return init_dynamic_array(&dstring->darray, DSTRING_INITIAL_SIZE);
}

void destroy_dstring(struct dstring *dstring) {
    destroy_dynamic_array(&dstring->darray);
}

enum da_grow_rc grow_dstring(struct dstring *dstring) {
    return grow_dynamic_array(&dstring->darray, DSTRING_MAX_BYTES);
}

char *get_dstring_chars(const struct dstring *dstring) {
    return dstring->darray.elements;
}

size_t get_dstring_size(const struct dstring *dstring) {
    return dstring->darray.size;
}
