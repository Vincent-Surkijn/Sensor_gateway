#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FREQUENCY 5
#define MIN (-10)
#define MAX 35

int main(){

float temp;

char buffer[80];
time_t rawtime;
struct tm *info;
time( &rawtime );
info = gmtime( &rawtime );
strftime(buffer,80,"%a %d %b %Y %I:%M:%S %p %Z",info);

srand(time(NULL));

//Format date: Wed 07 Oct 2020 04:02:10 PM UTC
//printf("%d\n",(int) ( (( (double)(MAX+1) - (double)(MIN-1) )*rand()/(RAND_MAX+1.0)) + (double)(MIN-1) ));

while(1){
	temp =  ( (( (double)(MAX+1) - (double)(MIN-1) )*rand()/(RAND_MAX+1.0)) + (double)(MIN-1));
	printf("Temperature = %1.2f @ %s \n", temp, buffer);
	fflush(stdout);
	sleep(FREQUENCY);
}

return 0;
}
