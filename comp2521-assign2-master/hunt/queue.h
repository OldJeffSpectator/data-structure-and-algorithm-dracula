
#include "places.h"
#include <stdbool.h>
typedef struct QueueRep *Queue; 


Queue newQueue (void); 
void dropQueue (Queue); 
void enQueue (Queue, location_t,int); 
void deQueue (Queue,location_t*,int*) ;
bool QueueisEmpty (Queue); 


