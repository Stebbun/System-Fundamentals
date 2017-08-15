#include "debug.h"
#include "arraylist.h"
#include "foreach.h"

pthread_once_t once_control = PTHREAD_ONCE_INIT;
pthread_key_t indexKey;

void *foreach_init(arraylist_t *self){
    void *ret = NULL;

    pthread_mutex_lock(&self->fecnt_mutex);
    self->foreachcnt++;
    if(self->foreachcnt == 1)
        pthread_mutex_lock(&self->delete_mutex);
    pthread_mutex_unlock(&self->fecnt_mutex);

    pthread_once(&once_control, create_key_once);

    void* ptr = pthread_getspecific(indexKey);
    if(ptr == NULL){
        ptr = malloc(sizeof(local_data_t));

        pthread_setspecific(indexKey, ptr);
    }

    local_data_t* dataPtr = (local_data_t*) ptr;
    dataPtr->currentIndex = 0;
    dataPtr->list = self;

    ret = get_index_al(self, 0);

    return ret;
}

void *foreach_next(arraylist_t *self, void* data){
    void *ret = NULL;

    local_data_t* dataPtr = pthread_getspecific(indexKey);
    if(data != NULL){
        //update the item at the current index
        insert_index_al(self, data, dataPtr->currentIndex);
        free(data);
    }


    if(dataPtr->currentIndex < ((arraylist_t*)self)->length){
        dataPtr->currentIndex++;
        ret = get_index_al(self, dataPtr->currentIndex);
    }
    else{
        foreach_break_f();
    }

    return ret;
}

size_t foreach_index(){
    size_t ret = 0;

    local_data_t* dataPtr = pthread_getspecific(indexKey);
    if(dataPtr != NULL){
        ret = dataPtr->currentIndex;
    }
    else{
        ret = UINT_MAX;
    }

    return ret;
}

bool foreach_break_f(){
    bool ret = true;

    local_data_t* dataPtr = pthread_getspecific(indexKey);
    pthread_mutex_lock(&dataPtr->list->fecnt_mutex);
    dataPtr->list->foreachcnt--;
    if(dataPtr->list->foreachcnt == 0)
        pthread_mutex_unlock(&dataPtr->list->delete_mutex);
    pthread_mutex_unlock(&dataPtr->list->fecnt_mutex);
    free(dataPtr);

    return ret;
}

int32_t apply(arraylist_t *self, int32_t (*application)(void*)){
    int32_t ret = 0;

    foreach(void, value, self){
        int val = (*application)(value);
        if(val == -1){
            value = NULL;
        }
    }

    return ret;
}

void create_key_once(){
    pthread_key_create(&indexKey, NULL);
}
