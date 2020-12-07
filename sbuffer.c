/**
 * \author Vincent Surkijn
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "sbuffer.h"

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

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
};

int sbuffer_init(sbuffer_t **buffer) {		// Thread safe
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
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

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {		// Thread safe: only gets called from within Mutex(see sbuffer_read)
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    *data = buffer->head->data;
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
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    dummy->readby1 = false;
    dummy->readby2 = false;
    pthread_mutex_lock(&list_mutex);	// grab Mutex: while reading size of list it can't change
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL)
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_mutex_unlock(&list_mutex);	// unlock after operations based on size of list are done
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data, int reader){		// Thread safe
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;

    if(reader == 1){
	if( (buffer->head->readby1) == true){
	    while( dummy->readby1 == true && dummy->next != NULL)	dummy = dummy->next;
	}
    }
    else if(reader == 2){
        if( (buffer->head->readby2) == true){
            while( dummy->readby2 == true && dummy->next != NULL)       dummy = dummy->next;
        }
    }

    pthread_mutex_lock(&list_mutex);    // grab Mutex: while reading state of node it can't change
    if(dummy->readby1 && dummy->readby2){	// If value was already read by both readers, it can be removed
	sbuffer_remove(buffer,data);
    }
    else{			// Else it can only be read, not removed
	*data = buffer->head->data;
	if( reader == 1)	buffer->head->readby1 = true;	// Now it has been read
	else if( reader == 2)        buffer->head->readby2 = true;   // Now it has been read
    }
    pthread_mutex_unlock(&list_mutex);  // unlock after operations based on state of node are done
    return SBUFFER_SUCCESS;
}
