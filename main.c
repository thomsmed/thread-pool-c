#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"
#include "work.h"


int main() {

   struct ThreadPool* p_threadPool = thread_pool_create(5);

   int intArg1[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
   int intArg2[] = { 110, 120, 130, 140, 150, 160, 170, 180, 190, 200 };
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

   int* p_intResults1[10];
   int* p_intResults2[10];
   double* p_doubleResults[10];

   ThreadPoolTask* p_task;
   for (int i = 0; i < 10; i++)
   {
      p_task = thread_pool_task_create((void* (*)(void*)) someCountingWork, &intArg1[i], (void**) &p_intResults1[i]);
      thread_pool_enqueue(p_threadPool, p_task);
      p_task = thread_pool_task_create((void* (*)(void*)) someStringManipulatingWork, &textArg[i], (void**) &p_intResults2[i]);
      thread_pool_enqueue(p_threadPool, p_task);
      p_task = thread_pool_task_create((void* (*)(void*)) someRandomCalculationWork, &intArg2[i], (void**) &p_doubleResults[i]);
      thread_pool_enqueue(p_threadPool, p_task);   
   }
   
   printf("Done queueing work! Let's wait for it to complete...\n");
   thread_pool_destroy(p_threadPool, WAIT);

   printf("\nResults from CountingWork: ");
   for (int i = 0; i < 10; i++) {
      printf("%i, ", *(p_intResults1[i]));
   }

   printf("\nResults from StringManipulationWork: ");
   for (int i = 0; i < 10; i++) {
      printf("\n->(length: %i): %s", *(p_intResults2[i]), textArg[i]);
   }

   printf("\nResults from RandomCalculationWork: ");
   for (int i = 0; i < 10; i++) {
      printf("%f, ", *(p_doubleResults[i]));
   }
   
   return 0;
};
