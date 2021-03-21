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

llnode *llist_create(byte ** arr2, arr_size size){
    if(!arr2) return 0;
    size = (size > 0) ? size : STD_SIZE;
    llnode *node = tmalloc(_size_llnode + roundup32(size));          
    if(!node) return NULL;                                            
    node->next = NULL; node->prev = NULL; node->meta.len = 0; node->meta.max = size;
    *arr2 = jump_llist(node); 
    return node;
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

static byte* resize_circ(byte** arr2, arr_size extra_len){
    
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
    llnode* new_node = llist_create(&new_node_ptr, new_size);
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

static arr_size push(byte *arr, byte *src, arr_size size){
    if(!arr) return 0;
    loca_meta* meta = cast_meta(arr);
    assert(meta->max >= meta->len);
    if((meta->max - size) < meta->len) return 0;
    memmove(&arr[meta->len],src, size);
    meta->len += size;
    return size;    
}

typedef byte* loca_resize(byte** , arr_size ); 

static arr_size push_many_grow(byte **arr2, byte *src, arr_size size, loca_resize* rsz){
        if(!arr2 || !(*arr2)) return 0;
        arr_size res = push((*arr2),src,size);
        if( res != 0) return res;
        *arr2 = rsz(arr2,size);
        return push((*arr2),src,size);
    }

/****************************************************************************
****                              PUSH ONE                               ****
****************************************************************************/

arr_size loca_push_one(byte **arr2, byte src){
    if(!arr2 || !(*arr2)) return 0;
    return push((*arr2),&src,1);
}

arr_size loca_push_one_vec(byte **arr2, byte src){
    return push_many_grow(arr2, &src, 1, resize_vec);
}

arr_size loca_push_one_llist(byte **arr2, byte src){
    return push_many_grow(arr2, &src, 1, resize_llist);
}

arr_size loca_push_one_circ(byte **arr2, byte src){
    return push_many_grow(arr2, &src, 1, resize_circ);    
}

/****************************************************************************
****                            PUSH MANY                                ****
****************************************************************************/

arr_size loca_push_many(byte **arr2, byte *src, arr_size size){
    if(!arr2 || !(*arr2)) return 0;
    return push((*arr2),src,size);
}

arr_size loca_push_many_vec(byte **arr2, byte *src, arr_size size){
    return push_many_grow(arr2,src, size, resize_vec);
}

arr_size loca_push_many_llist(byte **arr2, byte *src, arr_size size){
    return push_many_grow(arr2,src, size, resize_llist);
}

arr_size loca_push_many_circ(byte **arr2, byte *src, arr_size size){
    return push_many_grow(arr2,src, size, resize_circ);    
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
****                          ITERATOR                                   ****
****************************************************************************/

byte* iterator_begin(byte* arr){return &arr[0];}\
byte* iterator_end(byte* arr){return &arr[cast_meta(arr)->len];}
byte* iterator_llist_begin(byte* arr){

    if(arr==NULL) return NULL;
    llnode* first = cast_llist(arr);
    
    while(first->prev !=NULL) first = first->prev;

    return jump_llist(first);
}

byte* iterator_llist_end(byte* arr){
        
    llnode* last = cast_llist(arr);
    while(last !=NULL) last = last->prev;

    return &jump_llist(last)[last->meta.len];
}

/****************************************************************************
****                          VTABLES                                   ****
****************************************************************************/

const struct vTable_st array = {loca_create, 
                            loca_destroy,
                            loca_length,
                            loca_size,
                            loca_push_one,
                            loca_push_many,
                            loca_at,
                            iterator_begin,
                            iterator_end
                            };

const struct vTable_st vector = {.create = &loca_create, 
                            .destroy = &loca_destroy,
                            .length = &loca_length,
                            .size = &loca_size,
                            .push_one = &loca_push_one_vec,
                            .push_many = &loca_push_many_vec,
                            .at = &loca_at,
                            .it_begin = &iterator_begin,
                            .it_end = &iterator_end
                            };

const struct vTable_st llist = {.create = &loca_create, 
                            .destroy = &loca_destroy,
                            .length = &loca_length,
                            .size = &loca_size,
                            .push_one = &loca_push_one_llist,
                            .push_many = &loca_push_many_llist,
                            .at = &loca_at,
                            .it_begin = &iterator_begin,
                            .it_end = &iterator_end
                            };