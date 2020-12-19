/**
 * \author Vincent Surkijn
 */

#ifndef TIMEOUT
#define TIMEOUT 5
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <inttypes.h>
#include "config.h"
#include "connmgr.h"
#include "sbuffer.h"
#include "lib/dplist.h"
#include "lib/tcpsock.h"

typedef struct {
    tcpsock_t *sock;
    int conn_sd;
    sensor_id_t id;
    sensor_ts_t ts;
} connection_t;

void * connmgr_element_copy(void * element){
    connection_t *copy = malloc(sizeof(connection_t));
    copy->conn_sd = ( (connection_t*)(element) )->conn_sd;
    copy->ts = ( (connection_t*)(element) )->ts;
    return copy;
}

void connmgr_element_free(void ** element){
    free(*element);
    *element = NULL;
}

int connmgr_element_compare(void * x, void * y) {
    return ((((connection_t*)x)->ts < ((connection_t*)y)->ts) ? -1 : (((connection_t*)x)->ts == ((connection_t*)y)->ts) ? 0 : 1);
}

// conn_list is initialized as a global variable so it can be accessed from each function
dplist_t *conn_list;
// Init the poll_fd array as a global variable so it can be accessed from each function
struct pollfd *poll_fd;


void connmgr_add_conn(tcpsock_t *sock, int sd){
    connection_t * new_conn = malloc(sizeof(connection_t));
    new_conn->sock = sock;
    new_conn->conn_sd = sd;
    new_conn->ts = 0;	// No ts yet
    int index = dpl_size(conn_list);
    conn_list = dpl_insert_at_index(conn_list, new_conn, index, false);	// insert new connection at end of list
    struct pollfd *tmp = realloc(poll_fd,sizeof(struct pollfd)*(index+2));	// +2 because new connection and server need to be included
    if (NULL == tmp){
	fprintf(stderr, "Failed realloc\n");
    }
    else
    {
	poll_fd = tmp;
	tmp = NULL;
    }
    poll_fd[index+1].fd = sd;	// index+1 because this list also contains the server
    poll_fd[index+1].events = POLLIN;
}

void connmgr_listen(int port_number, sbuffer_t **buffer){
//    printf("Timeout = %d\n", TIMEOUT);
    tcpsock_t *server, *client;
    int serversd, clientsd;

    conn_list = dpl_create(connmgr_element_copy, connmgr_element_free, connmgr_element_compare);
    poll_fd = malloc(sizeof(struct pollfd));

    // Add server port to the polling list
    if(tcp_passive_open(&server,port_number) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    tcp_get_sd(server, &serversd);
//    printf("Server sd = %d\n",serversd);
    poll_fd[0].fd = serversd;
    poll_fd[0].events = POLLIN;

    //printf("Before while\n");
    bool loop = true;
    while(loop){
	//printf("poll");
    	int result = poll(poll_fd, dpl_size(conn_list) + 1, TIMEOUT*1000);	// pos -> amount of elements with non-zero revents fields, neg -> err, 0 -> timeout(in ms)
        sensor_data_t *data = malloc(sizeof(sensor_data_t));
        int bytes;
    	if(result<0){
	    fprintf(stderr, "Failure: an error occurred during polling\n");
    	}
    	else if(result==0){
	    write_fifo("Server timeout reached\n");
	    loop = false;
    	}
	else{
	    if(poll_fd[0].revents & POLLIN){	// If it's the server then a new sensor will be added to the poll list
	    	if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
	    	tcp_get_sd(client, &clientsd);
//	    	printf("client sd=%d\n",clientsd);
	    	connmgr_add_conn(client,clientsd);      // Add new connection to the poll list
	    }
	    int i;
	    for(i=0;i<dpl_size(conn_list);i++){
		//printf("For loop\n");
		if(poll_fd[i+1].revents & POLLIN){
		    connection_t *dummy;
		    time_t now;
		    do{
			// retrieve conn_list element from this sensor
			dummy = dpl_get_element_at_index(conn_list, i);
                	client = dummy->sock;
			now = time(NULL);
			// read sensor ID
                	bytes = sizeof(data->id);
                	result = tcp_receive(client, (void *) &(data->id), &bytes);
			dummy->id = data->id;
                        if( (long)(dummy->ts) == 0){	// If no timestamp is filled yet, the sensor is new
                            char *msg;
                            asprintf(&msg,"New sensor node connected with sensor id %d\n",data->id);
                            write_fifo(msg);
                        }

                	// read temperature
                	bytes = sizeof(data->value);
                	result = tcp_receive(client, (void *) &(data->value), &bytes);
                	// read timestamp
                	bytes = sizeof(data->ts);
                	result = tcp_receive(client, (void *) &(data->ts), &bytes);
			dummy->ts = data->ts;
                	if ((result == TCP_NO_ERROR) && bytes) {
                	    printf("Connmgr: sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data->id, data->value, (long int)(data->ts) );
			    sbuffer_insert(*buffer,data);
                	}
            	    }while(result == TCP_NO_ERROR && (((dummy->ts) - now) < TIMEOUT));
            	    if (result == TCP_CONNECTION_CLOSED){
                	printf("Peer has closed connection\n");
			dummy = dpl_get_element_at_index(conn_list, i);
			char *msg;
			asprintf(&msg,"Sensor node with id %d closed connection\n",dummy->id);
                        write_fifo(msg);
			poll_fd[i+1].fd = -1;	// stop listening to this one
			conn_list = dpl_remove_at_index(conn_list, i, true);	// can be removed from list as well
//			printf("Size of list now: %d\n", dpl_size(conn_list));
//			printf("Time: %ld\n", time(NULL));
                    }
		    else if(( (dummy->ts) - now) >= TIMEOUT){
			printf("Timeout reached for sensor %d\n", i+1);
		    }
            	    else{
                	printf("Error occured on connection to peer\n");
            	    }
            	    tcp_close(&client);
		}
	    }
	}
    }
//    printf("Closed server at %ld\n", time(NULL));
    tcp_close(&server);
}

void connmgr_free(){
//    printf("Freeing..\n");
//printf("Size of list: %d\n", dpl_size(conn_list));
    int size = dpl_size(conn_list);
    if(size > 0){	// check if list is empty
	int i;
	for(i=0;i<size;i++){	// if not empty, all connections need to be closed
	    connection_t *dummy = dpl_get_element_at_index(conn_list,i);
	    tcpsock_t *dummy_sock = dummy->sock;
	    tcp_close(&dummy_sock);
	}
    }
    dpl_free(&conn_list, true);
    free(poll_fd);
}
