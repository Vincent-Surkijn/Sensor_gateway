/**
* author: Vincent Surkijn
**/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "sensor_db.h"
#include "sbuffer.h"

// stringify preprocessor directives using 2-level preprocessor magic
// this avoids using directives like -DDB_NAME=\"some_db_name\"
#define REAL_TO_STRING(s) #s
#define TO_STRING(s) REAL_TO_STRING(s)    //force macro-expansion on s before stringify s

#ifndef DB_NAME
#define DB_NAME Sensor.db
#endif

#ifndef TABLE_NAME
#define TABLE_NAME SensorData
#endif

#define DBCONN sqlite3


typedef int (*callback_t)(void *, int, char **, char **);


DBCONN *init_connection(char clear_up_flag){
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(TO_STRING(DB_NAME), &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return NULL;
    }

    char q[250] = "";
    char *sql = q;
    // Impossible to give table name as a parameter in an sqlite query
    if(clear_up_flag == 1){
	//printf("flag = 1\n");
	char a[30] = "DROP TABLE IF EXISTS ";
	char b[100] = TO_STRING(TABLE_NAME);
	char c[20] = ";CREATE TABLE ";
	char e[100] = "(Id INTEGER PRIMARY KEY, sensor_id INTEGER,sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";

        strcat(q, a);
	strcat(q, b);
	strcat(q, c);
	strcat(q, b);
	strcat(q, e);
	//printf("%s\n",sql);

	//sql = "DROP TABLE IF EXISTS @table_name;"
          //      "CREATE TABLE @table_name(Id INTEGER PRIMARY KEY, sensor_id INTEGER,"
	  //	"sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";
    }
    else{
	//printf("flag != 1\n");
	char a[30] = "CREATE TABLE IF NOT EXISTS ";
	char b[100] = TO_STRING(TABLE_NAME);
	char c[100] = "(Id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";

        strcat(q, a);
        strcat(q, b);
        strcat(q, c);
	//printf("%s\n",sql);

	//sql = "CREATE TABLE IF NOT EXISTS @table_name (Id INTEGER PRIMARY KEY, sensor_id INTEGER,"
          //      "sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";
    }

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc == SQLITE_OK) {
	printf("Create query succesfully parsed\n");
    }
    else {
        fprintf(stderr, "Failed to execute connect statement: %s\n", sqlite3_errmsg(db));

	return NULL;
    }

    return db;
}


void disconnect(DBCONN *conn){
    sqlite3_close(conn);
    /*if(sqlite3_close(conn) != 0){
	fprintf(stderr, "Failed to close database: %s\n", sqlite3_errmsg(sqlite3_close(conn)));
    }*/
}


int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    char *sql = "INSERT INTO SensorData(sensor_id, sensor_value, timestamp) VALUES( @id , @val , @ts )";
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(conn, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        printf("Insert query succesfully parsed\n");
	int idx = sqlite3_bind_parameter_index(res, "@id");
        sqlite3_bind_int(res, idx, id);

        idx = sqlite3_bind_parameter_index(res, "@val");
        sqlite3_bind_double(res, idx, value);

        idx = sqlite3_bind_parameter_index(res, "@ts");
        time_t val_ts = ts;
	sqlite3_bind_int64(res, idx, val_ts);
    }
    else {
        fprintf(stderr, "Failed to execute insert statement: %s\n", sqlite3_errmsg(conn));
	return -1;
    }

    sqlite3_step(res);

    sqlite3_finalize(res);

    return 0;
}


int sensor_findBinFileSize(FILE *file){
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    size = size/(sizeof(sensor_id_t) + sizeof(sensor_value_t) + sizeof(sensor_ts_t));
    fseek(file, 0, SEEK_SET);
    printf("Bin file size: %d\n", size);
    return size;
}


int insert_sensor_from_sbuffer(DBCONN *conn, sbuffer_t **buffer){	// TODO: test
    int res;
    do{ // Read data values --> sbuffer
        sensor_data_t *data = malloc(sizeof(sensor_data_t));

        res = sbuffer_read(*buffer,data,SBUFFER_SENSORDB);
        if(res == SBUFFER_NO_DATA || res == SBUFFER_FINISHED){
	    sleep(1);
	    continue;
	}

	res = insert_sensor(conn, data->id, data->value, data->ts);

	//printf("Id: %hd -- ", data->id);
        //printf("Temp: %f -- \n", data->value);
        //printf("Time: %lld\n", (long long)(data->ts) );

//    }while(res == SBUFFER_SUCCESS);
    }while(res != SBUFFER_FAILURE);

    return 0;
}


int insert_sensor_from_file(DBCONN *conn, FILE *sensor_data){
    int size = sensor_findBinFileSize(sensor_data);
    int i;
    for(i=0; i<size; i++){     // Read data values --> bin file
        sensor_id_t id;
        fread(&id, sizeof(sensor_id_t),1,sensor_data);
	//printf("Id: %d -- ", id);

        sensor_value_t  temp;
        fread(&temp, sizeof(sensor_value_t),1,sensor_data);
        //printf("Temp: %f -- ", temp);

        time_t time;
        fread(&time, sizeof(time_t),1,sensor_data);
        //printf("Time: %lld\n", (long long)time);

	int res = insert_sensor(conn, id, temp, time);
	if(res!=0)	return -1;
    }
    return 0;
}


int find_sensor_all(DBCONN *conn, callback_t f){
    char *err_msg;
    char q[150] = "";
    char *sql = q;
    char a[20] = "SELECT * FROM ";
    char b[100] = TO_STRING(TABLE_NAME);

    strcat(q, a);
    strcat(q, b);
    printf("Query = %s\n", sql);

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
        printf("Select * query succesfully executed\n");
	return 0;
    }
    else {
        fprintf(stderr, "Failed to execute connect statement: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
}


int find_sensor_by_value(DBCONN *conn, sensor_value_t value, callback_t f){
    char *err_msg;
    char q[200] = "";
    char *sql = q;
    char a[20] = "SELECT * FROM ";
    char b[100] = TO_STRING(TABLE_NAME);
    char c[30] = " where sensor_value = ";
    char d[10];
    sprintf(d, "%f", value);
    //printf("double as char: %s\n",d);

    strcat(q, a);
    strcat(q, b);
    strcat(q, c);
    strcat(q, d);
    //printf("Query = %s\n", sql);

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
        printf("Select by value query succesfully executed\n");
        return 0;
    }
    else {
        fprintf(stderr, "Failed to execute select value statement: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
}


int find_sensor_exceed_value(DBCONN *conn, sensor_value_t value, callback_t f){
    char *err_msg;
    char q[200] = "";
    char *sql = q;
    char a[20] = "SELECT * FROM ";
    char b[100] = TO_STRING(TABLE_NAME);
    char c[30] = " where sensor_value > ";
    char d[10];
    sprintf(d, "%f", value);

    strcat(q, a);
    strcat(q, b);
    strcat(q, c);
    strcat(q, d);
    printf("Query = %s\n", sql);

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
        printf("Select by exceed value query succesfully executed\n");
        return 0;
    }
    else {
        fprintf(stderr, "Failed to execute exceed value statement: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
}


int find_sensor_by_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f){
    char *err_msg;
    char q[250] = "";
    char *sql = q;
    char a[20] = "SELECT * FROM ";
    strcat(q, a);
    char b[100] = TO_STRING(TABLE_NAME);
    char c[30] = " where timestamp = ";
    char d[10];
    sprintf(d, "%ld", (long)ts);	// After this a is NULL so concat before

    strcat(q, b);
    strcat(q, c);
    strcat(q, d);
    //printf("Query = %s\n", sql);

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
        printf("Select by timestamp query succesfully executed\n");
        return 0;
    }
    else {
        fprintf(stderr, "Failed to execute timestamp statement: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
}


int find_sensor_after_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f){
    char *err_msg;
    char q[250] = "";
    char *sql = q;
    char a[20] = "SELECT * FROM ";
    strcat(q, a);
    char b[100] = TO_STRING(TABLE_NAME);
    char c[30] = " where timestamp > ";
    char d[10];
    sprintf(d, "%ld", (long)ts);        // After this a is NULL so concat before

    strcat(q, b);
    strcat(q, c);
    strcat(q, d);
    printf("Query = %s\n", sql);

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
        printf("Select by timestamp query succesfully executed\n");
        return 0;
    }
    else {
        fprintf(stderr, "Failed to execute timestamp statement: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
}
