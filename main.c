#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"
#include "work.h"


int main() {

   struct ThreadPool* p_threadPool = thread_pool_create(5);

   ThreadPoolTask* p_task;
   int intArg1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
   int intArg2[] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
   char textArg[10][30] = {
      "Hello World",
      "Someone",
      "This is nice",
      "ok",
      "Thank you",
      "So this is how it works!",
      "Not again...",
      "Ok I'll wait",
      "Juice",
      "Orange"
   };
   for (int i = 0; i < 10; i++)
   {
      p_task = thread_pool_task_create((void* (*)(void*)) someCountingWork, &intArg1[i], NULL);
      thread_pool_enqueue(p_threadPool, p_task);
      p_task = thread_pool_task_create((void* (*)(void*)) someStringManipulatingWork, &textArg[i], NULL);
      thread_pool_enqueue(p_threadPool, p_task);
      p_task = thread_pool_task_create((void* (*)(void*)) someRandomCalculationWork, &intArg2[i], NULL);
      thread_pool_enqueue(p_threadPool, p_task);   
   }
   
   thread_pool_destroy(p_threadPool, WAIT);

   for (int i = 0; i < 10; i++) {
      printf("%s\n", textArg[i]);
   }
   
   return 0;
};
