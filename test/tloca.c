#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "../loca.h"
#include "../lib_maps.h"

#define assert_profile_array(arr_name_, len_, max_) \
    {                                               \
        assert(array.length(arr_name_) == len_);    \
        assert_counter++;                           \
        assert(array.size(arr_name_) == max_);      \
        assert_counter++;                           \
    }

#define assert_profile_vector(arr_name_, len_, max_) \
    {                                                \
        assert(vector.length(arr_name_) == len_);    \
        assert_counter++;                            \
        assert(vector.size(arr_name_) == max_);      \
        assert_counter++;                            \
    }

typedef uint8_t uchar;

int main()
{

    arr_size std_size = 13;
    arr_size rounded_size = roundup32(std_size);
    arr_size assert_counter = 0;
    uchar *tarr = NULL;
    uchar *tvec = NULL;

    printf("\n[\t TESTING CREATE - ARRAY, VECTOR AND LLIST \t]\n");

    //Check Calling functions with NULL
    assert(array.create(NULL, 0) == NULL && tarr == NULL);
    assert_counter++;
    assert(vector.create(NULL, 0) == NULL && tvec == NULL);
    assert_counter++;

    array.create(&tarr, std_size);
    assert(tarr != NULL);
    assert_counter++;

    vector.create(&tvec, std_size);
    assert(tvec != NULL);
    assert_counter++;

    printf("[\t TESTING CREATE - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    const uchar seed = 178;
    uchar values[rounded_size];
    values[0] = seed;
    for (size_t i = 1; i < rounded_size; i++)
    {
        values[i] = ++values[i - 1];
    }

    printf("[\t TESTING PUSH ONE NO RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

    assert(array.push_one(NULL, 30) == 0);
    assert_counter++;
    assert(vector.push_one(NULL, 30) == 0);
    assert_counter++;

    for (arr_size i = 0; i < rounded_size; i++)
    {
        assert(array.push_one(&tarr, values[i]) == 1);
        assert_counter++;
        assert_profile_array(tarr, (i + 1), rounded_size);
        assert(array.at(tarr, i) != NULL);
        assert_counter++;
        assert(*array.at(tarr, i) == values[i]);
        assert_counter++;

        assert(vector.push_one(&tvec, values[i]) == 1);
        assert_counter++;
        assert_profile_vector(tvec, (i + 1), rounded_size);

    }

    printf("[\t TESTING PUSH ONE NO RESIZE - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    assert_profile_array(tarr, rounded_size, rounded_size);
    assert_profile_array(tvec, rounded_size, rounded_size);

    printf("[\t TESTING PUSH ONE RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

    assert(array.push_one(&tarr, 5) == 0);
    assert_counter++;
    assert_profile_array(tarr, rounded_size, rounded_size);

    assert(vector.push_one(&tvec, 5) == 1);
    assert_counter++;
    assert_profile_array(tvec, (rounded_size + 1), rounded_size * 2);

    //just for clearing the full array 'tarr'
    array.clear(tarr);
    assert_profile_array(tarr, 0, rounded_size);
    assert_counter++;

    printf("[\t TESTING PUSH ONE RESIZE - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    printf("[\t TESTING PUSH MANY - ARRAY, VECTOR AND LLIST\t]\n");

    //should push 'rounded_size' items (16)
    assert(array.push_many(&tarr, values, rounded_size) == rounded_size);
    assert_counter++;
    assert_profile_array(tarr, rounded_size, rounded_size);
    assert_counter++;

    assert(vector.push_many(&tvec, values, rounded_size) == rounded_size);
    assert_counter++;
    assert_profile_vector(tvec, rounded_size * 2 + 1, rounded_size * 4);
    assert_counter++;

    //should push zero, and not change len or max
    assert(array.push_many(&tarr, values, rounded_size) == 0);
    assert_counter++;
    assert_profile_array(tarr, rounded_size, rounded_size);
    assert_counter++;

    printf("[\t TESTING PUSH MANY - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    printf("[\t TESTING ITERATORS - ARRAY, VECTOR AND LLIST\t]\n");

    uchar *it_b_teste = it_begin(tarr);
    uchar *it_e_teste = it_end(tarr);
    assert(it_b_teste != NULL);
    assert_counter++;
    assert(it_e_teste != NULL);
    assert_counter++;

    int it_counter = 0;
    for (uchar *it_b = array.it_begin(tarr); it_b != array.it_end(tarr); array.it_next(&it_b))
    {
        // printf("from iterator: value[%d] = %d\n",it_counter,*it_b);
        assert((*it_b) == values[it_counter++]);
        assert_counter++;
    }

    printf("[\t TESTING ITERATORS - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    printf("[\t TESTING ZIP ITERATORS - ARRAY, VECTOR AND LLIST\t]\n");

    zip_ptr2 z_it = zip_begin(tarr, tvec);
    const zip_ptr2 z_end = zip_end(tarr, tvec);

    assert(z_it.arr1 != NULL);
    assert(z_it.arr2 != NULL);

    for (size_t i = 0; i < loca_size(tarr); i++)
    {
    assert((*(z_it.arr1) == *loca_at(tarr,i))); assert_counter++;
    assert((*(z_it.arr2) == *loca_at(tvec,i))); assert_counter++;
    zip_next(&z_it); assert_counter++;
    }

    printf("[\t TESTING ZIP ITERATORS - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    printf("[\t TESTING FOR EACH - ARRAY, VECTOR AND LLIST\t]\n");

    byte *temp = array.for_each_map(tarr, minus_two_func);
    vector.destroy(temp);

    printf("[\t TESTING FOR EACH - PASSED - %ld OK \t]\n\n", assert_counter);
    assert_counter = 0;

    array.destroy(tarr);
    vector.destroy(tvec);

    return EXIT_SUCCESS;
}