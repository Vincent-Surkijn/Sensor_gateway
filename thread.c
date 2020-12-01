#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

void *reader1(){

}

void *reader2(){

}

void *writer(){

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

    pthread_create( &thread1, NULL, &reader1, NULL );
    pthread_create( &thread2, NULL, &reader2, NULL );
    pthread_create( &thread3, NULL, &writer, NULL );

    pthread_exit(NULL);

}
