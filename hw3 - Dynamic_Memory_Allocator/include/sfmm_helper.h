#ifndef SFMM_H_H
#define SFMM_H_H
#include <string.h>

void sf_coallesce(sf_free_header* freeBlock);

void sf_coallesce_prev(sf_footer* prevFooter, sf_free_header* currentHeader);

void sf_coallesce_next(sf_free_header* nextHeader, sf_free_header* currentHeader);

void sf_coallesce_both(sf_footer* prevFooter, sf_free_header* nextHeader, sf_free_header* currentHeader);

void sf_coallesce_splinter(sf_free_header* adjFreeBlock, uint64_t splinterSize);

void* sf_coallesce_realloc(sf_header* oldHeader, uint64_t combBlockSize, uint64_t requiredSize, uint64_t padding);

void* sf_realloc_smaller(sf_header* oldHeader, size_t size);

void* sf_realloc_larger(sf_header* oldHeader, size_t size);

void* sf_realloc_block();

#endif
