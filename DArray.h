/*
 * DArray.h
 *
 *  Created on: Nov 18, 2015
 *      Author: m056084
 */

#ifndef _DArray_h
#define _DArray_h
#include <stdlib.h>
#include "dbg.h"

typedef struct DArray
{
    int end;
    int max;
    size_t element_size;
    size_t expand_rate;
    void **contents;
} DArray;

DArray *DArray_create(size_t element_size, size_t initial_max);

void DArray_destroy(DArray *array);

void DArray_clear(DArray *array);

void DArray_clear_indicies(DArray *array, int start, int end);

int DArray_expand(DArray *array);

int DArray_contract(DArray *array);

int DArray_push(DArray *array, void *el);

void *DArray_pop(DArray *array);

void DArray_clear_destroy(DArray *array);

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

static inline void DArray_set(DArray *array, int i, void *el)
{
    if (array == NULL)
    {
        fprintf(stderr, "cannot set an element into a NULL array");
        return;
    }

    check(i < array->max, "darray attempt to set past max");

    if (i > array->end)
        array->end = i;
    array->contents[i] = el;
error: return;
}

static inline void *DArray_get(DArray *array, int i)
{
    if (array == NULL)
    {
        fprintf(stderr, "cannot get an element from a NULL array");
        return NULL;
    }

    check(i >= 0, "array index illegal");

    if (array->contents == NULL)
    {
        fprintf(stderr, "cannot get an element from a NULL array elements");
        return NULL;
    }

    check(i < array->max, "darray attempt to get past max");

    return array->contents[i];
error: return NULL;
}

static inline void *DArray_remove(DArray *array, int i)
{
    if (array == NULL)
    {
        fprintf(stderr, "cannot remove an element from a NULL array");
        return NULL;
    }

    void *el = array->contents[i];

    array->contents[i] = NULL;

    return el;
}

static inline void *DArray_new(DArray *array)
{
    if (array == NULL)
    {
        fprintf(stderr, "cannot create a new array based on a NULL array");
        return NULL;
    }

    check(array->element_size > 0, "Can't use DArray_new on 0 size darrays.");

    return calloc(1, array->element_size);

error: return NULL;
}

#define DArray_free(E) free((E))

#endif
