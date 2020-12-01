#include <stdio.h>
#include "sbuffer.h"

int main(){

sbuffer_t *buff;

int result = sbuffer_init(&buff);
if(result == SBUFFER_SUCCESS)	printf("Lib okay\n");
else	printf("Failed creation\n");

}
