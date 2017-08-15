#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <stdio.h>
#include <stdbool.h>
#include "arraylist.h"

/******************************************
 *                  ITEMS                 *
 ******************************************/
arraylist_t *global_list;

typedef struct {
    char* name;
    int32_t id;
    double gpa;
}student_t;

typedef struct{
    int i;
    float f;
    long double ld;
    char c1:4;
    char c2:4;
    short s;
    void *some_data;
}test_item_t;

/******************************************
 *              HELPER FUNCS              *
 ******************************************/
void test_item_t_free_func(void *argptr){
    test_item_t* ptr = (test_item_t*) argptr;
    if(!ptr)
        free(ptr->some_data);
    else
        cr_log_warn("%s\n", "Pointer was NULL");
}

void setup(void) {
    cr_log_warn("Setting up test");
    global_list = new_al(sizeof(test_item_t));
}

void teardown(void) {
    cr_log_error("Tearing down");
    delete_al(global_list, test_item_t_free_func);
}

/******************************************
 *                  TESTS                 *
 ******************************************/
Test(al_suite, 0_creation, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");
}

Test(al_suite, 1_deletion, .timeout=2){
    arraylist_t *locallist = new_al(sizeof(test_item_t));

    cr_assert_not_null(locallist, "List returned was NULL");

    delete_al(locallist, test_item_t_free_func);

    cr_assert(true, "Delete completed without crashing");
}

Test(al_suite, 2_insertion, .timeout=2, .init=setup, .fini=teardown){
    arraylist_t *locallist = new_al(sizeof(int));
    cr_assert(locallist->length == 0, "Length is not 0 when it should be");
    int i = 5;
    int* ptr = &i;
    insert_al(locallist, ptr);

    cr_assert(locallist->length == 1, "Length should be 1, but it's not");
    cr_assert(get_data_al(locallist, ptr) == 0, "data not found at correct index");

    delete_al(locallist, NULL);
}

Test(al_suite, 3_removal, .timeout=2, .init=setup, .fini=teardown){
    arraylist_t *locallist = new_al(sizeof(int));
    int i = 5;
    int* ptr = &i;
    insert_al(locallist, ptr);
    int j = 3;
    insert_al(locallist, &j);
    insert_al(locallist, &j);
    insert_al(locallist, &j);
    insert_al(locallist, &j);

    cr_assert(locallist->length == 5, "Length should be 5");
    cr_assert(locallist->capacity == 8, "capacity should be 8");

    remove_data_al(locallist, ptr);

    //arraylist should contain four 3s
    cr_assert(locallist->length == 4, "Length should be 4");
    cr_assert(locallist->capacity == 8, "capacity should be 8");
    cr_assert(get_data_al(locallist, &j) == 0, "data at index 0 should be 3");
    int* iptr = (int*)get_index_al(locallist, 0);
    cr_assert(*iptr == j, "value is not 3");

    delete_al(locallist, NULL);
}

