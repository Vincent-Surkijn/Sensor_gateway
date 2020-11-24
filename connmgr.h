/**
 * \author Vincent Surkijn
 */

#ifndef _CONNMGR_H_
#define _CONNMGR_H_


/* This method holds the core functionality of your connmgr.
 * It starts listening on the given port and when when a sensor node connects it writes the data to a sensor_data_recv file.
 * This file must have the same format as the sensor_data file in assignment 6 and 7.
*/
void connmgr_listen(int port_number);

/* This method should be called to clean up the connmgr, and to free all used memory.
 * After this no new connections will be accepted
*/
void connmgr_free();

#endif /* _CONNMGR_H_ */
