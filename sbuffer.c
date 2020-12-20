/**
 * \author Vincent Surkijn
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "sbuffer.h"

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node */
    sensor_data_t data;         /**< a structure containing the data */
    bool readby1;          	/**< boolean that is true if the values has already been read by reader1 */
    bool readby2;          	/**< boolean that is true if the values has already been read reader2 */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
    pthread_rwlock_t rw_lock;	/**< a read/write lock to be used for the list */

    pthread_cond_t empty;	/**< a condition variable to wait for when the buffer is empty */
    pthread_mutex_t m1;		/**< a mutex to use with the cond_wait for reader 1 */
    pthread_mutex_t m2;		/**< a mutex to use with the cond_wait for reader 2 */

    bool alive;			/**< a boolean to indicate whether the buffer is still being written to */
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    if(pthread_rwlock_init(&((*buffer)->rw_lock),NULL) != 0)	fprintf(stderr, "Failed to create rw lock\n");
    if(pthread_cond_init(&((*buffer)->empty),NULL) != 0)    	fprintf(stderr, "Failed to create condition variable\n");
    if(pthread_mutex_init(&((*buffer)->m1),NULL) != 0)		fprintf(stderr, "Failed to create mutex1\n");
    if(pthread_mutex_init(&((*buffer)->m2),NULL) != 0)		fprintf(stderr, "Failed to create mutex2\n");

    (*buffer)->alive = true;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    if(pthread_rwlock_destroy(&((*buffer)->rw_lock)) != 0)    	fprintf(stderr, "Failed to destroy rw lock\n");
    if(pthread_cond_destroy(&((*buffer)->empty)) != 0)    	fprintf(stderr, "Failed to destroy condition variable\n");
    if(pthread_mutex_destroy(&((*buffer)->m1)) != 0)		fprintf(stderr, "Failed to destroy mutex1\n");
    if(pthread_mutex_destroy(&((*buffer)->m2)) != 0)		fprintf(stderr, "Failed to destroy mutex2\n");

    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) 		// buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  					// buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    pthread_rwlock_wrlock(&buffer->rw_lock);
    pthread_mutex_lock(&buffer->m1);		//lock mutexes for cond_wait
    pthread_mutex_lock(&buffer->m2);

    if (buffer == NULL){
	pthread_rwlock_unlock(&buffer->rw_lock);
    	pthread_mutex_unlock(&buffer->m1);
	pthread_mutex_unlock(&buffer->m2);
	return SBUFFER_FAILURE;
    }
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL){
	pthread_rwlock_unlock(&buffer->rw_lock);
        pthread_mutex_unlock(&buffer->m1);
        pthread_mutex_unlock(&buffer->m2);
	return SBUFFER_FAILURE;
    }
    dummy->data = *data;
    dummy->next = NULL;
    dummy->readby1 = false;
    dummy->readby2 = false;
    if (buffer->tail == NULL) 			// buffer empty (buffer->head should also be NULL)
    {
        buffer->head = buffer->tail = dummy;
    } else 					// buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_cond_broadcast(&buffer->empty);	// Let consumers know that buffer has been updated
    pthread_mutex_unlock(&buffer->m1);
    pthread_mutex_unlock(&buffer->m2);

    pthread_rwlock_unlock(&buffer->rw_lock);
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data, int reader){
    sbuffer_node_t *dummy;
    pthread_rwlock_rdlock(&buffer->rw_lock);
    if (buffer == NULL){
        pthread_rwlock_unlock(&buffer->rw_lock);
        return SBUFFER_FAILURE;
    }
    if (buffer->head == NULL){
	pthread_rwlock_unlock(&buffer->rw_lock);

	pthread_mutex_lock(&buffer->m1);
	pthread_cond_wait(&buffer->empty,&buffer->m1);		// Don't need to check for spurious wake up, both threads need to read before condition can be false again
	pthread_mutex_unlock(&buffer->m1);

        return SBUFFER_NO_DATA;
    }

    dummy = buffer->head;
    if(reader == SBUFFER_DATAMGR){
	if( (buffer->head->readby1) == true){
	    while( dummy->readby1 == true && dummy->next != NULL)	dummy = dummy->next;
	}
        if(dummy->readby1 == true && dummy->next == NULL){      // This means that reader 1(datamgr) has read all values already
            data = NULL;
	    pthread_rwlock_unlock(&buffer->rw_lock);

            pthread_mutex_lock(&buffer->m1);
            pthread_cond_wait(&buffer->empty,&buffer->m1);  	// Don't need to check for spurious wake up, both threads need to read before condition can be false again
            pthread_mutex_unlock(&buffer->m1);

            return SBUFFER_FINISHED;
        }
    }
    else if(reader == SBUFFER_SENSORDB){
        if( (buffer->head->readby2) == true){
            while( dummy->readby2 == true && dummy->next != NULL)       dummy = dummy->next;
        }
	if(dummy->readby2 == true && dummy->next == NULL){	// This means that reader 2(sensor_db) has read all values already
	    data = NULL;
	    pthread_rwlock_unlock(&buffer->rw_lock);

            pthread_mutex_lock(&buffer->m2);
            pthread_cond_wait(&buffer->empty,&buffer->m2);  	// Don't need to check for spurious wake up, both threads need to read before condition can be false again
            pthread_mutex_unlock(&buffer->m2);

	    return SBUFFER_FINISHED;
	}
    }
    pthread_rwlock_unlock(&buffer->rw_lock);
    pthread_rwlock_wrlock(&buffer->rw_lock);			// Change to a write lock
    *data = dummy->data;
    if( reader == 1)	dummy->readby1 = true;			// Now it has been read by reader 1
    else if( reader == 2)        dummy->readby2 = true;   	// Now it has been read by reader 2
    if(dummy->readby1 && dummy->readby2){       		// If value was already read by both readers, it can be removed
        sbuffer_remove(buffer);
    }
    pthread_rwlock_unlock(&buffer->rw_lock);
    return SBUFFER_SUCCESS;
}

bool sbuffer_alive(sbuffer_t *buffer){
    return buffer->alive;
}

void sbuffer_died(sbuffer_t *buffer){
    pthread_mutex_lock(&buffer->m1);
    pthread_mutex_lock(&buffer->m2);
    buffer->alive = false;
    pthread_cond_broadcast(&buffer->empty);     // Wake up consumers because buffer has died
    pthread_mutex_unlock(&buffer->m1);
    pthread_mutex_unlock(&buffer->m2);
}
