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

mem_chunk_header* h_addresses [MEM_POOL_SIZE];

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
//Save address of first header
    h_addresses[0]=(mem_chunk_header*) mem_pool;
}
/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {
//Retrieve first free header and set status + size 
    int i = 0;
    while(h_addresses[i]->status=ALLOCATED){
	i++;
    }
    mem_chunk_header* first;
    first = h_addresses[i];
    first->status=ALLOCATED;
    first->size=tsize;
//Set status and size of next header
    mem_chunk_header* new_header;
    new_header = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + tsize);
    new_header->status=FREE;
    new_header->size=MEM_POOL_SIZE-sizeof(mem_chunk_header)-sizeof(first);//Still need to include size of previous occupied headers
	//What when occupied spaces after first free space?

//Retrieve first header and set status + size
/**    mem_chunk_header* first;
    first = h_addresses[0];
    first->status=ALLOCATED;
    first->size=tsize;
//Set status and size of next header
    mem_chunk_header* new_header;
    new_header = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size);	//mem location of new header
    new_header->status = FREE;
    new_header->size=MEM_POOL_SIZE-2*sizeof(mem_chunk_header)-tsize;
*/
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
	printf("%d\n", i->size);
	i++;
    }
}

//Debug:
int main(){
	ma_malloc(10);
	ma_print();
}
