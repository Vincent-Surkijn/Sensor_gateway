#include <string.h>
#include <stdio.h>

int main(){

char a[20] = "hello";
char b[20] = "world";

char tot[40] = "";

strcat(tot, a);
strcat(tot, b);

printf("%s", tot);

}
