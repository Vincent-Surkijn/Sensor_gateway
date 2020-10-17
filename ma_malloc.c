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
    ALLOCATED, FREE		//allocated -> 0; free -> 1
} mem_status;

//! Every item in the pool will have at least a header with size and status of the following data
typedef struct {
    size size;
    mem_status status;
} mem_chunk_header;

////! Every item in the pool will have a footer with size and status of the following data
typedef struct{
    size size;
    mem_status status;
} mem_chunk_footer;


static byte mem_pool[MEM_POOL_SIZE];

/**
 * Allocates array of bytes (memory pool) and initializes the memory allocator.
 * If some bytes have been used after calling ma_malloc(size), calling to ma_init() will result in clearing up the memory pool.
 */
void ma_init() {
    printf("Init is called\n");
//Clear mem_pool
    for (size_t i = 0; i < 100; ++i)
	mem_pool[i] = 0;
//Initialize first header
    mem_chunk_header* first;
    first = (mem_chunk_header*) mem_pool;	// mem_pool = &mem_pool[0]
    first->status = FREE;	// (*first).status = FREE;
    first->size = MEM_POOL_SIZE - sizeof(mem_chunk_header) - sizeof(mem_chunk_footer);	//don't use variable in sizeof()
//Initialize first footer
    mem_chunk_footer* firstf;
    firstf = (mem_chunk_footer*)((byte*)first + sizeof(mem_chunk_header) + first->size);
    firstf->status = first->status;
    firstf->size = first->size;
}

/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {
//printf("Requested size: %d\n", tsize);
//Retrieve first free header and set status
    printf("ma_malloc called\n");
    mem_chunk_header* first;
    mem_chunk_header* last;
    first  = (mem_chunk_header*)mem_pool;
    last = (mem_chunk_header*)((byte*)first + MEM_POOL_SIZE);
    int i =0;
    int size_needed = tsize + sizeof(mem_chunk_header) + sizeof(mem_chunk_footer);
    while(first->status==ALLOCATED || (first->size)<size_needed ){
	first = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size + sizeof(mem_chunk_footer));
	i++;
	if(first>=last){
	    printf("Not enough space left!\n");
	    return NULL;
	}
/** Debug*/
	printf("Loop %d\n", i);
	printf("Status of header in loop: %d\n",first->status);
	printf("Size of header in loop: %d\n",first->size);
        printf("Address of header in loop: %p\n",first);
  /**/  }
    printf("Amount of loops: %d\n",i);
    int temp_size = first->size;
    first->status=ALLOCATED;
    first->size=tsize;
//Set status and size of next header
    mem_chunk_header* new_header;	//New_header is the header that comes right after the newly allocated chunk
    new_header = (mem_chunk_header*)(((byte*)first) + sizeof(mem_chunk_header) + first->size + sizeof(mem_chunk_footer));	//mem location of new header
    new_header->status = FREE;
    new_header->size = temp_size - sizeof(mem_chunk_header) - tsize - sizeof(mem_chunk_footer);
//Set status and size of next footer
    mem_chunk_footer* new_footer;
    new_footer = (mem_chunk_footer*)(((byte*)new_header) + sizeof(mem_chunk_header) + new_header->size);
    new_footer->status = new_header->status;
    new_footer->size = new_header->size;

/** Debug*/
    printf("Size of new header: %d\n", new_header->size);
    printf("Address of new header: %p\n", new_header);
    printf("Size of new footer: %d\n", new_footer->size);
    printf("Address of new footer: %p\n", new_footer);/**/
//Return pointer to allocated memory
    byte* allocated_mem;
    allocated_mem = (byte*)((byte*)first + sizeof(mem_chunk_header));
    //printf("Address of allocated header: %p\n", first);
    //printf("Address of allocated memory: %p\n", allocated_mem);
    return allocated_mem;
}

/**
 * Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses.
 * Implement also the coalescing behavior.
 */
void ma_free(void *ptr) {
    printf("Free was called\n");
    mem_chunk_header* temp;
    temp = (mem_chunk_header*)((byte*)ptr - sizeof(mem_chunk_header));
    printf("Address of chunk to free: %p\n", ptr);
    printf("Address of header to free: %p\n", temp);
//Set chunk to free
    temp->status = 1;
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

/** Debug:*/
int main(){
	ma_init();
	ma_malloc(10);
        ma_malloc(15);

	char* ptr;
        ptr = ma_malloc(20);
        mem_chunk_header* ptr_h;
        ptr_h = (mem_chunk_header*)((byte*)ptr - sizeof(mem_chunk_header));

printf("Allocating 600\n");
	ma_malloc(600);
	ma_malloc(10);
	ma_free(ptr);
	ma_malloc(3);
	ma_malloc(30);

	//ma_print();
}/**/
