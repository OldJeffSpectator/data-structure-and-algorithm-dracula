#include "places.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct node_location* Node_location;
typedef struct queue_location* Queue_location;
struct queue_location{
   Node_location head;  
   Node_location tail;
   int num;
};
struct node_location{
   Node_location next;
   location_t item;
};
bool queue_location_in(Queue_location q, location_t n);
Queue_location queue_location_new(void);
location_t queue_location_de(Queue_location q);
void queue_location_en(Queue_location q,location_t v);
int queue_location_size(Queue_location q);
void queue_location_drop(Queue_location q);
void deleteNode(Node_location *head_ref, location_t key,int* count) ;
void up_date_tail(Queue_location q);
