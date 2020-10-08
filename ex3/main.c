#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FREQUENCY 5
#define MIN (-10)
#define MAX 35

int main(){

//float temp;

//Variables for printing date in right format
char buffer[80];
time_t rawtime;
//struct tm *info;

srand(time(NULL));

//Format date: Wed 07 Oct 2020 04:02:10 PM UTC

while(1){
	time( &rawtime );
	struct tm *info = gmtime( &rawtime );
	strftime(buffer,80,"%a %d %b %Y %I:%M:%S %p %Z",info);

	float temp = ((double)rand() * (MAX-MIN))/(double)RAND_MAX+MIN;
	printf("Temperature = %1.2f @ %s \n", temp, buffer);
	fflush(stdout);
	sleep(FREQUENCY);
	}

return 0;
}
