#include <unistd.h>
#include "setjmp.h"
#include "heap.h"

unsigned char *get_data_for_test(char *file_name, int *file_length);

// trivial make: gcc -o heap *.c
// indented as a reusable library
int main(int argc, const char * argv[])
{
    // max number of heaps is 100. Each heap is backed by a
    // a dynmaic array of heap node.
    initialize_heaps(100);
    
    // create a heap with 112 heap nodes. Node size cannot
    // change, but inidivial nodes can be created and destroyed
    int heap_index = create_new_heap(112);
    if (heap_index == -1)
    {
        return -1;
    }
    
    // test destroying the heap
    destroy_heap(heap_index);
    
    // did it work? Try destroyig again, will see error that cannot
    // destroy non-existent heap
    destroy_heap(heap_index);
    
    // create a new one for a test
    heap_index = create_new_heap(112);
    
    int file_length = 0;
    
    // put your full path to test_data.bin
    unsigned char *inmemory  = get_data_for_test("test_data.bin", &file_length);
    
    // copy data and place in memory, freeing original data
    unsigned char *to_heap = malloc(sizeof(unsigned char) * file_length);
    memcpy(to_heap, inmemory, file_length);
    free(inmemory);
    
    // note this is "safe", that if this node has not yet been
    // created or had been previously destroyed, it is "lazy"
    // initialzed (malloc'd)
    set_data_into_heap(heap_index, 4, file_length, to_heap);
    
    struct data_node *a_node = get_data_from_heap(heap_index, 4);
    if (a_node == NULL)
    {
        printf("No heap node, heap = %i, node = %i\n", heap_index, 4);
    }
    else
    {
        unsigned char *buffer = a_node->bytes;
        if (a_node->bytes == NULL)
        {
            printf("No data for heap = %i, node = %i\n", heap_index, 4);
        }
        else
        {
            // print some of the data
            printf("%02X,%02X,%02X,%02X\n",buffer[0],buffer[1],buffer[2],buffer[3]);
        }
    }
    
    destroy_heap_node(heap_index, 4);
    
    // should see printed error that nodes 1-4 weren't intialized
    // That is because node 4 has been destroyed and 1 to 3 never
    // created
    a_node = get_data_from_heap(heap_index, 4);
    
    destroy_heap(heap_index);
}

unsigned char *get_data_for_test(char *file_name, int *file_length)
{
    FILE *file;
    unsigned char *inmemory;
    
    //Open file
    file = fopen(file_name, "rb");
    if (!file)
    {
        fprintf(stderr, "Unable to open file %s ", file_name);
        return NULL;
    }
    
    //Get file length
    fseek(file, 0, SEEK_END);
    *file_length = (int) ftell(file);
    fseek(file, 0, SEEK_SET);
    
    //Allocate memory
    inmemory = (unsigned char *)malloc(*file_length+1);
    if (!inmemory)
    {
        fprintf(stderr, "Memory error!");
        fclose(file);
        return NULL;
    }
    
    //Read file contents into buffer
    fread(inmemory, *file_length, 1, file);
    fclose(file);

    return inmemory;
}

unsigned char* get_data(int heap_index, int node)
{
	struct data_node* a_node = get_data_from_heap(heap_index, node);
	if (a_node == NULL)
	{
		fprintf(stderr, "empty node\n");
		return NULL;
	}
	else if (a_node->bytes == NULL)
	{
		fprintf(stderr, "empty byte array\n");
		return NULL;
	}

	unsigned char* result = a_node->bytes;

	if (result == NULL)
	{
		return NULL;
	}

	return result;
}

void initializeHeaps(int number_heaps)
{
	initialize_heaps((int) number_heaps);
}

int createNewHeap(int size)
{
	int index = create_new_heap(size);
	printf("\nnew heap index = %i\n", index);
	return index;
}

int destroyHeap(int heap_index)
{
    printf("destroy heap %i\n", heap_index);
	destroy_heap(heap_index);

	return 0;
}

int destroyHeapNode(int heap_index, int node)
{
	destroy_heap_node(heap_index, node);

	return 0;
}

int destroyHeapNodes(int heap_index, int node_start, int node_end)
{
    printf("Destroying heap nodes for heap %i for nodes %i to %i\n", heap_index, node_start, node_end);
	destroy_heap_nodes(heap_index, node_start, node_end);

	return 0;
}

void setDataIntoHeap(int heap_index, int node, unsigned char* data, int datasize)
{
	if (datasize <= 0)
	{
		fprintf(stderr, "illegal length of passed array: %i\n", datasize);
		return;
	}

	struct data_node* a_node = get_data_from_heap(heap_index, node);
	if (a_node != NULL && a_node->bytes != NULL)
	{
		//Already data in that node, no-op
		return;
	}
    else
    {
        set_data_into_heap(heap_index, node, datasize, data);
    }
}
