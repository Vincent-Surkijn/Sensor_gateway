#include <limits.h>
#include <stdio.h>

int main(){
	int integer = 0;
	float fl = 0;
	double db = 0;
	void myRoutine();
	void * pointer;

	short int sh_int = 0;

	long int lg_int = 0;
        long double lg_db  = 0;

	char c = '1';

	printf("The size of int is: %ld \n", sizeof(integer));
	printf("The size of float is: %ld \n", sizeof(fl));
        printf("The size of double is: %ld \n", sizeof(db));
	printf("The size of void is: %ld \n", sizeof(myRoutine()));
	printf("The size of pointer is: %ld \n \n", sizeof(pointer));

        printf("The size of short int is: %ld \n \n", sizeof(sh_int));

        printf("The size of long int is: %ld \n", sizeof(lg_int));
        printf("The size of long double is: %ld \n \n", sizeof(lg_db));

	printf("Standard char max is: %d  \n", CHAR_MAX);
	printf("The standard char is a signed char \n");

	return 0;
}
