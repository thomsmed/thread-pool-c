#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <pthread.h>
#include <errno.h>
#include "queue.h"

typedef enum {
    WAIT, // Wait for all tasks to be done, then stop gracefully
    STOP, // Wait for current running tasks to finish, then stop gracefully
    FORCE_STOP // Do not wait for running tasks, and force full stop
} ThreadPoolDestroyFlag;

typedef struct ThreadPool ThreadPool;
typedef struct ThreadPoolTask ThreadPoolTask;

ThreadPoolTask* thread_pool_task_create(void* (*work_routine)(void*), void* argument, void** result);
ThreadPool* thread_pool_create(int size);
void thread_pool_destroy(ThreadPool* p_threadPool, char flags);
void thread_pool_enqueue(ThreadPool* p_threadPool, ThreadPoolTask* p_task);

#endif