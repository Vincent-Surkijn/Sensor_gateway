/**
 * \author Vincent Surkijn
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "sbuffer.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#error SET_MAX_TEMP not set
#endif

#ifndef SET_MIN_TEMP
#error SET_MIN_TEMP not set
#endif

/*
 * definition of error codes
 * */
#define DATAMGR_NO_ERROR 0
#define DATAMGR_OUTOFRANGE_ERROR 1 //error due to a temperature being out of range#pragma GCC error
#define DATAMGR_INVALID_ERROR 2 //error due to e.g. operation on NULL list or invalid ID

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, err_code)    do {                       \
                      if (err_code==DATAMGR_OUTOFRANGE_ERROR) {                   \
                        fprintf(stderr, "OUTOFRANGE ERROR: There was a temperature out of range\n");            \
                      }                                                 \
                      if (err_code==DATAMGR_INVALID_ERROR) {                   \
                        fprintf(stderr, "INVALID ERROR: an invalid operation has occured\n");              \
                      }                                                 \
                    } while(0)

/**
 * This method is needed for the creation of the dplist
 * It makes a copy of the passed element
 * \param element pointer to the element to be copied
 * \return a pointer to the copy
*/
void * datamgr_element_copy(void * element);

/**
 * This method is needed for the creation of the dplist
 * It frees the passed element
 * \param element is a pointer to a pointer to the element to be freed
*/
void datamgr_element_free(void ** element);

/**
 * This method is needed for the creation of the dplist
 * It compares two elements based on their sensor_id
 * \param x is a pointer to the first element
 * \param y is a pointer to the seconde element
 * \returns -1 if x<y, 0 if x==y and 1 if x>y
*/
int datamgr_element_compare(void * x, void *y);

/**
 * This method finds the amount of lines in a file
 * \param file is a pointer to the file
 * \returns the amount of lines
*/
int datamgr_findFileSize(FILE *file);

/**
 * This method finds the amount of entries in a binary file based on the size of 1 entry
 * \param file is a pointer to the file
 * \returns the amount of entries
*/
int datamgr_findBinFileSize(FILE *file);

/**
 * This method creates a sensor_data_t struct from some variables
 * \param id is the sensor id
 * \param room_id is the room id
 * \param amount is the amount of temperature values for the sensor
 * \param value is the running avg of the sensor
 * \param ts is the timestamp of the last modification for the sensor
 * \returns a pointer to the created element
*/
sensor_data_t *createElement(sensor_id_t id, room_id_t room_id, sensor_amount_values_t amount, sensor_value_t value, sensor_ts_t ts);

/**
 * This method gets the index of an id of a sensor in the dplist
 *\param sensor_id is the id to be searched
 *\returns index if the id is found, -1 if it's not found
*/
int datamgr_get_index_of_sensor_id(sensor_id_t sensor_id);

/**
 * This method checks if the avg of the sensor at a certain index doesn't exceed borders
 * It prints a message containing sensor Id, room Id, timestamp and whether it was too hot or cold
 * \param index is the index of the sensor in the list that will be checked
 * \returns 1 if too hot, -1 if too cold and 0 if neither
*/
int datamgr_check_avg_at_index(int index);

/**
 * This method updates the array with values of the sensor at a certain index with a value
 * \param index is the index of the sensor to update
 * \param value is the value that will be added
*/
void datamgr_update_value_array(int index, double value);

/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them.
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t **buffer);

/**
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free();

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id);

/**
 * Gets the running AVG of a certain senor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id);

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id);

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int datamgr_get_total_sensors();

#endif  //DATAMGR_H_
