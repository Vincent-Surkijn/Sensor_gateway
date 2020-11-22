#include <stdio.h>
#include "dplist.h"

int main(){

    int i = dpl_size(NULL);

    printf("Size = %d\n", i);

    void *ptr = dpl_insert_at_index(NULL, NULL, 0, true);

    printf("Ptr: %p\n", ptr);

}
