/**
 * \author Vincent Surkijn
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sqlite3.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "connmgr.h"
#include "sbuffer.h"
#include "config.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "lib/dplist.h"
#include "lib/tcpsock.h"
#include "main.h"

int port;
sbuffer_t *buffer;

/*** all parent functions ***/
void *datamgr(){
    FILE *fp_map = fopen("./room_sensor.map", "r");
    if (fp_map == NULL) {
	perror("Opening map failed: ");
        return NULL;
    }
    while(sbuffer_alive(buffer)){
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
    while(sbuffer_alive(buffer)){
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
    sbuffer_died(buffer);
    printf("Exiting connmgr thread with %d\n", sbuffer_alive(buffer));
    return NULL;
}

void shut_down(){
    pid_t log_pid;
    int exit_status;
    printf("Ending parent process\n");

    char *msg;
    asprintf(&msg,"Log process has ended\n");
    write_fifo(msg);

    log_pid = wait(&exit_status);
    if(log_pid == -1)    perror("Error executing wait for child process");
    if (WIFEXITED(exit_status)){
	printf("Child exited successfully\n");
    }
    else{
	fprintf(stderr,"Child exited abnormally\n");
    }

    pthread_exit(NULL);
}

void parent(){
    pthread_t thread1, thread2, thread3;
    int res;

    sbuffer_init(&buffer);

    res = mkfifo(FIFO_NAME, 0666);	// Create FIFO with read&write(6) permissions for all
    if(res == -1)	perror("An error ocurred while creating the FIFO");
    char *msg;
    asprintf(&msg,"Starting new log process\n");
    write_fifo(msg);

    if(pthread_create( &thread1, NULL, &datamgr, NULL ) != 0) printf("Can't create datamgr thread\n");
    if(pthread_create( &thread2, NULL, &sensordb, NULL ) != 0) printf("Can't create sensordb thread\n");
    if(pthread_create( &thread3, NULL, &connmgr, NULL ) != 0) printf("Can't create connmgr thread\n");

    if(pthread_join(thread3, NULL) !=0) printf("can't join connmgr thread\n\n");
    if(pthread_join(thread1, NULL) !=0) printf("can't join datamgr thread\n\n");
    if(pthread_join(thread2, NULL) !=0) printf("can't join sensordb thread\n\n");

    printf("Freeing...\n");
    sbuffer_free(&buffer);
    free(buffer);

    shut_down();
}


/*** all child functions ***/

void read_fifo(){
    FILE *log_file;
    FILE *fifo;
    int res;
    char msg[200];	// Make sure it is big enough for all the messages

    fifo = fopen(FIFO_NAME, "r");
    if (fifo == NULL) {
        perror("Opening fifo failed: ");
    }

    log_file = fopen("sensor_gateway.log", "a");	// Write to end of log file
    if (log_file == NULL) {
        perror("Opening log_file failed: ");
    }

    char *str_result;
    do{
	str_result = fgets(msg, 200, fifo);
	if ( str_result != NULL ){
	    printf("Message received: %s", msg);
	    //TODO: write msg to log_file(maybe with fprintf?)
	}
    }while(strcmp(msg, "Log process has ended\n")!=0);

    res = fclose(fifo);
    if(res != 0) perror("Fifo close failed");

    res = fclose(log_file);
    if(res != 0) perror("Log file close failed");

    return;
}

void write_fifo(char *msg){
    FILE *fifo;
    int res;

    fifo = fopen(FIFO_NAME, "w");
    if (fifo == NULL) {
        perror("Opening fifo failed: ");
    }

    res = fputs(msg,fifo);
    if(res == EOF)	perror("Failed to write to fifo");

    fflush(fifo);

    fclose(fifo);
}

void child(){
    read_fifo();

    printf("Exiting child process\n");

    exit(EXIT_SUCCESS);
}


/*** main function ***/

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

    pid_t gw_pid, log_pid;	// Init pid for gateway and log processes

    gw_pid = getpid();
    printf("Parent process (pid = %d) is started ...\n", gw_pid);

    log_pid = fork();
    if(log_pid == -1)	perror("Error executing fork");

    if(log_pid == 0){
	child();
    }
    else{
	printf("Parent process (pid = %d) has created child process (pid = %d)\n", gw_pid, log_pid);
	parent();
    }

    return 0;
}
