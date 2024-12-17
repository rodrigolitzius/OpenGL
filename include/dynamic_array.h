#ifndef TDE_DYNARRAY
#define TDE_DYNARRAY

#include <stdlib.h>

struct DynamicArray {
    void** data; size_t sizeof_datatype; int length;
};

struct DynamicArray* dynarray_create(size_t sizeof_datatype);
void dynarray_add(struct DynamicArray* array, void* data);

#endif
