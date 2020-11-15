/**
* author: Vincent Surkijn
**/

#define SET_MAX_TEMP 50
#define SET_MIN_TEMP 0

#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include "config.h"
#include "datamgr.h"
#include "dplist.h"

int findFileSize(FILE *file){
    int lines = 0;
    char c;
    for (c = getc(file); c != EOF; c = getc(file)){
        if (c == '\n'){         // Increment amount for each line
            lines++;
        }
    }
    fseek(file, 0, SEEK_SET);  // Set cursor back to start of file, rewind(file) can also be used, but doesn't return value in case of error
    printf("Lines: %d\n", lines);
    return lines;
}

int findBinFileSize(FILE *file){
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    size = size/(sizeof(uint16_t) + sizeof(double) + sizeof(time_t));
    fseek(file, 0, SEEK_SET);
    printf("Size: %d\n", size);
    return size;
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){

    uint16_t room_id;
    uint16_t sensor_id;

    int size1 = findFileSize(fp_sensor_map);
    int size2 = findBinFileSize(fp_sensor_data);

    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    printf("Sensor_map: \n");
    int i;
    for(i=0; i<size1; i++){	// Read map values
	fscanf(fp_sensor_map, "%hd", &room_id);
	fscanf(fp_sensor_map, "%hd", &sensor_id);

	printf("Room: %hd -- Sensor: %hd\n", room_id, sensor_id);
    }

    printf("Sensor Data: \n");
    for(i=0; i<size2; i++){	// Read data values --> bin file
	uint16_t id;
	fread(&id, sizeof(uint16_t),1,fp_sensor_data);
	printf("Id: %hd -- ", id);

        double temp;
        fread(&temp, sizeof(double),1,fp_sensor_data);
        printf("Temp: %f -- ", temp);

        time_t time;
        fread(&time, sizeof(time_t),1,fp_sensor_data);
        printf("Time: %lld\n", (long long)time);
    }
}
