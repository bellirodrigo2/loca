#include <string.h>
#include <stdio.h> //for testing only
#include <assert.h> //for testing only

#include "loca.h"

#define STD_SIZE 64

struct loca_meta_st{
    arr_size len;
    arr_size max;
};

struct llnode_st{
    struct llnode_st* next;
    struct llnode_st* prev;
    struct loca_meta_st meta;
};

struct circheader_st{
    arr_size virtual_end;
    struct loca_meta_st meta;
};

static const arr_size _size_loca_meta = sizeof(loca_meta);
static const arr_size _size_llnode = sizeof(llnode);
static const arr_size _size_circheader = sizeof(circheader);

void meta_print(byte* arr){
    loca_meta* lm = cast_meta(arr);
    printf("{ meta_loca->LEN = %ld, meta_loca->SIZE = %ld }\n",lm->len, lm->max);
    }

void meta_print_llist(byte* arr){
    if(!arr) printf("{ NULL }\n");
    llnode* node = cast_llist(arr);
    if(node->next ==NULL && node->prev ==NULL)
        printf("{ meta_loca->LEN = %ld, meta_loca->SIZE = %ld, NEXT = NULL, PREV = NULL }\n",node->meta.len, node->meta.max);
    else if(node->next ==NULL) 
        printf("{ meta_loca->LEN = %ld, meta_loca->SIZE = %ld, NEXT = NULL, PREV = %p }\n",node->meta.len, node->meta.max, (void*)node->prev);
    else if(node->prev == NULL)
        printf("{ meta_loca->LEN = %ld, meta_loca->SIZE = %ld, NEXT = %p, PREV = NULL }\n",node->meta.len, node->meta.max, (void*)node->next);
    else
        printf("{ meta_loca->LEN = %ld, meta_loca->SIZE = %ld, NEXT = %p, PREV = %p }\n",node->meta.len, node->meta.max, (void*)node->next, (void*)node->prev);
    }

/****************************************************************************
****                CREATE DESTROY GETTERS RESIZE                        ****
****************************************************************************/

loca_meta *loca_create(byte ** arr2, arr_size size){
    if(!arr2) return 0;
    size = (size > 0) ? size : STD_SIZE;
    loca_meta *meta = tmalloc(_size_loca_meta + roundup32(size));          
    if(!meta) return NULL;                                            
    meta->len = 0; meta->max = size;
    *arr2 = jump_meta(meta); 
    return meta;
  }

void loca_destroy(byte *arr){
    if (!arr) return; 
    loca_meta *meta = cast_meta(arr);
    tfree(meta);
}

loca_meta *llist_create(byte ** arr2, arr_size size){
    if(!arr2) return 0;
    size = (size > 0) ? size : STD_SIZE;
    llnode *node = tmalloc(_size_llnode + roundup32(size));          
    if(!node) return NULL;                                            
    node->next = NULL; node->prev = NULL; node->meta.len = 0; node->meta.max = size;
    *arr2 = jump_llist(node); 
    return cast_meta(*arr2);
  }

void llist_destroy(byte *arr){
    if (!arr) return; 
    llnode *meta = cast_llist(arr);
    tfree(meta);
}

arr_size loca_length(byte *arr){
    if(arr) return cast_meta(arr)->len;
    return 0;
}

arr_size loca_size(byte *arr){
    if(arr) return cast_meta(arr)->max;
    return 0;
}

arr_size loca_clear(byte *arr){
    if(arr){
        loca_meta* meta = cast_meta(arr);
        meta->len=0;
        return meta->len;
    }
    return 1;
}

static byte* resize_circ(byte** arr2, arr_size extra_len){
    if(arr2 || (*arr2)) return NULL;
    circheader* header = cast_circ((*arr2));
    header->virtual_end = header->meta.len;
    header->meta.len=0;
    return (*arr2);
}

static byte* resize_llist(byte** arr2, arr_size extra_len){
    if(!arr2 || !(*arr2)) return 0;
    //falta checar size_t overflow
    //fazer roundup64 unsigned
    llnode* node = cast_llist((*arr2));
    loca_meta* meta = &node->meta;
    arr_size new_size = (meta->max > extra_len) ? meta->max : roundup32(extra_len);
    byte * new_node_ptr = NULL;
    loca_meta* new_meta = llist_create(&new_node_ptr, new_size);
    llnode* new_node =cast_llist(new_node_ptr);
    node->next = new_node;
    new_node->prev = node;
    return new_node_ptr;   
}

static byte* resize_vec(byte** arr2, arr_size extra_len){
    if(!arr2 || !(*arr2)) return 0;
    //falta checar size_t overflow
    //fazer roundup64 unsigned
    loca_meta* meta = cast_meta((*arr2));
    arr_size new_size = meta->max + extra_len;
    roundup32(new_size);
    meta->max = new_size;
    meta = (loca_meta*)realloc(meta, meta->max);
    if(!meta) return NULL;
    return jump_meta(meta);
}

static arr_size push_orNot(byte *arr, byte *src, arr_size size){
    if(!arr) return 0;
    loca_meta* meta = cast_meta(arr);
    assert(meta->max >= meta->len);
    if((meta->max - size) < meta->len) return 0;
    memmove(&arr[meta->len],src, size);
    meta->len += size;
    return size;    
}

static arr_size push_some(byte *arr, byte *src, arr_size size){

    if(!arr) return 0;
    loca_meta* meta = cast_meta(arr);
    assert(meta->max >= meta->len);
    arr_size available = ((meta->max - size) > meta->len) ? size : (meta->max - meta->len);
    if(!available) return 0;
    memmove(&arr[meta->len],src, available);
    meta->len += available;
    return available;    
}


typedef byte* loca_resize(byte** , arr_size ); 

static arr_size push_grow(byte **arr2, byte *src, arr_size size, loca_resize* rsz, bool is_string){
        if(!arr2 || !(*arr2)) return 0;
        arr_size res = 0;

        if(is_string){ res = push_orNot((*arr2),src,size);}
        else{ res = push_some((*arr2),src,size);}
        if( res == size) return res;
        *arr2 = rsz(arr2,(size-res));
        return res + push_orNot((*arr2),src,(size-res));
    }

/****************************************************************************
****                              PUSH ONE                               ****
****************************************************************************/

arr_size loca_push_one(byte **arr2, byte src){
    if(!arr2 || !(*arr2)) return 0;
    return push_orNot((*arr2),&src,1);
}

arr_size loca_push_one_vec(byte **arr2, byte src){
    return push_grow(arr2, &src, 1, resize_vec,1);
}

arr_size loca_push_one_llist(byte **arr2, byte src){
    return push_grow(arr2, &src, 1, resize_llist,1);
}

arr_size loca_push_one_circ(byte **arr2, byte src){
    return push_grow(arr2, &src, 1, resize_circ,1);    
}

/****************************************************************************
****                            PUSH STR                                 ****
****************************************************************************/

arr_size loca_push_str(byte **arr2, byte *src, arr_size size, bool zero_terminated){
    if(!arr2 || !(*arr2)) return 0;
    //falta add o zero terminated
    return push_orNot((*arr2),src,size);
}

arr_size loca_push_str_vec(byte **arr2, byte *src, arr_size size, bool zero_terminated){
    //falta add o zero terminated
    return push_grow(arr2,src, size, resize_vec,1);
}

arr_size loca_push_str_llist(byte **arr2, byte *src, arr_size size, bool zero_terminated){
    //falta add o zero terminated
    return push_grow(arr2,src, size, resize_llist,1);
}

arr_size loca_push_str_circ(byte **arr2, byte *src, arr_size size, bool zero_terminated){
    //falta add o zero terminated
    return push_grow(arr2,src, size, resize_circ,1);    
}

/****************************************************************************
****                            PUSH MANY                                ****
****************************************************************************/

arr_size loca_push_many(byte **arr2, byte *src, arr_size size){
    if(!arr2 || !(*arr2)) return 0;
    return push_some((*arr2),src,size);
}

arr_size loca_push_many_vec(byte **arr2, byte *src, arr_size size){
    return push_grow(arr2,src, size, resize_vec,1); //push_some makes no sense to vectors
}

arr_size loca_push_many_llist(byte **arr2, byte *src, arr_size size){
    return push_grow(arr2,src, size, resize_llist,0);
}

arr_size loca_push_many_circ(byte **arr2, byte *src, arr_size size){
    return push_grow(arr2,src, size, resize_circ,0);    
}


/****************************************************************************
****                                 AT                                  ****
****************************************************************************/

//falta implementar indice negativo
//e o que fazer se index > len
byte *loca_at(byte *arr, arr_size index){
    arr_size temp = cast_meta(arr)->len;
    return ((index < temp) ? &arr[index] : NULL);
}

/****************************************************************************
****                            UTILS                                    ****
****************************************************************************/

byte *loca_copy(byte *arr){
    if(!arr) return NULL;
    loca_meta* meta = cast_meta(arr);
    byte* copy = NULL;
    loca_meta* copy_meta = loca_create(&copy,meta->max);
    push_orNot(copy,arr,meta->len);
    return copy;
}

byte *loca_llist_copy(byte *arr){
    return NULL;
}

/****************************************************************************
****                          ITERATOR                                   ****
****************************************************************************/

byte* it_begin(byte* arr){if(!arr) return NULL; return &arr[0];}\
byte* it_end(byte* arr){if(!arr) return NULL; return &arr[cast_meta(arr)->len];}
byte* it_next(byte** arr){if(!arr || !(*arr)) return NULL; return ++(*arr);}
byte* it_prev(byte** arr){if(!arr || !(*arr)) return NULL;return --(*arr);}

byte* it_llist_begin(byte* arr){

    if(arr==NULL) return NULL;
    llnode* first = cast_llist(arr);
    while(first->prev !=NULL){
        first = first->prev;}

    return jump_llist(first);
}

byte* it_llist_end(byte* arr){

    if(arr==NULL) return NULL;
    llnode* last = cast_llist(arr);
    while(last->next !=NULL) last = last->next;

    return jump_llist(last);
}

byte* it_llist_next(byte** arr){
    if(!arr && !(*arr)) return NULL;
    llnode* node = cast_llist(*arr);
    if(!node->next) return NULL;
    node = node->next;
    return jump_llist(node);

    }

byte* it_llist_prev(byte** arr){
    if(!arr && !(*arr)) return NULL;
        llnode* node = cast_llist(*arr);
        if(!node->next) return NULL;
        node = node->prev;
    return jump_llist(node);
}

byte* zip_begin(byte* arr1, byte* arr2){
    if(!arr1 || !arr2) return NULL; 

    byte** res = tmalloc(2*sizeof(void*));
    res[0]=&arr1[0];
    res[1]=&arr2[0];

    return *res;
}
byte* zip_end(byte* arr1, byte* arr2){
    if(!arr1 || !arr2) return NULL; 

    byte** res = tmalloc(2*sizeof(void*));
    res[0]=&arr1[cast_meta(arr1)->len];
    res[1]=&arr2[cast_meta(arr2)->len];

    return *res;
}
byte* zip_prev(byte* arr1, byte* arr2){
    return NULL;
}
byte* zip_next(byte* arr1, byte* arr2){
    return NULL;
}

byte* zip_destroy(byte* arr1, byte* arr2){
    return NULL;
}

/****************************************************************************
****                          FOR EACH                                   ****
****************************************************************************/

static byte* for_each_base(byte* arr, map_func* map, byte* mapped, arr_size init_size){

    byte* temp = NULL;
    vector.create(&mapped, init_size);
    vector.create(&temp, UINT8_MAX);

    arr_size it_counter=0;
    for (byte* it_b = array.it_begin(arr); it_b != array.it_end(arr); array.it_next(&it_b)){
        map(&arr[it_counter],temp);
        assert(temp != NULL);
        array.push_one(&arr,*temp);
        it_counter++;
        vector.clear(temp);
    }
    vector.destroy(temp);
    return mapped;
}

byte* for_each_map(byte* arr, map_func* map){

    byte* mapped = NULL;
    return for_each_base(arr,map,mapped,loca_length(arr));
}

byte* for_each_filter(byte* arr, filter_func* filter){
    byte* mapped = NULL;
    return for_each_base(arr,filter,mapped,loca_length(arr));

}

byte* for_each_llist_map(byte* arr, map_func* map){
    return NULL;
}

byte* for_each_llist_filter(byte* arr, filter_func* filter){
    return NULL;

}

/****************************************************************************
****                          VTABLES                                   ****
****************************************************************************/

const struct vTable_st array = {loca_create, 
                            loca_destroy,
                            loca_length,
                            loca_size,
                            loca_clear,
                            loca_push_one,
                            loca_push_many,
                            loca_push_str,
                            loca_at,
                            it_begin,
                            it_end,
                            it_next,
                            it_prev,
                            for_each_map,
                            for_each_filter
                            };

const struct vTable_st vector = {loca_create, 
                            loca_destroy,
                            loca_length,
                            loca_size,
                            loca_clear,
                            loca_push_one_vec,
                            loca_push_many_vec,
                            loca_push_str_vec,
                            loca_at,
                            it_begin,
                            it_end,
                            it_next,
                            it_prev,
                            for_each_map,
                            for_each_filter
                            };

const struct vTable_st llist = {llist_create, 
                            loca_destroy,
                            loca_length,
                            loca_size,
                            loca_clear,
                            loca_push_one_llist,
                            loca_push_many_llist,
                            loca_push_str_vec,
                            loca_at,
                            it_llist_begin,
                            it_llist_end,
                            it_llist_next,
                            it_llist_prev,
                            for_each_llist_map,
                            for_each_llist_filter
                            };