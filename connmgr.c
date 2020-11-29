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

// conn_list is initialized as a global variable so it can be accessed from each function
dplist_t *conn_list;
// Init the poll_fd array as a global variable so it can be accessed from each function
struct pollfd *poll_fd;


void connmgr_add_conn(int sd){
    connection_t * new_conn = malloc(sizeof(connection_t));
    new_conn->conn_sd = sd;
    new_conn->ts = 0;	// No ts yet
    int index = dpl_size(conn_list);
    conn_list = dpl_insert_at_index(conn_list, new_conn, index, false);	// insert new connection at end of list
    realloc(poll_fd,sizeof(struct pollfd)*(index+2));	// +2 because new connection and server need to be included
    poll_fd[index+1].fd = sd;	// index+1 because this list also contains the server
    poll_fd[index+1].events = POLLIN;
}

void connmgr_listen(int port_number){
    tcpsock_t *server, *client;
    int serversd, clientsd;

    conn_list = dpl_create(element_copy, element_free, element_compare);
    poll_fd = malloc(sizeof(struct pollfd));

    // Add server port to the polling list
    if(tcp_passive_open(&server,port_number) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    tcp_get_sd(server, &serversd);
    printf("OG sd=%d\n",serversd);
    printf("Poll_fd init size: %ld\n", sizeof(poll_fd));
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
	    if(poll_fd[0].revents & POLLIN){	// If it's the server then a new sensor will be added to the poll list
	    	if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
	    	tcp_get_sd(client, &clientsd);
	    	printf("client sd=%d\n",clientsd);
	    	// Add new connection to the poll list
	    	connmgr_add_conn(clientsd);
		printf("Poll_fd size: %ld\n", sizeof(poll_fd));
	    }
	}
    }
}

void connmgr_free(){
	printf("connmgr.c\n");
}
