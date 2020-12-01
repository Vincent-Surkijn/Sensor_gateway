#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
	sensor_data_t *data;
	res = sbuffer_remove(buffer, data);
	printf("Reader 1 read:\n");
	printf("Id: %hd -- ", data->id);
        printf("Temp: %f -- ", data->value);
        printf("Time: %lld\n", (long long)(data->ts) );
    }while(res == SBUFFER_SUCCESS);
}

void *reader2(){
    int res;
    do{
        sensor_data_t *data;
        res = sbuffer_remove(buffer, data);
        printf("Reader 2 read:\n");
        printf("Id: %hd -- ", data->id);
        printf("Temp: %f -- ", data->value);
        printf("Time: %lld\n", (long long)(data->ts) );
    }while(res == SBUFFER_SUCCESS);
}

void *writer(){
    int size = thread_findBinFileSize(fp_data);
    int i;
    for(i=0; i<size; i++){	// Read data values --> bin file
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
    }
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

    pthread_exit(NULL);

}
