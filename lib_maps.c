#include <stdlib.h>
#include <stdio.h>
#include "lib_maps.h"

byte* minus_two_func(byte* value, byte* res){
    if(!value) return NULL; 
    *res = (*value)-2; 
    printf("new value is: %u\n",*res);
    return res;
    }