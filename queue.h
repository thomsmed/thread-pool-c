#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>


typedef struct Queue Queue;

Queue* queue_create();
void queue_destroy(Queue* p_queue);
void queue_enqueue(Queue* p_queue, void* p_value);
void* queue_dequeue(Queue* p_queue);
void* queue_peek(Queue* p_queue);

#endif