/**
* author: Vincent Surkijn
**/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "sensor_db.h"

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
    sqlite3_stmt *res;

    int rc = sqlite3_open(TO_STRING(DB_NAME), &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return NULL;
    }

    char q[250] = "";
    char *sql = &q;
    // Impossible to give table name as a parameter in an sqlite query
    if(clear_up_flag == 1){
	printf("flag = 1\n");
	char a[30] = "DROP TABLE IF EXISTS ";
	char b[100] = TO_STRING(TABLE_NAME);
	char c[20] = ";CREATE TABLE ";
	char e[100] = "(Id INTEGER PRIMARY KEY, sensor_id INTEGER,sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";

        strcat(q, a);
	strcat(q, b);
	strcat(q, c);
	strcat(q, b);
	strcat(q, e);
	printf("%s\n",sql);
	//sql = "DROP TABLE IF EXISTS @table_name;"
          //      "CREATE TABLE @table_name(Id INTEGER PRIMARY KEY, sensor_id INTEGER,"
	  //	"sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";
    }
    else{
	printf("flag != 1\n");
	char a[30] = "CREATE TABLE IF NOT EXISTS ";
	char b[100] = TO_STRING(TABLE_NAME);
	char c[100] = "(Id INTEGER PRIMARY KEY, sensor_id INTEGER, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";

        strcat(q, a);
        strcat(q, b);
        strcat(q, c);
	printf("%s\n",sql);
	//sql = "CREATE TABLE IF NOT EXISTS @table_name (Id INTEGER PRIMARY KEY, sensor_id INTEGER,"
          //      "sensor_value DECIMAL(4,2), timestamp TIMESTAMP);";
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);	// Converts query to bin code, syntax error when table name = parameter

    if (rc == SQLITE_OK) {
	printf("Query succesfully parsed\n");
    }
    else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

	return NULL;
    }

    int step = sqlite3_step(res);

    if (step == SQLITE_ROW) {

        printf("%s: ", sqlite3_column_text(res, 0));
        printf("%s\n", sqlite3_column_text(res, 1));

    }

    sqlite3_finalize(res);

    return db;
}


void disconnect(DBCONN *conn){
    sqlite3_close(conn);
}


int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);
