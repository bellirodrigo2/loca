#include <string.h>
#include <stdio.h> //for testing only
#include <assert.h> //for testing only

#include "loca.h"

#define STD_SIZE 64

struct loca_meta_st{
    arr_size len;
    arr_size max;
};

static const arr_size _size_loca_meta = sizeof(loca_meta);

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


/****************************************************************************
****                          ITERATOR                                   ****
****************************************************************************/

byte* it_begin(byte* arr){if(!arr) return NULL; return &arr[0];}\
byte* it_end(byte* arr){if(!arr) return NULL; return &arr[cast_meta(arr)->len];}
byte* it_next(byte** arr){if(!arr || !(*arr)) return NULL; return ++(*arr);}
byte* it_prev(byte** arr){if(!arr || !(*arr)) return NULL;return --(*arr);}

zip_ptr2 zip_begin(byte* arr1, byte* arr2){
    if(!arr1 || !arr2){
        zip_ptr2 res = {NULL,NULL};
        return res;
    };

    zip_ptr2 res = {&arr1[0],&arr2[0]};
    return res;
}
zip_ptr2 zip_end(byte* arr1, byte* arr2){
    if(!arr1 || !arr2){
        zip_ptr2 res = {NULL,NULL};
        return res;
    };

    zip_ptr2 res = {&arr1[cast_meta(arr1)->len],&arr1[cast_meta(arr2)->len]};
    return res;
}

void zip_next(zip_ptr2* zipit){
    (*zipit).arr1 = it_next(&(*zipit).arr1);
    (*zipit).arr2 = it_next(&(*zipit).arr2);
}

void zip_prev(zip_ptr2* zipit){
    (*zipit).arr1 = it_prev(&(*zipit).arr1);
    (*zipit).arr2 = it_prev(&(*zipit).arr2);
}

void zip_destroy(byte* zip1, byte* zip2){
    if(zip1) tfree(zip1); 
    if(zip1) tfree(zip1); 
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
