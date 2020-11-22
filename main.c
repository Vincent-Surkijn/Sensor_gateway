/**
* author: Vincent Surkijn
**/
#define SET_MAX_TEMP 50
#define SET_MIN_TEMP 0
#ifndef DB_NAME
#define DB_NAME Sensor.db
#endif
#ifndef TABLE_NAME
#define TABLE_NAME SensorData
#endif
#define DBCONN sqlite3

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <sqlite3.h>
#include <string.h>
#include "config.h"
#include "datamgr.h"
#include "sensor_db.h"

int main(){

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

    DBCONN *db = init_connection(0);

    disconnect(db);
    // Free everything
    datamgr_free();
    free(fp_map);
    free(fp_data);
}
