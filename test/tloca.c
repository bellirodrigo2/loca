#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "../loca.h"

#define assert_profile_array(arr_name_, len_, max_)\
    {assert(array.length( arr_name_ ) == len_ );\
    assert_counter++;\
    assert(array.size( arr_name_ ) == max_ );\
    assert_counter++;\
    }

#define assert_profile_vector(arr_name_, len_, max_)\
    {assert(vector.length( arr_name_ ) == len_ );\
    assert_counter++;\
    assert(vector.size( arr_name_ ) == max_ );\
    assert_counter++;\
    }

#define assert_profile_llist(arr_name_, len_, max_)\
    {assert(llist.length( arr_name_ ) == len_ );\
    assert_counter++;\
    assert(llist.size( arr_name_ ) == max_ );\
    assert_counter++;\
    }

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
    assert_counter=0;

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
    assert_counter=0;

    assert_profile_array(tarr,rounded_size,rounded_size);
    assert_profile_array(tvec,rounded_size,rounded_size);
    assert_profile_array(tllist,rounded_size,rounded_size);

    printf("[\t TESTING PUSH ONE RESIZE - ARRAY, VECTOR AND LLIST\t]\n");

    assert(array.push_one(&tarr,5)==0); assert_counter++;
    assert_profile_array(tarr,rounded_size,rounded_size);

    assert(vector.push_one(&tvec,5)==1); assert_counter++;
    assert_profile_array(tvec,(rounded_size+1),rounded_size*2);

    uchar* addrs[8];
    addrs[0]= tllist;
    // printf("addr[0]: %p\n",(void*)addrs[0]);
    assert(addrs[0] == tllist);assert_counter++;
    assert(llist.push_one(&tllist,5)==1); assert_counter++;
    assert_profile_array(tllist,1,rounded_size); assert_counter++;
    assert(addrs[0] != tllist);assert_counter++;
    addrs[1] = tllist;
    // printf("addr[1]: %p\n",(void*)addrs[1]);

    //just for clearing the full array 'tarr'
    array.clear(tarr);
    assert_profile_array(tarr,0,rounded_size); assert_counter++;

    printf("[\t TESTING PUSH ONE RESIZE - PASSED - %ld OK \t]\n\n",assert_counter);
    assert_counter=0;

    printf("[\t TESTING PUSH MANY - ARRAY, VECTOR AND LLIST\t]\n");
        
    //should push 'rounded_size' items (16)
    assert(array.push_many(&tarr,values,rounded_size)==rounded_size); assert_counter++;
    assert_profile_array(tarr,rounded_size,rounded_size); assert_counter++;

    assert(vector.push_many(&tvec,values,rounded_size)==rounded_size); assert_counter++;
    assert_profile_vector(tvec,rounded_size*2+1,rounded_size*4); assert_counter++;

    // printf("quanto %ld\n",llist.push_many(&tllist,values,rounded_size));
    assert(llist.push_many(&tllist,values,rounded_size)==rounded_size); assert_counter++;
    assert_profile_array(tllist,1,rounded_size); assert_counter++;
    addrs[2] = tllist;
    // printf("addr[2]: %p\n",(void*)addrs[2]);

    //should push zero, and not change len or max
    //should push zero, and not change len or max
    assert(array.push_many(&tarr,values,rounded_size)==0); assert_counter++;
    assert_profile_array(tarr,rounded_size,rounded_size); assert_counter++;

    printf("[\t TESTING PUSH MANY - PASSED - %ld OK \t]\n\n",assert_counter);
    assert_counter=0;

    printf("[\t TESTING ITERATORS - ARRAY, VECTOR AND LLIST\t]\n");

    uchar* it_b_teste = iterator_begin(tarr);
    uchar* it_e_teste = iterator_end(tarr);
    assert(it_b_teste!=NULL); assert_counter++;
    assert(it_e_teste!=NULL); assert_counter++;

    int it_counter=0;
    for (uchar* it_b = array.it_begin(tarr); it_b != array.it_end(tarr); array.it_next(&it_b)){
        // printf("from iterator: value[%d] = %d\n",it_counter,*it_b);
        assert((*it_b) == values[it_counter++]);
        assert_counter++;
    }

    uchar* it_lb = iterator_llist_begin(tllist);
    uchar* it_le = iterator_llist_end(tllist);
    assert(it_lb  == addrs[0]);assert_counter++;
    assert(it_le == addrs[2]);assert_counter++;
    it_counter = 0;

    while (it_lb != NULL){
        // meta_print_llist(it_lb);
        assert(it_lb == addrs[it_counter]); assert_counter++;
        it_counter++;
        it_lb = llist.it_next(&it_lb);
    }

    it_lb = llist.it_begin(tllist);
    it_le = llist.it_end(tllist);
    assert(it_lb!=NULL);
    assert(it_le!=NULL);
    assert(it_lb  == addrs[0]);assert_counter++;
    assert(it_le == addrs[2]);assert_counter++;
    it_counter=0;

    printf("[\t TESTING ITERATORS - PASSED - %ld OK \t]\n\n",assert_counter);

    printf("[\t TESTING FOR EACH - ARRAY, VECTOR AND LLIST\t]\n");
    // array.for_each_map(tarr,);
    printf("[\t TESTING FOR EACH - PASSED - %ld OK \t]\n\n",assert_counter);


    array.destroy(tarr);
    vector.destroy(tvec);

    uchar *tarr2=NULL;
    uchar *tvec2=NULL;
    uchar *tllist2=NULL;


    return EXIT_SUCCESS;
}