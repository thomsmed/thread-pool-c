#include "work.h"


void* someHeavyWork(void* argument) {
    int* id = (int*) argument;
    printf("Thread %i: Doing some heavy work...\n", *id);
    sleep(4);
    printf("Thread %i: Heavy work done!\n", *id);
    return NULL;
};

void* someMediumWork(void* argument) {
    int* id = (int*) argument;
    printf("Thread %i: Doing some medium work.\n", *id);
    sleep(2);
    printf("Thread %i: Medium work done!\n", *id);
    return NULL;
};

void* someEasyWork(void* argument) {
    int* id = (int*) argument;
    printf("Thread %i: Doing some easy work!\n", *id);
    printf("Thread %i: Easy work done!\n", *id);
    return NULL;
};

int* someCountingWork(int* p_maxCount) {
    int maxCount = *p_maxCount;
    int result = 0;
    printf("Doing some counting work...\n");
    for (int i = 0; i < maxCount; i++) {
        result += i;
    }
    int* p_result = malloc(sizeof(int));
    *p_result = result;
    printf("Counting work done!\n");
    return p_result;
};

int* someStringManipulatingWork(char* p_string) {
    printf("Doing some string manipulation work...\n");
    int l = strlen(p_string);
    int ll = l / 2;
    for (int i = 0; i < ll; i++) {
        char c = *(p_string + i);
        *(p_string + i) = *(p_string + (l - 1 - i));
        *(p_string + (l - 1 - i)) = c;
    }
    int* result = malloc(sizeof(int));
    *result = l;
    printf("String manipulation work done!\n");
    return result;
};

double* someRandomCalculationWork(int* p_seed) {
    printf("Doing some random calculation work...\n");
    srand(*p_seed);
    double* d = malloc(sizeof(int));
    *d = rand();
    printf("Random calculation work doen!\n");
    return d;
};