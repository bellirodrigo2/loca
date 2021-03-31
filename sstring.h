#ifndef SSTRING_H
#define SSTRING_H

//sstri max len = 256
//sstre max #of sstri = 256
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef uint8_t* sstri;

#define max_sstri_len (UINT8_MAX-1)

#define tmalloc malloc
#define tcalloc calloc
#define trealloc realloc
#define tfree free

//sstri[0] = len;
//sstre[0] = max_len;
//sstre[1] = len;

#define sstr_len_ptr(SSTR_) (SSTR_ - 1)
#define sstr_len(SSTR_) (*sstr_len_ptr(SSTR_))

#define sstre_len_incr(SSTR_, size_) (sstr_len(SSTR_) += size_)
#define sstre_len_decr(SSTR_, size_) (sstr_len(SSTR_) -= size_)

/****************************************************************************
****                               SSTRING                               ****
****************************************************************************/

size_t cstrlen(const char *str);
uint8_t sstrlen(const sstri str);
const char* sstri_to_cstr(const sstri sstr);
sstri sstri_from_cstr(const char* cstr);
void sstri_destroy(const sstri cstr);
int sstri_compare_cstr(const sstri str, const char* cstr);
int sstri_compare_sstr(const sstri str1, const sstri str2);
sstri sstri_copy(const sstri cstr);
sstri sstri_cat(const sstri str1, const sstri str2);

#define sstri_sprintf(mt_sstri, ...)\
  {mt_sstri = tmalloc(max_sstri_len+1);\
  ubyte len = sprintf((mt_sstri+1),__VA_ARGS__);\
  mt_sstri[0]=len;\
  mt_sstri = realloc(mt_sstri,(len+1));\
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif