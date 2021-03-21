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
    arr_size rounded_sizeV = roundup32(std_size);
    arr_size rounded_sizeL = roundup32(std_size);
    arr_size assert_counter = 0;
    uchar *arr_uchar=NULL;
    uchar *vec_uchar=NULL;
    uchar *llist_uchar=NULL;

    printf("\n[\t TESTING CREATE - ARRAY, VECTOR AND LLIST \t]\n");

    array.create(&arr_uchar, std_size);
    vector.create(&vec_uchar, std_size);
    llist.create(&llist_uchar, std_size);

    assert(arr_uchar!=NULL && vec_uchar!=NULL && llist_uchar!=NULL);
    assert_counter+=3;

    printf("[\t TESTING CREATE - PASSED - %ld OK \t]\n\n",assert_counter);

    uchar uchar_po = 178;
    uchar uchar_poV = 178;
    uchar uchar_poL = 178;
    uchar replicaV = uchar_po;
    uchar treplicaV = replicaV;

    printf("[\t TESTING PUSH ONE NO RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

    for (arr_size i = 0; i < array.size(arr_uchar); i++)
    {
        assert(array.push_one(&arr_uchar, uchar_po++) == 1);
        assert_counter++;
        assert_profile_array(arr_uchar, (i + 1), rounded_size);

        assert(vector.push_one(&vec_uchar, uchar_poV++) == 1);
        assert_counter++;
        assert_profile_vector(vec_uchar, (i + 1), rounded_sizeV);

        assert(llist.push_one(&llist_uchar, uchar_poL++) == 1);
        assert_counter++;
        assert_profile_vector(llist_uchar, (i + 1), rounded_sizeL);
    }

    printf("[\t TESTING PUSH ONE NO RESIZE - PASSED - %ld OK \t]\n\n",assert_counter);

    printf("[\t TESTING PUSH ONE RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

    arr_size expanded_size = rounded_size * 2;

    //no grow. push_one return 0 and doesn't push
    assert(array.push_one(&arr_uchar, 120) == 0);
    assert(array.push_one(&arr_uchar, 120) == 0);
    assert_counter++;
    assert_profile_array(arr_uchar, array.size(arr_uchar), rounded_size);

    //should grow 2x. push_one return 2 in that case
    assert(vector.push_one(&vec_uchar, uchar_poV++) == 1);
    assert_counter++;
    assert_profile_vector(vec_uchar, (++rounded_sizeV), expanded_size);

    assert(vector.push_one(&vec_uchar, uchar_poV++) == 1);
    assert_counter++;
    assert_profile_array(vec_uchar, (++rounded_sizeV), expanded_size);

    //should add new node to linked list
    assert(llist.push_one(&llist_uchar, uchar_poL++) == 1);
    assert_counter++;
    meta_print(llist_uchar);

    assert_profile_llist(llist_uchar, 1, rounded_sizeL);
    assert(llist.push_one(&llist_uchar, uchar_poL++) == 1);
    assert_counter++;
    assert_profile_llist(llist_uchar, 2, rounded_sizeL);

    printf("[\t TESTING PUSH ONE RESIZE - PASSED - %ld OK \t]\n\n",assert_counter);

    printf("[\t TESTING PUSH MANY - ARRAY, VECTOR ANS LLIST \t]\n");

    uchar to_many[] = {0, 1, 2, 3, 4};

    assert(vector.push_many(&vec_uchar, to_many, sizeof(to_many)) == sizeof(to_many));
    assert_counter++;
    assert_profile_array(vec_uchar, (rounded_sizeV + sizeof(to_many)), expanded_size);

    for (size_t i = 0; i < rounded_sizeV; i++){
        assert(vector.at(vec_uchar, i) !=NULL);
        assert(*vector.at(vec_uchar, i) == replicaV++);
    }
    for(size_t i=0;i<sizeof(to_many);i++){
        assert(vector.at(vec_uchar, i+rounded_sizeV) !=NULL);
        assert(*(vector.at(vec_uchar, i+rounded_sizeV)) == to_many[i]);
    }

    printf("[\t TESTING PUSH MANY - PASSED - %ld OK \t]\n\n",assert_counter);

    printf("[\t TESTING ITERATOR - ARRAY, VECTOR ANS LLIST \t]\n");

    uchar* begin = iterator_begin(vec_uchar);
    uchar* end = iterator_end(vec_uchar);
    size_t counter=0;
    size_t counter2=0;

    while((begin != end)){
        if(begin){
            if(counter < 18 ) {assert((*begin) == treplicaV++);}
            else assert((*begin) == to_many[counter2++]);
        };
        begin++;
        counter++;
        assert_counter++;
    }
    printf("[\t TESTING ITERATOR - PASSED - %ld OK \t]\n\n",assert_counter);

    uchar for_push_many[] = {0,1,2,3,4,5,6,7,8,9,10};

    vector.push_many(&vec_uchar,for_push_many, sizeof(for_push_many));

    arr_size expanded_size2 = expanded_size * 2;
    assert(vector.size(vec_uchar)==expanded_size2);

    printf("[\t DESTROYING.. \t]\n\n");

    array.destroy(arr_uchar);
    vector.destroy(vec_uchar);
    llist.destroy(llist_uchar);

    return EXIT_SUCCESS;
}