/**
 * \author Vincent Surkijn
 */
#define _GNU_SOURCE

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
	if(free_element == true){
	    printf("Freeing element..\n");
	    list_ptr->element_free(&(dummy->element));
	}
	printf("Element after free_element: %p\n", dummy->element);
	free(dummy);
        dummy = prevn;
    }
    printf("Element in dummy now: %p\n", dummy->element);
    if(free_element == true){
	printf("Freeing element..\n");
        list_ptr->element_free(&(dummy->element));
    }
    printf("Element after free_element: %p\n", dummy->element);
    free(dummy);
    free(*list);        // Free list(contains head)
    *list=NULL;         // Free ptr to list
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/
dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    //DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);

//!!!! Here the boolean should be used!!!!
    if(insert_copy){
	printf("Copy inserted\n");
	if(element != NULL){
	    list_node->element = list->element_copy(element);
	}
	else{
	   list_node->element = NULL;		// Copy of NULL is just NULL
	}
    }
    else{
	printf("No copy inserted\n");
	list_node->element = element;
    }

    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
        }
    }
    return list;

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
    if(list==NULL) return 0;
    if(list->head==NULL) return 0;

    //Get ref at index and then return its element
    dplist_node_t* temp;
    temp = dpl_get_reference_at_index(list, index);
    return temp->element;

}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    int count;
    dplist_node_t *dummy;

    if(list==NULL) return -1;

    if (list->head == NULL){
        printf("No elements in list\n");
        return -1;
    }

    if(element == NULL){
	printf("Element is NULL\n");
	if(list->head->element == NULL){
	    printf("Element found at 0\n");
	    return 0;
	}
	else{
            for (dummy = list->head, count = 0; count<dpl_size(list); dummy = dummy->next, count++) {
                if( dummy->element == NULL ){
         	    printf("Element found at %d\n", count);
            	    return count;
        	}
    	    }
	    return -1;
	}
    }


    if( list->element_compare(list->head->element, element) == 0){
        printf("Element found at 0\n");
        return 0;
    }

    for (dummy = list->head, count = 0; count<dpl_size(list); dummy = dummy->next, count++) {
        if( list->element_compare(dummy->element, element) == 0){
            printf("Element found at %d\n", count);
            return count;
        }
    }
    printf("Element not in list\n");
    return -1;
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
    if(list == NULL)	return NULL;
    if(list->head == NULL)	return NULL;
    if(reference == NULL)	return NULL;
    if(list->head == reference)	return list->head->element;

    dplist_node_t *dummy;

    for (dummy = list->head; dummy->next != NULL; dummy = dummy->next){
        if (dummy->next == reference) return dummy->next->element;
    }
    return NULL;
}

/** Debug
typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name);
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

int main(){
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    printf("Size of list: %d\n", dpl_size(list));

    printf("Inserting...\n");
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    dpl_insert_at_index(list, element, 0, false);
    my_element_t **ptr = (my_element_t**)(&(dpl_get_reference_at_index(list, 0)->element));
    printf("Element ptr in node before free: %p\n",*ptr);

    printf("Size of list: %d\n", dpl_size(list));

    printf("Inserting copy...\n");
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 's';
    element2->id = 2;
    element2->name = name2;
    dpl_insert_at_index(list, element2, 0, true);

    printf("Size of list: %d\n", dpl_size(list));

    printf("List: \n");
    printf("Element name at index 0: %c\n", *(( (my_element_t *)(dpl_get_element_at_index(list, 0)) )->name) );
    printf("Element name at index 1: %c\n", *(( (my_element_t *)(dpl_get_element_at_index(list, 1)) )->name) );


    printf("Freeing...\n");
    dpl_free(&list, true);      // Free element as well
    printf("Element ptr in node: %p\n",*ptr);

    printf("Free the real element2 as well\n");
    element_free( (void **)(&element2) );

}/**/
