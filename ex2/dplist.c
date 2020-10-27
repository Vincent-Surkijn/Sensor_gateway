/**
 * \author Vincent Surkijn
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

//#define DEBUG

/*
 * definition of error codes
 */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1   //error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2  //error due to a list operation applied on a NULL list

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
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    // pointer drawing breakpoint
    return list;
}

void dpl_free(dplist_t **list) {
    dplist_t *list_ptr = *list;	//list_ptr is a copy of the ptr that points to start of the list
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
	printf("Element in loop: %c\n", *(dummy->element));
	free(dummy->element);
	free(dummy);
	//free(dummy->next);
	//dummy->prev = NULL;
	//dummy->element = 0;
	dummy = prevn;
    }
    printf("Element in dummy now: %c\n", *(dummy->element));
    free(dummy);
    //free(dummy->next);
    //free(dummy->prev);
    //dummy->element=0;
    free(*list);	// Free list(contains head)
    *list = NULL;	// Set ptr to list to null
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/

dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    //DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/
dplist_t *dpl_remove_at_index(dplist_t *list, int index) {
    //printf("List head: %p\n", list->head);
    if(list==NULL) return NULL;
    if(list->head==NULL) return list;	//covers case 1
    else if(index <= 0){	//covers case 2
	printf("Rem case 2\n");
	dplist_node_t *toRem = dpl_get_reference_at_index(list, 0);
	if(toRem->next != NULL){
	    dplist_node_t* next = toRem->next;
	    next->prev = NULL;
            list->head = next;
	}
	else{
	    list->head = NULL;
	}
	free(toRem->element);
        free(toRem);
	//toRem->next = NULL;
	//toRem->element = 0;
	return list;
    }
    else if(index >= dpl_size(list) - 1){	//covers case 3
        printf("Rem case 3\n");
	dplist_node_t *toRem = dpl_get_reference_at_index(list, 99);
	if(toRem->prev != NULL){
            dplist_node_t* prev = toRem->prev;
            prev->next = NULL;
	}
	else{
	    list->head = NULL;
	}
        free(toRem->element);
        free(toRem);
	//toRem->prev = NULL;
	//toRem->element = 0;
	return list;
    }
    else{	//covers case 4
        printf("Rem case 4\n");
        dplist_node_t *toRem = dpl_get_reference_at_index(list, index);
	printf("Element to remove: %c\n", *(toRem->element));
	//printf("NextElement to remove: %c\n", *(toRem->next->element));
	//printf("Prev Element to remove: %c\n", *(toRem->prev->element));
        toRem->next->prev = toRem->prev;
	toRem->prev->next = toRem->next;
        //printf("Next prev: %p\n", toRem->next->prev);
        //printf("Prev next: %p\n", toRem->prev->next);
	free(toRem->element);
	free(toRem);
	//toRem->prev = NULL;
	//toRem->next = NULL;
	//toRem->element = 0;
	return list;
    }
}

int dpl_size(dplist_t *list) {
    int count;
    dplist_node_t *dummy;
    if(list == NULL) return -1;
    if (list->head == NULL) return 0;
    for (dummy = list->head, count = 1; dummy->next != NULL; dummy = dummy->next, count++) {}
    return count;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count;
    dplist_node_t *dummy;
    if(list == NULL) return NULL;
    if (list->head == NULL) return NULL;
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
        if (count >= index) return dummy;
    }
    return dummy;
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {
    if(list==NULL) return 0;
    if(list->head==NULL) return 0;

    //Get ref at index and then return its element
    dplist_node_t* temp;
    temp = dpl_get_reference_at_index(list, index);
    return temp->element;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {
    int count;
    dplist_node_t *dummy;

    if(list==NULL) return -1;

    if (list->head == NULL){
        printf("No elements in list\n");
        return -1;
    }

    if(list->head->element == element){
        printf("Element %c found at 0\n", *element);
        return 0;
    }

    for (dummy = list->head, count = 0; count<dpl_size(list); dummy = dummy->next, count++) {
	//printf("Element in getindex loop: %p\n", dummy->element);
	if(dummy->element==element){
            printf("Element %c found at %d\n",*element, count);
            return count;
        }
    }
    printf("Element not in list\n");
    return -1;
}

/** Debug:
int main(){
    dplist_t *list = dpl_create();

    char *ptr = malloc(sizeof(char));
    *ptr='A';
    char *ptrb = malloc(sizeof(char));
    *ptrb='B';
    char *ptrc = malloc(sizeof(char));
    *ptrc='C';
    char *ptrd = malloc(sizeof(char));
    *ptrd='D';

    list = dpl_insert_at_index(list, ptr, 0);
    list = dpl_insert_at_index(list,ptrb , 1);
    list = dpl_insert_at_index(list, ptrc, 2);
    list = dpl_insert_at_index(list, ptrd, 3);

    printf("Size of list before remove: %d\n", dpl_size(list));
    dplist_t *result = dpl_remove_at_index(list, 0);
    printf("Size of list after remove: %d\n", dpl_size(result));

    list = dpl_insert_at_index(list, ptrb, 0);

    printf("Size of list before remove2: %d\n", dpl_size(list));
    result = dpl_remove_at_index(list, -1);
    printf("Size of list after remove2: %d\n", dpl_size(result));


    dpl_get_index_of_element(list, ptr);
    dpl_get_index_of_element(list, ptrb);
    dpl_get_index_of_element(list, ptrc);
    dpl_get_index_of_element(list, ptrd);

    printf("Element at index 0: %c\n",*(dpl_get_element_at_index(list, 0)));
    printf("Element at index 1: %c\n",*(dpl_get_element_at_index(list, 1)));
    printf("Element at index 2: %c\n",*(dpl_get_element_at_index(list, 2)));
    printf("Element at index 3: %c\n",*(dpl_get_element_at_index(list, 3)));

    printf("Freeing memory now...\n");
    dpl_free(&list);
    printf("List: %p\n",list);
}
/**/
