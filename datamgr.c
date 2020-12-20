/**
* author: Vincent Surkijn
**/

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
    return lines;
}

int datamgr_findBinFileSize(FILE *file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    size = size/(sizeof(sensor_id_t) + sizeof(sensor_value_t) + sizeof(sensor_ts_t));
    fseek(file, 0, SEEK_SET);
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
	    return i;
	}
    }
    ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
    return -1;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
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
	char *msg;
	asprintf(&msg,"Sensor %d in room %d was too hot at %lld\n", sensor->id, sensor->room_id, (long long)(sensor->ts));
	write_fifo(msg);
	free(msg);
	return 1;
    }
    else if(avg < SET_MIN_TEMP){
        ERROR_HANDLER(true, DATAMGR_OUTOFRANGE_ERROR);
        char *msg;
        asprintf(&msg,"Sensor %d in room %d was too hot at %lld\n", sensor->id, sensor->room_id, (long long)(sensor->ts));
	write_fifo(msg);
	free(msg);
	return -1;
    }
    else{
	return 0;
    }
}

void datamgr_update_value_array(int index, double value){
    sensor_data_t *sensor = dpl_get_element_at_index(list, index);
    if(sensor == NULL){      					// list is empty or NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
        return;
    }
    sensor->values[(sensor->amount)%(RUN_AVG_LENGTH)] = value;
}

room_id_t datamgr_get_room_id(sensor_id_t sensor_id){
    int size = dpl_size(list);
    if(size==-1){      						// list is NULL
	ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
	return -1;
    }
    int i;
    for(i = 0; i<size; i++){
        if( ( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->id == sensor_id ){
            return ( (sensor_data_t *)(dpl_get_element_at_index(list, i)) )->room_id;
        }
    }
    char *msg;
    asprintf(&msg,"Sensor id not found in list\n");
    write_fifo(msg);
    free(msg);
    ERROR_HANDLER(true, DATAMGR_INVALID_ERROR);
    return -1;
}

sensor_ts_t datamgr_get_last_modified(sensor_id_t sensor_id){
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
    printf("Sensor_map: \n");
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

// Read sensor data
    int res;
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    do{
	res = sbuffer_read(*buffer,data,SBUFFER_DATAMGR);

	if(res == SBUFFER_NO_DATA || res == SBUFFER_FINISHED){
            if(sbuffer_alive(*buffer)){       // If buffer is still being updated, keep doing loop
/*TODO*/printf("Datamgr empty\n");
                continue;
            }
            else break;
	}

	int index = datamgr_get_index_of_sensor_id(data->id);

	if(index==-1){				//Invalid index -> id not found
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

    free(data);
}
