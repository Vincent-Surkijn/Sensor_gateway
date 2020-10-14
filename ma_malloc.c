/**
 * \author Vincent Surkijn
 */

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
//Initialize first header
    mem_chunk_header* first;
    first = (mem_chunk_header*) mem_pool;	// mem_pool = &mem_pool[0]
    first->status = FREE;	// (*first).status = FREE;
    first->size = MEM_POOL_SIZE - sizeof(mem_chunk_header);	//don't use variable in sizeof()
    printf("Size of first header: %d\n", first->size);
}

/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {
//Retrieve first free header and set status
    printf("TEst\n");
    mem_chunk_header* first = (mem_chunk_header*)mem_pool;
    int i =0;
    while(first->status==ALLOCATED && first->size>0){ //Mem_pool has to be cleared or random positive values can still be there!
	first = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size);
	printf("%d\n",i);
	i++;
	printf("Status of header: %d\n",first->status);
    }
    first->status=ALLOCATED;
//Set status and size of next header
    mem_chunk_header* new_header;
    new_header = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size);	//mem location of new header
    new_header->status = FREE;
    new_header->size=first->size - sizeof(mem_chunk_header) - tsize;
//Now set size of first free header
    first->size=tsize;
//Return pointer to allocated memory
    return (byte*)first + sizeof(mem_chunk_header);
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

//Debug:
int main(){
	ma_init();
	ma_malloc(10);
	ma_malloc(20);
	ma_malloc(15);
//	ma_print();
}
