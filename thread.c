#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "config.h"
#include "sbuffer.h"

/*** testing threads ***
void *func1(){
    int i;
    for(i=0;i<5;i++){
        printf("Thread 1 -- cycle: %d\n", i);
        usleep(500);
    }
}

void *func2(){
    int i;
    for(i=0;i<5;i++){
	printf("Thread 2 -- cycle: %d\n", i);
	usleep(500);
    }
}
*/

FILE *fp_data;
sbuffer_t *buffer;
//pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
volatile bool writerQuit = false;
// Debug:
volatile int counts1;
volatile int counts2;
volatile int counts3;

int thread_findBinFileSize(FILE *file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    size = size/(sizeof(sensor_id_t) + sizeof(sensor_value_t) + sizeof(sensor_ts_t));
    fseek(file, 0, SEEK_SET);
    //printf("Size: %d\n", size);
    return size;
}

void *reader1(){
int res;
do{
  /*  pthread_mutex_lock( &cond_mutex );
    sensor_data_t *temp = malloc(sizeof(sensor_data_t));
    while(true){
	res = sbuffer_read(buffer, temp, 0);
	if(res == SBUFFER_NO_DATA){	// check if list is not empty with dummy reader 0
	    //printf("Reader1 Waiting for data\n");
	    pthread_cond_wait(&cond1, &cond_mutex);
	}
	else break;
    }
    pthread_mutex_unlock( &cond_mutex );
    free(temp);
*/
//    do{
//	pthread_mutex_lock( &data_mutex );	// lock thread
	sensor_data_t *data = malloc(sizeof(sensor_data_t));
	res = sbuffer_read(buffer, data, 1);
	if(res == SBUFFER_SUCCESS){
	    printf("Reader 1 read:\n");
	    printf("Id: %hd -- ", data->id);
            printf("Temp: %f -- ", data->value);
            printf("Time: %lld\n", (long long)(data->ts) );
	    counts1++;
	}
	else	continue;//usleep(1);	// if no data: sleep for 1ms
	free(data);
//	pthread_mutex_unlock( &data_mutex );	// unlock thread
  //  }while(res == SBUFFER_SUCCESS);
}while(!(writerQuit && res == SBUFFER_NO_DATA));
printf("Exiting reader1\n");
}

void *reader2(){
int res;
do{
/*    pthread_mutex_lock( &cond_mutex );
    sensor_data_t *temp = malloc(sizeof(sensor_data_t));
    while(true){
        res = sbuffer_read(buffer, temp, 0);
	if(res == SBUFFER_NO_DATA){   // check if list is not empty with dummy reader 0
            //printf("Reader2 Waiting for data\n");
            pthread_cond_wait(&cond1, &cond_mutex);
        }
        else break;
    }
    pthread_mutex_unlock( &cond_mutex );
    free(temp);
*/
   // do{
  //      pthread_mutex_lock( &data_mutex );      // lock thread
        sensor_data_t *data = malloc(sizeof(sensor_data_t));
        res = sbuffer_read(buffer, data, 2);
        if(res == SBUFFER_SUCCESS){
	    printf("Reader 2 read:\n");
            printf("Id: %hd -- ", data->id);
            printf("Temp: %f -- ", data->value);
            printf("Time: %lld\n", (long long)(data->ts) );
	    counts2++;
	}
	else continue;//usleep(1);	// if no data: sleep for 1ms
	free(data);
    //    pthread_mutex_unlock( &data_mutex );    // unlock thread
 //   }while(res == SBUFFER_SUCCESS);
}while(!(writerQuit && res == SBUFFER_NO_DATA));
printf("Exiting reader2\n");
}

void *writer(){
//    pthread_mutex_lock( &data_mutex );      // lock thread
    int size = thread_findBinFileSize(fp_data);
    int i;
    for(i=0; i<size; i++){	// Read data values --> bin file
  //      pthread_mutex_lock( &data_mutex );      // lock thread
	printf("Writer writes: \n");

	sensor_id_t id;
	fread(&id, sizeof(sensor_id_t),1,fp_data);
	//printf("Id: %hd -- ", id);

        double temp;
        fread(&temp, sizeof(sensor_value_t),1,fp_data);
        //printf("Temp: %f -- ", temp);

        time_t time;
        fread(&time, sizeof(sensor_ts_t),1,fp_data);
        //printf("Time: %lld\n", (long long)time);

	// insert read data into buffer
	sensor_data_t *data = malloc(sizeof(sensor_data_t));
	data->id = id;
	data->value = temp;
	data->ts = time;
	sbuffer_insert(buffer, data);
	counts3++;
//	pthread_cond_broadcast(&cond1);	// Signal that new data is added
	free(data);
//        pthread_mutex_unlock( &data_mutex );    // unlock thread
    }
    fclose(fp_data);
//    pthread_mutex_unlock( &data_mutex );    // unlock thread
    writerQuit = true;
    pthread_exit(NULL);
}

int main(){

/*** testing threads ***
    pthread_t thread1, thread2;

    pthread_create( &thread1, NULL, &func1, NULL );
    pthread_create( &thread2, NULL, &func2, NULL );

    pthread_exit(NULL);
*/
    pthread_t thread1, thread2, thread3;
    fp_data = fopen("./sensor_data", "rb");

    if (fp_data == NULL) {
        perror("Opening data failed: ");
	return -1;
    }

    int res = sbuffer_init(&buffer);
    if(res != SBUFFER_SUCCESS){
	fprintf(stderr, "Failure: couldn't create sbuffer!\n");
	return -1;
    }

    pthread_create( &thread1, NULL, &reader1, NULL );
    pthread_create( &thread2, NULL, &reader2, NULL );
    pthread_create( &thread3, NULL, &writer, NULL );

    if(pthread_join(thread3, NULL) !=0) printf("can't join write thread\n\n");
    if(pthread_join(thread1, NULL) !=0) printf("can't join read1 thread\n\n");
    if(pthread_join(thread2, NULL) !=0) printf("can't join read2 thread\n\n");

printf("Reader 1 read %d values\n",counts1);
printf("Reader 2 read %d values\n",counts2);
printf("Writer wrote %d values\n",counts3);

printf("Freeing...\n");
    sbuffer_free(&buffer);
    free(buffer);

    pthread_exit(NULL);
}
