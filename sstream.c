
#include <stdint.h>
#include "loca.h"
#include "sstring.h"

uint64_t *sstream_create(uint8_t **arr2, uint64_t size){
    return loca_create(arr2, size);
}

void sstream_destroy(uint8_t *arr){
    loca_destroy(arr);
}

uint64_t sstream_push(uint8_t **arr2, const char* str){
    uint8_t len = cstrlen(str);
    loca_push_one_vec(arr2,len);
    arr_size copied = loca_push_str_vec(arr2, (byte*)str, len,0);
    return copied;
}

// uint64_t sstream_push_sorted(uint8_t **arr2, const char* str){
    // return loca_push_sorted_vec(arr2, (byte*)str, cstrlen(str),0);
// }

uint8_t *sstream_it_begin(uint8_t *arr){
    return it_begin(arr);
}

uint8_t *sstream_it_end(uint8_t *arr){
    return it_end(arr);
}

uint8_t *sstream_it_next(uint8_t **arr){
    return it_next_str(arr);
}

uint8_t *sstream_it_prev(uint8_t **arr){
    return it_prev_str(arr);
}

const char *sstream_it_cstr(uint8_t *arr){
    return sstri_to_cstr(&arr[1]);
}

uint64_t sstream_words(uint8_t *arr){
    uint8_t* it_b = sstream_it_begin(arr);
    uint8_t* it_e = sstream_it_end(arr);
    int it_counter = 0;
    for (uint8_t *it_b = sstream_it_begin(arr); it_b < sstream_it_end(arr); it_b = sstream_it_next(&it_b))
        ++it_counter;
    return it_counter;
}

uint64_t sstream_total_size(uint8_t *arr){
    return loca_length(arr);
}
