/**
 * \author Vincent Surkijn
 */

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>

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

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListFree)
    {
        // Test free NULL
        dplist_t *list = NULL;
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure1: expected result to be NULL , but was %p", list);

        // Test free empty list
        list = dpl_create();
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure2: expected result to be NULL, but was %p", list);
        // Test free with one element
        list = dpl_create();
	list = dpl_insert_at_index(list, 'A', 0);
	dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure3: expected result to be NULL, but was %p", list);
        // Test free with multiple element
        list = dpl_create();
        list = dpl_insert_at_index(list, 'A', 0);
        list = dpl_insert_at_index(list, 'B', 1);
        list = dpl_insert_at_index(list, 'C', 2);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure4: expected result to be NULL, but was %p", list);
    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
        // Test inserting at index -1
        dplist_t *result = dpl_insert_at_index(NULL, 'A', -1);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // Test inserting at index 0
	result = dpl_insert_at_index(NULL, 'A', 0);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // Test inserting at index 99
        result = dpl_insert_at_index(NULL, 'A', 99);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
    }
END_TEST


START_TEST(test_ListInsertAtIndexListEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', -1);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
    //Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

    //Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 99);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
}
END_TEST

START_TEST(test_ListInsertAtIndexListOneElement)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', -1);
    result = dpl_insert_at_index(list, 'B', -1);
    ck_assert_msg(dpl_size(result) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
    //Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 0);
    ck_assert_msg(dpl_size(result) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

    //Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 99);
    result = dpl_insert_at_index(list, 'B', 99);
    ck_assert_msg(dpl_size(result) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

}
END_TEST

START_TEST(test_ListInsertAtIndexListMultipleElements)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', -1);
    result = dpl_insert_at_index(list, 'B', -1);
    result = dpl_insert_at_index(list, 'C', -1);
    result = dpl_insert_at_index(list, 'D', -1);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
    //Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 0);
    result = dpl_insert_at_index(list, 'C', 0);
    result = dpl_insert_at_index(list, 'D', 0);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

    //Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 99);
    result = dpl_insert_at_index(list, 'B', 99);
    result = dpl_insert_at_index(list, 'C', 99);
    result = dpl_insert_at_index(list, 'D', 99);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
}
END_TEST


START_TEST(test_Size)
{
    // Test when List = NULL
    dplist_t *list = NULL;
    int size = dpl_size(list);
    ck_assert_msg(size == -1, "Failure: expected size to be -1, got a size of %d",size);

    // Test when List = empty
    list = dpl_create();
    size = dpl_size(list);
    ck_assert_msg(size == 0, "Failure: expected size to be 0, got a size of %d",size);

    // Test when List has 1 element
    dplist_t *result = dpl_insert_at_index(list, 'A', 0);
    size = dpl_size(list);
    ck_assert_msg(size == 1, "Failure: expected size to be 1, got a size of %d",size);

    //
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    result = dpl_insert_at_index(list, 'D', 3);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",size);
}
END_TEST


START_TEST(test_RemoveAtIndex)
{
    // TODO:
    // Test when List = NULL
    dplist_t *list = NULL;
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
    // Test getReference when index = -1
    dplist_t *list = dpl_create();
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
    // Test getReference when index = -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', 0);
    dplist_node_t *result =  dpl_get_reference_at_index(list, -1);
    ck_assert_msg(result == list->head,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(list, 0);
    ck_assert_msg(result == list->head,"Failure: expected NULL, but got %p",result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(list, 99);
    ck_assert_msg(result == list->head,"Failure: expected NULL, but got %p",result);
}
END_TEST

START_TEST(test_GetReferenceAtIndexListMultipleElements)
{
    // Initialize list, can be reused
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', 0);
    list = dpl_insert_at_index(list, 'B', 1);
    list = dpl_insert_at_index(list, 'C', 2);
    list = dpl_insert_at_index(list, 'D', 3);

    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_reference_at_index(list, -1);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(list, 0);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p",list->head,result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(list, 99);
    ck_assert_msg(result->element == 'D',"Failure: expected D, but got %c",result->element);
}
END_TEST


START_TEST(test_GetIndexOfElementListNull)
{
    int result = dpl_get_index_of_element(NULL, 'A');
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListEmpty)
{
    dplist_t *list = dpl_create();
    int result = dpl_get_index_of_element(list, 'A');
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListOneElement)
{
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', 0);

    // Test GetIndexOfElement when in the list
    int result = dpl_get_index_of_element(list, 'A');
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    // Test GetIndexOfElement when not in the list
    result = dpl_get_index_of_element(list, 'B');
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListMultipleElements)
{
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', 0);
    list = dpl_insert_at_index(list, 'B', 1);
    list = dpl_insert_at_index(list, 'C', 2);
    list = dpl_insert_at_index(list, 'D', 3);

    // Test GetIndexOfElement when in the list
    int result = dpl_get_index_of_element(list, 'A');
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    // Test GetIndexOfElement when in the list
    result = dpl_get_index_of_element(list, 'B');
    ck_assert_msg(result == 1, "Failure: expected 1, but got %d", result);
    // Test GetIndexOfElement when not in the list
    result = dpl_get_index_of_element(list, 'X');
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetElementAtIndexListNull)
{
    //Test GetElementAtIndex when index = -1
    element_t result = dpl_get_element_at_index(NULL,-1);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    //Test GetElementAtIndex when index = 0
    result = dpl_get_element_at_index(NULL,0);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    //Test GetElementAtIndex when index = 99
    result = dpl_get_element_at_index(NULL,99);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
}
END_TEST


START_TEST(test_GetElementAtIndexListEmpty)
{
    //Test GetElementAtIndex when index = -1
    dplist_t *list = dpl_create();
    element_t result = dpl_get_element_at_index(list,-1);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    //Test GetElementAtIndex when index = 0
    result = dpl_get_element_at_index(list,0);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    //Test GetElementAtIndex when index = 99
    result = dpl_get_element_at_index(list,99);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
}
END_TEST


START_TEST(test_GetElementAtIndexListOneElement)
{
    //Test GetElementAtIndex when index = -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', 0);
    element_t result = dpl_get_element_at_index(list,-1);
    ck_assert_msg(result == 'A', "Failure: expected A, but got %c", result);
    //Test GetElementAtIndex when index = 0
    result = dpl_get_element_at_index(list,0);
    ck_assert_msg(result == 'A', "Failure: expected 0, but got %c", result);
    //Test GetElementAtIndex when index = 99
    result = dpl_get_element_at_index(list,99);
    ck_assert_msg(result == 'A', "Failure: expected 0, but got %c", result);
}
END_TEST


START_TEST(test_GetElementAtIndexListMultipleElements)
{
    //Test GetElementAtIndex when index = -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, 'A', 0);
    list = dpl_insert_at_index(list, 'B', 1);
    list = dpl_insert_at_index(list, 'C', 2);
    list = dpl_insert_at_index(list, 'D', 3);
    element_t result = dpl_get_element_at_index(list,-1);
    ck_assert_msg(result == 'A', "Failure: expected A, but got %c", result);
    //Test GetElementAtIndex when index = 0
    result = dpl_get_element_at_index(list,0);
    ck_assert_msg(result == 'A', "Failure: expected 0, but got %c", result);
    //Test GetElementAtIndex when index = 99
    result = dpl_get_element_at_index(list,99);
    ck_assert_msg(result == 'D', "Failure: expected 0, but got %c", result);
}
END_TEST


//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX1");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListNULL);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListEmpty);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListOneElement);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListMultipleElements);
    tcase_add_test(tc1_1, test_Size);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListNull);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListEmpty);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListOneElement);
    tcase_add_test(tc1_1, test_GetReferenceAtIndexListMultipleElements);
    tcase_add_test(tc1_1,test_GetIndexOfElementListNull);
    tcase_add_test(tc1_1,test_GetIndexOfElementListEmpty);
    tcase_add_test(tc1_1,test_GetIndexOfElementListOneElement);
    tcase_add_test(tc1_1,test_GetIndexOfElementListMultipleElements);
    tcase_add_test(tc1_1,test_GetElementAtIndexListNull);
    tcase_add_test(tc1_1,test_GetElementAtIndexListEmpty);
    tcase_add_test(tc1_1,test_GetElementAtIndexListOneElement);
    tcase_add_test(tc1_1,test_GetElementAtIndexListMultipleElements);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
