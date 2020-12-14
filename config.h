/**
 * \author Vincent Surkijn
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef RUN_AVG_LENGTH
 #define RUN_AVG_LENGTH 5
#endif

#include <stdint.h>
#include <time.h>

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

