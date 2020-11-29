#include <stdio.h>
#include <stdlib.h>

int *array;

int main(){
array[0] = 1;
printf("init size %ld\n", sizeof(array)/sizeof(array[0]));
array[1] = 2;
printf("new size %ld\n", sizeof(array)/sizeof(array[0]));
}
