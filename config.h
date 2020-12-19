/**
 * \author Vincent Surkijn
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef RUN_AVG_LENGTH
 #define RUN_AVG_LENGTH 5		// The length of the running average of the datamgr
#endif

#ifndef DB_NAME
#define DB_NAME Sensor.db		// The name of the database of the sensor_db
#endif

#ifndef TABLE_NAME
#define TABLE_NAME SensorData		// The name of the table of the sensor_db
#endif

#define DBCONN sqlite3			// For the type of the db connection in sensor_db
#define WAIT_TIME 5			// The amount of time in seconds the program will wait between connection attempts
#define CONN_TRIES 3			// The amount of times the program will try to connect to the database
#define FIFO_NAME "the_FIFO"		// The name of the FIFO
#define LOG_NAME "sensor_gateway.log"	// The name of the log file

#include <stdint.h>
#include <time.h>
#include "main.h"	// Include main.h so all 'mgr's can use the FIFO functions

typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef int sensor_amount_values_t;
typedef double sensor_value_t;
typedef double sensor_values_t [RUN_AVG_LENGTH];
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

/**
 * structure to hold sensor data
 */
typedef struct {
    sensor_id_t id;         			/** < sensor id */
    room_id_t room_id;				/** < room id */
    sensor_amount_values_t amount;		/** < amount of values */
    sensor_value_t value;			/** < running avg of the sensor values */
    sensor_values_t values;		   	/** < last RUN_AVG_LENGTH sensor values */
    sensor_ts_t ts;         			/** < last sensor timestamp */
} sensor_data_t;


#endif /* _CONFIG_H_ */

