/**
 * \author Vincent Surkijn
 */

#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#include <stdbool.h>
#include "config.h"

#define SBUFFER_FAILURE -1
#define SBUFFER_SUCCESS 0
#define SBUFFER_NO_DATA 1
#define SBUFFER_FINISHED 2

#define SBUFFER_DATAMGR 1
#define SBUFFER_SENSORDB 2
typedef struct sbuffer sbuffer_t;

/**
 * Allocates and initializes a new shared buffer
 * \param buffer a double pointer to the buffer that needs to be initialized
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_init(sbuffer_t **buffer);

/**
 * All allocated resources are freed and cleaned up
 * \param buffer a double pointer to the buffer that needs to be freed
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_free(sbuffer_t **buffer);

/**
 * Removes the first sensor data in 'buffer' (at the 'head') and returns this sensor data as '*data'
 * If 'buffer' is empty, the function doesn't block until new sensor data becomes available but returns SBUFFER_NO_DATA
 * \param buffer a pointer to the buffer that is used
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_remove(sbuffer_t *buffer);

/**
 * Inserts the sensor data in 'data' at the end of 'buffer' (at the 'tail')
 * \param buffer a pointer to the buffer that is used
 * \param data a pointer to sensor_data_t data, that will be copied into the buffer
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occured
 */
int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data);

/**
 * Reads the first value of the buffer
 * \param buffer a pointer to the buffer that is used
 * \param data a pointer to sensor_data_t data, that will be copied into the buffer
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occured
 */
int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data, int reader);

/**
 * Returns the value of alive
 * \param buffer a pointer to the buffer that is used
 * \return value of alive
 */
bool sbuffer_alive(sbuffer_t *buffer);

/**
 * Sets the value of alive to false
 * \param buffer a pointer to the buffer that is used
 */
void sbuffer_died(sbuffer_t *buffer);

#endif  //_SBUFFER_H_
