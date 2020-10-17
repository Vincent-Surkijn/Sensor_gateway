/**
 * \author Vincent Surkijn
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ma_malloc.h"

#define MEM_POOL_SIZE 600   //in bytes
typedef unsigned char byte;

//! An enum of statuses that can be used in the header
typedef enum {
    ALLOCATED, FREE
} mem_status;

//! Every item in the pool will have at least a header with size and status of the following data
typedef struct {
    size size;
    mem_status status;
} mem_chunk_header;


static byte mem_pool[MEM_POOL_SIZE];

/**
 * Allocates array of bytes (memory pool) and initializes the memory allocator.
 * If some bytes have been used after calling ma_malloc(size), calling to ma_init() will result in clearing up the memory pool.
 */
void ma_init() {
//Clear mem_pool
    for (size_t i = 0; i < 100; ++i)
	mem_pool[i] = 0;
//Initialize first header
    mem_chunk_header* first;
    first = (mem_chunk_header*) mem_pool;	// mem_pool = &mem_pool[0]
    first->status = FREE;	// (*first).status = FREE;
    first->size = MEM_POOL_SIZE - sizeof(mem_chunk_header);	//don't use variable in sizeof()
    printf("Size of first header: %d\n", first->size);
    printf("Address of first header: %p\n", first);
}

/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {
//Retrieve first free header and set status
    printf("ma_malloc called\n");
    mem_chunk_header* first;
    first  = (mem_chunk_header*)mem_pool;
    int i =0;
    while(first->status==ALLOCATED){ //Mem_pool has to be cleared or random positive values can still be there!
	first = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size);
	i++;
/** Debug
	printf("Status of header in loop: %d\n",first->status);
	printf("Size of header in loop: %d\n",first->size);
        printf("Address of header in loop: %p\n",first);
  /**/  }
    //printf("Amount of loops: %d\n",i);
//Also the size needs to fit
    if(tsize>first->size){
	printf("Not enough space!\n");
	return NULL;
    }
    int temp_size = first->size;
    first->status=ALLOCATED;
    first->size=tsize;
//Set status and size of next header
    mem_chunk_header* new_header;
    new_header = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size);	//mem location of new header
    new_header->status = FREE;
    new_header->size = temp_size - sizeof(mem_chunk_header) - tsize;

    printf("Size of new header: %d\n", new_header->size);
    printf("Address of new header: %p\n", new_header);
//Return pointer to allocated memory
    return (byte*)((byte*)first + sizeof(mem_chunk_header));
}

/**
 * Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses.
 * Implement also the coalescing behavior.
 */
void ma_free(void *ptr) {

    //TODO: add your code here

}

/**
 * This function is only for debugging. It prints out the entire memory pool.
 * Print info on headers and content of all elements in your pool
 */
void ma_print(void) {
    mem_chunk_header* i = (mem_chunk_header*)mem_pool;
    mem_chunk_header* last = (mem_chunk_header*)mem_pool + MEM_POOL_SIZE;
    while(i<last){
//	printf("Address: %p\n",(void*)i);
	printf("%d\n", i->size);
	i++;
    }
}

/** Debug:
int main(){
	char *ptr[15];
	ma_init();
	ma_malloc(600);
	ptr[0] = ma_malloc(400);
	printf("Going to call the ptr\n");
	assert(ptr[0] != NULL);
	ma_malloc(15);
	ma_malloc(600);
	ma_malloc(45);
	ma_init();
	ma_malloc(10);
        ma_malloc(15);
        ma_malloc(20);
	//ma_print();
}*/
