#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "sstring.h"

#define tmalloc malloc
#define tcalloc calloc
#define trealloc realloc
#define tfree free

static size_t safe_strlen(const char *str, size_t max_len)
{
    const char * end = (const char *)memchr(str, '\0', max_len);
    if (end == NULL)
        return max_len;
    else
        return end - str;
}
size_t cstrlen(const char *str){
    return safe_strlen(str, max_sstri_len);
}
uint8_t sstrlen(const sstri str){
    return str[-1];
}

const char* sstri_to_cstr(const sstri sstr){
    uint8_t* len = sstr_len_ptr(sstr);
    assert(len !=NULL && (*len) <= max_sstri_len);
    char* temp = tmalloc((*len)+1);
    if(!temp) return NULL;
    memmove(temp, sstr, *len);
    temp[*len]='\0';
    return temp;
}

sstri sstri_from_cstr(const char* cstr)
{
    uint8_t len = cstrlen(cstr);
    sstri temp = tmalloc(len+1);
    if(!temp) return NULL;
    memmove(temp+1, cstr, len);
    temp[0]=len;
    return &temp[1];
}

void sstri_destroy(const sstri cstr){
    if(!cstr) return;
    tfree(&cstr[-1]);
}

int sstri_compare_cstr(const sstri str, const char* cstr){
    return strncmp((char*)str,cstr,str[-1]);
}

int sstri_compare_sstr(const sstri str1, const sstri str2){
    return strncmp((char*)str1,(char*)str2,str1[-1]);
}

sstri sstri_copy(const sstri sstr){
    int size = sstrlen(sstr);
    sstri temp = tmalloc(size+1);
    memcpy(temp,&sstr[-1],size+1);
    return &temp[1];
}

sstri sstri_cat(const sstri str1, const sstri str2){
    if(str1 ==NULL || str2 ==NULL ) return NULL;
    int size1 = str1[-1]; int size2 = str2[-1];
    int total_size = size1 + size2;
    total_size = (total_size <= max_sstri_len) ? total_size : max_sstri_len;
    sstri temp = tmalloc(total_size);
    temp[0] = total_size;
    memmove(&temp[1],str1,size1);
    memmove(&temp[size1+1],str2,size2);
    return &temp[1];
}
