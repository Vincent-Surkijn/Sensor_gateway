/**
 * \author Vincent Surkijn
 */

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include "config.h"
#include "connmgr.h"
#include "lib/dplist.h"
#include "lib/tcpsock.h"

int main(int argc, char **argv){
    int port;

    if( argc == 2){
        char *a = argv[1];
        port = atoi(a);         // Take port number from command line
        printf("Port number = %d\n", port);
    }
    else{
        fprintf(stderr, "Failure: wrong amount of arguments passed to connmgr.c\n");
        return -1;
    }

    connmgr_listen(port);

	// For testing the compile and link process
	connmgr_free();

    return 0;
}
