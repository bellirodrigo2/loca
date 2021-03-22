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
typedef struct llnode_st llnode;
typedef struct circheader_st circheader;

#define tmalloc malloc
#define tcalloc calloc
#define trealloc realloc
#define tfree free

#define cast_meta(arr_) ((loca_meta *)((char *)arr_ - _size_loca_meta))
#define cast_llist(arr_) ((llnode *)((char *)arr_ - _size_llnode))
#define cast_circ(arr_) ((circheader *)((char *)arr_ - _size_circheader))
#define jump_meta(arr_) ((byte *)((char *)arr_ + _size_loca_meta))
#define jump_llist(arr_) ((byte *)((char *)arr_ + _size_llnode))
#define jump_circ(arr_) ((byte *)((char *)arr_ + _size_circheader))

void meta_print(byte* lm);
void meta_print_llist(byte* lm);

/****************************************************************************
****                        FUNCTION PROTOTYPES                          ****
****************************************************************************/

//create an array and return a pointer to this array
loca_meta *loca_create(byte **arr2, arr_size size);
loca_meta *llist_create(byte **arr2, arr_size size);

//just free the allocated object
void loca_destroy(byte *arr);
void llist_destroy(byte *arr);

//return lenth
arr_size loca_length(byte *arr);

//return maximum size
arr_size loca_size(byte *arr);

//clear the array
arr_size loca_clear(byte *arr);


//push one item to the array
arr_size loca_push_one(byte **arr2, byte src);
arr_size loca_push_one_vec(byte **arr2, byte src);
arr_size loca_push_one_llist(byte **arr, byte src);
arr_size loca_push_one_circ(byte **arr, byte src);

//push multiple items to the array
arr_size loca_push_many(byte **arr2, byte *src, arr_size size);
arr_size loca_push_many_vec(byte **arr2, byte *src, arr_size size);
arr_size loca_push_many_llist(byte **arr2, byte *src, arr_size size);
arr_size loca_push_many_circ(byte **arr, byte *src, arr_size size);

//push multiple items to the array as a string. Add all or add no
arr_size loca_push_str(byte **arr2, byte *src, arr_size size, bool zero_terminated);
arr_size loca_push_str_vec(byte **arr2, byte *src, arr_size size,bool zero_terminated);
arr_size loca_push_str_llist(byte **arr2, byte *src, arr_size size,bool zero_terminated);
arr_size loca_push_str_circ(byte **arr, byte *src, arr_size size,bool zero_terminated);

//return a pointer to arr[index]
//if the index is invalid, NULL is returned instead
byte *loca_at(byte *arr, arr_size index);

//utils
byte *loca_copy(byte *arr);
byte *loca_llist_copy(byte *arr);


//iterator
byte* iterator_begin(byte* arr);
byte* iterator_end(byte* arr);
byte* iterator_next(byte** arr);
byte* iterator_prev(byte** arr);

byte* iterator_llist_begin(byte* arr);
byte* iterator_llist_end(byte* arr);
byte* iterator_llist_next(byte** arr);
byte* iterator_llist_prev(byte** arr);

byte* iterator_circ_end(byte* arr);

//for each
typedef void map(byte*,byte*);
typedef void reduce(byte*,byte*,byte*);
typedef map filter;
byte* for_each_map(byte* arr, map* map_func);
byte* for_each_filter(byte* arr, filter* filter_func);

byte* for_each_llist_map(byte* arr, map* map_func);
byte* for_each_llist_filter(byte* arr, filter* filter_func);

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
typedef byte* iterator_begin_(byte* );
typedef byte* iterator_end_(byte* );
typedef byte* iterator_next_(byte** );
typedef byte* iterator_prev_(byte** );
typedef byte* iterator_for_each_map_(byte*, map*);

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
    iterator_begin_* it_begin;
    iterator_end_* it_end;
    iterator_next_* it_next;
    iterator_prev_* it_prev;
    iterator_for_each_map_* for_each_map;
    iterator_for_each_map_* for_each_filter;
};

extern const vTable array;
extern const vTable vector;
extern const vTable llist;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif