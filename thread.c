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
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile bool empty;

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
	pthread_mutex_lock( &data_mutex );	// lock thread
	sensor_data_t *data = malloc(sizeof(sensor_data_t));
	res = sbuffer_remove(buffer, data);	// geeft segmentation fault, zelfs zonder threads, maar niet als er ene print statement tss staat(zie main)?
	printf("Reader 1 read:\n");
	printf("Id: %hd -- ", data->id);
        printf("Temp: %f -- ", data->value);
        printf("Time: %lld\n", (long long)(data->ts) );
	pthread_mutex_unlock( &data_mutex );	// unlock thread
    }while(res == SBUFFER_SUCCESS && !empty);
}

void *reader2(){
    int res;
    do{
        pthread_mutex_lock( &data_mutex );      // lock thread
        sensor_data_t *data = malloc(sizeof(sensor_data_t));
        res = sbuffer_remove(buffer, data);
        printf("Reader 2 read:\n");
        printf("Id: %hd -- ", data->id);
        printf("Temp: %f -- ", data->value);
        printf("Time: %lld\n", (long long)(data->ts) );
        pthread_mutex_unlock( &data_mutex );    // unlock thread
    }while(res == SBUFFER_SUCCESS && !empty);
}

void *writer(){
    pthread_mutex_lock( &data_mutex );      // lock thread
    int size = thread_findBinFileSize(fp_data);
    int i;
    for(i=0; i<size; i++){	// Read data values --> bin file
//        pthread_mutex_lock( &data_mutex );      // lock thread
	printf("Writer writes: \n");

	sensor_id_t id;
	fread(&id, sizeof(sensor_id_t),1,fp_data);
	printf("Id: %hd -- ", id);

        double temp;
        fread(&temp, sizeof(sensor_value_t),1,fp_data);
        printf("Temp: %f -- ", temp);

        time_t time;
        fread(&time, sizeof(sensor_ts_t),1,fp_data);
        printf("Time: %lld\n", (long long)time);

	// insert read data into buffer
	sensor_data_t *data = malloc(sizeof(sensor_data_t));
	data->id = id;
	data->value = temp;
	data->ts = time;
	sbuffer_insert(buffer, data);
	empty = false;
  //      pthread_mutex_unlock( &data_mutex );    // unlock thread
    }
    fclose(fp_data);
    pthread_mutex_unlock( &data_mutex );    // unlock thread
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
    empty = true;
    if(res != SBUFFER_SUCCESS){
	fprintf(stderr, "Failure: couldn't create sbuffer!\n");
	return -1;
    }

    pthread_create( &thread1, NULL, &reader1, NULL );
    pthread_create( &thread2, NULL, &reader2, NULL );
    pthread_create( &thread3, NULL, &writer, NULL );

    pthread_exit(NULL);


}
