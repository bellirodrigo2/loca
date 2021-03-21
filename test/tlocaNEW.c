#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "../loca.h"

#define assert_profile_array(arr_name_, len_, max_)\
    {assert(array.length( arr_name_ ) == len_ && array.size( arr_name_ ) == max_ );\
    assert_counter+=2;}

#define assert_profile_vector(arr_name_, len_, max_)\
    {assert(vector.length( arr_name_ ) == len_ && vector.size( arr_name_ ) == max_ );\
    assert_counter+=2;}

#define assert_profile_llist(arr_name_, len_, max_)\
    {assert(llist.length( arr_name_ ) == len_ && llist.size( arr_name_ ) == max_ );\
    assert_counter+=2;}

typedef uint8_t uchar;

int main()
{

    arr_size std_size = 13;
    arr_size rounded_size = roundup32(std_size);
    arr_size assert_counter = 0;
    uchar *tarr=NULL;
    uchar *tvec=NULL;
    uchar *tllist=NULL;

    printf("\n[\t TESTING CREATE - ARRAY, VECTOR AND LLIST \t]\n");

    //Check Calling functions with NULL
    assert(array.create(NULL,0)==NULL && tarr==NULL); assert_counter++;
    assert(vector.create(NULL,0)==NULL && tvec==NULL); assert_counter++;
    assert(llist.create(NULL,0)==NULL && tllist==NULL); assert_counter++;

    array.create(&tarr, std_size);
    assert(tarr!=NULL); assert_counter++;

    vector.create(&tvec,std_size);
    assert(tvec!=NULL); assert_counter++;
    
    llist.create(&tllist,std_size);
    assert(tllist!=NULL); assert_counter++;

    printf("[\t TESTING CREATE - PASSED - %ld OK \t]\n\n",assert_counter);

    const uchar seed = 178;
    uchar values[rounded_size];
    values[0]=seed;
    for (size_t i = 1; i < rounded_size; i++){
        values[i] = ++values[i-1];  
    }

        printf("[\t TESTING PUSH ONE NO RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

        assert(array.push_one(NULL, 30) == 0); assert_counter++;
        assert(vector.push_one(NULL, 30) == 0); assert_counter++;
        assert(llist.push_one(NULL, 30) == 0); assert_counter++;

    for (arr_size i = 0; i < rounded_size; i++)
    {
        assert(array.push_one(&tarr, values[i]) == 1); assert_counter++;
        assert_profile_array(tarr, (i+1), rounded_size);
        assert(array.at(tarr,i)!=NULL); assert_counter++;
        assert(*array.at(tarr,i)==values[i]);assert_counter++;

        assert(vector.push_one(&tvec, values[i]) == 1); assert_counter++;
        assert_profile_vector(tvec, (i+1), rounded_size);

        assert(llist.push_one(&tllist, values[i]) == 1); assert_counter++;
        assert_profile_llist(tarr, (i+1), rounded_size);
    }

    printf("[\t TESTING PUSH ONE NO RESIZE - PASSED - %ld OK \t]\n\n",assert_counter);

    assert_profile_array(tarr,rounded_size,rounded_size);
    assert_profile_array(tvec,rounded_size,rounded_size);
    assert_profile_array(tllist,rounded_size,rounded_size);

    printf("[\t TESTING PUSH ONE RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

    assert(array.push_one(&tarr,5)==0); assert_counter++;
    assert_profile_array(tarr,rounded_size,rounded_size);

    assert(vector.push_one(&tvec,5)==1); assert_counter++;
    assert_profile_array(tvec,(rounded_size+1),rounded_size*2);

    uchar* old = tllist;
    assert(old == tllist);assert_counter++;
    assert(llist.push_one(&tllist,5)==1); assert_counter++;
    assert_profile_array(tllist,1,rounded_size); assert_counter++;
    assert(old != tllist);assert_counter++;

    printf("[\t TESTING PUSH ONE RESIZE - PASSED - %ld OK \t]\n\n",assert_counter);

    array.destroy(tarr);
    vector.destroy(tvec);
    uchar* it_b_llist = iterator_llist_begin(tllist);
    assert(it_b_llist == old);assert_counter++;

    return EXIT_SUCCESS;
}