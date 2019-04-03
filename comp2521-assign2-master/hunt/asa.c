#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>

#define DRACULA 4
#define RECORD_POINT 30
#define NO_RESRICT -1

typedef struct node_dv* Node_dv;
typedef struct queue_dv* Queue_dv;
struct queue_dv{
   Node_dv head;  
   Node_dv tail;
	int max;
   int num;
};
struct node_dv{
   Node_dv next;
   size_t item;
};
bool queue_dv_in(Queue_dv q, size_t n);
Queue_dv queue_dv_new(int max);
size_t queue_dv_de(Queue_dv q);
void queue_dv_en(Queue_dv q,size_t v);
int queue_dv_size(Queue_dv q);
void queue_dv_drop(Queue_dv q);

int main(void){

    Queue_dv q = queue_dv_new(6);
    
   queue_dv_en(q,1);
   
    queue_dv_de(q);
    queue_dv_en(q,1);
    queue_dv_de(q);
    queue_dv_en(q,1);
    queue_dv_de(q);
   assert(q->num==0);

    queue_dv_drop(q);
    


}

int queue_dv_size(Queue_dv q){
   return q->num;

}


void queue_dv_drop(Queue_dv q){

	
	
	
	while(queue_dv_size(q) > 0){
		
		queue_dv_de(q);
		
		
		
	}
	free(q);
}
void queue_dv_en(Queue_dv q,size_t v){
   
  Node_dv new = malloc(sizeof(struct node_dv));
   	new->item = v;
   	new->next = NULL;
  
    // If queue is empty, then new node is front and rear both 
    if (q->tail == NULL) 
    { 
       q->head = new;
	   q->tail = new; 
	   q->num++;
       return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->tail->next = new;
    q->tail= new; 
	   q->num++;
   
}
size_t queue_dv_de(Queue_dv q){
	 // If queue is empty, return NULL. 
    if (q->head == NULL) {
		q->num = 0;
		return -1; 
	}
       
  
    // Store previous front and move front one node ahead 
    Node_dv tmp = q->head;
    q->head = q->head->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->head == NULL) q->tail = NULL;

	q->num--;
	size_t r = tmp->item;
	free(tmp);
    return r;

}

Queue_dv queue_dv_new(int max){
   Queue_dv new = malloc(sizeof(struct queue_dv));
   new->num = 0;
	new->max = max;
   new->head = NULL;
   new->tail = NULL;
   return new;
}
bool queue_dv_in(Queue_dv q, size_t n){
	for(Node_dv curr = q->head; curr != NULL ; curr = curr->next){
		if(  curr == q->head  && queue_dv_size(q)==6)continue; 
		if(curr->item == n){
			return true;
		}
	}
	return false;

}