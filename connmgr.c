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

void connmgr_listen(int port_number){
    struct pollfd poll_fd[1];   // Init the poll_fd array
    tcpsock_t *server;
    int serversd;

    // Add port to the polling list
    if(tcp_passive_open(&server,port_number) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    tcp_get_sd(server, &serversd);
    printf("sd=%d\n",serversd);
    poll_fd[0].fd = serversd;
    poll_fd[0].events = POLLIN;

    printf("Before while\n");
    while(1){
	printf("poll");
    	int result = poll(poll_fd, 1, -1);	// pos -> amount of elements with non-zero revents fields, neg -> err, 0 -> timeout
    	if(result<0){
	    fprintf(stderr, "Failure: an error occurred during polling\n");
    	}
    	else if(result==0){
            printf("Timeout reached\n");
	    return;
    	}
	else{
            if(poll_fd[0].revents & POLLIN){
    	        printf("received data!\n");
            }
            else{
            	printf("Disconnected\n");
            }
	}
    }
    printf("After while");
}

void connmgr_free(){
	printf("connmgr.c\n");
}
