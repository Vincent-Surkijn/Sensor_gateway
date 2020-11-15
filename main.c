/**
* author: Vincent Surkijn
**/

#define SET_MAX_TEMP 50
#define SET_MIN_TEMP 0

#include "config.h"
#include "datamgr.h"
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

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
}
