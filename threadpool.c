#include "threadpool.h"


typedef struct ThreadPoolThread
{
    int id;
    ThreadPool* p_pool;
    pthread_t thread;
} ThreadPoolThread;

struct ThreadPoolTask
{
    int id;
    void* argument;
    void* (*work_routine)(void*);
    void** result;
};

struct ThreadPool
{
    pthread_mutex_t taskQueueMutex;
    pthread_mutex_t completedTaskQueueMutex;
    pthread_cond_t taskQueueCond;
    pthread_cond_t completedTaskQueueCond;
    ThreadPoolThread* p_threadPoolThreads;
    int threadPoolThreadsCount;
    Queue* p_taskQueue;
    Queue* p_completedTaskQueue;
};

void thread_cleanup_routine(void* p_argument) {
    ThreadPoolThread* p_self = (ThreadPoolThread*) p_argument;
    pthread_mutex_t* p_taskQueueMutex = &(p_self->p_pool->taskQueueMutex);
    int code = pthread_mutex_unlock(p_taskQueueMutex);
};

void* thread_start_routine(void* p_argument) {
    ThreadPoolThread* p_self = (ThreadPoolThread*) p_argument;

    pthread_mutex_t* p_taskQueueMutex = &(p_self->p_pool->taskQueueMutex);
    pthread_cond_t* p_taskQueueCond = &(p_self->p_pool->taskQueueCond);
    Queue* p_taskQueue = p_self->p_pool->p_taskQueue;

    pthread_mutex_t* p_completedTaskQueueMutex = &(p_self->p_pool->completedTaskQueueMutex);
    pthread_cond_t* p_completedTaskQueueCond = &(p_self->p_pool->completedTaskQueueCond);
    Queue* p_completedTaskQueue = p_self->p_pool->p_completedTaskQueue;

    int oldState, oldType;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldType);

    pthread_cleanup_push(thread_cleanup_routine, p_argument);
    
    while (1)
    {
        int code = pthread_mutex_lock(p_taskQueueMutex);
        
        ThreadPoolTask* p_task = queue_dequeue(p_taskQueue);
        while (p_task == NULL) {
            // Signal that the queue is empty
            code = pthread_cond_signal(p_completedTaskQueueCond);
            // No new work - go to sleep and wait for signal
            code = pthread_cond_wait(p_taskQueueCond, p_taskQueueMutex);
            // Signal arised - get the work
            p_task = queue_dequeue(p_taskQueue);
        }

        code = pthread_mutex_unlock(p_taskQueueMutex);

        // Do the work
        void* result = p_task->work_routine(p_task->argument);
        if ((p_task->result != NULL)) {
            *(p_task->result) = result;
        }
        
        code = pthread_mutex_lock(p_completedTaskQueueMutex);
        queue_enqueue(p_completedTaskQueue, p_task);
        code = pthread_mutex_unlock(p_completedTaskQueueMutex);
    }

    pthread_cleanup_pop(0); // Do not execute, just pop.

    pthread_exit(NULL);
    return NULL;
};

ThreadPoolTask* thread_pool_task_create(void* (*work_routine)(void*), void* argument, void** result)
{
    ThreadPoolTask* p_threadPoolTask = malloc(sizeof(ThreadPoolTask));

    p_threadPoolTask->work_routine = work_routine;
    p_threadPoolTask->argument = argument;
    p_threadPoolTask->result = result;

    return p_threadPoolTask;
};

ThreadPool* thread_pool_create(int size)
{
    ThreadPool* p_threadPool = malloc(sizeof(ThreadPool));
    if (p_threadPool == NULL) { return NULL; }
    p_threadPool->threadPoolThreadsCount = 0;

    int code = pthread_cond_init((&p_threadPool->taskQueueCond), NULL);
    if (code != 0) { return NULL; }

    code = pthread_cond_init((&p_threadPool->completedTaskQueueCond), NULL);
    if (code != 0) { return NULL; }
    
    code = pthread_mutex_init((&p_threadPool->taskQueueMutex), NULL);
    if (code != 0) { return NULL; }

    code = pthread_mutex_init((&p_threadPool->completedTaskQueueMutex), NULL);
    if (code != 0) { return NULL; }

    p_threadPool->p_taskQueue = queue_create();
    if (p_threadPool->p_taskQueue == NULL) { return NULL; }

    p_threadPool->p_completedTaskQueue = queue_create();
    if (p_threadPool->p_completedTaskQueue == NULL) { return NULL; }
    
    p_threadPool->p_threadPoolThreads = malloc(sizeof(ThreadPoolThread) * size);
    
    for (int i = 0; i < size; i++) {
        p_threadPool->threadPoolThreadsCount++;
        (p_threadPool->p_threadPoolThreads + i)->id = i;
        (p_threadPool->p_threadPoolThreads + i)->p_pool = p_threadPool;
        code = pthread_create(&((p_threadPool->p_threadPoolThreads + i)->thread), NULL, thread_start_routine, (void*) (p_threadPool->p_threadPoolThreads + i));
    }

    return p_threadPool; 
};

void thread_pool_destroy(ThreadPool* p_threadPool, char flags)
{
    if (p_threadPool == NULL) { return; }

    int code;
    int count = p_threadPool->threadPoolThreadsCount;
    ThreadPoolThread* p_threadPoolThread = p_threadPool->p_threadPoolThreads;

    if (flags == WAIT) {
        // Wait for jobs to finish
        code = pthread_mutex_lock(&(p_threadPool->taskQueueMutex));
        ThreadPoolTask* p_task = queue_peek(p_threadPool->p_taskQueue);
        while (p_task != NULL)
        {
            code = pthread_cond_wait(&(p_threadPool->completedTaskQueueCond), &(p_threadPool->taskQueueMutex));
            p_task = queue_peek(p_threadPool->p_taskQueue);
        }
        code = pthread_mutex_unlock(&(p_threadPool->taskQueueMutex));
    }

    for (int i = 0; i < p_threadPool->threadPoolThreadsCount; i++)
    {
        if (flags & FORCE_STOP) {
            // Don't wait for threads to finish
            code = pthread_detach((p_threadPoolThread + i)->thread);
            code = pthread_cancel((p_threadPoolThread + i)->thread);
        } else {
            // Wait for threads to finish curent jobs
            code = pthread_cancel((p_threadPoolThread + i)->thread);
            code = pthread_join((p_threadPoolThread + i)->thread, NULL);
        }
    }

    ThreadPoolTask* p_task = queue_dequeue(p_threadPool->p_completedTaskQueue);
    while (p_task != NULL)
    {
        free(p_task);
        p_task = queue_dequeue(p_threadPool->p_completedTaskQueue);
    }
    
    free(p_threadPoolThread);

    code = pthread_mutex_destroy(&(p_threadPool->taskQueueMutex));
    code = pthread_mutex_destroy(&(p_threadPool->completedTaskQueueMutex));

    code = pthread_cond_destroy(&(p_threadPool->taskQueueCond));

    queue_destroy(p_threadPool->p_taskQueue);
    queue_destroy(p_threadPool->p_completedTaskQueue);

    free(p_threadPool);
};

void thread_pool_enqueue(ThreadPool* p_threadPool, ThreadPoolTask* p_task) {
    if (p_threadPool == NULL) { return; }
    
    int code = pthread_mutex_lock(&(p_threadPool->taskQueueMutex));

    queue_enqueue(p_threadPool->p_taskQueue, p_task);

    code = pthread_mutex_unlock(&(p_threadPool->taskQueueMutex));

    code = pthread_cond_signal(&(p_threadPool->taskQueueCond));
};
