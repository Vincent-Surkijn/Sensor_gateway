/**
 * \author Vincent Surkijn
 */

#include <stdio.h>
#include "config.h"
#include "connmgr.h"
#include "lib/dplist.h"
#include "lib/tcpsock.h"

int main(){
    printf("test\n");
    printf("Size: %d\n",dpl_size(NULL));
    printf("tcp_close(NULL)\n");
    tcp_close(NULL);

    return 0;
}
