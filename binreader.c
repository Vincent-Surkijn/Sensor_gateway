#include <stdio.h>
#include "config.h"

int findBinFileSize(FILE *file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    size = size/(sizeof(sensor_id_t) + sizeof(sensor_value_t) + sizeof(sensor_ts_t));
    fseek(file, 0, SEEK_SET);
    //printf("Size: %d\n", size);
    return size;
}


int main(){
FILE *fp_bin = fopen("./sensor_data_recv", "r");

if (fp_bin == NULL) {
    perror("Data failed: ");
    return -1;
}

int size2 = findBinFileSize(fp_bin);
int i;
for(i=0; i<size2; i++){	// Read data values --> bin file
	uint16_t id;
	fread(&id, sizeof(uint16_t),1,fp_bin);
	printf("Id: %hd -- ", id);

        double temp;
        fread(&temp, sizeof(double),1,fp_bin);
        printf("Temp: %f -- ", temp);

        time_t time;
        fread(&time, sizeof(time_t),1,fp_bin);
        printf("Time: %lld\n", (long long)time);
}
}
