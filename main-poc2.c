#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "queue.h"
#include "work.h"


#define POOL_SIZE 5

typedef struct Pool Pool;
typedef struct PoolThread PoolThread;
typedef struct Task Task;

struct Pool
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    PoolThread* p_poolThreads;
    PoolThread* p_head;
    PoolThread* p_tail;
    int threadCount;
    Queue* p_taskQueue;
};

struct PoolThread
{
    int id;
    pthread_t p_thread;
    Pool* p_pool;
    PoolThread* next;
};

struct Task
{
    int id;
    void* (*workRoutine)(void*);
    void* p_workArgument;
    void* p_workResult;
};

void* taskRoutine(void* p_argument) {
    PoolThread* p_self = (PoolThread*) p_argument;
    pthread_mutex_t* p_mutex = &(p_self->p_pool->mutex);
    pthread_cond_t* p_condition = &(p_self->p_pool->condition);
    Queue* p_taskQueue = p_self->p_pool->p_taskQueue;

    while (1)
    {
        pthread_mutex_lock(p_mutex);
        Task* p_task = (Task*) queue_dequeue(p_taskQueue);
        while (p_task == NULL)
        {
            pthread_cond_wait(p_condition, p_mutex);
            p_task = (Task*) queue_dequeue(p_taskQueue);
        }
        pthread_mutex_unlock(p_mutex);

        void* (*workRoutine)(void*) = p_task->workRoutine;
        if (workRoutine != NULL) {
            p_task->p_workResult = workRoutine((void*) &(p_self->id));
            // p_task->p_workResult = workRoutine(p_task->p_workArgument);
        }
    }
    
    pthread_exit(NULL);
    return NULL;
};

int __main() {

    Pool* p_pool = (Pool*) malloc(sizeof(Pool));
    p_pool->p_poolThreads = (PoolThread*) malloc(sizeof(PoolThread) * POOL_SIZE);
    p_pool->p_taskQueue = queue_create();
    p_pool->threadCount = 0;

    pthread_cond_init(&(p_pool->condition), NULL);
    pthread_mutex_init(&(p_pool->mutex), NULL);

    int i = 0;
    for (; i < POOL_SIZE; i++) {
        if (i > 0) {
            (p_pool->p_poolThreads + i)->next = p_pool->p_poolThreads + i - 1;
        }
        (p_pool->p_poolThreads + i)->id = i;
        (p_pool->p_poolThreads + i)->p_pool = p_pool;
        pthread_create(&((p_pool->p_poolThreads + i)->p_thread), NULL, taskRoutine, (void*) (p_pool->p_poolThreads + i));
    }
    p_pool->threadCount = i;
    p_pool->p_head = p_pool->p_poolThreads;
    p_pool->p_tail = p_pool->p_poolThreads + i - 1;

    // Queue some work:
    Task* p_task;
    for (int i = 10; i > 0; i--) {
        p_task = (Task*) malloc(sizeof(Task));
        p_task->id = i * 100;
        p_task->workRoutine = someHeavyWork;
        pthread_mutex_lock(&(p_pool->mutex));
        queue_enqueue(p_pool->p_taskQueue, p_task);
        pthread_mutex_unlock(&(p_pool->mutex));
        pthread_cond_signal(&(p_pool->condition));
        
        p_task = (Task*) malloc(sizeof(Task));
        p_task->id = i * 200;
        p_task->workRoutine = someMediumWork;
        pthread_mutex_lock(&(p_pool->mutex));
        queue_enqueue(p_pool->p_taskQueue, p_task);
        pthread_mutex_unlock(&(p_pool->mutex));
        pthread_cond_signal(&(p_pool->condition));
        
        p_task = (Task*) malloc(sizeof(Task));
        p_task->id = i * 300;
        p_task->workRoutine = someEasyWork;
        pthread_mutex_lock(&(p_pool->mutex));
        queue_enqueue(p_pool->p_taskQueue, p_task);
        pthread_mutex_unlock(&(p_pool->mutex));
        pthread_cond_signal(&(p_pool->condition));
    }

    printf("Main: Done queueing work.\n");

    sleep(120);

    return 0;
};
