/**
* author: Vincent Surkijn
**/

#define _GNU_SOURCE

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


typedef int (*callback_t)(void *, int, char **, char **);


DBCONN *init_connection(char clear_up_flag){
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(TO_STRING(DB_NAME), &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
	write_fifo("Connection to the SQL database failed\n");
        sqlite3_close(db);

        return NULL;
    }

    write_fifo("Connected to the SQL database\n");

    char q[250] = "";
    char *sql = q;
    if(clear_up_flag == 1){
	char a[30] = "DROP TABLE IF EXISTS ";
	char b[100] = TO_STRING(TABLE_NAME);
	char c[20] = ";CREATE TABLE ";
	char e[100] = "(Id INTEGER PRIMARY KEY, sensor_id INTEGER,sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";

        strcat(q, a);
	strcat(q, b);
	strcat(q, c);
	strcat(q, b);
	strcat(q, e);

	//sql = "DROP TABLE IF EXISTS @table_name CREATE TABLE @table_name(Id INTEGER PRIMARY KEY, sensor_id INTEGER,sensor_value DECIMAL(4,2), timestamp TIMESTAMP);"

	write_fifo("New SQL table created\n");
    }
    else{
	char a[30] = "CREATE TABLE IF NOT EXISTS ";
	char b[100] = TO_STRING(TABLE_NAME);
	char c[100] = "(Id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";

        strcat(q, a);
        strcat(q, b);
        strcat(q, c);

	//sql = "CREATE TABLE IF NOT EXISTS @table_name (Id INTEGER PRIMARY KEY, sensor_id INTEGER,sensor_value DECIMAL(4,2), timestamp TIMESTAMP);"
    }

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc == SQLITE_OK) {
	#ifdef DEBUG
	    printf("Create query succesfully parsed\n");
	#endif
    }
    else {
        fprintf(stderr, "Failed to execute connect statement: %s\n", sqlite3_errmsg(db));
	return NULL;
    }

    return db;
}


void disconnect(DBCONN *conn){
    if(sqlite3_close(conn) != 0){
	fprintf(stderr, "Failed to close database: %s\n", sqlite3_errmsg(conn));
    }
}


int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    char *sql = "INSERT INTO SensorData(sensor_id, sensor_value, timestamp) VALUES( @id , @val , @ts )";
    sqlite3_stmt *res;
    int rc = sqlite3_prepare_v2(conn, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
	int idx = sqlite3_bind_parameter_index(res, "@id");
        sqlite3_bind_int(res, idx, id);

        idx = sqlite3_bind_parameter_index(res, "@val");
        sqlite3_bind_double(res, idx, value);

        idx = sqlite3_bind_parameter_index(res, "@ts");
        sensor_ts_t val_ts = ts;
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


int insert_sensor_from_sbuffer(DBCONN *conn, sbuffer_t **buffer){
    int res;
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    do{
        res = sbuffer_read(*buffer,data,SBUFFER_SENSORDB);
        if(res == SBUFFER_NO_DATA || res == SBUFFER_FINISHED){
            if(sbuffer_alive(*buffer)){ 		// If buffer is still being updated, keep doing loop
/*TODO*/printf("Sensor_db empty\n");
                continue;
            }
            else break;
	}
	res = insert_sensor(conn, data->id, data->value, data->ts);
    }while(res != SBUFFER_FAILURE);
    free(data);
    if(res == SBUFFER_FAILURE)	return -1;
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

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
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

    strcat(q, a);
    strcat(q, b);
    strcat(q, c);
    strcat(q, d);

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
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

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
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

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
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

    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);

    if (rc == SQLITE_OK) {
        return 0;
    }
    else {
        fprintf(stderr, "Failed to execute timestamp statement: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
}
