#include <stdio.h>
#include <assert.h> 
#include "queue.h"
#include "places.h"
#include "map.h"
#include <stdlib.h>
#include <stdbool.h>
typedef struct Node Node; 


typedef struct QueueRep QueueRep; 

struct Node {
    location_t v; 
    int rail_index; 
    struct Node *next; 
};


struct QueueRep {
     struct Node *head;
     struct Node *tail; 
};


Queue newQueue (void) 
{
    Queue new = malloc(sizeof(*new)); 
    assert (new != NULL); 
    new->head = NULL; 
    new->tail = NULL; 
    return new; 

}


void dropQueue (Queue Q) 
{
    Node *curr, *next; 
    assert (Q != NULL); 

    // free node list
    curr = Q->head; 
    while (curr != NULL) {
        next = curr->next; 
        free (curr); 
        curr = next; 
    }

    // free the queue itself
    free (Q); 

}


void enQueue (Queue Q, location_t place, int rail_ind)  
{
    assert (Q != NULL); 
    Node *new = malloc(sizeof(*new)); 
    assert (new != NULL); 
    new->v = place; 
    new->rail_index = rail_ind;
    new->next = NULL; 
    if (Q->head == NULL) 
        Q->head = new; 
    if (Q->tail != NULL) 
        Q->tail->next = new; 
    Q->tail = new; 
}



void deQueue (Queue Q,location_t* l,int* i) 
{
    assert (Q != NULL); 
    assert (Q->head != NULL); 
    Node *old = Q->head; 
    *i = old->rail_index;
    *l = old->v;
    Q->head = old->next; 
    if (Q->head == NULL)
        Q->tail = NULL; 
    free(old); 
}

bool QueueisEmpty (Queue Q)
{
    return (Q->head == NULL); 
}




