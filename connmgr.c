/**
 * \author Vincent Surkijn
 */

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <inttypes.h>
#include "config.h"
#include "connmgr.h"
#include "lib/dplist.h"
#include "lib/tcpsock.h"

typedef struct {
    int conn_sd;
    sensor_ts_t ts;
} connection_t;

void * element_copy(void * element){
    connection_t* copy = malloc(sizeof(connection_t));
    int new_conn_sd = ( (connection_t*)(element) )->conn_sd;
    sensor_ts_t new_ts = ( (connection_t*)(element) )->ts;
}

void element_free(void ** element){
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((connection_t*)x)->ts < ((connection_t*)y)->ts) ? -1 : (((connection_t*)x)->ts == ((connection_t*)y)->ts) ? 0 : 1);
}

dplist_t *conn_list = dpl_create(element_copy, element_free, element_compare);

void connmgr_listen(int port_number){
    struct pollfd poll_fd[1];   // Init the poll_fd array
    tcpsock_t *server, *client;
    int serversd;

    // Add port to the polling list
    if(tcp_passive_open(&server,port_number) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    tcp_get_sd(server, &serversd);
    printf("sd=%d\n",serversd);
    poll_fd[0].fd = serversd;
    poll_fd[0].events = POLLIN;

    printf("Before while\n");
    while(1){
	//printf("poll");
    	int result = poll(poll_fd, 1, -1);	// pos -> amount of elements with non-zero revents fields, neg -> err, 0 -> timeout
        sensor_data_t data;
        int bytes;
    	if(result<0){
	    fprintf(stderr, "Failure: an error occurred during polling\n");
    	}
    	else if(result==0){
            printf("Timeout reached\n");
	    return;
    	}
	else{
	    if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
	    do{
//            while(poll_fd[0].revents & POLLIN){
    	        //printf("received data!\n");
                // read sensor ID
                bytes = sizeof(data.id);
                result = tcp_receive(client, (void *) &data.id, &bytes);
                // read temperature
                bytes = sizeof(data.value);
                result = tcp_receive(client, (void *) &data.value, &bytes);
                // read timestamp
            	bytes = sizeof(data.ts);
            	result = tcp_receive(client, (void *) &data.ts, &bytes);
            	if ((result == TCP_NO_ERROR) && bytes) {
                    printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                       (long int) data.ts);
            	}
            }while(result == TCP_NO_ERROR);
            if (result == TCP_CONNECTION_CLOSED){
                printf("Peer has closed connection\n");
	    }
    	    else{
                printf("Error occured on connection to peer\n");
	    }
            tcp_close(&client);
	}
    }
    printf("After while");
}

void connmgr_free(){
	printf("connmgr.c\n");
}
