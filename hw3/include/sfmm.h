/**
 * === DO NOT MODIFY THIS FILE ===
 * If you need some other prototypes or constants in a header, please put them
 * in another header file.
 *
 * When we grade, we will be replacing this file with our own copy.
 * You have been warned.
 * === DO NOT MODIFY THIS FILE ===
 */
#ifndef SFMM_H
#define SFMM_H
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PADDING_SIZE_BITS 4
#define SPLINTER_SIZE_BITS 5
#define UNUSED_SIZE_BITS 9
#define REQUEST_SIZE_BITS 14
#define BLOCK_SIZE_BITS 28
#define SPLINTER_CONSUMED_BITS 2
#define ALLOC_SIZE_BITS 2






#define SF_HEADER_SIZE \
    ((ALLOC_SIZE_BITS + BLOCK_SIZE_BITS + UNUSED_SIZE_BITS + \
      PADDING_SIZE_BITS + REQUEST_SIZE_BITS + SPLINTER_SIZE_BITS + SPLINTER_CONSUMED_BITS) >> 3)

#define SF_FOOTER_SIZE SF_HEADER_SIZE

/*

                                      Format of a memory block
    +-----------------------------------------------------------------------------------------+
    |                                       64-bits wide                                      |
    +-----------------------------------------------------------------------------------------+

    +-------------+--------------+---------+---------------+---------------+----------+-------+
    | padding_size| splinter_size|  unused | requested_size|   block_size  | splinter | alloc | <- Header Block
    |   in bytes  |    in bytes  |  9 bits |   in bytes    |   in bytes    |    0s    |   0a  |
    |   4 bits    |    5 bits    |         |   14 bits     |   28 bits     |  2 bits  | 2 bits|
    +-------------+--------------+---------+---------------+---------------+----------+-------+
    |                                                                                         | Content of
    |                             Payload and Padding                                         | the payload
    |                                (N Memory Rows)                                          |
    |                                                                                         |
    |                                                                                         |
    +------------------------------------------------------+---------------+----------+-------+
    |                           unused                     |  block_size   | splinter | alloc |
    |                                                      |  in bytes     |    0s    |   0a  | <- Footer Block
    |                           32 bits                    |  28 bits      |  2 bits  | 2 bits|
    +------------------------------------------------------+---------------+----------+-------+
*/

struct __attribute__((__packed__)) sf_header {
  uint64_t          alloc : ALLOC_SIZE_BITS;
  uint64_t       splinter : SPLINTER_CONSUMED_BITS;
  uint64_t     block_size : BLOCK_SIZE_BITS;
  uint64_t requested_size : REQUEST_SIZE_BITS;
  uint64_t    unused_bits : UNUSED_SIZE_BITS;
  uint64_t  splinter_size : SPLINTER_SIZE_BITS;
  uint64_t   padding_size : PADDING_SIZE_BITS;

};

typedef struct sf_header sf_header;

struct __attribute__((__packed__)) sf_free_header {
    sf_header header;
    /* Note: These next two fields are only used when the block is free.
     *       They are not part of header, but we place them here for ease
     *       of access.
     */
    struct sf_free_header *next;
    struct sf_free_header *prev;
};

typedef struct sf_free_header sf_free_header;

struct __attribute__((__packed__)) sf_footer {
    uint64_t      alloc : ALLOC_SIZE_BITS;
    uint64_t   splinter : SPLINTER_CONSUMED_BITS;
    uint64_t block_size : BLOCK_SIZE_BITS;
    /* Other 32-bits are unused */
};

typedef struct sf_footer sf_footer;

typedef struct {
    size_t allocatedBlocks;
    size_t splinterBlocks;
    size_t padding;
    size_t splintering;
    size_t coalesces;
    double peakMemoryUtilization;
} info;
/**
 * You should store the head of your free list in this variable.
 */
extern sf_free_header *freelist_head;

/* sfmm.c: Where you will define your functions for this assignment. */

/*
 * This is your implementation of malloc. It acquires uninitialized memory that
 * is aligned and padded properly for the underlying system.
 * @param size The number of bytes requested to be allocated.
 * @return If successful, the pointer to a valid region of memory of the
 * requessted size is returned, else NULL is returned and ERRNO is set  accordingly.
 * If size is set to zero, then NULL is returned.
 */
void *sf_malloc(size_t size);


/*
 * Resizes the memory pointed to by ptr to size bytes.
 * @param ptr Address of the memory region to resize.
 * @param size The minimum size to resize the memory to.
 * @return If successful, the pointer to a valid region of memory is
 * returned, else NULL is returned and ERRNO is set accordingly.
 *
 * A realloc call with a size of zero returns NULL and sets ERRNO
 * accordingly.
 */
void *sf_realloc(void *ptr, size_t size);

/*
 *  This function will copy the the correct values to the fields
 *  in the memory info struct.
 *
 *  @param meminfo A pointer to the memory info struct passed
 *  to the function, upon return it will containt the calculated
 *  for current fragmentation
 *
 *  @return If successful return 0, if failure return -1
 */
int sf_info(info* ptr);

/*
 * Marks a dynamically allocated region as no longer in use.
 * Adds the newly freed block to the free list.
 * @param ptr Address of memory returned by the function sf_malloc.
 * If the address of the memory being freed is not valid, this
 * function sets ERRNO accordingly.
 */
void sf_free(void *ptr);


/* sfutil.c: Helper functions already created for this assignment. */

/*
 * This function must be called in your program's implementation ONCE,
 * before the use of any of your allocator functions.
 * This function DOES NOT allocate any space to your allocator.
 */
void sf_mem_init();

/*
 * This routine will finialize your memory allocator. It should be called
 * in your program once, prior to exiting your program.
 * If implemented cleanly, your program should have no memory leaks in
 * valgrind after this function is called.
 */
void sf_mem_fini();

/*
 * This function changes the position of your program's break.
 * Calling sf_sbrk(0) can be used to find the current location of the heap.
 *
 * @param increment The number of bytes requested.
 * If 0 sf_sbrk will return current location of the heap,
 * Otherwise the program's break will be incremented by ceil(increment / 4096) * 4096.
 *
 * @return On success, sf_sbrk(size_t increment) returns the previous program break.
 * (If the break was increased, then this value is a pointer to the
 * start of the newly allocated memory). On error, (void *) -1 is returned,
 * and errno is set to ENOMEM.
 */
void *sf_sbrk(size_t increment);

/*
 * Function which outputs the state of the free-list to stdout.
 * Performs checks on the placement of the header and footer,
 * and if the memory payload is correctly aligned.
 * See sf_snapshot section for details on output format.
 * @param verbose If true, snapshot will additionally print out
 * each memory block using sf_blockprint function.
 */
void sf_snapshot(bool verbose);

/*
* Function which prints human readable block format
* @param block Address of the block header in memory.
*/
void sf_blockprint(void* block);

/*
 * Prints human readable block format from the address of the payload.
 * IE. subtracts header size from the data pointer to obtain the address
 * of the block header. Calls sf_blockprint internally to print.
 * @param data Pointer to payload data in memory
 * (value returned by sf_malloc).
 */
void sf_varprint(void *data);

#endif
