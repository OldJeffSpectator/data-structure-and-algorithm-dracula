#include <stdio.h>
#include <assert.h> 
#include "queue_location.h"
#include "places.h"
#include "map.h"
#include <stdlib.h>
#include <stdbool.h>

/////////////////////////////////////////
// queue_location functions

// queue_location functions

int queue_location_size(Queue_location q){
	int num = q->num;
   return num;

}


void queue_location_drop(Queue_location q){

	while(queue_location_size(q) > 0){
		
		queue_location_de(q);
		/*
		printf("%d\t",q->num);
		for(Node_location curr = q->head; curr!= NULL;curr = curr->next){
			printf("%s ",location_get_name(curr->item));
		}
		puts("\n");
		*/
		
	}
	free(q);
}
void queue_location_en(Queue_location q,location_t v){
   
	/*if( q->head != NULL){
		printf("before :%d head: %s->",q->num,location_get_name(q->head->item));
	for(Node_location curr =q->head; curr!= NULL; curr = curr->next){
		printf("|%s| ",location_get_name(curr->item));
	}
	printf("<-tail %s ",location_get_name(q->tail->item));
	puts("");
	}*/
	
    Node_location new = malloc(sizeof(struct node_location));
   	new->item = v;
   	new->next = NULL;
  
    // If queue is empty, then new node is front and rear both 
    if (q->tail == NULL) 
    { 
       q->head = q->tail = new; 
	   q->num++;
       return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->tail->next = new;
    q->tail= new; 
	q->num++;
   
   
   /* printf("after :%d head: %s->",q->num,location_get_name(q->head->item));
	for(Node_location curr =q->head; curr!= NULL; curr = curr->next){
		printf("|%s| ",location_get_name(curr->item));
	}
	printf("<-tail %s ",location_get_name(q->tail->item));
	puts("\n");*/

   
   
}
location_t queue_location_de(Queue_location q){

    // If queue is empty, return NULL. 
    if (q->head == NULL) {
		q->num = 0;
		return UNKNOWN_LOCATION; 
	}
       
  
    // Store previous front and move front one node ahead 
    Node_location tmp = q->head;
    q->head = q->head->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->head == NULL) 
       q->tail = NULL; 
	q->num--;
	location_t r = tmp->item;
	free(tmp);
    return r;
	/*
	assert(q->head != NULL);
   	Node_location tmp = q->head;
   	location_t v = tmp->item;
	if( q->num == 1){
		q->tail = NULL;
		q->head = NULL;
	}else{
		q->head = q->head->next;
	}
	
   
	
   q->num--;
   free(tmp);
   check(q);
   return v;
   */

}

Queue_location queue_location_new(void){
   Queue_location new = malloc(sizeof(struct queue_location));
   new->num = 0;
   new->head = NULL;
   new->tail = NULL;
   return new;
}
bool queue_location_in(Queue_location q, location_t n){
	for(Node_location curr = q->head; curr != NULL ; curr = curr->next){
		if(  curr == q->head  && queue_location_size(q)==6)continue; 
		if(curr->item == n){
			return true;
		}
	}
	return false;

}

/* Given a reference (pointer to pointer) to the head of a list 
   and a key, deletes the first occurrence of key in linked list */
void deleteNode(Node_location *head_ref, location_t key,int* count) 
{ 
    // Store head node 
    Node_location temp = *head_ref, prev = NULL; 
  
    // If head node itself holds the key to be deleted 
    if (temp != NULL && temp->item == key) 
    { 
        *head_ref = temp->next;   // Changed head 
		(*count)++;
        free(temp);               // free old head 
        return; 
    } 
  
    // Search for the key to be deleted, keep track of the 
    // previous node as we need to change 'prev->next' 
    while (temp != NULL && temp->item != key) 
    { 
        prev = temp; 
        temp = temp->next; 
    } 
  
    // If key was not present in linked list 
    if (temp == NULL) return; 
  
    // Unlink the node from linked list 
    prev->next = temp->next; 
	(*count)++;
    free(temp);  // Free memory 
} 
void up_date_tail(Queue_location q){
    q->num = 0;
	q->tail = q->head;
	for(Node_location curr = q->head; curr != NULL; curr = curr->next){
		q->num ++;
		q->tail = curr;
	}


}


