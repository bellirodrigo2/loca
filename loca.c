#include <string.h>
#include <stdio.h> //for testing only
#include <assert.h> //for testing only

#include "loca.h"

#define STD_SIZE 64

void meta_print(byte* arr){
    uint64_t* len = cast_meta_len(arr);
    uint64_t* max = cast_meta_max(arr);
    printf("{ meta_loca->LEN = %ld, meta_loca->SIZE = %ld }\n",*len, *max);
    }


/****************************************************************************
****                CREATE DESTROY GETTERS RESIZE                        ****
****************************************************************************/
static arr_size* loca_new(byte ** arr2, arr_size size, bool is_alloc){
    if(!arr2) return 0;\
    size = (size > 0) ? size : STD_SIZE;
    arr_size *meta=NULL;
    if(!is_alloc) {
        meta = tmalloc(2*sizeof(arr_size) + roundup32(size));
        if(!meta) return NULL;
    } else meta = cast_meta_len(*arr2);
    meta[0] = 0; meta[1] = size;
    *arr2 = jump_meta_len(meta); 
    return meta;

}

arr_size *loca_create(byte ** arr2, arr_size size){
    return loca_new(arr2, size,0);    
  }

arr_size *loca_init(byte ** arr2, arr_size size){
    return loca_new(arr2, size,1);    
  }

void loca_destroy(byte *arr){
    if (!arr) return; 
    uint64_t* len = cast_meta_len(arr);
    tfree(len);
}

arr_size loca_length(byte *arr){
    if(arr) return *cast_meta_len(arr);
    return 0;
}

arr_size loca_size(byte *arr){
    if(arr) return *cast_meta_max(arr);
    return 0;
}

arr_size loca_clear(byte *arr){
    if(arr){
        arr_size* meta = cast_meta_len(arr);
        *meta=0;
        return *meta;
    }
    return 1;
}

static byte* resize_vec(byte** arr2, arr_size extra_len){
    if(!arr2 || !(*arr2)) return 0;
    //falta checar size_t overflow
    //fazer roundup64 unsigned
    arr_size* meta_max = cast_meta_max((*arr2));
    arr_size* meta_len = cast_meta_len((*arr2));
    arr_size new_size = *meta_max + extra_len;
    roundup32(new_size);
    *meta_max = new_size;
    meta_len = (arr_size*)realloc(meta_len, *meta_max);
    if(!meta_len) return NULL;
    return jump_meta_len(meta_len);
}

static arr_size push_orNot(byte *arr, byte *src, arr_size size){
    if(!arr) return 0;
    arr_size* meta_max = cast_meta_max(arr);
    arr_size* meta_len = cast_meta_len(arr);
    assert(*meta_max >= *meta_len);
    if((*meta_max - size) < *meta_len) return 0;
    memmove(&arr[*meta_len],src, size);
    *meta_len += size;
    return size;    
}

static arr_size push_some(byte *arr, byte *src, arr_size size){

    if(!arr) return 0;
    arr_size* meta_max = cast_meta_max(arr);
    arr_size* meta_len = cast_meta_len(arr);
    assert(*meta_max >= *meta_len);
    arr_size available = ((* meta_max - size) > *meta_len) ? size : (*meta_max - *meta_len);
    if(!available) return 0;
    memmove(&arr[*meta_len],src, available);
    *meta_len += available;
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
    return ((index < *cast_meta_len(arr)) ? &arr[index] : NULL);
}

/****************************************************************************
****                            UTILS                                    ****
****************************************************************************/

byte *loca_copy(byte *arr){
    if(!arr) return NULL;
    arr_size* meta_len = cast_meta_len(arr);
    arr_size* meta_max = cast_meta_max(arr);
    byte* copy = tmalloc(2*sizeof(arr_size) + *meta_max);
    memmove(copy,meta_len,*meta_len);
    return copy;
}

byte *loca_trim(byte *arr){
    
    uint64_t* len = cast_meta_len(arr);
    uint64_t* max = cast_meta_max(arr);
    if(len>=max) return arr;
    *max = *len;
    len = (arr_size*)realloc(len, *len);
    if(!len) return NULL;
    return jump_meta_len(len);
}

byte *loca_find_str(byte *arr, const char *arr_tgt){
    for (byte* it_b = array.it_begin(arr); it_b != array.it_end(arr); array.it_next(&it_b)){
        const char* cstr = (char*)it_b+1;
        if(strcmp(cstr,arr_tgt)==0) return it_b;
        //mudar para
        //sstri sstri_compare_sstr(const sstri *str, const sstri* cstr);
    }
    return NULL;
}

byte *loca_replace_str(byte *arr, arr_size index, byte* value, arr_size size){

    byte* it_b = it_begin(arr);

    for (size_t i = 0; i < index; i++){if((it_b = it_next_str(&it_b))==NULL) return NULL;}
    // byte str_size = *(it_b);
    arr_size* len = cast_meta_len(it_b);
    arr_size* max = cast_meta_max(it_b);
    if(max-size < len) return NULL;
    memmove((it_b+size),it_b,size);
    memmove(it_b,value,size);

    return it_b;
}


/****************************************************************************
****                          ITERATOR                                   ****
****************************************************************************/

byte* it_begin(byte* arr){if(!arr) return NULL; return &arr[0];}\
byte* it_end(byte* arr){if(!arr) return NULL; return &arr[*cast_meta_len(arr)];}
byte* it_next(byte** arr){if(!arr || !(*arr)) return NULL; return ++(*arr);}
byte* it_prev(byte** arr){if(!arr || !(*arr)) return NULL;return --(*arr);}

byte* it_next_str(byte** arr){
    if(!arr || !(*arr)) return NULL; 
    byte str_size = *(*arr);
    *arr += str_size+1;
    return (*arr);
    }

byte* it_prev_str(byte** arr){
    if(!arr || !(*arr)) return NULL; 
    byte str_size = *(arr[0]);
    *arr += str_size+1;
    return (*arr);
    }

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

    zip_ptr2 res = {&arr1[*cast_meta_len(arr1)],&arr1[*cast_meta_len(arr2)]};
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
