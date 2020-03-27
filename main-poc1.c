#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"
#include "work.h"

#define POOL_SIZE 5

pthread_mutex_t mutex;
pthread_cond_t condition;
pthread_t* p_threadPool;
Queue* p_workQueue;
int* p_ids;

void* workRoutine(void* p_argument) {
    int* id = (int*) p_argument;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        void* (*work)(void*) = queue_dequeue(p_workQueue);
        while (work == NULL)
        {
            pthread_cond_wait(&condition, &mutex);
            work = queue_dequeue(p_workQueue);
        }
        pthread_mutex_unlock(&mutex);

        void* result = work((void*) id);
    }
    
    pthread_exit(NULL);
    return NULL;
};

int _main() {

    pthread_cond_init(&condition, NULL);
    pthread_mutex_init(&mutex, NULL);

    p_ids = malloc(sizeof(p_ids) * POOL_SIZE);
    p_threadPool = malloc(sizeof(p_threadPool) * POOL_SIZE);
    p_workQueue = queue_create();

    for (int i = 0; i < POOL_SIZE; i++) {
        *(p_ids + i) = i;
        pthread_create((p_threadPool + i), NULL, workRoutine, (p_ids + i));
    }

    // Queue some work:
    for (int i = 10; i > 0; i--) {
        pthread_mutex_lock(&mutex);
        queue_enqueue(p_workQueue, someHeavyWork);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condition);
        
        pthread_mutex_lock(&mutex);
        queue_enqueue(p_workQueue, someMediumWork);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condition);
        
        pthread_mutex_lock(&mutex);
        queue_enqueue(p_workQueue, someEasyWork);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condition);
    }

    printf("Main: Done queueing work.\n");

    sleep(120);

    return 0;
};