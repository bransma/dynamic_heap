/**
 Dynamic heap; flexible, variable sized heap
 */
#include "heap.h"
#include <stdlib.h>
#include <string.h>

// recompile this with your favorite number, if you wish to change it. One million seems plenty.
#define MAX_HEAP_INDICIES 1000000

// the "master" heap, that points to heap instances
static DArray *heaps = 0;

// the available pool of indices for the as yet to be filled heap_instances
static DArray *indicies;

void initialize_heaps(int number_heaps)
{
	if (heaps != NULL)
	{
		fprintf(stderr, "Cannot re-initialize heaps\n");
		return;
	}

	if (number_heaps > MAX_HEAP_INDICIES)
	{
		fprintf(stderr, "Illegal number of heaps: %i, cannot exceed the maximum allowed %i\n", number_heaps, MAX_HEAP_INDICIES);
		return;
	}

	if (number_heaps <= 0)
	{
		fprintf(stderr, "Illegal number of heaps: %i, heaps can't be initialized\n", number_heaps);
		return;
	}

    heaps = DArray_create(sizeof(struct heap_instance), number_heaps);
    indicies = DArray_create(sizeof(int), number_heaps);

    int i = 0;
    // add 'i' to the available pool of indices, put into the array highest to lowest
    for (i = MAX_HEAP_INDICIES - 1; i >= 0; i--)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        DArray_push(indicies, index);
    }
}

int create_new_heap(int size)
{
    // select the index of the new heap from pool of available indices. Safe, if its not in there,
    // a heap at that index

    if (heaps == NULL)
    {
        fprintf(stderr, "Cannot create a heap, heaps not initialized\n");
        return -1;
    }

    if (size <= 0)
    {
        fprintf(stderr, "Illegal size for heap: %i, the number of nodes Cannot <= 0\n", size);
        return -1;
    }

    int *index = DArray_pop(indicies);

    // pointer to one of the 'number_heaps' heap instance
    struct heap_instance *a_heap = malloc(sizeof(struct heap_instance));

    // place it in the heap
    DArray_set(heaps, *index, a_heap);

    // allocate 'size' data_nodes
    a_heap->nodes = DArray_create(sizeof(struct data_node), size);

    // return the index such that a caller can indicate which heap_instance to get/set/destroy
    return *index;
}

void destroy_heap(int index)
{
    if (heaps == NULL)
    {
        fprintf(stderr, "Cannot destroy a heap, heaps not initialized\n");
        return;
    }

	if (index < 0)
	{
		fprintf(stderr, "Illegal heap index: %i, non-existent heap\n", index);
		return;
	}

    struct heap_instance * a_heap = DArray_get(heaps, index);

    if (a_heap == NULL)
    {
        fprintf(stderr, "Cannot destroy non-existent heap\n");
        return;
    }

    if (a_heap->nodes == NULL)
    {
        //fprintf(stderr, "no nodes to delete in heap %i\n", index);
        free(a_heap);
        return;
    }

    destroy_heap_nodes(index, 0, a_heap->nodes->end);

    // remove a_heap from heaps
    DArray_clear_indicies(heaps, index, index);
}

void destroy_heap_nodes(int index, int start, int end)
{
    if (heaps == NULL)
    {
        fprintf(stderr, "Cannot destroy a heap node(s), heaps not initialized\n");
        return;
    }

	if (index < 0)
	{
		fprintf(stderr, "Illegal heap index: %i, cannot destroy heap nodes from a non-existent heap\n", index);
		return;
	}

	if (start < 0)
	{
		fprintf(stderr, "Starting heap node is illegal: %i, no heap nodes will be destroyed for heap %i\n", start, index);
		return;
	}

	if (end < 0)
	{
		fprintf(stderr, "Ending heap node is illegal: %i, no heap nodes will be destroyed for heap %i\n", end, index);
		return;
	}

    struct heap_instance * a_heap = DArray_get(heaps, index);

    if (a_heap == NULL)
    {
		fprintf(stderr, "Heap at index %i is NULL, no heap nodes will be destroyed for heap %i\n", end, index);
		return;
    }

    int i = 0;
    for (i = start; i <= end; i++)
    {
        struct data_node *a_node = DArray_get(a_heap->nodes, i);

        // may never have been allocated, get access violation if try to dereference to free (non-existent) bytes
        if (a_node == NULL)
        {
            fprintf(stderr, "heap node %i wasn't initialized\n", i);
            continue;
        }

        if (a_node->bytes != NULL)
        {
            free(a_node->bytes);
        }
    }

    // free each node from start to end, such that heaps -> heap_instance -> node[start-end] == NULL; mark each free'd as NULL to avoid re-free
    DArray_clear_indicies(a_heap->nodes, start, end);
}

void destroy_heap_node(int index, int node)
{
    destroy_heap_nodes(index, node, node);
}

// Lazy initialization of a data_node
int set_data_into_heap(int index, int node, int imageDataLen, unsigned char* imageData)
{
    // If the data is not set into the heap, return error code 0
    int rc = 1;
    if (imageData == NULL)
    {
        fprintf(stderr, "Cannot set null data into heap %i heap node %i\n",index, node);
        rc = 0;
    }

    if (imageDataLen <= 0)
    {
        fprintf(stderr, "Cannot set data into heap if length <= 0, heap %i heap node %i\n",index, node);
        rc = 0;
    }

    if (heaps == NULL)
    {
        fprintf(stderr, "Cannot set data into a heap, heaps not initialized\n");
        rc = 0;
    }

    if (index < 0)
    {
        fprintf(stderr, "Illegal heap index: %i, heap index must be >= 0\n", index);
        rc = 0;
    }

    if (node < 0)
    {
        fprintf(stderr, "Illegal heap node index: %i for heap %i, heap node index must be >= 0\n", node, index);
        rc = 0;
    }

    struct heap_instance * a_heap = DArray_get(heaps, index);

    if (a_heap == NULL)
    {
        fprintf(stderr, "Cannot add data into non-existent heap\n");
        rc = 0;
    }

    if (rc == 0)
    {
    	return rc;
    }

    struct data_node *a_node = DArray_get(a_heap->nodes, node);

    int created = 0;
    if (a_node == NULL)
    {
        // create the data node "on the fly" at that index, in which to set the data, if it's not present
        created = 1;
        a_node = malloc(sizeof(struct data_node));
    }

    if (a_node->bytes != NULL && !created)
    {
        fprintf(stderr, "Cannot set data into an already populated heap node; heap %i heap node %i\n", index, node);
        rc = 0;
    }
    else
    {

        DArray_set(a_heap->nodes, node, a_node);

        // put the data into the node
        a_node->bytes = imageData;


        //printf("set_data_into_heap: %02X,%02X,%02X,%02X\n",a_node->bytes[0],a_node->bytes[1],a_node->bytes[2],a_node->bytes[3]);

        a_node->length = imageDataLen;
    }

    return rc;
}

struct data_node* get_data_from_heap(int index, int node)
{
	if (heaps == NULL)
	{
		fprintf(stderr, "Cannot get data from a heap, heaps not initialized\n");
		return NULL;
	}

	if (index < 0)
	{
		fprintf(stderr, "Illegal heap index: %i, heap index must be >= 0\n", index);
		return NULL;
	}

	if (node < 0)
	{
		fprintf(stderr, "Illegal heap node index: %i for heap %i, heap node index must be >= 0\n", node, index);
		return NULL;
	}

    struct heap_instance * a_heap = DArray_get(heaps, index);

    if (a_heap == NULL)
    {
        fprintf(stderr, "Cannot retrieve data from non-existent heap\n");
        return NULL;
    }

    struct data_node *a_node = DArray_get(a_heap->nodes, node);

    if (a_node == NULL)
    {
        fprintf(stderr, "Cannot retrieve data from non-existent heap node.\n");
        return NULL;
    }

    return a_node;
}
