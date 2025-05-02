#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "dynamic_array.h"

struct DynamicArray* dynarray_create(size_t sizeof_datatype) {
    struct DynamicArray* dynarray = safe_malloc(sizeof(struct DynamicArray));

    dynarray->sizeof_datatype = sizeof_datatype;
    dynarray->length = 0;
    dynarray->data = NULL;

    return dynarray;
}

void dynarray_add(struct DynamicArray* array, void* data) {
    array->data = realloc(array->data, sizeof(void*)*(array->length+1));

    array->data[array->length] = safe_malloc(array->sizeof_datatype);

    memcpy(array->data[array->length], data, array->sizeof_datatype);
    array->length += 1;
}

void dynarray_free(struct DynamicArray* array) {
    for (int i=0; i<array->length; i++) {
        free(array->data[i]);
    }

    if (array->length != 0) {
        free(array->data);
    }
    
    free(array);
}