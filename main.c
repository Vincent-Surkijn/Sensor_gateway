/**
 * \author Vincent Surkijn
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sqlite3.h>
#include "connmgr.h"
#include "sbuffer.h"
#include "config.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "lib/dplist.h"
#include "lib/tcpsock.h"

int port;
sbuffer_t *buffer;
volatile bool alive = true;

void *datamgr(){
    FILE *fp_map = fopen("./room_sensor.map", "r");
    if (fp_map == NULL) {
	perror("Opening map failed: ");
        return NULL;
    }
    while(alive){
        datamgr_parse_sensor_files(fp_map,&buffer);
    }
    datamgr_free();
    fclose(fp_map);
    printf("Exiting datamgr thread\n");
    return NULL;
}


void *sensordb(){
    int attempts = 0;
    DBCONN *conn;
    while(attempts<3){
	conn = init_connection(1);
        if(conn==NULL){
            attempts++;
            continue;
        }
	else break;
    }
    printf("Connected to db!\n");
    while(alive){
    	int res = insert_sensor_from_sbuffer(conn,&buffer);
	//printf("Main sensor res = %d\n",res);
    	if(res != 0) printf("Error while inserting sensor into sqlite db\n");
    }
    disconnect(conn);
    printf("Exiting sensordb thread\n");
    return NULL;
}


void *connmgr(){
    connmgr_listen(port,&buffer);
    connmgr_free();
    printf("Exiting connmgr thread\n");
    alive = false;
    return NULL;
}


void parent(){
    pthread_t thread1, thread2, thread3;

    sbuffer_init(&buffer);

    if(pthread_create( &thread1, NULL, &datamgr, NULL ) != 0) printf("Can't create datamgr thread\n");
    if(pthread_create( &thread2, NULL, &sensordb, NULL ) != 0) printf("Can't create sensordb thread\n");
    if(pthread_create( &thread3, NULL, &connmgr, NULL ) != 0) printf("Can't create connmgr thread\n");

    if(pthread_join(thread3, NULL) !=0) printf("can't join connmgr thread\n\n");
    if(pthread_join(thread1, NULL) !=0) printf("can't join datamgr thread\n\n");
    if(pthread_join(thread2, NULL) !=0) printf("can't join sensordb thread\n\n");

    printf("Freeing...\n");
    sbuffer_free(&buffer);
    free(buffer);

    pthread_exit(NULL);

}


int main(int argc, char **argv){

    if( argc == 2){
        char *a = argv[1];
        port = atoi(a);         // Take port number from command line
        printf("Port number = %d\n", port);
    }
    else{
        fprintf(stderr, "Failure: wrong amount of arguments passed to connmgr.c\n");
        return -1;
    }

    parent();

    return 0;
}
