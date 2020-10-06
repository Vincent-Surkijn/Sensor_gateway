#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FREQUENCY 5
#define MIN (-10)
#define MAX 35

int main(){

srand(time(NULL));

printf("%d\n",(int) ( (( (double)(MAX+1) - (double)(MIN-1) )*rand()/(RAND_MAX+1.0)) + (double)(MIN-1) ));
printf("%d\n",(int) ( (( (double)(MAX+1) - (double)(MIN-1) )*rand()/(RAND_MAX+1.0)) + (double)(MIN-1) ));
printf("%d\n",(int) ( (( (double)(MAX+1) - (double)(MIN-1) )*rand()/(RAND_MAX+1.0)) + (double)(MIN-1) ));
printf("%d\n",(int) ( (( (double)(MAX+1) - (double)(MIN-1) )*rand()/(RAND_MAX+1.0)) + (double)(MIN-1) ));

return 0;
}
