#include "queue.h"

struct Queue
{
    struct Node* p_head;
    struct Node* p_tail;
};

typedef struct Node
{
    struct Node* p_next;
    void* p_value;
} Node;

Queue* queue_create() {
    Queue* p_queue = (Queue*) malloc(sizeof(Queue));
    
    if (p_queue != NULL) {
        p_queue->p_head = NULL;
        p_queue->p_tail = NULL;
    }
    
    return p_queue;
};

void queue_destroy(Queue* p_queue) {
    if (p_queue == NULL) { return; }
    if (p_queue->p_head == NULL) { return; }

    Node* node = p_queue->p_head;
    Node* next = NULL;
    while (node != NULL)
    {
        next = node->p_next;
        free(node);
        node = next;
    }

    free(p_queue);
};

void queue_enqueue(Queue* p_queue, void* p_value) {
    if (p_queue == NULL) { return; }

    Node* p_node = (Node*) malloc(sizeof(Node));
    p_node->p_value = p_value;
    p_node->p_next = NULL;

    if (p_queue->p_tail == NULL) {
        p_queue->p_head = p_node;
    } else {
        p_queue->p_tail->p_next = p_node;
    }
    
    p_queue->p_tail = p_node;
};

void* queue_dequeue(Queue* p_queue) {
    if (p_queue == NULL) { return NULL; }
    
    if (p_queue->p_head == NULL) {
        return NULL;
    }

    Node* p_node = p_queue->p_head;
    void* p_value = p_node->p_value;

    p_queue->p_head = p_node->p_next;
    if (p_queue->p_head == NULL) {
        p_queue->p_tail = NULL;
    }

    free(p_node);

    return p_value;
}

void* queue_peek(Queue* p_queue) {
if (p_queue == NULL) { return NULL; }
    
    if (p_queue->p_head == NULL) {
        return NULL;
    }

    Node* p_node = p_queue->p_head;
    void* p_value = p_node->p_value;

    return p_value;
};
