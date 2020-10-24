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
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL , but was %p", list);

        // Test free empty list
        list = dpl_create();
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL, but was %p", list);
        // Test free with one element
        list = dpl_create();
	char a = 'A';
	char *ptr = &a;
	list = dpl_insert_at_index(list, ptr, 0);
	dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL, but was %p", list);
        // Test free with multiple elements
        list = dpl_create();
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
        list = dpl_insert_at_index(list, ptr, 0);
        list = dpl_insert_at_index(list, ptrb, 1);
        list = dpl_insert_at_index(list, ptrc, 2);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL, but was %p", list);
    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
        // Test inserting at index -1
        char a = 'A';
        char *ptr = &a;
	char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
        dplist_t *result = dpl_insert_at_index(NULL, ptr, -1);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // Test inserting at index 0
	result = dpl_insert_at_index(NULL, ptr, 0);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // Test inserting at index 99
        result = dpl_insert_at_index(NULL, ptr, 99);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
    }
END_TEST


START_TEST(test_ListInsertAtIndexListEmpty)
{
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, ptr, -1);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
    //Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, ptr, 0);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

    //Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, ptr, 99);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
}
END_TEST

START_TEST(test_ListInsertAtIndexListOneElement)
{
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, ptr, -1);
    result = dpl_insert_at_index(list, ptrb, -1);
    ck_assert_msg(dpl_size(result) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
    //Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, ptr, 0);
    result = dpl_insert_at_index(list, ptrb, 0);
    ck_assert_msg(dpl_size(result) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

    //Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, ptr, 99);
    result = dpl_insert_at_index(list, ptrb, 99);
    ck_assert_msg(dpl_size(result) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

}
END_TEST

START_TEST(test_ListInsertAtIndexListMultipleElements)
{
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
	char d = 'D';
	char *ptrd = &d;
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, ptr, -1);
    result = dpl_insert_at_index(list, ptrb, -1);
    result = dpl_insert_at_index(list, ptrc, -1);
    result = dpl_insert_at_index(list, ptrd, -1);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
    //Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, ptr, 0);
    result = dpl_insert_at_index(list, ptrb, 0);
    result = dpl_insert_at_index(list, ptrc, 0);
    result = dpl_insert_at_index(list, ptrd, 0);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);

    //Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, ptr, 99);
    result = dpl_insert_at_index(list, ptrb, 99);
    result = dpl_insert_at_index(list, ptrc, 99);
    result = dpl_insert_at_index(list, ptrd, 99);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",
                                         dpl_size(result));
    dpl_free(&list);
}
END_TEST


START_TEST(test_Size)
{
        char a = 'A';
        char *ptr = &a;
        char d = 'D';
        char *ptrd = &d;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;

    // Test when List = NULL
    dplist_t *list = NULL;
    int size = dpl_size(list);
    ck_assert_msg(size == -1, "Failure: expected size to be -1, got a size of %d",size);

    // Test when List = empty
    list = dpl_create();
    size = dpl_size(list);
    ck_assert_msg(size == 0, "Failure: expected size to be 0, got a size of %d",size);

    // Test when List has 1 element
    dplist_t *result = dpl_insert_at_index(list, ptr, 0);
    size = dpl_size(list);
    ck_assert_msg(size == 1, "Failure: expected size to be 1, got a size of %d",size);

    // Test when List has multiple elements
    result = dpl_insert_at_index(list, ptrb, 1);
    result = dpl_insert_at_index(list, ptrc, 2);
    result = dpl_insert_at_index(list, ptrd, 3);
    ck_assert_msg(dpl_size(result) == 4, "Failure: expected list to have size of 4, got a size of %d",size);
}
END_TEST


START_TEST(test_RemoveAtIndexListNull)
{
    // Test RemoveAtIndex when index = -1
    dplist_t *result = dpl_remove_at_index(NULL, -1);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = 0
    result = dpl_remove_at_index(NULL, 0);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
    // Test RemoveAtIndex when index = 99
    result = dpl_remove_at_index(NULL, 99);
    ck_assert_msg(result == NULL,"Failure: expected NULL, but got %p",result);
}
END_TEST


START_TEST(test_RemoveAtIndexListEmpty)
{
    dplist_t *list = dpl_create();
    // Test RemoveAtIndex when index = -1
    dplist_t *result = dpl_remove_at_index(list, -1);
    ck_assert_msg(result == list,"Failure1: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = 0
    result = dpl_remove_at_index(list, 0);
    ck_assert_msg(result == list,"Failure2: expected %p, but got %p",list,result);
    // Test RemoveAtIndex when index = 99
    result = dpl_remove_at_index(list, 99);
    ck_assert_msg(result == list,"Failure3: expected %p, but got %p",list,result);
}
END_TEST


START_TEST(test_RemoveAtIndexListOneElement)
{
        char a = 'A';
        char *ptr = &a;
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
    // Test RemoveAtIndex when index = -1
    dplist_t *result = dpl_remove_at_index(list, -1);
    ck_assert_msg(dpl_size(result) == 0,"Failure: expected size to be 0, but it is %d",dpl_size(result));
    // Test RemoveAtIndex when index = 0
    list = dpl_insert_at_index(list, ptr, 0);
    result = dpl_remove_at_index(list, 0);
    ck_assert_msg(dpl_size(result) == 0,"Failure: expected size to be 0, but it is %d",dpl_size(result));
    // Test RemoveAtIndex when index = 99
    list = dpl_insert_at_index(list, ptr, 0);
    result = dpl_remove_at_index(list, 99);
    ck_assert_msg(dpl_size(result) == 0,"Failure: expected size to be 0, but it is %d",dpl_size(result));
}
END_TEST


START_TEST(test_RemoveAtIndexListMultipleElements)
{
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
	char d = 'D';
	char *ptrd = &d;
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
    list = dpl_insert_at_index(list, ptrb, 1);
    list = dpl_insert_at_index(list, ptrc, 2);
    list = dpl_insert_at_index(list, ptrd, 3);
    // Test RemoveAtIndex when index = -1
    dplist_t *result = dpl_remove_at_index(list, -1);
    ck_assert_msg(dpl_size(result) == 3,"Failure: expected size to be 3, but it is %d",dpl_size(result));
    // Test RemoveAtIndex when index = 0
    list = dpl_insert_at_index(list, ptr, -1);
    result = dpl_remove_at_index(list, 0);
    ck_assert_msg(dpl_size(result) == 3,"Failure: expected size to be 3, but it is %d",dpl_size(result));
    // Test RemoveAtIndex when index = 99
    list = dpl_insert_at_index(list, ptr, 0);
    result = dpl_remove_at_index(list, 99);
    ck_assert_msg(dpl_size(result) == 3,"Failure: expected size to be 3, but it is %d",dpl_size(result));
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
        char a = 'A';
        char *ptr = &a;
    // Test getReference when index = -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
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
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
	char d = 'D';
	char *ptrd = &d;
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
    list = dpl_insert_at_index(list, ptrb, 1);
    list = dpl_insert_at_index(list, ptrc, 2);
    list = dpl_insert_at_index(list, ptrd, 3);

    // Test getReference when index = -1
    dplist_node_t *result =  dpl_get_reference_at_index(list, -1);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p", list->head,result);

    // Test getReference when index = 0
    result =  dpl_get_reference_at_index(list, 0);
    ck_assert_msg(result == list->head,"Failure: expected %p, but got %p",list->head,result);

    // Test getReference when index = 99
    result =  dpl_get_reference_at_index(list, 99);
    ck_assert_msg(result->element == ptrd,"Failure: expected D, but got %c",*(result->element));
}
END_TEST


START_TEST(test_GetIndexOfElementListNull)
{
        char a = 'A';
        char *ptr = &a;
    int result = dpl_get_index_of_element(NULL, ptr);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListEmpty)
{
        char a = 'A';
        char *ptr = &a;
    dplist_t *list = dpl_create();
    int result = dpl_get_index_of_element(list, ptr);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListOneElement)
{
        char a = 'A';
        char *ptr = &a;
	char b = 'B';
	char *ptrb = &b;
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);

    // Test GetIndexOfElement when in the list
    int result = dpl_get_index_of_element(list, ptr);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    // Test GetIndexOfElement when not in the list
    result = dpl_get_index_of_element(list, ptrb);
    ck_assert_msg(result == -1, "Failure: expected -1, but got %d", result);
}
END_TEST


START_TEST(test_GetIndexOfElementListMultipleElements)
{
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
	char d = 'D';
	char *ptrd = &d;
	char x = 'X';
	char *ptrx = &x;
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
    list = dpl_insert_at_index(list, ptrb, 1);
    list = dpl_insert_at_index(list, ptrc, 2);
    list = dpl_insert_at_index(list, ptrd, 3);

    // Test GetIndexOfElement when in the list
    int result = dpl_get_index_of_element(list, ptr);
    ck_assert_msg(result == 0, "Failure: expected 0, but got %d", result);
    // Test GetIndexOfElement when in the list
    result = dpl_get_index_of_element(list, ptrb);
    ck_assert_msg(result == 1, "Failure: expected 1, but got %d", result);
    // Test GetIndexOfElement when not in the list
    result = dpl_get_index_of_element(list, ptrx);
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
        char a = 'A';
        char *ptr = &a;
    //Test GetElementAtIndex when index = -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
    element_t result = dpl_get_element_at_index(list,-1);
    ck_assert_msg(result == ptr, "Failure: expected A, but got %c", *result);
    //Test GetElementAtIndex when index = 0
    result = dpl_get_element_at_index(list,0);
    ck_assert_msg(result == ptr, "Failure: expected 0, but got %c", *result);
    //Test GetElementAtIndex when index = 99
    result = dpl_get_element_at_index(list,99);
    ck_assert_msg(result == ptr, "Failure: expected 0, but got %c", *result);
}
END_TEST


START_TEST(test_GetElementAtIndexListMultipleElements)
{
        char a = 'A';
        char *ptr = &a;
        char b = 'B';
        char *ptrb = &b;
        char c = 'C';
        char *ptrc = &c;
	char d = 'D';
	char *ptrd = &d;
    //Test GetElementAtIndex when index = -1
    dplist_t *list = dpl_create();
    list = dpl_insert_at_index(list, ptr, 0);
    list = dpl_insert_at_index(list, ptrb, 1);
    list = dpl_insert_at_index(list, ptrc, 2);
    list = dpl_insert_at_index(list, ptrd, 3);
    element_t result = dpl_get_element_at_index(list,-1);
    ck_assert_msg(result == ptr, "Failure: expected A, but got %c", *result);
    //Test GetElementAtIndex when index = 0
    result = dpl_get_element_at_index(list,0);
    ck_assert_msg(result == ptr, "Failure: expected 0, but got %c", *result);
    //Test GetElementAtIndex when index = 99
    result = dpl_get_element_at_index(list,99);
    ck_assert_msg(result == ptrd, "Failure: expected 0, but got %c", *result);
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
    tcase_add_test(tc1_1,test_RemoveAtIndexListNull);
    tcase_add_test(tc1_1,test_RemoveAtIndexListEmpty);
    tcase_add_test(tc1_1,test_RemoveAtIndexListOneElement);
    tcase_add_test(tc1_1,test_RemoveAtIndexListMultipleElements);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
