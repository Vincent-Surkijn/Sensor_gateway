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
	printf("Element in loop: %c\n", dummy->element);
	free(dummy->next);
	dummy->prev = NULL;
	dummy->element = 0;
	dummy = prevn;
    }
    printf("Element in dummy now: %c\n", dummy->element);
    free(dummy->next);
    free(dummy->prev);
    dummy->element = 0;
    *list = NULL;
    list = NULL;
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

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {

    //TODO: add your code here

}

int dpl_size(dplist_t *list) {
    int count;
    dplist_node_t *dummy;
    if(list == NULL) return -1;
    if (list->head == NULL) return 0;
    for (dummy = list->head, count = 1; dummy->next != NULL; dummy = dummy->next, count++) {
    }
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
        printf("Element %c found at 0", element);
        return 0;
    }

    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
	if(dummy->element==element){
            printf("Element %c found at %d\n",element, count);
            return count;
        }
    }
    printf("Element not in list");
    return -1;
}

/** Debug:
int main(){
    dplist_t *list;
    list = dpl_create();

    list = dpl_insert_at_index(list, 'A', 0);
    //list = dpl_insert_at_index(list, 'B', 1);
    //list = dpl_insert_at_index(list, 'C', 2);

   int index = dpl_size(list)-1;
   while(index>0){
	printf("Prev addr: %p\n",dpl_get_reference_at_index(list, index)->prev);
        printf("Element: %c\n",dpl_get_reference_at_index(list, index)->element);
	index--;}

    dpl_free(&list);

    printf("List: %p\n", list);

/*    printf("Size of list before insert: %d\n", dpl_size(list));
    *dpl_insert_at_index(list, 'A' , 0);
    *dpl_insert_at_index(list, 'B' , 1);
    *dpl_insert_at_index(list, 'C' , 2);
    *dpl_insert_at_index(list, 'D' , 3);
    printf("Size of list: %d\n", dpl_size(list));

    dpl_get_index_of_element(list, 'A');
    dpl_get_index_of_element(list, 'C');
    printf("Element at index 0: %c\n",dpl_get_element_at_index(list, 0));
    printf("Element at index 1: %c\n",dpl_get_element_at_index(list, 1));
    printf("Element at index 2: %c\n",dpl_get_element_at_index(list, 2));
    printf("Element at index 3: %c\n",dpl_get_element_at_index(list, 3));

//Print contents
dplist_node_t *dummy;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    if (list->head == NULL){
        printf("No elements in list\n");
        return -1;
    }
    for (dummy = list->head; dummy->next != NULL; dummy = dummy->next) {
        printf("Element %c\n", dummy->element);
    }
        printf("Element %c\n", dummy->element);
}
/**/
