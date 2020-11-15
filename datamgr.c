/**
* author: Vincent Surkijn
**/

/*
 * definition of error codes
 * */
#define DATAMGR_NO_ERROR 0
#define DATAMGR_MEMORY_ERROR 1 // error due to mem alloc failure
#define DATAMGR_INVALID_ERROR 2 //error due to a list operation applied on a NULL list
#define DATAMGR_NO_DEFINE_ERROR 3 //error due to parameters not defined

#ifdef DEBUG
#define DEBUG_PRINTF(...)	\
        do {	\
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	\
            fprintf(stderr,__VA_ARGS__);	\
            fflush(stderr);	\
                } while(0)	\
#else
#define DEBUG_PRINTF(...) (void)0	\
#endif

#define DATAMGR_ERR_HANDLER(condition, err_code)	\
    do {	\
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");	\
            assert(!(condition));	\
        } while(0)	\

#ifndef SET_MAX_TEMP
 DATAMGR_ERR_HANDLER(true, DATAMGR_NO_DEFINE_ERROR);
#endif
#ifndef SET_MIN_TEMP
 DATAMGR_ERR_HANDLER(true, DATAMGR_NO_DEFINE_ERROR);
#endif
#ifndef RUN_AVG_LENGTH
 #define RUN_AVG_LENGTH 5
#endif

#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include "config.h"
#include "datamgr.h"
#include "lib/dplist.h"


// Define list as a global variable so it can be used everywhere in datamgr.c
dplist_t *list;

void * element_copy(void * element){
    sensor_data_t* copy = malloc(sizeof(sensor_data_t));
    copy->id = ( (sensor_data_t*)element  )->id;
    copy->room_id = ( (sensor_data_t*)element  )->room_id;
    copy->amount = ( (sensor_data_t*)element  )->amount;
    copy->value = ( (sensor_data_t*)element  )->value;
    copy->ts = ( (sensor_data_t*)element  )->ts;
    return (void *) copy;
}

void element_free(void ** element){
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y){
    return ((((sensor_data_t*)x)->id < ((sensor_data_t*)y)->id) ? -1 : (((sensor_data_t*)x)->id == ((sensor_data_t*)y)->id) ? 0 : 1);
}

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

sensor_data_t *createElement(sensor_id_t id, room_id_t room_id, sensor_amount_values_t amount, sensor_value_t value, sensor_ts_t ts){
    sensor_data_t *element = malloc(sizeof(sensor_data_t));
    element->id = id;
    element->room_id = room_id;
    element->amount = amount;
    element->value = value;
    element->ts = ts;
    return element;
}

int datamgr_get_index_of_sensor_id(sensor_id_t sensor_id){
    int size = dpl_size(list);
    int i;
    for(i = 0; i<size; i++){
	if( ((sensor_data_t *)(dpl_get_element_at_index(list, i)))->id == sensor_id ){
	    printf("Sensor id found at %d\n", i);
	    return i;
	}
    }
    printf("Sensor id not found in list\n");
    return -1;
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){

    room_id_t room_id;
    sensor_id_t sensor_id;

    int size1 = findFileSize(fp_sensor_map);
    int size2 = findBinFileSize(fp_sensor_data);

    list = dpl_create(element_copy, element_free, element_compare);

// Read map data
    printf("Sensor_map: \n");
    int i;
    for(i=0; i<size1; i++){	// Read map values
	fscanf(fp_sensor_map, "%hd", &room_id);
	fscanf(fp_sensor_map, "%hd", &sensor_id);

	printf("Room: %hd -- Sensor: %hd\n", room_id, sensor_id);

	sensor_data_t *sensor = createElement(sensor_id, room_id, 0, 0, 0);
	list = dpl_insert_at_index(list, sensor, i, true);
    }
    printf("List size after inserts: %d\n", dpl_size(list));

// Read sensor data
    printf("Sensor Data: \n");
    for(i=0; i<size2; i++){	// Read data values --> bin file
	uint16_t id;
	fread(&id, sizeof(uint16_t),1,fp_sensor_data);
	printf("Id: %hd -- ", id);

	int index = datamgr_get_index_of_sensor_id(id);

        double temp;
        fread(&temp, sizeof(double),1,fp_sensor_data);
        printf("Temp: %f -- ", temp);

	(( (sensor_data_t *)(dpl_get_element_at_index(list, index)) )->amount)++;
	( (sensor_data_t *)(dpl_get_element_at_index(list, index)) )->value += temp;

        time_t time;
        fread(&time, sizeof(time_t),1,fp_sensor_data);
        printf("Time: %lld\n", (long long)time);

	( (sensor_data_t *)(dpl_get_element_at_index(list, index)) )->ts = time;
    }

    printf("Element amount at index 0: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 0)) )->amount );
    printf("Element amount at index 1: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->amount );
    printf("Element amount at index 2: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 2)) )->amount );
    printf("Element amount at index 3: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 3)) )->amount );
    printf("Element amount at index 4: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 4)) )->amount );
    printf("Element amount at index 5: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 5)) )->amount );
    printf("Element amount at index 6: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 6)) )->amount );
    printf("Element amount at index 7: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 7)) )->amount );
	datamgr_get_index_of_sensor_id(99);
	printf("RUN_AVG_LENGTH: %d\n", RUN_AVG_LENGTH);
	printf("SET_MIN_TEMP: %d\n", SET_MIN_TEMP);
	printf("SET_MAX_TEMP: %d\n", SET_MAX_TEMP);
}

/*int main(){

    FILE *fp_map = fopen("./room_sensor.map", "r");
    FILE *fp_data = fopen("./sensor_data", "rb");

    if (fp_map == NULL) {
        perror("Map failed: ");
        return -1;
    }
    if (fp_data == NULL) {
        perror("Data failed: ");
        return -1;
    }

    datamgr_parse_sensor_files( fp_map, fp_data);

    printf("Element id at index 0: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 0)) )->id );
    printf("Element id at index 1: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->id );
    printf("Element id at index 2: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 2)) )->id );
    printf("Element id at index 3: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 3)) )->id );
    printf("Element id at index 4: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 4)) )->id );
    printf("Element id at index 5: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 5)) )->id );
    printf("Element id at index 6: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 6)) )->id );
    printf("Element id at index 7: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 7)) )->id );

}*/
