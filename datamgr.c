/**
* author: Vincent Surkijn
**/

#ifndef RUN_AVG_LENGTH
 #define RUN_AVG_LENGTH 5
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "config.h"
#include "datamgr.h"
#include "sbuffer.h"
#include "lib/dplist.h"


// Define list as a global variable so it can be used everywhere in datamgr.c
dplist_t *list;

void * datamgr_element_copy(void * element){
    sensor_data_t* copy = malloc(sizeof(sensor_data_t));
    copy->id = ( (sensor_data_t*)element  )->id;
    copy->room_id = ( (sensor_data_t*)element  )->room_id;
    copy->amount = ( (sensor_data_t*)element  )->amount;
    copy->values[0] = ( (sensor_data_t*)element  )->values[0];
    copy->ts = ( (sensor_data_t*)element  )->ts;
    return (void *) copy;
}

void datamgr_element_free(void ** element){
    //printf("In element_free\n");
    free(*element);
    *element = NULL;
}

int datamgr_element_compare(void * x, void * y){
    return ((((sensor_data_t*)x)->id < ((sensor_data_t*)y)->id) ? -1 : (((sensor_data_t*)x)->id == ((sensor_data_t*)y)->id) ? 0 : 1);
}

int datamgr_findFileSize(FILE *file){
    int lines = 0;
    char c;
    for (c = getc(file); c != EOF; c = getc(file)){
        if (c == '\n'){         // Increment amount for each line
            lines++;
        }
    }
    fseek(file, 0, SEEK_SET);  // Set cursor back to start of file, rewind(file) can also be used, but doesn't return value in case of error
    //printf("Lines: %d\n", lines);
    return lines;
}

int datamgr_findBinFileSize(FILE *file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    size = size/(sizeof(sensor_id_t) + sizeof(sensor_value_t) + sizeof(sensor_ts_t));
    fseek(file, 0, SEEK_SET);
    //printf("Size: %d\n", size);
    return size;
}

sensor_data_t *createElement(sensor_id_t id, room_id_t room_id, sensor_amount_values_t amount, double value, sensor_ts_t ts){
    sensor_data_t *element = malloc(sizeof(sensor_data_t));
    element->id = id;
    element->room_id = room_id;
    element->amount = amount;
    int i;
    for (i=0; i<RUN_AVG_LENGTH; i++){
	element->values[i] = value;
    }
    element->value = value;
    element->ts = ts;
    return element;
}

int datamgr_get_index_of_sensor_id(sensor_id_t sensor_id){
    int size = dpl_size(list);
    int i;
    if(size == -1){		// list is NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
	return -1;
    }
    for(i = 0; i<size; i++){
	if( ( (sensor_data_t*)(dpl_get_element_at_index(list, i)) )->id == sensor_id ){
	    //printf("Sensor id found at %d\n", i);
	    return i;
	}
    }
    printf("Sensor id not found in list\n");
    ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
    return -1;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    //printf("Find avg of sensor: %d\n", sensor_id);
    int index = datamgr_get_index_of_sensor_id(sensor_id);
    if(index == -1)	return -1;	// Invalid return
    sensor_data_t *sensor = dpl_get_element_at_index(list, index);
    double avg;
    int i;
    if( (sensor->amount)>=RUN_AVG_LENGTH ){
        for(i=0; i<RUN_AVG_LENGTH; i++){
            avg += sensor->values[i];
        }
    }
    else{
        return 0;
    }
    return ( (sensor_value_t)(avg/RUN_AVG_LENGTH) );
}


int datamgr_check_avg_at_index(int index){
    sensor_data_t *sensor = dpl_get_element_at_index(list, index);
    if(sensor == NULL){      // list is empty or NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
        return -1;
    }
    double avg = datamgr_get_avg(sensor->id);
    if(avg > SET_MAX_TEMP){
        ERROR_HANDLER(true, DATAMGR_OUTOFRANGE_ERROR);
	printf("Sensor %d in room %d was too hot at %lld\n", sensor->id, sensor->room_id, (long long)(sensor->ts));
	return 1;
    }
    else if(avg < SET_MIN_TEMP){
        ERROR_HANDLER(true, DATAMGR_OUTOFRANGE_ERROR);
        printf("Sensor %d in room %d was too cold at %lld\n", sensor->id, sensor->room_id, (long long)(sensor->ts));
	return -1;
    }
    else{
	return 0;
    }
}

void datamgr_update_value_array(int index, double value){
    //printf("Update array called\n");
    sensor_data_t *sensor = dpl_get_element_at_index(list, index);
    if(sensor == NULL){      // list is empty or NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
        return;
    }
    sensor->values[(sensor->amount)%(RUN_AVG_LENGTH)] = value;
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
    int size = dpl_size(list);
    if(size==-1){      // list is NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
	return -1;
    }
    int i;
    for(i = 0; i<size; i++){
        if( ( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->id == sensor_id ){
            return ( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->room_id;
        }
    }
    printf("Sensor id not found in list\n");
    ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
    return -1;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    int index = datamgr_get_index_of_sensor_id(sensor_id);
    if(index == -1)     return 0;       // Invalid return
    sensor_data_t *sensor = dpl_get_element_at_index(list, index);
    return sensor->ts;
}

int datamgr_get_total_sensors(){
    int size = dpl_size(list);
    if(size==-1){      // list is NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
    }
    return size;
}

void datamgr_free(){
    dpl_free(&list, true);
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t **buffer){

  int size1 = datamgr_findFileSize(fp_sensor_map);

    list = dpl_create(datamgr_element_copy, datamgr_element_free, datamgr_element_compare);

// Read map data
    //printf("Sensor_map: \n");
    int i;
    for(i=0; i<size1; i++){	// Read map values
	room_id_t room_id;
	sensor_id_t sensor_id;

	fscanf(fp_sensor_map, "%hd", &room_id);
	fscanf(fp_sensor_map, "%hd", &sensor_id);

	printf("Room: %hd -- Sensor: %hd\n", room_id, sensor_id);

	sensor_data_t *sensor = createElement(sensor_id, room_id, 0, 0, 0);
	list = dpl_insert_at_index(list, sensor, i, false);
    }
    //printf("List size after inserts: %d\n", dpl_size(list));

// Read sensor data
    //printf("Sensor Data: \n");
    int res;
    do{	// Read data values --> sbuffer
	sensor_data_t *data = malloc(sizeof(sensor_data_t));

	res = sbuffer_read(*buffer,data,SBUFFER_DATAMGR);
	//printf("Id: %hd -- ", data->id);

	if(res == SBUFFER_NO_DATA){
	    //usleep(1);
	    continue;
	}
        else if(res == SBUFFER_FINISHED){	// Everything read
            if(sbuffer_alive(*buffer)){       // If buffer is still being updated, wait for new value
                printf("Datamgr going to sleep\n");
                sleep(1);
                continue;
            }
            else break;                 // If connmgr stopped & everything read -> stop reading
        }

	int index = datamgr_get_index_of_sensor_id(data->id);

        //printf("Temp: %f -- \n", data->value);

        //printf("Time: %lld\n", (long long)(data->ts) );

	if(index==-1){	//Invalid index -> id not found
	    fprintf(stderr, "Tried to add data of non existing sensor id\n");
	}
	else{
            datamgr_update_value_array(index, data->value);
            (( (sensor_data_t*)(dpl_get_element_at_index(list, index)) )->amount)++;
	    ( (sensor_data_t*)(dpl_get_element_at_index(list, index)) )->ts = data->ts;

            if((( (sensor_data_t *)(dpl_get_element_at_index(list, index)) )->amount)>=RUN_AVG_LENGTH){      // If enough values present in the sensor, the avg can be calculated
    	        datamgr_check_avg_at_index(index);
            }
	}
    }while(res != SBUFFER_FAILURE);



/**    printf("Element id at index 0: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 0)) )->id );
    printf("Element id at index 1: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->id );
    printf("Element id at index 2: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 2)) )->id );
    printf("Element id at index 3: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 3)) )->id );
    printf("Element id at index 4: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 4)) )->id );
    printf("Element id at index 5: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 5)) )->id );
    printf("Element id at index 6: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 6)) )->id );
    printf("Element id at index 7: %hd\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 7)) )->id );


    printf("Element value1 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[0] );
    printf("Element value2 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[1] );
    printf("Element value3 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[2] );
    printf("Element value4 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[3] );
    printf("Element value5 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[4] );
    printf("Element value6 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[5] );
    printf("Element value7 at index 1: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->values[6] );
    printf("Element value at index 7: %f\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 7)) )->values[1] );

    printf("Run avg of index 0: %f\n", datamgr_get_avg((  (sensor_data_t *)(dpl_get_element_at_index(list, 0)) )->id));
    printf("Run avg of index 1: %f\n", datamgr_get_avg((  (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->id));

    printf("Element amount at index 0: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 0)) )->amount );
    printf("Element amount at index 1: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->amount );
    printf("Element amount at index 2: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 2)) )->amount );
    printf("Element amount at index 3: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 3)) )->amount );
    printf("Element amount at index 4: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 4)) )->amount );
    printf("Element amount at index 5: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 5)) )->amount );
    printf("Element amount at index 6: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 6)) )->amount );
    printf("Element amount at index 7: %d\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 7)) )->amount );*/

	printf("Size of list: %d\n", datamgr_get_total_sensors());
	printf("RUN_AVG_LENGTH: %d\n", RUN_AVG_LENGTH);
	printf("SET_MIN_TEMP: %f\n", SET_MIN_TEMP);
	printf("SET_MAX_TEMP: %f\n", SET_MAX_TEMP);
}

void datamgr_print(int i){
    printf("Element value1 at index %d: %f\n", i,( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->values[0] );
    printf("Element value2 at index %d: %f\n", i,( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->values[1] );
    printf("Element value3 at index %d: %f\n", i,( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->values[2] );
    printf("Element value4 at index %d: %f\n", i,( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->values[3] );
    printf("Element value5 at index %d: %f\n", i,( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->values[4] );
    printf("Element amount at index 1: %d\n\n", ( (sensor_data_t *)(dpl_get_element_at_index(list, 1)) )->amount );
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
