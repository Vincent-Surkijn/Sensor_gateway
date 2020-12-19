/**
 * \author Vincent Surkijn
 */

#ifndef MAIN_H_
#define MAIN_H_

//#define FIFO_NAME "the_FIFO"

/**
 * This method runs the datamgr thread
*/
void *datamgr();

/**
 * This method runs the sensor_db thread
*/
void *sensordb();

/**
 * This method runs the connmgr thread
*/
void *connmgr();

/**
 * This method shuts down the 2 processes
 * It waits for the child to finish and then closes it
*/
void shut_down();

/**
 * This method runs the gateway(parent) process
*/
void parent();

/**
 * This method runs the log(child) process
*/
void child();

/**
 * This method runs the main thread
 * \param argc is the amount of arguments passed in the command(should be 2: name of executable + port number)
 * \param argv is a pointer to a pointer to a char array with all the input of the command
 * \returns 0 if exitted succesfully, non-zero if an error occurred
*/
int main(int argc, char **argv);

/*** Here start all the functions needed for FIFO I/O ***/

/**
 * This method is used by the log process to read the FIFO
 * It also writes the messages to the log file
*/
void read_fifo();

/**
 * This method is used by the gateway process to write to the FIFO
 * \param msg is a pointer to the message that is written to the FIFO
*/
void write_fifo(char *msg);

#endif /* MAIN_H_ */
