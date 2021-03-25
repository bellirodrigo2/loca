#ifndef LOCA_H
#define LOCA_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

typedef size_t arr_size;
typedef uint8_t byte;
typedef struct loca_meta_st loca_meta;

#define tmalloc malloc
#define tcalloc calloc
#define trealloc realloc
#define tfree free

#define cast_meta(arr_) ((loca_meta *)((char *)arr_ - _size_loca_meta))
#define jump_meta(arr_) ((byte *)((char *)arr_ + _size_loca_meta))

void meta_print(byte* lm);

/****************************************************************************
****                        FUNCTION PROTOTYPES                          ****
****************************************************************************/

//create an array and return a pointer to this array
loca_meta *loca_create(byte **arr2, arr_size size);

//just free the allocated object
void loca_destroy(byte *arr);

//return lenth
arr_size loca_length(byte *arr);

//return maximum size
arr_size loca_size(byte *arr);

//clear the array
arr_size loca_clear(byte *arr);

//push one item to the array
arr_size loca_push_one(byte **arr2, byte src);
arr_size loca_push_one_vec(byte **arr2, byte src);

//push multiple items to the array
arr_size loca_push_many(byte **arr2, byte *src, arr_size size);
arr_size loca_push_many_vec(byte **arr2, byte *src, arr_size size);

//push multiple items to the array as a string. Add all or add no
arr_size loca_push_str(byte **arr2, byte *src, arr_size size, bool zero_terminated);
arr_size loca_push_str_vec(byte **arr2, byte *src, arr_size size,bool zero_terminated);

//push ordered
typedef int cmp(byte *, byte *);
arr_size loca_push_sorted(byte *arr, byte* src, arr_size size,cmp* compare);

//return a pointer to arr[index]
//if the index is invalid, NULL is returned instead
byte *loca_at(byte *arr, arr_size index);

//utils
byte *loca_copy(byte *arr);
byte *loca_trim(byte *arr);
byte *loca_replace(byte *arr, arr_size index, byte* value, arr_size value_size);

//iterator
byte* it_begin(byte* arr);
byte* it_end(byte* arr);
byte* it_next(byte** arr);
byte* it_prev(byte** arr);

typedef struct zip_pointer_st2 zip_ptr2;

byte* zip_begin(byte* arr1, byte* arr2);
byte* zip_end(byte* arr1, byte* arr2);
byte* zip_prev(byte* arr1, byte* arr2);
byte* zip_next(byte* arr1, byte* arr2);
byte* zip_destroy(byte* arr1, byte* arr2);

//for each
typedef byte* map_func(byte*,byte*);
typedef byte* reduce_func(byte*,byte*,byte*);
typedef map_func filter_func;
byte* for_each_map(byte* arr, map_func* map_func);
byte* for_each_filter(byte* arr, filter_func* filter_func);

/****************************************************************************
****                               VTABLE                                ****
****************************************************************************/
typedef loca_meta* create_(byte **, arr_size);
typedef void destroy_(byte *);
typedef arr_size size_(byte *);
typedef arr_size push_one_(byte **, byte);
typedef arr_size push_many_(byte **, byte *, arr_size );
typedef arr_size push_str_(byte **, byte *, arr_size,bool );
typedef byte *at_(byte *, arr_size );
typedef byte* it_begin_(byte* );
typedef byte* it_end_(byte* );
typedef byte* it_next_(byte** );
typedef byte* it_prev_(byte** );
typedef byte* it_for_each_map_(byte*, map_func*);

typedef struct vTable_st vTable;
struct vTable_st{
    create_* create;
    destroy_* destroy;
    size_* length;
    size_* size;
    size_* clear;
    push_one_* push_one;
    push_many_* push_many;
    push_str_* push_str;
    at_* at;
    it_begin_* it_begin;
    it_end_* it_end;
    it_next_* it_next;
    it_prev_* it_prev;
    it_for_each_map_* for_each_map;
    it_for_each_map_* for_each_filter;
};

extern const vTable array;
extern const vTable vector;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif