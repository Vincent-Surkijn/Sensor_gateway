#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(){
int max = 20;
char first[max];
char second[max];
char name[max];
char str[max];

printf("Voornaam: \n");
scanf("%s", first);

printf("Achternaam: \n");
scanf("%s", second);

printf("Hello %s %s \n", first, second);

int i = 0;
while(i<max){
	str[i] = toupper(second[i]);
	i++;
}

printf("Str: %s \n", str);

printf("Result: %d \n", strcmp(second,str));

int j = 0;
while(j<max){
	name[j] = first[j];
	j++;
}
strcat(strcat(name," "), second); //safer:strncat(name, second, 15)
printf("Concatenated: %s \n", name);

printf("Geboortejaar: \n");
int year;
scanf("%d", &year);

char name2[max];
int k = 0;
while(k<max){
        name2[k] = name[k];
        k++;
}
snprintf(name, 5*max, " %s %d \n",name2, year);
printf("%s", name);

int l;
char result[max];
sscanf (name,"%s %s %d", first, second, &year);
printf ("First: %s \n",first);
printf ("Second: %s \n",second);
printf ("Year: %d \n",year);

return 0;
}
