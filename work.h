#ifndef WORK_H_
#define WORK_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


void* someHeavyWork(void* p_argument);

void* someMediumWork(void* p_argument);

void* someEasyWork(void* p_argument);

int* someCountingWork(int* p_maxCount);

int* someStringManipulatingWork(char* p_string);

double* someRandomCalculationWork(int* p_seed);

#endif