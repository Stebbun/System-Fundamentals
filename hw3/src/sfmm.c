/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include "sfmm_helper.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;
static int pages = 0;
static void* start;		//start of heap

static size_t allocatedBlocks = 0;
static size_t splinterBlocks = 0;
static size_t totalPadding = 0;
static size_t splintering = 0;
static size_t coalesces = 0;
static double peakMemoryUtilization = 0;
static double currentTotalPayload = 0;

void *sf_malloc(size_t size) {
	if(size <= 0){
		errno = EINVAL;
		return NULL;
	}

	//request heap space if freelist_head is null
	if(freelist_head == NULL){
		freelist_head = sf_sbrk(4096);
		if(pages == 0){
			start = freelist_head;
		}
		if(pages < 4){
			pages++;
		}
		else{
			errno = ENOMEM;
			return NULL;
		}

		freelist_head->next = NULL;
		freelist_head->prev = NULL;

		freelist_head->header.alloc = 0;
		freelist_head->header.splinter = 0;
		freelist_head->header.block_size = 256;
		freelist_head->header.requested_size = 0;
		freelist_head->header.unused_bits = 0;
		freelist_head->header.splinter_size = 0;
		freelist_head->header.padding_size = 0;

		char* tempPtr =  (char*)freelist_head;

		sf_footer* footer = (sf_footer*)(tempPtr + 4096 - SF_FOOTER_SIZE);
		footer->alloc = 0;
		footer->splinter = 0;
		footer->block_size = 256;
	}

	//now we have a free block. We allocate space for the malloc call
	//iterate though the freelist to find the best fit.
	sf_free_header* cursor = freelist_head;
	sf_free_header* minBlock = NULL;

	uint64_t blockSize = SF_HEADER_SIZE + SF_FOOTER_SIZE + size;
	uint64_t padding = 16 - (blockSize % 16);
	if(padding == 16){
		padding = 0;
	}
	blockSize += padding;

	//printf("%i\n", blockSize);
	do{
		if((cursor->header.block_size << 4) >= blockSize){
			if(minBlock == NULL){
				minBlock = cursor;
			}
			else if((minBlock->header.block_size << 4) > cursor->header.block_size){
				minBlock = cursor;
			}
		}

		cursor = cursor->next;
	}while(cursor != NULL);

	//what happens if a fit couldnt be found? handle this below
	//keep track of max page count
	while(minBlock == NULL){
		//add a page
		sf_free_header* newFreeBlock = sf_sbrk(4096);
		if(pages < 4){
			pages++;
		}
		else{
			errno = ENOMEM;
			return NULL;
		}
		newFreeBlock->next = NULL;

		newFreeBlock->header.alloc = 0;
		newFreeBlock->header.splinter = 0;
		newFreeBlock->header.block_size = 256;
		newFreeBlock->header.requested_size = 0;
		newFreeBlock->header.unused_bits = 0;
		newFreeBlock->header.splinter_size = 0;
		newFreeBlock->header.padding_size = 0;

		char* tempPtr =  (char*)newFreeBlock;

		sf_footer* footer = (sf_footer*)(tempPtr + 4096 - SF_FOOTER_SIZE);
		footer->alloc = 0;
		footer->splinter = 0;
		footer->block_size = 256;

		//find the last node in the free list and connect it with newFreeBlock
		sf_free_header* ptr = freelist_head;
		while(ptr->next != NULL){
			ptr = ptr->next;
		}
		ptr->next = newFreeBlock;
		newFreeBlock->prev = ptr;

		//check if the footer before newFreeBlock is free block
		//check also if there is a free block before newFreeBlock to coallesce
		if(newFreeBlock->prev != NULL){
			char* footPtr = ( (char*) newFreeBlock) - 8;
			sf_footer* footBlockPtr = (sf_footer*) footPtr;

			//check alloc
			if(footBlockPtr->alloc == 0){
				//then coallesce
				sf_coallesce_prev(footBlockPtr, newFreeBlock);
			}
		}
		//iterate through free list for a minBlock
		sf_free_header* cursor = freelist_head;
		do{
			if((cursor->header.block_size << 4) >= blockSize){
				if(minBlock == NULL){
					minBlock = cursor;
				}
				else if((minBlock->header.block_size << 4) > cursor->header.block_size){
					minBlock = cursor;
				}
			}

			cursor = cursor->next;
		}while(cursor != NULL);
	}

	//now the best fit block has been selected
	//how much do i need to move the header by?
	//if alloc block size and free block size are the same, dont need to move header and split
	//if alloc block size and free block size are not the same, we must move header to a higher address.
	//minBlock is smallest free block, blockSize is the alloc block
	uint64_t freeBlockByteSize = minBlock->header.block_size << 4;
	uint64_t leftoverFreeBlock = freeBlockByteSize - blockSize;
	uint64_t splinterSize = 0;
	uint64_t isSplinter = 0;
	if(freeBlockByteSize != blockSize){

		//check if the leftover free block is less than 32 bytes
		if(leftoverFreeBlock < 32){
			//then the leftover free block would be a splinter and we get rid of the free block altogether
			splinterSize = leftoverFreeBlock;
			isSplinter = 1;

			//then we dont need to move the header since it will be deleted.
			//then we need to link the previous and next nodes together
			if(minBlock->prev != NULL && minBlock->next != NULL){
				sf_free_header* prevNode = minBlock->prev;
				sf_free_header* nextNode = minBlock->next;

				prevNode->next = nextNode;
				nextNode->prev = prevNode;
			}

			if(minBlock->prev == NULL && minBlock->next != NULL){
				freelist_head = minBlock->next;
				freelist_head->prev = NULL;
			}
			if(minBlock->prev != NULL && minBlock->next == NULL){
				minBlock->prev->next = NULL;
			}
			if(minBlock->prev == NULL && minBlock->next == NULL){
				freelist_head = NULL;
			}
		}
		else{
			//otherwise there are no splinters and we move the free block header by blockSize bytes
			char* newFreeHeader = (char*) minBlock;
			newFreeHeader += blockSize;

			//newFreeHeader now points to correct header location but its fields are empty;
			sf_free_header* castedFreeHeader = (sf_free_header*) newFreeHeader;
			//update the header
			castedFreeHeader->header.alloc = 0;
			castedFreeHeader->header.splinter = 0;
			castedFreeHeader->header.block_size = leftoverFreeBlock >> 4;
			castedFreeHeader->header.requested_size = 0;
			castedFreeHeader->header.unused_bits = 0;
			castedFreeHeader->header.splinter_size = 0;
			castedFreeHeader->header.padding_size = 0;
			if(minBlock->prev != NULL){
				castedFreeHeader->prev = minBlock->prev;
			}
			else{
				castedFreeHeader->prev = NULL;
				freelist_head = castedFreeHeader;
			}

			if(minBlock->next != NULL){
				castedFreeHeader->next = minBlock->next;
			}

			//also update footer

			sf_footer* footer = (sf_footer*)(newFreeHeader + (castedFreeHeader->header.block_size << 4) - SF_FOOTER_SIZE);
			footer->alloc = 0;
			footer->splinter = 0;
			footer->block_size = castedFreeHeader->header.block_size;
		}

	}
	else{
		if(minBlock->prev != NULL && minBlock->next != NULL){
				sf_free_header* prevNode = minBlock->prev;
				sf_free_header* nextNode = minBlock->next;

				prevNode->next = nextNode;
				nextNode->prev = prevNode;
		}
		if(minBlock->prev == NULL && minBlock->next != NULL){
				freelist_head = minBlock->next;
				freelist_head->prev = NULL;
		}

		if(minBlock->prev != NULL && minBlock->next == NULL){
				minBlock->prev->next = NULL;
		}
		if(minBlock->prev == NULL && minBlock->next == NULL){
			freelist_head = NULL;
		}
	}
	//insert the allocated header and footer
	//minBlock points to beginning of original free header
	//blockSize is size of allocated block
	sf_header* allocatedBlockHeader = (sf_header*) minBlock;
	allocatedBlockHeader->alloc = 1;
	allocatedBlockHeader->splinter = isSplinter;
	allocatedBlockHeader->block_size = (blockSize >> 4) + (splinterSize >> 4);
	allocatedBlockHeader->requested_size = size;
	allocatedBlockHeader->unused_bits = 0;
	allocatedBlockHeader->splinter_size = splinterSize;
	allocatedBlockHeader->padding_size = padding;

	char* alloBlockHeaderChar = (char*) allocatedBlockHeader;
	void* payloadPtr = ((char*)alloBlockHeaderChar) + SF_HEADER_SIZE;

	sf_footer* footer = (sf_footer*)(alloBlockHeaderChar + (allocatedBlockHeader->block_size << 4) - SF_FOOTER_SIZE);
	footer->alloc = 1;
	footer->splinter = isSplinter;
	footer->block_size = (blockSize >> 4) + (splinterSize >> 4);

	allocatedBlocks++;
	currentTotalPayload += allocatedBlockHeader->requested_size;
	double currentUtil = currentTotalPayload / (pages*4096);
	if(currentUtil > peakMemoryUtilization)
		peakMemoryUtilization = currentUtil;

	if(allocatedBlockHeader->splinter == 1){
		splinterBlocks++;
		splintering += allocatedBlockHeader->splinter_size;
	}
	totalPadding += padding;

	return payloadPtr;
}

void *sf_realloc(void *ptr, size_t size) {
	sf_header* blockHeader = (sf_header*) (((char*)ptr) - SF_HEADER_SIZE);

	void* endPtr = sf_sbrk(0);
	if(!(ptr >= start && ptr < endPtr)){
		errno = EINVAL;
		return NULL;
	}

	if(blockHeader->alloc == 0){
		errno = EINVAL;
		return NULL;
	}

	if(size == 0){
		free(ptr);
		return NULL;
	}


	if(blockHeader->requested_size > size){
		//to smaller size
		currentTotalPayload += size - blockHeader->requested_size;
		double currentUtil = currentTotalPayload / (pages*4096);
		if(currentUtil > peakMemoryUtilization)
			peakMemoryUtilization = currentUtil;
		return sf_realloc_smaller(blockHeader, size);
	}
	else if(blockHeader->requested_size == size){
		//sizes are the same, return pointer
		return ptr;
	}
	else{
		//to larger size
		currentTotalPayload += size - blockHeader->requested_size;
		double currentUtil = currentTotalPayload / (pages*4096);
		if(currentUtil > peakMemoryUtilization)
			peakMemoryUtilization = currentUtil;
		return sf_realloc_larger(blockHeader, size);
	}
}

void sf_free(void* ptr) {
	char* temp = (char*) ptr;
	temp -= SF_HEADER_SIZE;

	//check if valid ptr
	void* endPtr = sf_sbrk(0);
	if(!(ptr >= start && ptr < endPtr)){
		errno = EINVAL;
		return;
	}

	//turn allocated block into free block
	sf_header* allocatedBlock = (sf_header*) temp;
	uint64_t blockSize = allocatedBlock->block_size;

	if(allocatedBlock->alloc == 0){
		errno = EINVAL;
		return;
	}

	if(allocatedBlock->splinter == 1){
		splinterBlocks--;
		splintering -= allocatedBlock->splinter_size;
	}
	totalPadding -= allocatedBlock->padding_size;

	currentTotalPayload -= allocatedBlock->requested_size;

	sf_free_header* freeBlockHeader = (sf_free_header*) temp;
	freeBlockHeader->header.alloc = 0;
	freeBlockHeader->header.splinter = 0;
	freeBlockHeader->header.block_size = blockSize;
	freeBlockHeader->header.requested_size = 0;
	freeBlockHeader->header.unused_bits = 0;
	freeBlockHeader->header.splinter_size = 0;
	freeBlockHeader->header.padding_size = 0;

	//insert the free block correctly into the free list
	sf_free_header* cursor = freelist_head;

	//if address < head address
	if(freeBlockHeader < freelist_head){
		//freeblockHeader becomes new head
		freeBlockHeader->next = freelist_head;
		freelist_head->prev = freeBlockHeader;
		freelist_head = freeBlockHeader;
	}
	else{
		while(cursor->next != NULL && cursor->next < freeBlockHeader){
			cursor = cursor->next;
		}
		freeBlockHeader->prev = cursor;
		freeBlockHeader->next = cursor->next;

		if(cursor->next != NULL){
			cursor->next->prev = freeBlockHeader;
		}

		cursor->next = freeBlockHeader;

	}


	//coallesce the free blocks
	sf_coallesce(freeBlockHeader);

	allocatedBlocks--;

	return;
}

int sf_info(info* ptr) {
	if(pages == 0)
		return -1;

	ptr->allocatedBlocks = allocatedBlocks;
	ptr->splinterBlocks = splinterBlocks;
	ptr->padding = totalPadding;
	ptr->splintering = splintering;
	ptr->coalesces = coalesces;
	ptr->peakMemoryUtilization = peakMemoryUtilization;

	return 0;
}

void sf_coallesce(sf_free_header* freeBlock){
	sf_footer* prevFooter;
	sf_free_header* nextHeader;

	int isPrevBlockFree = 1;
	//check if footer before is alloc'd
	char* temp = (char*) freeBlock;
	if(freeBlock == freelist_head){
		isPrevBlockFree = 1;
	}
	else{
		temp -= SF_FOOTER_SIZE;
		prevFooter = (sf_footer*) temp;
		isPrevBlockFree = prevFooter->alloc;
	}

	//check if header after is alloc'd

	int isNextBlockFree = 0;
	temp = (char*) freeBlock;
	if(freeBlock->next != NULL){
		temp += (freeBlock->header.block_size) << 4;
		nextHeader = (sf_free_header*) temp;
		isNextBlockFree = nextHeader->header.alloc;
	}

	//for convenience, 1 will represent allocated and 0 will represent free
	if(isPrevBlockFree == 1 && isNextBlockFree == 1){
		//prev is alloc and next is alloc, so there is no coallescing to be done
	}
	else if(isPrevBlockFree == 1 && isNextBlockFree == 0){
		//prev is alloc and next is free, coallesce next block
		sf_coallesce_next(nextHeader, freeBlock);
	}
	else if(isPrevBlockFree == 0 && isNextBlockFree == 1){
		//prev is free and next is alloc, coallesce prev block
		sf_coallesce_prev(prevFooter, freeBlock);
	}
	else if(isPrevBlockFree == 0 && isNextBlockFree == 0){
		//prev is free and next is free, coallesce prev and next block
		sf_coallesce_both(prevFooter, nextHeader, freeBlock);
	}
}

void sf_coallesce_prev(sf_footer* prevFooter, sf_free_header* currentHeader){
	coalesces++;
	//get header of prevFooter
	void* headerPtr = ( (char*) prevFooter) + 8 - (prevFooter->block_size << 4);
	sf_free_header* prevHeader = (sf_free_header*) headerPtr;

	//update previous header
	prevHeader->header.block_size += currentHeader->header.block_size;

	//get current footer

	void* footerPtr = ( (char*) currentHeader) + (currentHeader->header.block_size << 4) - 8;
	sf_footer* currentFooter = (sf_footer*) footerPtr;

	//update current footer
	currentFooter->block_size = prevHeader->header.block_size;

	//update next and previous pointers
	//update next pointer for new header to next pointer for old header
	prevHeader->next = currentHeader->next;
}

void sf_coallesce_next(sf_free_header* nextHeader, sf_free_header* currentHeader){
	coalesces++;
	//update header
	currentHeader->header.block_size += nextHeader->header.block_size;

	char* temp = (char*) currentHeader;

	//update footer
	sf_footer* nextFooter = (sf_footer*) (temp + (currentHeader->header.block_size << 4) - SF_FOOTER_SIZE);

	nextFooter->block_size = currentHeader->header.block_size;

	//update current block's next pointer to next block's next pointer
	currentHeader->next = nextHeader->next;

}

void sf_coallesce_both(sf_footer* prevFooter, sf_free_header* nextHeader, sf_free_header* currentHeader){
	sf_coallesce_next(nextHeader, currentHeader);
	sf_coallesce_prev(prevFooter, currentHeader);
	coalesces--;
}

void* sf_realloc_smaller(sf_header* oldHeader, size_t size){
	totalPadding -= oldHeader->padding_size;
	if(oldHeader->splinter == 1){
		splinterBlocks--;
		splintering -=  oldHeader->splinter_size;
	}

	uint64_t oldBlockSize = oldHeader->block_size;
	uint64_t newBlockSize = SF_HEADER_SIZE + SF_FOOTER_SIZE + size;
	uint64_t padding = 16 - (newBlockSize % 16);
	if(padding == 16){
		padding = 0;
	}
	newBlockSize += padding;
	uint64_t splinterSize = (oldHeader->block_size << 4) - newBlockSize;
	splinterSize = splinterSize % 32;
	// int x = splinterSize;
	// int y = oldHeader->block_size << 4;
	// int z = newBlockSize;

	// printf("%i %i %i\n", x, y, z);


	totalPadding += padding;

	if(splinterSize == 0){
		char* temp = (char*) oldHeader;
		temp += newBlockSize - SF_FOOTER_SIZE;
		sf_footer* newFooter = (sf_footer*) temp;

		//update header
		oldHeader->block_size = newBlockSize >> 4;
		oldHeader->splinter = 0;
		oldHeader->requested_size = size;
		oldHeader->splinter_size = 0;
		oldHeader->padding_size = padding;
		oldHeader->alloc = 1;

		//update footer
		newFooter->alloc = 1;
		newFooter->splinter = 0;
		newFooter->block_size = newBlockSize >> 4;

		//sf_blockprint(oldHeader);

		temp = (char*) newFooter;
		temp += SF_FOOTER_SIZE;

		//set values of new free block and insert into free list
		//coallesce if next block is also free
		//sf_free_header* newFreeBlock = (sf_free_header*) temp;
		//what is the block size of the new free block?
		//old block size - new block size
		uint64_t freeBlockSize = ((oldBlockSize << 4) - newBlockSize);
		sf_free_header* freeBlock = (sf_free_header*) temp;

		freeBlock->header.alloc = 0;
		freeBlock->header.splinter = 0;
		freeBlock->header.block_size = freeBlockSize >> 4;
		freeBlock->header.requested_size = 0;
		freeBlock->header.unused_bits = 0;
		freeBlock->header.splinter_size = 0;
		freeBlock->header.padding_size = 0;

		temp += freeBlockSize - SF_FOOTER_SIZE;

		sf_footer* freeFooter = (sf_footer*) temp;
		freeFooter->alloc = 0;
		freeFooter->splinter = 0;
		freeFooter->block_size = freeBlockSize >> 4;

		//insert freeblock into freelist
		sf_free_header* cursor = freelist_head;

		//if address < head address
		if(freeBlock < freelist_head){
			//freeblockHeader becomes new head
			freeBlock->next = freelist_head;
			freelist_head->prev = freeBlock;
			freelist_head = freeBlock;
		}
		else{
			while(cursor->next != NULL && cursor->next < freeBlock){
				cursor = cursor->next;
			}
			freeBlock->prev = cursor;
			freeBlock->next = cursor->next;

			if(cursor->next != NULL){
				cursor->next->prev = freeBlock;
			}

			cursor->next = freeBlock;

		}

		sf_coallesce(freeBlock);
	}
	else{
		//if there is a splinter, check if there is an adjacent free block
		//else update the header and footer to indicate the splinter and size of splinter
		splinterBlocks++;
		splintering += splinterSize;

		char* temp = (char*) oldHeader;
		int isCase1 = 0;
		temp += oldBlockSize << 4;

		sf_header* nextHeader = (sf_header*) temp;

		if((void*)temp < sf_sbrk(0)){
			if(nextHeader->alloc == 0){
				sf_free_header* adjFreeBlock = (sf_free_header*) nextHeader;
				sf_coallesce_splinter(adjFreeBlock, splinterSize);
				isCase1 = 1;
			}
		}
		if(isCase1 == 0){
			//update header and footer to indicate splinter
			oldHeader->splinter = 1;
			oldHeader->splinter_size = splinterSize;
			oldHeader->requested_size = size;
			oldHeader->padding_size = padding;

			temp -= SF_HEADER_SIZE;

			sf_footer* footer = (sf_footer*) temp;
			footer->splinter = 1;
		}
	}

	return ((char*)oldHeader + SF_HEADER_SIZE);
}

void* sf_realloc_larger(sf_header* oldHeader, size_t size){
	totalPadding -= oldHeader->padding_size;
	//check if theres an adjacent free block in memory

	//sf_blockprint(oldHeader);
	char* temp = (char*) oldHeader;
	temp += oldHeader->block_size << 4;

	// int j = oldHeader->block_size << 4;
	// printf("%i\n", j);

	 sf_header* nextHeader = (sf_header*) temp;

	if((void*)temp < sf_sbrk(0)){
		if(nextHeader->alloc == 0){
			//check if this block and next free block can hold the new block
			uint64_t combBlockSize = (oldHeader->block_size + nextHeader->block_size) << 4;
			uint64_t requiredSize = SF_HEADER_SIZE + SF_FOOTER_SIZE + size;
			uint64_t padding = 16 - (size % 16);
			if(padding == 16){
				padding = 0;
			}
			requiredSize += padding;
			totalPadding += padding;

			if(combBlockSize >= requiredSize){
				//then coallesce the two blocks and split if possible
				return sf_coallesce_realloc(oldHeader, combBlockSize, requiredSize, padding);
			}
			else{
				//move the data to a new block and free the old block
				return sf_realloc_block(oldHeader, size);
			}
		}
	}
	return sf_realloc_block(oldHeader, size);
}

void sf_coallesce_splinter(sf_free_header* adjFreeBlock, uint64_t splinterSize){
	coalesces++;
	uint64_t newBlockSize = adjFreeBlock->header.block_size + (splinterSize >> 4);

	char* temp = (char*) adjFreeBlock;
	temp -= splinterSize;

	sf_free_header* newFreeBlock = (sf_free_header*) temp;

	newFreeBlock->header.alloc = 0;
	newFreeBlock->header.splinter = 0;
	newFreeBlock->header.block_size = newBlockSize;
	newFreeBlock->header.requested_size = 0;
	newFreeBlock->header.unused_bits = 0;
	newFreeBlock->header.splinter_size = 0;
	newFreeBlock->header.padding_size = 0;
	newFreeBlock->prev = adjFreeBlock->prev;
	newFreeBlock->next = adjFreeBlock->next;

	//update footer
	temp = (char*) newFreeBlock;
	temp += (newBlockSize << 4) - SF_FOOTER_SIZE;
	sf_footer* footer = (sf_footer*) temp;
	footer->block_size = newBlockSize;
}

void* sf_coallesce_realloc(sf_header* oldHeader, uint64_t combBlockSize, uint64_t requiredSize, uint64_t padding){
	uint64_t diff = combBlockSize - requiredSize;
	coalesces++;

	if(diff == 0){
		//perfect fit
		//fix the free list and delete free block
		//update header and footer
		char* temp = (char*) oldHeader;
		temp += oldHeader->block_size << 4;

		sf_free_header* freeBlock = (sf_free_header*) temp;

		sf_free_header* prevFree = freeBlock->prev;
		sf_free_header* nextFree = freeBlock->next;
		if(prevFree != NULL){
			prevFree->next = nextFree;
		}
		if(nextFree != NULL){
			nextFree->prev = prevFree;
		}

		//update header and footer
		oldHeader->splinter = 0;
		oldHeader->block_size = requiredSize >> 4;
		oldHeader->requested_size = requiredSize - SF_FOOTER_SIZE - SF_HEADER_SIZE - padding;
		oldHeader->splinter_size = 0;
		oldHeader->padding_size = padding;

		//update footer
		temp = (char*) oldHeader;
		temp += (oldHeader->block_size << 4) -  SF_FOOTER_SIZE;
		sf_footer* footer = (sf_footer*) temp;
		footer->block_size = oldHeader->block_size;
		footer->alloc = 1;
		footer->splinter = 0;

		return ((char*)oldHeader) + SF_HEADER_SIZE;
	}
	else if(diff < 32){
		//fix the free list and delete free block
		char* temp = (char*) oldHeader;
		temp += oldHeader->block_size << 4;

		sf_free_header* freeBlock = (sf_free_header*) temp;

		sf_free_header* prevFree = freeBlock->prev;
		sf_free_header* nextFree = freeBlock->next;
		if(prevFree != NULL){
			prevFree->next = nextFree;
		}
		if(nextFree != NULL){
			nextFree->prev = prevFree;
		}

		//update header and footer to indicate splinter
		oldHeader->splinter = 1;
		oldHeader->block_size = requiredSize >> 4;
		oldHeader->requested_size = requiredSize - SF_FOOTER_SIZE - SF_HEADER_SIZE - padding;
		oldHeader->splinter_size = diff;
		oldHeader->padding_size = padding;

		//update footer
		temp = (char*) oldHeader;
		temp += (oldHeader->block_size << 4) - SF_FOOTER_SIZE;
		sf_footer* footer = (sf_footer*) temp;
		footer->block_size = oldHeader->block_size;
		footer->alloc = 1;
		footer->splinter = 1;

		return ((char*)oldHeader) + SF_HEADER_SIZE;
	}
	else{
		//split the block by moving free header higher
		uint64_t oldBlockSize = oldHeader->block_size << 4;
		char* temp = (char*) oldHeader;
		uint64_t shftAmt = requiredSize - oldBlockSize;
		temp += oldBlockSize;
		sf_free_header* freeBlock = (sf_free_header*) temp;

		sf_free_header* prevFree = freeBlock->prev;
		sf_free_header* nextFree = freeBlock->next;

		temp = (char*) freeBlock;
		temp += shftAmt;
		freeBlock = (sf_free_header*) temp;
		freeBlock->prev = prevFree;
		freeBlock->next = nextFree;

		if(prevFree != NULL){
			prevFree->next = nextFree;
		}
		if(nextFree != NULL){
			nextFree->prev = prevFree;
		}

		freeBlock->header.alloc = 0;
		freeBlock->header.splinter = 0;
		freeBlock->header.block_size = diff;
		freeBlock->header.requested_size = 0;
		freeBlock->header.unused_bits = 0;
		freeBlock->header.splinter_size = 0;
		freeBlock->header.padding_size = 0;

		//update free block footer
		temp = (char*) freeBlock;
		temp += diff - SF_FOOTER_SIZE;
		sf_footer* footer = (sf_footer*) temp;
		footer->block_size = diff;

		//update realloc'd block
		oldHeader->block_size = requiredSize >> 4;
		oldHeader->requested_size = requiredSize - SF_HEADER_SIZE - SF_FOOTER_SIZE - padding;
		oldHeader->unused_bits = 0;
		oldHeader->splinter_size = 0;
		oldHeader->padding_size = padding;

		temp = (char*) oldHeader;
		temp += (oldHeader->block_size << 4) - SF_FOOTER_SIZE;
		sf_footer* aFooter = (sf_footer*) temp;
		aFooter->alloc = 1;
		aFooter->splinter = 0;
		aFooter->block_size = oldHeader->block_size;

		return ((char*)oldHeader) + SF_HEADER_SIZE;
	}
}

void* sf_realloc_block(sf_header* oldHeader, size_t size){
	void* newBlock = sf_malloc(size);
	if(newBlock == NULL){
		return NULL;
	}
	char* dataPtr = ((char*)oldHeader) + SF_HEADER_SIZE;
	memcpy(newBlock, dataPtr, oldHeader->requested_size);

	char* temp = ((char*)oldHeader) + SF_HEADER_SIZE;
	sf_free(temp);

	return newBlock;
}
