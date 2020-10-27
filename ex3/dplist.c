/**
 * \author Vincent Surkijn
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions -> parameters for function *dpl_create
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y) )
{
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    dplist_t *list_ptr = *list; //list_ptr is a copy of the ptr that points to start of the list
    if(list_ptr == NULL) return;
    if(list_ptr->head == NULL){
        *list = NULL;
        return;
    }
    int index = dpl_size(list_ptr) - 1;  //Start at last index of list
    dplist_node_t *dummy = dpl_get_reference_at_index(list_ptr,index);
    printf("Size of list_ptr: %d\n",index + 1);
    while(dummy->prev != NULL)
    {
        dplist_node_t* prevn = dummy->prev;
        //printf("Element in loop: %c\n", dummy->element);
	if(free_element){
	    list_ptr->element_free(dummy->element);
	}
	free(dummy);
        dummy = prevn;
    }
    //printf("Element in dummy now: %c\n", dummy->element);
    free(dummy);
    free(*list);        // Free list(contains head)
    *list=NULL;         // Free ptr to list
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    //TODO: add your code here

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    //TODO: add your code here

}

int dpl_size(dplist_t *list) {
    int count;
    dplist_node_t *dummy;
    if(list == NULL) return -1;
    if (list->head == NULL) return 0;
    for (dummy = list->head, count = 1; dummy->next != NULL; dummy = dummy->next, count++){}
    return count;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here

}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //TODO: add your code here

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count;
    dplist_node_t *dummy;
    if(list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++){
        if (count >= index) return dummy;
    }
    return dummy;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    //TODO: add your code here

}


