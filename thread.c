#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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


int main(){
    pthread_t thread1, thread2;

    pthread_create( &thread1, NULL, &func1, NULL );
    pthread_create( &thread2, NULL, &func2, NULL );

    pthread_exit(NULL);
}
