#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "sfmm.h"
#include "debug.h"

// Define 20 megabytes as the max heap size
#define MAX_HEAP_SIZE (20 * (1 << 20))
#define VALUE1_VALUE 320
#define VALUE2_VALUE 0xDEADBEEFF00D

#define press_to_cont()                    \
    do {                                   \
        printf("Press Enter to Continue"); \
        while (getchar() != '\n')          \
            ;                              \
        printf("\n");                      \
    } while (0)

#define null_check(ptr, size)                                                                  \
    do {                                                                                       \
        if ((ptr) == NULL) {                                                                   \
            error("Failed to allocate %lu byte(s) for an integer using sf_malloc.\n", (size)); \
            error("%s\n","Aborting...");                                                            \
            assert(false);                                                                     \
        } else {                                                                               \
            success("sf_malloc returned a non-null address: %p\n", (void*)(ptr));                     \
        }                                                                                      \
    } while (0)

#define payload_check(ptr)                                                                   \
    do {                                                                                     \
        if ((unsigned long)(ptr) % 16 != 0) {                                                \
            warn("Returned payload address is not divisble by a quadword. %p %% 16 = %lu\n", \
                 (void*)(ptr), (unsigned long)(ptr) % 16);                                          \
        }                                                                                    \
    } while (0)

#define check_prim_contents(actual_value, expected_value, fmt_spec, name)                          \
    do {                                                                                           \
        if (*(actual_value) != (expected_value)) {                                                 \
            error("Expected " name " to be " fmt_spec " but got " fmt_spec "\n", (expected_value), \
                  *(actual_value));                                                                \
            error("%s\n","Aborting...");                                                                \
            assert(false);                                                                         \
        } else {                                                                                   \
            success(name " retained the value of " fmt_spec " after assignment\n",                 \
                    (expected_value));                                                             \
        }                                                                                          \
    } while (0)

int main(int argc, char *argv[]) {
    // Initialize the custom allocator
    sf_mem_init(MAX_HEAP_SIZE);

    // Tell the user about the fields
    info("Initialized heap with %dmb of heap space.\n", MAX_HEAP_SIZE >> 20);
    press_to_cont();

    // Print out title for first test
    printf("=== Test1: Allocation test ===\n");
    // Test #1: Allocate an integer
    int *value1 = sf_malloc(sizeof(int));
    null_check(value1, sizeof(int));
    payload_check(value1);
    // Print out the allocator block
    sf_varprint(value1);
    press_to_cont();

    // Now assign a value
    printf("=== Test2: Assignment test ===\n");
    info("Attempting to assign value1 = %d\n", VALUE1_VALUE);
    // Assign the value
    *value1 = VALUE1_VALUE;
    // Now check its value
    check_prim_contents(value1, VALUE1_VALUE, "%d", "value1");
    press_to_cont();

    printf("=== Test3: Allocate a second variable ===\n");
    info("Attempting to assign value2 = %ld\n", VALUE2_VALUE);
    long *value2 = sf_malloc(sizeof(long));
    null_check(value2, sizeof(long));
    payload_check(value2);
    // Assign a value
    *value2 = VALUE2_VALUE;
    // Check value
    check_prim_contents(value2, VALUE2_VALUE, "%ld", "value2");
    press_to_cont();

    printf("=== Test4: does value1 still equal %d ===\n", VALUE1_VALUE);
    check_prim_contents(value1, VALUE1_VALUE, "%d", "value1");
    press_to_cont();

    // Snapshot the freelist
    printf("=== Test5: Perform a snapshot ===\n");
    sf_snapshot(true);
    press_to_cont();

    // Free a variable
    printf("=== Test6: Free a block and snapshot ===\n");
    info("%s\n", "Freeing value1...");
    sf_free(value1);
    sf_snapshot(true);
    press_to_cont();

    // Allocate more memory
    printf("=== Test7: 8192 byte allocation ===\n");
    void *memory = sf_malloc(8192);
    sf_free(memory);
    press_to_cont();

    sf_mem_fini();

    return EXIT_SUCCESS;
}
