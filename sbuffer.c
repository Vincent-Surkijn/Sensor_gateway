/**
 * \author Vincent Surkijn
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "sbuffer.h"

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
    bool readby1;          // boolean that is true if the values has already been read by reader1
    bool readby2;          // boolean that is true if the values has already been read reader2
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
    pthread_rwlock_t rw_lock;	// a read/write lock to be used for the list
    pthread_cond_t empty;	// a condition variable to wait for when the buffer is empty
};

int sbuffer_init(sbuffer_t **buffer) {		// Thread safe
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    (*buffer)->rw_lock = lock;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {		// Thread safe
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer) {		// Thread safe: only gets called from within rw_lock(see sbuffer_read)
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {		// Thread safe
    sbuffer_node_t *dummy;
    pthread_rwlock_wrlock(&buffer->rw_lock);
    if (buffer == NULL){
	pthread_rwlock_unlock(&buffer->rw_lock);
	return SBUFFER_FAILURE;
    }
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL){
	pthread_rwlock_unlock(&buffer->rw_lock);
	return SBUFFER_FAILURE;
    }
    dummy->data = *data;
    dummy->next = NULL;
    dummy->readby1 = false;
    dummy->readby2 = false;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL)
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_rwlock_unlock(&buffer->rw_lock);
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data, int reader){		// Thread safe
    sbuffer_node_t *dummy;
    pthread_rwlock_rdlock(&buffer->rw_lock);
    if (buffer == NULL){
        pthread_rwlock_unlock(&buffer->rw_lock);
        return SBUFFER_FAILURE;
    }
    dummy = malloc(sizeof(sbuffer_node_t));
    if (buffer->head == NULL){
        pthread_rwlock_unlock(&buffer->rw_lock);
        return SBUFFER_NO_DATA;
    }

    dummy = buffer->head;
    if(reader == 1){
	if( (buffer->head->readby1) == true){
	    while( dummy->readby1 == true && dummy->next != NULL)	dummy = dummy->next;
	}
        if(dummy->readby1 == true && dummy->next == NULL){      // This means that reader 1 has read all values already
            data = NULL;
	    pthread_rwlock_unlock(&buffer->rw_lock);
            return SBUFFER_FINISHED;
        }
    }
    else if(reader == 2){
        if( (buffer->head->readby2) == true){
            while( dummy->readby2 == true && dummy->next != NULL)       dummy = dummy->next;
        }
	if(dummy->readby2 == true && dummy->next == NULL){	// This means that reader 2 has read all values already
	    data = NULL;
	    pthread_rwlock_unlock(&buffer->rw_lock);
	    return SBUFFER_FINISHED;
	}
    }
    pthread_rwlock_unlock(&buffer->rw_lock);
    pthread_rwlock_wrlock(&buffer->rw_lock);	// Change to a write lock
    *data = dummy->data;
    if( reader == 1)	dummy->readby1 = true;	// Now it has been read
    else if( reader == 2)        dummy->readby2 = true;   // Now it has been read
    if(dummy->readby1 && dummy->readby2){       // If value was already read by both readers, it can be removed
        sbuffer_remove(buffer);
    }
    pthread_rwlock_unlock(&buffer->rw_lock);
    return SBUFFER_SUCCESS;
}
