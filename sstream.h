#ifndef SSTREAM_H
#define SSTREAM_H

#include <stdint.h>

    uint64_t *sstream_create(uint8_t **arr2, uint64_t size);
    void sstream_destroy(uint8_t *arr);
    uint64_t sstream_words(uint8_t *arr);
    uint64_t sstream_total_size(uint8_t *arr);
    uint64_t sstream_push(uint8_t **arr2, const char* str);
    uint64_t sstream_push_sorted(uint8_t **arr2, const char* str);
    uint8_t *sstream_it_begin(uint8_t *arr);
    uint8_t *sstream_it_end(uint8_t *arr);
    uint8_t *sstream_it_next(uint8_t **arr);
    uint8_t *sstream_it_prev(uint8_t **arr);
    const char *sstream_it_cstr(uint8_t *arr);

#endif