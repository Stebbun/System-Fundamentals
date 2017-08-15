#include "arraylist.h"
#include <errno.h>

/**
 * @visibility HIDDEN FROM USER
 * @return     true on success, false on failure
 */
//writer method
static bool resize_al(arraylist_t* self){
    bool ret = false;

    if(self->length == self->capacity){
    	//grow
    	//capacity of array will double
    	self->base = realloc(self->base, self->capacity*2);
    	if(self->base == NULL){
    		free(self);
    		errno = ENOMEM;
    		return NULL;
    	}
    	self->capacity *= 2;
    	ret = true;
    }
    else if(self->length == (self->capacity/2) - 1){
    	//shrink
    	//make sure not to shrink below INIT_SZ
    	if(self->capacity/2 >= INIT_SZ){
    		self->base = realloc(self->base, self->capacity/2);
	    	if(self->base == NULL){
	    		free(self);
	    		errno = ENOMEM;
	    		return NULL;
	    	}
	    	self->capacity /= 2;
    		ret = true;
    	}
    }

    return ret;
}

arraylist_t *new_al(size_t item_size){
    arraylist_t* ret = malloc(sizeof(arraylist_t));
    if(ret == NULL){
    	//don't forget to set errno here
    	errno = ENOMEM;
    	return NULL;
    }
    ret->capacity = INIT_SZ;
    ret->length = 0;
    ret->item_size = item_size;
    ret->base = calloc(INIT_SZ, item_size);
    if(ret->base == NULL){
    	//have to free ret or mem leaks
    	free(ret);
    	errno = ENOMEM;
    	return NULL;
    }
    sem_init(&ret->mutex, 0, 1);
    sem_init(&ret->write, 0, 1);
    ret->readcnt = 0;

    return ret;
}

//writer method
size_t insert_al(arraylist_t *self, void* data){
	lock(&self->write);
	//critical code starts
	if(self->length == self->capacity){
    	resize_al(self);
	}
	void* dest = ((char*)self->base) + (self->length * self->item_size);
	memcpy(dest, data, self->item_size);
	self->length++;
	//critical code ends
	unlock(&self->write);

    return self->length;
}

void insert_index_al(arraylist_t* self, void* data, size_t index){
	//writer method
	lock(&self->write);
	if(index < self->length){
		char* dest = ((char*)self->base) + (index * self->item_size);
		memcpy(dest, data, self->item_size);
	}
	unlock(&self->write);
}

//reader
size_t get_data_al(arraylist_t *self, void *data){
    size_t ret = 0;
    if(data == NULL){
    	return ret;
    }

    lock(&self->mutex);
    self->readcnt++;
    if(self->readcnt == 1)
    	lock(&self->write);
    unlock(&self->mutex);

    //start: critical code
    char* ptr = (char*) self->base;
    for(int i = 0; i < self->length; i++){
    	if(memcmp(ptr, data, self->item_size) == 0){
    		ret = i;
    		return ret;
    	}
    	ptr += self->item_size;
    }
    //end: critical code

    lock(&self->mutex);
    self->readcnt--;
    if(self->readcnt == 0)
    	unlock(&self->write);
    unlock(&self->mutex);

    //not found
    ret = UINT_MAX;
    errno = ENOMEM;
    return ret;
}

//reader
void *get_index_al(arraylist_t *self, size_t index){
    void *ret = NULL;

    lock(&self->mutex);
    self->readcnt++;
    if(self->readcnt == 1)
    	lock(&self->write);
    unlock(&self->mutex);

    //critical
    char* ptr = (char*) self->base;
    if(index >= self->length){
    	ptr += (self->length - 1) * self->item_size;
    }
    else{
    	ptr += index * self->item_size;
    }

    if((ret = malloc(self->item_size)) == NULL ){
    	errno = ENOMEM;
    	return NULL;
    }

    memcpy(ret, ptr, self->item_size);
    //critical
    lock(&self->mutex);
    self->readcnt--;
    if(self->readcnt == 0)
    	unlock(&self->write);
    unlock(&self->mutex);

    return ret;
}

//writer
bool remove_data_al(arraylist_t *self, void *data){
    bool ret = false;

    //need to lock before accessing self->base
    pthread_mutex_lock(&self->delete_mutex);
    lock(&self->write);
    char* ptr = (char*) self->base;
    //get address of data
    int i = 0;
    if(data == NULL){
    	//use base address
    }
    else{
    	for(i = 0; i < self->length; i++){
	    	if(memcmp(ptr, data, self->item_size) == 0){
	    		//break out of for loop once address for data is found
	    		ret = true;
	    		break;
	    	}
    		ptr += self->item_size;
    	}
    	if(ret == false)
    		return ret;
    }
    //ptr now contains address of data in the arraylist
    //i contains index of the data
    for(int j = i; j < self->length-1; j++){
    	memcpy(ptr, ptr + self->item_size, self->item_size);
    	ptr += self->item_size;
    }
    self->length--;

    if(self->length == (self->capacity/2) - 1){
    	resize_al(self);
    }
    unlock(&self->write);
    pthread_mutex_unlock(&self->delete_mutex);

    return ret;
}

//writer
void *remove_index_al(arraylist_t *self, size_t index){
    void *ret = 0;

    pthread_mutex_lock(&self->delete_mutex);
    lock(&self->write);

    //critical
    //get address of item at that index
    char* ptr = (char*) self->base;
    if(index >= self->length){
    	ptr += (self->length - 1) * self->item_size;
    }
    else{
    	ptr += index * self->item_size;
    }

    //make a copy
    //allocate space for the copy
    if((ret = malloc(self->item_size)) == NULL ){
    	errno = ENOMEM;
    	return NULL;
    }

    memcpy(ret, ptr, self->item_size);

    for(int j = index; j < self->length-1; j++){
    	memcpy(ptr, ptr + self->item_size, self->item_size);
    	ptr += self->item_size;
    }
    self->length--;


    if(self->length == (self->capacity/2) - 1){
    	resize_al(self);
    }
    unlock(&self->write);
    pthread_mutex_unlock(&self->delete_mutex);

    return ret;
}

//writer???
void delete_al(arraylist_t *self, void (*free_item_func)(void*)){
	if( (free_item_func) == NULL ){
		lock(&self->write);
	}
	else{
		lock(&self->write);
		char* ptr = self->base;
		for(int i = 0; i < self->length; i++){
			(*free_item_func)(ptr);
	    }
    	ptr += self->item_size;
	}


	free(self);
	unlock(&self->write);
    return;
}

void lock(sem_t* sem){
	sem_wait(sem);
}

void unlock(sem_t* sem){
	sem_post(sem);
}
