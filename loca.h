#ifndef LOCA_H
#define LOCA_H

#include <stdlib.h>
#include <stdint.h>

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

/****************************************************************************
****                        FUNCTION PROTOTYPES                          ****
****************************************************************************/

//create an array and return a pointer to this array
loca_meta *loca_create(byte **arr2, arr_size size);
llnode *llist_create(byte **arr2, arr_size size);

//just free the allocated object
void loca_destroy(byte *arr);
void llist_destroy(byte *arr);

//return lenth
arr_size loca_length(byte *arr);

//return maximum size
arr_size loca_size(byte *arr);

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

//return a pointer to arr[index]
//if the index is invalid, NULL is returned instead
byte *loca_at(byte *arr, arr_size index);

/****************************************************************************
****                             ITERATOR                                ****
****************************************************************************/
byte* iterator_begin(byte* arr);
byte* iterator_end(byte* arr);
byte* iterator_llist_begin(byte* arr);
byte* iterator_llist_end(byte* arr);
byte* iterator_circ_begin(byte* arr);
byte* iterator_circ_end(byte* arr);

/****************************************************************************
****                               VTABLE                                ****
****************************************************************************/
typedef loca_meta* create_(byte **, arr_size);
typedef void destroy_(byte *);
typedef arr_size length_(byte *);
typedef arr_size size_(byte *);
typedef arr_size push_one_(byte **, byte);
typedef arr_size push_many_(byte **, byte *, arr_size );
typedef byte *at_(byte *, arr_size );
typedef byte* iterator_begin_(byte* );
typedef byte* iterator_end_(byte* );

typedef struct vTable_st vTable;
struct vTable_st{
    create_* create;
    destroy_* destroy;
    length_* length;
    size_* size;
    push_one_* push_one;
    push_many_* push_many;
    at_* at;
    iterator_begin_* it_begin;
    iterator_end_* it_end;
};

extern const vTable array;
extern const vTable vector;
extern const vTable llist;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif