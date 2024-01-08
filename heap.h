/*
 * dynamic heap
 */

#ifndef HEAP_H_
#define HEAP_H_

#include "DArray.h"
#include "dbg.h"

// the heap instance points at the data nodes that hold the byte[]s
struct heap_instance
{
	DArray *nodes;
};

struct data_node
{
	int length; // length of the byte[] stored
	unsigned char * bytes; // the byte[]
};

void initialize_heaps(int number_heaps);

int create_new_heap(int size);

void destroy_heap(int index);

int set_data_into_heap(int indexHeap, int index, int imageDataLen, unsigned char* imageData);

struct data_node* get_data_from_heap(int indexHeap, int index);

void destroy_heap_nodes(int index, int start, int end);

void destroy_heap_node(int index, int nodeNum);

#endif /* HEAP_H_ */
