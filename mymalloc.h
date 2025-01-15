#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stdio.h>
#include <stddef.h> // for size_t as original malloc uses the same

#define MEMORY_SIZE 25000

void* MyMalloc(size_t size);
void MyFree(void* ptr);
void displayMemory();

#endif