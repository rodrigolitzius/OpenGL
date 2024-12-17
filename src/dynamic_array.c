#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"

struct DynamicArray* dynarray_create(size_t sizeof_datatype) {
    struct DynamicArray* dynarray = malloc(sizeof(struct DynamicArray));

    dynarray->sizeof_datatype = sizeof_datatype;
    dynarray->length = 0;
    dynarray->data = NULL;

    return dynarray;
}

void dynarray_add(struct DynamicArray* array, void* data) {
    array->data = realloc(array->data, sizeof(void*)*(array->length+1));

    array->data[array->length] = malloc(array->sizeof_datatype);

    memcpy(array->data[array->length], data, array->sizeof_datatype);
    array->length += 1;
}