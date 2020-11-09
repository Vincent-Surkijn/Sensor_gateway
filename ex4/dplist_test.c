/**
 * \author Vincent Surkijn
 */

#define _GNU_SOURCE

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListFree)
    {
        // Test free NULL, don't use callback
        dplist_t *list = NULL;
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free NULL, use callback
        list = NULL;
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, don't use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free with one element without callback
        list = dpl_create(element_copy, element_free, element_compare);
         my_element_t *element = malloc(sizeof(my_element_t));
	char *name = malloc(sizeof(char));
	*name = 'v';
	element->id = 1;
	element->name = name;
	dpl_insert_at_index(list, element, 0, false);

	dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
	element_free( (void **)(&element) );

        // Test free with one element with callback, also test if inserted elements are set to NULL
        list = dpl_create(element_copy, element_free, element_compare);
        element = malloc(sizeof(my_element_t));
        name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;
        dpl_insert_at_index(list, element, 0, true);

	my_element_t **ptr = (my_element_t**)(&(dpl_get_reference_at_index(list, 0)->element));

        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
        ck_assert_msg(*ptr == NULL, "Failure: expected ptr to be NULL");

        // Test free with multiple elements without callback
        list = dpl_create(element_copy, element_free, element_compare);

	element = malloc(sizeof(my_element_t));
        name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;
        dpl_insert_at_index(list, element, 0, false);

	my_element_t *element2 = malloc(sizeof(my_element_t));
	char *name2 = malloc(sizeof(char));
        *name2 = 's';
        element2->id = 2;
        element2->name = name2;
        dpl_insert_at_index(list, element2, 0, true);

	dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free with multiple elements with callback, also test if inserted elements are set to NULL
        list = dpl_create(element_copy, element_free, element_compare);

        element = malloc(sizeof(my_element_t));
        name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;
        dpl_insert_at_index(list, element, 0, false);

        ptr = (my_element_t**)(&(dpl_get_reference_at_index(list, 0)->element));

        element2 = malloc(sizeof(my_element_t));
        name2 = malloc(sizeof(char));
        *name2 = 's';
        element2->id = 2;
        element2->name = name2;
        dpl_insert_at_index(list, element2, 0, true);

	my_element_t **ptr2 = (my_element_t**)(&(dpl_get_reference_at_index(list, 1)->element));

        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
        ck_assert_msg(*ptr == NULL, "Failure: expected ptr to be NULL");
        ck_assert_msg(*ptr2 == NULL, "Failure: expected ptr2 to be NULL");

    }
END_TEST

//dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy);
START_TEST(test_InsertAtIndexListNULL){
        dplist_t *list = NULL;

        my_element_t *element = NULL;

	// Test insertAtIndex when index = -1 & element = NULL;
        dplist_t *result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

        // Test insertAtIndex when index = 0 & element = NULL;
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

        // Test insertAtIndex when index = 0 & element = NULL & insert_copy = true;
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");



	element = malloc(sizeof(my_element_t));
        char *name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;


	// Test insertAtIndex when index = -1 & insert_copy = false
	result =  dpl_insert_at_index(list, element, -1, false);
	ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

	// Test insertAtIndex when index = 0 & insert_copy = false
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

        // Test insertAtIndex when index = 99 & insert_copy = false
        result =  dpl_insert_at_index(list, element, 99, false);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

        // Test insertAtIndex when index = -1 & insert_copy = true
        result =  dpl_insert_at_index(list, element, -1, true);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

        // Test insertAtIndex when index = 0 & insert_copy = true
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

        // Test insertAtIndex when index = 99 & insert_copy = true
        result =  dpl_insert_at_index(list, element, 99, true);
        ck_assert_msg(result == NULL, "Failure: expected result to be NULL");

}
END_TEST


START_TEST(test_InsertAtIndexListEmpty){
        dplist_t *list = dpl_create(element_copy, element_free, element_compare);

        my_element_t *element = NULL;

        // Test insertAtIndex when index = -1 & element = NULL;
        list = dpl_create(element_copy, element_free, element_compare);
        dplist_t *result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(dpl_size(result) == 1, "Failure1: expected size to be 1, but was %d", dpl_size(result));
	dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & element = NULL;
        list = dpl_create(element_copy, element_free, element_compare);
	result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(dpl_size(result) == 1, "Failure2: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & element = NULL & insert_copy = true;
        list = dpl_create(element_copy, element_free, element_compare);
	result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(dpl_size(result) == 1, "Failure3: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);



        element = malloc(sizeof(my_element_t));
        char *name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;


        // Test insertAtIndex when index = -1 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
	result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(dpl_size(result) == 1, "Failure4: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(dpl_size(result) == 1, "Failure5: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 99 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        result =  dpl_insert_at_index(list, element, 99, false);
        ck_assert_msg(dpl_size(result) == 1, "Failure6: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = -1 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        result =  dpl_insert_at_index(list, element, -1, true);
        ck_assert_msg(dpl_size(result) == 1, "Failure7: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(dpl_size(result) == 1, "Failure8: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 99 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        result =  dpl_insert_at_index(list, element, 99, true);
        ck_assert_msg(dpl_size(result) == 1, "Failure9: expected size to be 1, but was %d", dpl_size(result));
        dpl_free(&list, false);

}
END_TEST


START_TEST(test_InsertAtIndexListOneElement){
        dplist_t *list = dpl_create(element_copy, element_free, element_compare);
        my_element_t *element2 = malloc(sizeof(my_element_t));
        char *name2 = malloc(sizeof(char));
        *name2 = 'a';
        element2->id = 2;
        element2->name = name2;

        my_element_t *element = NULL;

        // Test insertAtIndex when index = -1 & element = NULL;
        list = dpl_create(element_copy, element_free, element_compare);
	list = dpl_insert_at_index(list, element2, -1, false);
        dplist_t *result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(dpl_size(result) == 2, "Failure1: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & element = NULL;
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(dpl_size(result) == 2, "Failure2: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & element = NULL & insert_copy = true;
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(dpl_size(result) == 2, "Failure3: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);



        element = malloc(sizeof(my_element_t));
        char *name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;

        // Test insertAtIndex when index = -1 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(dpl_size(result) == 2, "Failure4: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(dpl_size(result) == 2, "Failure5: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 99 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, 99, false);
        ck_assert_msg(dpl_size(result) == 2, "Failure6: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = -1 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, -1, true);
        ck_assert_msg(dpl_size(result) == 2, "Failure7: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(dpl_size(result) == 2, "Failure8: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 99 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        result =  dpl_insert_at_index(list, element, 99, true);
        ck_assert_msg(dpl_size(result) == 2, "Failure9: expected size to be 2, but was %d", dpl_size(result));
        dpl_free(&list, false);

}
END_TEST


START_TEST(test_InsertAtIndexListMultipleElements){
        dplist_t *list = dpl_create(element_copy, element_free, element_compare);
        my_element_t *element2 = malloc(sizeof(my_element_t));
        char *name2 = malloc(sizeof(char));
        *name2 = 'a';
        element2->id = 2;
        element2->name = name2;
        my_element_t *element3 = malloc(sizeof(my_element_t));
        char *name3 = malloc(sizeof(char));
        *name3 = 'b';
        element3->id = 3;
        element3->name = name3;


        my_element_t *element = NULL;

        // Test insertAtIndex when index = -1 & element = NULL;
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        dplist_t *result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(dpl_size(result) == 3, "Failure1: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & element = NULL;
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(dpl_size(result) == 3, "Failure2: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & element = NULL & insert_copy = true;
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(dpl_size(result) == 3, "Failure3: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);



        element = malloc(sizeof(my_element_t));
        char *name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;

        // Test insertAtIndex when index = -1 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg(dpl_size(result) == 3, "Failure4: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, 0, false);
        ck_assert_msg(dpl_size(result) == 3, "Failure5: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 99 & insert_copy = false
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, 99, false);
        ck_assert_msg(dpl_size(result) == 3, "Failure6: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = -1 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, -1, true);
        ck_assert_msg(dpl_size(result) == 3, "Failure7: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 0 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, 0, true);
        ck_assert_msg(dpl_size(result) == 3, "Failure8: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

        // Test insertAtIndex when index = 99 & insert_copy = true
        list = dpl_create(element_copy, element_free, element_compare);
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        result =  dpl_insert_at_index(list, element, 99, true);
        ck_assert_msg(dpl_size(result) == 3, "Failure9: expected size to be 3, but was %d", dpl_size(result));
        dpl_free(&list, false);

}
END_TEST


START_TEST(test_dplSize){

	// Test dpl_size when list is NULL
	dplist_t *list = NULL;
	ck_assert_msg( dpl_size(list) == -1, "Failure: expected size to be -1, but was %d", dpl_size(list));

	// Test dpl_size when list is empty
	list = dpl_create(element_copy, element_free, element_compare);
        ck_assert_msg( dpl_size(list) == 0, "Failure: expected size to be 0, but was %d", dpl_size(list));

        // Test dpl_size when list has 1 element
        list = dpl_create(element_copy, element_free, element_compare);
        my_element_t *element = malloc(sizeof(my_element_t));
        char *name = malloc(sizeof(char));
        *name = 'v';
        element->id = 1;
        element->name = name;
        list = dpl_insert_at_index(list, element, -1, false);
        ck_assert_msg( dpl_size(list) == 1, "Failure: expected size to be 1, but was %d", dpl_size(list));

	// Test dpl_size when list has multiple elements
        my_element_t *element2 = malloc(sizeof(my_element_t));
        char *name2 = malloc(sizeof(char));
        *name2 = 'a';
        element2->id = 2;
        element2->name = name2;
        my_element_t *element3 = malloc(sizeof(my_element_t));
        char *name3 = malloc(sizeof(char));
        *name3 = 'b';
        element3->id = 3;
        element3->name = name3;
        list = dpl_insert_at_index(list, element2, -1, false);
        list = dpl_insert_at_index(list, element3, -1, false);
        ck_assert_msg( dpl_size(list) == 3, "Failure: expected size to be 3, but was %d", dpl_size(list));

}
END_TEST


START_TEST(test_GetReferenceAtIndexListNull)
{
    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_reference_at_index(NULL, -1);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(NULL, 0);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(NULL, 99);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
}
END_TEST


START_TEST(test_GetReferenceAtIndexListEmpty)
{
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_reference_at_index(list, -1);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(list, 0);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(list, 99);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
}
END_TEST


START_TEST(test_GetReferenceAtIndexListOneElement)
{

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);

    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_reference_at_index(list, -1);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(list, 0);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(list, 99);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);
}
END_TEST


START_TEST(test_GetReferenceAtIndexListMultipleElements)
{

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;
    list = dpl_insert_at_index(list, element2, -1, false);
    list = dpl_insert_at_index(list, element3, -1, false);


    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_reference_at_index(list, -1);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(list, 0);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(list, 99);
    my_element_t **ptr = (my_element_t**)(&(result->element));
    ck_assert_msg( *ptr == dpl_get_element_at_index(list,99) ,"Failure: expected %p, but got %p", dpl_get_element_at_index(list,99), *ptr );
}
END_TEST


START_TEST(test_GetIndexOfElementListNull)
{
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;

    int result = dpl_get_index_of_element(NULL, element);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);

    // Test GetIndexOfElement when element is NULL
    element = NULL;
    result = dpl_get_index_of_element(NULL, element);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);

}
END_TEST


START_TEST(test_GetIndexOfElementListEmpty)
{
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;

    int result = dpl_get_index_of_element(list, element);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);

    // Test GetIndexOfElement when element is NULL
    element = NULL;
    result = dpl_get_index_of_element(NULL, element);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListOneElement)
{
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;

    // Test GetIndexOfElement when in the list
    int result = dpl_get_index_of_element(list, element);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    // Test GetIndexOfElement when not in the list
    result = dpl_get_index_of_element(list, element2);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
    dpl_free(&list, true);

    // Test GetIndexOfElement when element is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    element = NULL;
    list = dpl_insert_at_index(list, element, -1, false);
    result = dpl_get_index_of_element(list, element);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListMultipleElements)
{
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;
    list = dpl_insert_at_index(list, element2, 1, false);

    // Test GetIndexOfElement when in the list
    int result = dpl_get_index_of_element(list, element2);
    ck_assert_msg(result == 1, "Failure: expected 1, but got %d", result);
    // Test GetIndexOfElement when not in the list
    result = dpl_get_index_of_element(list, element3);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
    dpl_free(&list, true);

    // Test GetIndexOfElement when element is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    element = NULL;
    list = dpl_insert_at_index(list, element, -1, true);
    list = dpl_insert_at_index(list, element2, 1, true);
    result = dpl_get_index_of_element(list, element);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
}
END_TEST


START_TEST(test_GetElementAtRefListResultNull){
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dplist_node_t *node = malloc(sizeof(dplist_node_t));
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;
    list = dpl_insert_at_index(list, element2, 1, false);

    // Test GetElementAtRef when list is NULL
    void *result = dpl_get_element_at_reference(NULL, node);
    ck_assert_msg(result == NULL, "Failure: expected NULL, but got %p", result);
    // Test GetElementAtRef when list is empty
    result = dpl_get_element_at_reference(list, node);
    ck_assert_msg(result == NULL, "Failure: expected NULL, but got %p", result);
    // Test GetElementAtRef when reference is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, -1, false);
    result = dpl_get_element_at_reference(list, NULL);
    ck_assert_msg(result == NULL, "Failure: expected NULL, but got %p", result);
}
END_TEST


START_TEST(test_GetElementAtRefListOneElement){
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);

    // Test GetElementAtRef when reference is not in list
    void *result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == NULL, "Failure: expected NULL, but got %p", result);

    // Test GetElementAtRef when reference is in list
    ref = dpl_get_reference_at_index(list, 1);
    element = dpl_get_element_at_index(list, 1);
    result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == element, "Failure: expected %p, but got %p", element, result);

    // Test GetElementAtRef when reference is first
    ref = dpl_get_reference_at_index(list, 0);
    element = dpl_get_element_at_index(list, 0);
    result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == element, "Failure: expected %p, but got %p", element, result);

    // Test GetElementAtRef when reference is last
    ref = dpl_get_reference_at_index(list, 99);
    element = dpl_get_element_at_index(list, 99);
    result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == element, "Failure: expected %p, but got %p", element, result);
}
END_TEST


START_TEST(test_GetElementAtRefListMultipleElements){
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);


    // Test GetElementAtRef when reference is not in list
    void *result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == NULL, "Failure: expected NULL, but got %p", result);

    // Test GetElementAtRef when reference is in list
    ref = dpl_get_reference_at_index(list, 1);
    element = dpl_get_element_at_index(list, 1);
    result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == element, "Failure: expected %p, but got %p", element, result);

    // Test GetElementAtRef when reference is first
    ref = dpl_get_reference_at_index(list, 0);
    element = dpl_get_element_at_index(list, 0);
    result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == element, "Failure: expected %p, but got %p", element, result);

    // Test GetElementAtRef when reference is last
    ref = dpl_get_reference_at_index(list, 99);
    element = dpl_get_element_at_index(list, 99);
    result = dpl_get_element_at_reference(list, ref);
    ck_assert_msg(result == element, "Failure: expected %p, but got %p", element, result);
}
END_TEST



START_TEST(test_GetElementAtIndexListNull)
{
    // Test getElement when index = -1
    dplist_node_t *result =  dpl_get_element_at_index(NULL, -1);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getElement when index = 0
    result =  dpl_get_element_at_index(NULL, 0);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getElement when index = 99
    result =  dpl_get_element_at_index(NULL, 99);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
}
END_TEST


START_TEST(test_GetElementAtIndexListEmpty)
{
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_element_at_index(list, -1);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 0
    result =  dpl_get_element_at_index(list, 0);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 99
    result =  dpl_get_element_at_index(list, 99);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
}
END_TEST


START_TEST(test_GetElementAtIndexListOneElement)
{

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);

    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_element_at_index(list, -1);
    ck_assert_msg(result == list->head->element,"Failure: expected %p, but got %p", list->head->element,result);

    // Test getReference when index = 0
    result =  dpl_get_element_at_index(list, 0);
    ck_assert_msg(result == list->head->element,"Failure: expected %p, but got %p", list->head->element,result);

    // Test getReference when index = 99
    result =  dpl_get_element_at_index(list, 99);
    ck_assert_msg(result == list->head->element,"Failure: expected %p, but got %p", list->head->element,result);
}
END_TEST


START_TEST(test_GetElementAtIndexListMultipleElements)
{

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;
    list = dpl_insert_at_index(list, element2, 0, false);
    list = dpl_insert_at_index(list, element3, 99, false);


    // Test getReference when index = -1
    my_element_t *result =  dpl_get_element_at_index(list, -1);
    ck_assert_msg(result == list->head->element,"Failure: expected %p, but got %p", list->head->element,result);

    // Test getReference when index = 0
    result =  dpl_get_element_at_index(list, 0);
    ck_assert_msg(result == list->head->element,"Failure: expected %p, but got %p", list->head->element,result);

    // Test getReference when index = 99
    result =  dpl_get_element_at_index(list, 99);
    ck_assert_msg( result->name == name3 ,"Failure: expected %p, but got %p", name3, result->name );
}
END_TEST


START_TEST(test_RemoveAtIndexListNull)
{
    // Test RemoveAtIndex when index = -1 & free_element = true
    dplist_t *result = dpl_remove_at_index(NULL, -1, true);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = 0
    result = dpl_remove_at_index(NULL, 0, true);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = 99
    result = dpl_remove_at_index(NULL, 99, true);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = -1 & free_element = false
    result = dpl_remove_at_index(NULL, -1, false);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = 0
    result = dpl_remove_at_index(NULL, 0, false);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = 99
    result = dpl_remove_at_index(NULL, 99, false);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);

}
END_TEST


START_TEST(test_RemoveAtIndexListEmpty)
{
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    // Test RemoveAtIndex when index = -1 & free_element = true
    dplist_t *result = dpl_remove_at_index(list, -1, true);
    ck_assert_msg(result == list,"Failure1: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = 0
    result = dpl_remove_at_index(list, 0, true);
    ck_assert_msg(result == list,"Failure2: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = 99
    result = dpl_remove_at_index(list, 99, true);
    ck_assert_msg(result == list,"Failure3: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = -1 & free_element = false
    result = dpl_remove_at_index(list, -1, false);
    ck_assert_msg(result == list,"Failure1: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = 0
    result = dpl_remove_at_index(list, 0, false);
    ck_assert_msg(result == list,"Failure2: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = 99
    result = dpl_remove_at_index(list, 99, false);
    ck_assert_msg(result == list,"Failure3: expected %p, but got %p",list,result);

}
END_TEST


START_TEST(test_RemoveAtIndexListOneElement){
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    list = dpl_insert_at_index(list, element, -1, false);
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;


    // Test RemoveAtIndex when index = -1 & free_element = false
    dplist_t *result = dpl_remove_at_index(list, -1, false);
    ck_assert_msg(dpl_size(result)==0,"Failure1: expected 0, but got %d",dpl_size(result));

    // Test RemoveAtIndex when index = 0 & free_element = false
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_remove_at_index(list, 0, false);
    ck_assert_msg(dpl_size(result)==0,"Failure2: expected 0, but got %d",dpl_size(result));

    // Test RemoveAtIndex when index = 99 & free_element = false
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_remove_at_index(list, 99, false);
    ck_assert_msg(dpl_size(result)==0,"Failure3: expected 0, but got %d",dpl_size(result));



    // Test RemoveAtIndex when index = -1 & free_element = true
    list = dpl_insert_at_index(list, element, 0, false);
    my_element_t **ptr1 = (my_element_t**)(&(dpl_get_reference_at_index(list, -1)->element));
    result = dpl_remove_at_index(list, -1, true);
    ck_assert_msg(dpl_size(result)==0,"Failure4: expected 0, but got %d",dpl_size(result));
    ck_assert_msg(*ptr1 == NULL,"Failure4: expected ptr to be NULL");


    // Test RemoveAtIndex when index = 0 & free_element = true
    list = dpl_insert_at_index(list, element2, 0, false);
    ptr1 = (my_element_t**)(&(dpl_get_reference_at_index(result, 0)->element));
    result = dpl_remove_at_index(list, 0, true);
    ck_assert_msg(dpl_size(result)==0,"Failure5: expected 0, but got %d",dpl_size(result));
    ck_assert_msg(*ptr1 == NULL,"Failure5: expected ptr to be NULL");

    // Test RemoveAtIndex when index = 99 & free_element = true
    list = dpl_insert_at_index(list, element3, 0, false);
    ptr1 = (my_element_t**)(&(dpl_get_reference_at_index(result, 0)->element));
    result = dpl_remove_at_index(list, 99, true);
    ck_assert_msg(dpl_size(result)==0,"Failure6: expected 0, but got %d",dpl_size(result));
    ck_assert_msg(*ptr1 == NULL,"Failure6: expected ptr to be NULL");

}
END_TEST


START_TEST(test_RemoveAtIndexListMultipleElements){
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;
    my_element_t *element4 = malloc(sizeof(my_element_t));
    char *name4 = malloc(sizeof(char));
    *name4 = 'c';
    element4->id = 4;
    element4->name = name4;
    my_element_t *element5 = malloc(sizeof(my_element_t));
    char *name5 = malloc(sizeof(char));
    *name5 = 'd';
    element5->id = 5;
    element5->name = name5;

    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);

    // Test RemoveAtIndex when index = -1 & free_element = false
    dplist_t *result = dpl_remove_at_index(list, -1, false);
    ck_assert_msg(dpl_size(result)==2,"Failure1: expected 2, but got %d",dpl_size(result));

    // Test RemoveAtIndex when index = 0 & free_element = false
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_remove_at_index(list, 0, false);
    ck_assert_msg(dpl_size(result)==2,"Failure2: expected 2, but got %d",dpl_size(result));

    // Test RemoveAtIndex when index = 99 & free_element = false
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_remove_at_index(list, 99, false);
    ck_assert_msg(dpl_size(result)==2,"Failure3: expected 2, but got %d",dpl_size(result));




    // Test RemoveAtIndex when index = -1 & free_element = true
    list = dpl_insert_at_index(list, element, 0, false);
    my_element_t **ptr1 = (my_element_t**)(&(dpl_get_reference_at_index(list, -1)->element));
    result = dpl_remove_at_index(list, -1, true);
    ck_assert_msg(dpl_size(result)==2,"Failure4: expected 2, but got %d",dpl_size(result));
    ck_assert_msg(*ptr1 == NULL,"Failure4: expected ptr to be NULL");


    // Test RemoveAtIndex when index = 0 & free_element = true
    list = dpl_insert_at_index(list, element4, 0, false);
    ptr1 = (my_element_t**)(&(dpl_get_reference_at_index(result, 0)->element));
    result = dpl_remove_at_index(list, 0, true);
    ck_assert_msg(dpl_size(result)==2,"Failure5: expected 2, but got %d",dpl_size(result));
    ck_assert_msg(*ptr1 == NULL,"Failure5: expected ptr to be NULL");

    // Test RemoveAtIndex when index = 99 & free_element = true
    list = dpl_insert_at_index(list, element5, 0, false);
    ptr1 = (my_element_t**)(&(dpl_get_reference_at_index(result, 99)->element));
    result = dpl_remove_at_index(list, 99, true);
    ck_assert_msg(dpl_size(result)==2,"Failure6: expected 2, but got %d",dpl_size(result));
    ck_assert_msg(*ptr1 == NULL,"Failure6: expected ptr to be NULL");

}
END_TEST


START_TEST(test_GetFirstReference){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    // Test GetFirstRef when list is NULL
    dplist_node_t *result = dpl_get_first_reference(NULL);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetFirstRef when list is empty
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_get_first_reference(list);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetFirstRef when list has one element
    list = dpl_insert_at_index(list, element, 0, false);
    dplist_node_t *ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_first_reference(list);
    ck_assert_msg( result == ref, "Failure: expected result to be NULL");

    // Test GetFirstRef when list has multiple elements
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_first_reference(list);
    ck_assert_msg( result == ref, "Failure: expected result to be NULL");
}
END_TEST


START_TEST(test_GetLastReference){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    // Test GetLastRef when list is NULL
    dplist_node_t *result = dpl_get_last_reference(NULL);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetLasttRef when list is empty
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_get_last_reference(list);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetLastRef when list has one element
    list = dpl_insert_at_index(list, element, 0, false);
    dplist_node_t *ref = dpl_get_reference_at_index(list, 99);
    result = dpl_get_last_reference(list);
    ck_assert_msg( result == ref, "Failure: expected result to be NULL");

    // Test GetLastRef when list has multiple elements
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    ref = dpl_get_reference_at_index(list, 99);
    result = dpl_get_last_reference(list);
    ck_assert_msg( result == ref, "Failure: expected result to be NULL");
}
END_TEST


START_TEST(test_GetNextReferenceListResultNull){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    dplist_node_t *ref = malloc(sizeof(dplist_node_t));

    // Test GetNextRef when list is NULL
    dplist_node_t *result = dpl_get_next_reference(NULL,ref );
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetNexttRef when list is empty
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetNexttRef when ref is NULL and list has 1 element
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_get_next_reference(list, NULL);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetNexttRef when ref is NULL and list has multiple elements
    list = dpl_insert_at_index(list, element2, 0, false);
    list = dpl_insert_at_index(list, element3, 0, false);
    result = dpl_get_next_reference(list, NULL);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

}
END_TEST


START_TEST(test_GetNextReferenceListOneElement){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);

    // Test GetNexttRef when ref is not in list
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    dplist_node_t *result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetNexttRef when ref is in list (& is both first and last element)
    ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

}
END_TEST


START_TEST(test_GetNextReferenceListMultipleElements){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;


    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);

    // Test GetNexttRef when ref is not in list
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    dplist_node_t *result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetNexttRef when ref is in list
    ref = dpl_get_reference_at_index(list, 1);
    dplist_node_t *next = dpl_get_reference_at_index(list, 99);
    result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == next, "Failure: expected result to be %p, but was %p", next, result);

    // Test GetNexttRef when ref is first
    ref = dpl_get_reference_at_index(list, 0);
    next = dpl_get_reference_at_index(list, 1);
    result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == next, "Failure: expected result to be %p, but was %p", next, result);

    // Test GetNexttRef when ref is last
    ref = dpl_get_reference_at_index(list, 99);
    result = dpl_get_next_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL, but was %p", result);
}
END_TEST


START_TEST(test_GetPrevReferenceListResultNull){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    dplist_node_t *ref = malloc(sizeof(dplist_node_t));

    // Test GetPrevRef when list is NULL
    dplist_node_t *result = dpl_get_previous_reference(NULL,ref );
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetPrevRef when list is empty
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetPrevRef when ref is NULL and list has 1 element
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_get_previous_reference(list, NULL);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetPrevRef when ref is NULL and list has multiple elements
    list = dpl_insert_at_index(list, element2, 0, false);
    list = dpl_insert_at_index(list, element3, 0, false);
    result = dpl_get_previous_reference(list, NULL);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

}
END_TEST


START_TEST(test_GetPrevReferenceListOneElement){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);

    // Test GetPrevRef when ref is not in list
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    dplist_node_t *result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetPrevRef when ref is in list (& is both first and last element)
    ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

}
END_TEST


START_TEST(test_GetPrevReferenceListMultipleElements){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;


    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);

    // Test GetPrevRef when ref is not in list
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    dplist_node_t *result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test GetPrevRef when ref is in list
    ref = dpl_get_reference_at_index(list, 1);
    dplist_node_t *prev = dpl_get_reference_at_index(list, 0);
    result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == prev, "Failure: expected result to be %p, but was %p", prev, result);

    // Test GetPrevRef when ref is first
    ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL, but was %p", result);

    // Test GetPrevRef when ref is last
    ref = dpl_get_reference_at_index(list, 99);
    prev = dpl_get_reference_at_index(list, 1);
    result = dpl_get_previous_reference(list, ref);
    ck_assert_msg( result == prev, "Failure: expected result to be %p, but was %p", prev, result);
}
END_TEST


START_TEST(test_GetReferenceOfElement){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    // Test GetRefOfElement when list is NULL
    dplist_node_t *result1 = dpl_get_reference_of_element(NULL, element);
    dplist_node_t *result2 = dpl_get_reference_of_element(NULL, NULL);
    ck_assert_msg( result1 == NULL, "Failure: expected result to be NULL, but was %p", result1);
    ck_assert_msg( result2 == NULL, "Failure: expected result to be NULL, but was %p", result2);

    // Test GetRefOfElement when list is empty
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result1 = dpl_get_reference_of_element(list, element);
    result2 = dpl_get_reference_of_element(list, NULL);
    ck_assert_msg( result1 == NULL, "Failure: expected result to be NULL, but was %p", result1);
    ck_assert_msg( result2 == NULL, "Failure: expected result to be NULL, but was %p", result2);

    // Test GetRefOfElement when list has one element
    list = dpl_insert_at_index(list, element, 0, false);
    dplist_node_t *ref = dpl_get_reference_at_index(list, 0);
    result1 = dpl_get_reference_of_element(list, element);
    result2 = dpl_get_reference_of_element(list, NULL);
    ck_assert_msg( result1 == ref, "Failure: expected result to be %p, but was %p", ref, result1);
    ck_assert_msg( result2 == NULL, "Failure: expected result to be NULL, but was %p", result2);

    // Test GetRefOfElement when list has multiple elements
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    ref = dpl_get_reference_at_index(list, 99);
    result1 = dpl_get_reference_of_element(list, element3);
    result2 = dpl_get_reference_of_element(list, NULL);
    ck_assert_msg( result1 == ref, "Failure: expected result to be %p, but was %p", ref, result1);
    ck_assert_msg( result2 == NULL, "Failure: expected result to be NULL, but was %p", result2);

}
END_TEST


START_TEST(test_GetIndexOfReferenceListResultUnvalid){
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    // Test GetIndexOfRef when list is NULL
    int result = dpl_get_index_of_reference(NULL,ref);
    ck_assert_msg( result == -1, "Failure: expected result to be -1");

    // Test GetIndexOfRef when list is empty
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_get_index_of_reference(list,ref);
    ck_assert_msg( result == -1, "Failure: expected result to be -1");

    // Test GetIndexOfRef when ref is NULL
    result = dpl_get_index_of_reference(list,NULL);
    ck_assert_msg( result == -1, "Failure: expected result to be -1");

    // Test GetIndexOfRef when list has one element
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_get_index_of_reference(list,NULL);
    ck_assert_msg( result == -1, "Failure: expected result to be -1");

    // Test GetIndexOfRef when list has multiple elements
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    result = dpl_get_index_of_reference(list,NULL);
    ck_assert_msg( result == -1, "Failure: expected result to be -1");
}
END_TEST


START_TEST(test_GetIndexOfReferenceListOneElement){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);

    // Test GetIndexOfRef when ref is not in list
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    int result = dpl_get_index_of_reference(list,ref);
    ck_assert_msg( result == -1, "Failure: expected result to be -1, but was %d", result);

    // Test GetIndexOfRef when ref is in list (& is both first and last element)
    ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_index_of_reference(list,ref);
    ck_assert_msg( result == 0, "Failure: expected result to be 0, but was %d", result);

}
END_TEST


START_TEST(test_GetIndexOfReferenceListMultipleElements){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;


    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);

    // Test GetIndexOfRef when ref is not in list
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    int result = dpl_get_index_of_reference(list,ref);
    ck_assert_msg( result == -1, "Failure: expected result to be -1, but was %d", result);

    // Test GetIndexOfRef when ref is in list
    ref = dpl_get_reference_at_index(list, 1);
    result = dpl_get_index_of_reference(list,ref);
    ck_assert_msg( result == 1, "Failure: expected result to be 1, but was %d", result);

    // Test GetIndexOfRef when ref is first
    ref = dpl_get_reference_at_index(list, 0);
    result = dpl_get_index_of_reference(list,ref);
    ck_assert_msg( result == 0, "Failure: expected result to be 0, but was %d", result);

    // Test GetIndexOfRef when ref is last
    ref = dpl_get_reference_at_index(list, 99);
    result = dpl_get_index_of_reference(list, ref);
    ck_assert_msg( result == 2, "Failure: expected result to be 2, but was %d", result);
}
END_TEST


START_TEST(test_insertAtReferenceListNothingInserted){
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    // Test InsertAtRef when list is NULL
    dplist_t *result = dpl_insert_at_reference(NULL, element, ref, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertAtRef when ref is NULL
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_at_reference(list, element, NULL, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertAtRef when list is empty
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_at_reference(list, element, ref, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test InsertAtRef when list has one element & ref is not in list
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_insert_at_reference(list, element, ref, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));

    // Test InsertAtRef when list has multiple elements & ref is not in list
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    result = dpl_insert_at_reference(list, element, ref, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));

    dpl_free(&list,false);

// Copy = true

    // Test InsertAtRef when list is NULL
    result = dpl_insert_at_reference(NULL, element, ref, true);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertAtRef when ref is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_at_reference(list, element, NULL, true);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertAtRef when list is empty
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_at_reference(list, element, ref, true);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test InsertAtRef when list has one element & ref is not in list
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_insert_at_reference(list, element, ref,true);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));

    // Test InsertAtRef when list has multiple elements & ref is not in list
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    result = dpl_insert_at_reference(list, element, ref, true);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));

}
END_TEST


START_TEST(test_insertAtReferenceListOneElement){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);

    // Test InsertAtRef when element is NULL
    dplist_node_t *ref = dpl_get_reference_at_index(list,0);
    dplist_t *result = dpl_insert_at_reference(list, NULL, ref, false);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));

    // Test InsertAtRef when reference is in list (& both first and last element)
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_insert_at_reference(list, element2, ref, false);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    dpl_free(&list,false);

//Copy = true

    // Test InsertAtRef when element is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_insert_at_reference(list, NULL, ref, true);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test InsertAtRef when reference is in list (& both first and last element)
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_insert_at_reference(list, element2, ref, true);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    dpl_free(&list,false);

}
END_TEST


START_TEST(test_insertAtReferenceListMultipleElements){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);

    // Test InsertAtRef when element is NULL
    dplist_node_t *ref = dpl_get_reference_at_index(list,0);
    dplist_t *result = dpl_insert_at_reference(list, NULL, ref, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));

    // Test InsertAtRef when reference is first
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_insert_at_reference(list, element2, ref, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test InsertAtRef when reference is last
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,99);
    result = dpl_insert_at_reference(list, element2, ref, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    dpl_free(&list,false);


//Copy = true

    // Test InsertAtRef when element is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_insert_at_reference(list, NULL, ref, true);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test InsertAtRef when reference is first element
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_insert_at_reference(list, element2, ref, true);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test InsertAtRef when reference is last
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,99);
    result = dpl_insert_at_reference(list, element2, ref, true);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    dpl_free(&list,false);

}
END_TEST


START_TEST(test_insertSortedListNULL){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;

    // Test InsertSorted when element is not null
    dplist_t *result = dpl_insert_sorted(NULL, element, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertSorted when element is null
    result = dpl_insert_sorted(NULL, element, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertSorted when element is not null
    result = dpl_insert_sorted(NULL, element, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test InsertSorted when element is null
    result = dpl_insert_sorted(NULL, element, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");
}
END_TEST


START_TEST(test_insertSortedListEmpty){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    // Test InsertSorted when element is not null
    dplist_t *result = dpl_insert_sorted(list, element, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list, false);

    // Test InsertSorted when element is null
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_sorted(list, element, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list, false);

    // Test InsertSorted when element is not null
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_sorted(list, element, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list, false);

    // Test InsertSorted when element is null
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_insert_sorted(list, element, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list, false);
}
END_TEST


START_TEST(test_insertSortedListOneElement){

    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);

    // Test InsertSorted when element is not null
    dplist_t *result = dpl_insert_sorted(list, element2, false);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    int res_idx = dpl_get_index_of_element(list, element2);
    ck_assert_msg( res_idx  == 1, "Failure: expected idx to be 1, but was %d", res_idx);
    dpl_free(&list, false);

    // Test InsertSorted when element is null
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_insert_sorted(list, element2, false);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    res_idx = dpl_get_index_of_element(list, element2);
    ck_assert_msg( res_idx  == 1, "Failure: expected idx to be 1, but was %d", res_idx);
    dpl_free(&list, false);

    // Test InsertSorted when element is not null
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_insert_sorted(list, element2, false);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    res_idx = dpl_get_index_of_element(list, element2);
    ck_assert_msg( res_idx  == 1, "Failure: expected idx to be 1, but was %d", res_idx);
    dpl_free(&list, false);

    // Test InsertSorted when element is null
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_insert_sorted(list, element2, false);
    ck_assert_msg( dpl_size(result) == 2, "Failure: expected size to be 2, but was %d", dpl_size(result));
    res_idx = dpl_get_index_of_element(list, element2);
    ck_assert_msg( res_idx  == 1, "Failure: expected idx to be 1, but was %d", res_idx);
    dpl_free(&list, false);
}
END_TEST


START_TEST(test_insertSortedListMultipleElements){

    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);


    // Test InsertSorted when element is not null
    dplist_t *result = dpl_insert_sorted(list, element3, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    int res_idx = dpl_get_index_of_element(list, element3);
    ck_assert_msg( res_idx  == 2, "Failure: expected idx to be 2, but was %d", res_idx);
    dpl_free(&list, false);

    // Test InsertSorted when element is null
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    result = dpl_insert_sorted(list, element3, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    res_idx = dpl_get_index_of_element(list, element3);
    ck_assert_msg( res_idx  == 2, "Failure: expected idx to be 2, but was %d", res_idx);
    dpl_free(&list, false);

    // Test InsertSorted when element is not null
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    result = dpl_insert_sorted(list, element3, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    res_idx = dpl_get_index_of_element(list, element3);
    ck_assert_msg( res_idx  == 2, "Failure: expected idx to be 2, but was %d", res_idx);
    dpl_free(&list, false);

    // Test InsertSorted when element is null
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    result = dpl_insert_sorted(list, element3, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));
    res_idx = dpl_get_index_of_element(list, element3);
    ck_assert_msg( res_idx  == 2, "Failure: expected idx to be 2, but was %d", res_idx);
    dpl_free(&list, false);
}
END_TEST


START_TEST(test_removeAtReferenceListNothingRemoved){
    dplist_node_t *ref = malloc(sizeof(dplist_node_t));
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;

    // Test RemoveAtRef when list is NULL
    dplist_t *result = dpl_remove_at_reference(NULL, ref, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test RemoveAtRef when ref is NULL
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_remove_at_reference(list, NULL, false);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test RemoveAtRef when list is empty
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_remove_at_reference(list, ref, false);
    ck_assert_msg( dpl_size(result) == 0, "Failure: expected size to be 0, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test RemoveAtRef when list has one element & ref is not in list
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_remove_at_reference(list, ref, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));

    // Test RemoveAtRef when list has multiple elements & ref is not in list
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    result = dpl_remove_at_reference(list, ref, false);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));

    dpl_free(&list,false);

// Free_element = true

    // Test RemoveAtRef when list is NULL
    result = dpl_remove_at_reference(NULL, ref, true);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test RemoveAtRef when ref is NULL
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_remove_at_reference(list, NULL, true);
    ck_assert_msg( result == NULL, "Failure: expected result to be NULL");

    // Test RemoveAtRef when list is empty
    list = dpl_create(element_copy, element_free, element_compare);
    result = dpl_remove_at_reference(list, ref, true);
    ck_assert_msg( dpl_size(result) == 0, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test RemoveAtRef when list has one element & ref is not in list
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    result = dpl_remove_at_reference(list,  ref,true);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));

    // Test RemoveAtRef when list has multiple elements & ref is not in list
    list = dpl_insert_at_index(list, element2, 1, false);
    list = dpl_insert_at_index(list, element3, 2, false);
    result = dpl_remove_at_reference(list, ref, true);
    ck_assert_msg( dpl_size(result) == 3, "Failure: expected size to be 3, but was %d", dpl_size(result));

}
END_TEST


START_TEST(test_removeAtReferenceListOneElement){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);

    // Test RemoveAtRef when reference is in list (& both first and last element)
    dplist_node_t *ref = dpl_get_reference_at_index(list,0);
    dplist_t *result = dpl_remove_at_reference(list, ref, false);
    ck_assert_msg( dpl_size(result) == 0, "Failure: expected size to be 0, but was %d", dpl_size(result));
    dpl_free(&list,false);

//Free_element = true

    // Test InsertAtRef when reference is in list (& both first and last element)
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_remove_at_reference(list, ref, true);
    ck_assert_msg( dpl_size(result) == 0, "Failure: expected size to be 0, but was %d", dpl_size(result));
    dpl_free(&list,false);
}
END_TEST


START_TEST(test_removeAtReferenceListMultipleElements){
    my_element_t *element = malloc(sizeof(my_element_t));
    char *name = malloc(sizeof(char));
    *name = 'v';
    element->id = 1;
    element->name = name;
    my_element_t *element2 = malloc(sizeof(my_element_t));
    char *name2 = malloc(sizeof(char));
    *name2 = 'a';
    element2->id = 2;
    element2->name = name2;
    my_element_t *element3 = malloc(sizeof(my_element_t));
    char *name3 = malloc(sizeof(char));
    *name3 = 'b';
    element3->id = 3;
    element3->name = name3;


    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);

    // Test RemoveAtRef when reference is first
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    dplist_node_t *ref = dpl_get_reference_at_index(list,0);
    dplist_t *result = dpl_remove_at_reference(list, ref, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test RemoveAtRef when reference is last
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,99);
    result = dpl_remove_at_reference(list, ref, false);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);


//Free_element = true

    // Test RemoveAtRef when reference is first element
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,0);
    result = dpl_remove_at_reference(list, ref, true);
    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);

    // Test RemoveAtRef when reference is last
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 0, false);
    ref = dpl_get_reference_at_index(list,1);
    result = dpl_remove_at_reference(list, ref, true);

    ck_assert_msg( dpl_size(result) == 1, "Failure: expected size to be 1, but was %d", dpl_size(result));
    dpl_free(&list,false);
}
END_TEST



//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX3");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1, test_InsertAtIndexListNULL);
    tcase_add_test(tc1_1, test_InsertAtIndexListEmpty);
    tcase_add_test(tc1_1, test_InsertAtIndexListOneElement);
    tcase_add_test(tc1_1, test_InsertAtIndexListMultipleElements);
    tcase_add_test(tc1_1, test_dplSize);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListNull);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListEmpty);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListOneElement);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListMultipleElements);
    tcase_add_test(tc1_1, test_GetIndexOfElementListNull);
    tcase_add_test(tc1_1, test_GetIndexOfElementListEmpty);
    tcase_add_test(tc1_1, test_GetIndexOfElementListOneElement);
    tcase_add_test(tc1_1, test_GetIndexOfElementListMultipleElements);
    tcase_add_test(tc1_1, test_GetElementAtRefListResultNull);
    tcase_add_test(tc1_1, test_GetElementAtRefListOneElement);
    tcase_add_test(tc1_1, test_GetElementAtRefListMultipleElements);
    tcase_add_test(tc1_1, test_GetElementAtIndexListNull);
    tcase_add_test(tc1_1, test_GetElementAtIndexListEmpty);
    tcase_add_test(tc1_1, test_GetElementAtIndexListOneElement);
    tcase_add_test(tc1_1, test_GetElementAtIndexListMultipleElements);
    tcase_add_test(tc1_1, test_RemoveAtIndexListNull);
    tcase_add_test(tc1_1, test_RemoveAtIndexListEmpty);
    tcase_add_test(tc1_1, test_RemoveAtIndexListOneElement);
    tcase_add_test(tc1_1, test_RemoveAtIndexListMultipleElements);
    tcase_add_test(tc1_1, test_GetFirstReference);
    tcase_add_test(tc1_1, test_GetLastReference);
    tcase_add_test(tc1_1, test_GetNextReferenceListResultNull);
    tcase_add_test(tc1_1, test_GetNextReferenceListOneElement);
    tcase_add_test(tc1_1, test_GetNextReferenceListMultipleElements);
    tcase_add_test(tc1_1, test_GetPrevReferenceListResultNull);
    tcase_add_test(tc1_1, test_GetPrevReferenceListOneElement);
    tcase_add_test(tc1_1, test_GetPrevReferenceListMultipleElements);
    tcase_add_test(tc1_1, test_GetReferenceOfElement);
    tcase_add_test(tc1_1, test_GetIndexOfReferenceListResultUnvalid);
    tcase_add_test(tc1_1, test_GetIndexOfReferenceListOneElement);
    tcase_add_test(tc1_1, test_GetIndexOfReferenceListMultipleElements);
    tcase_add_test(tc1_1, test_insertAtReferenceListNothingInserted);
    tcase_add_test(tc1_1, test_insertAtReferenceListOneElement);
    tcase_add_test(tc1_1, test_insertAtReferenceListMultipleElements);
    tcase_add_test(tc1_1, test_insertSortedListNULL);
    tcase_add_test(tc1_1, test_insertSortedListEmpty);
    tcase_add_test(tc1_1, test_insertSortedListOneElement);
    tcase_add_test(tc1_1, test_insertSortedListMultipleElements);
    tcase_add_test(tc1_1, test_removeAtReferenceListNothingRemoved);
    tcase_add_test(tc1_1, test_removeAtReferenceListOneElement);
    tcase_add_test(tc1_1, test_removeAtReferenceListMultipleElements);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
