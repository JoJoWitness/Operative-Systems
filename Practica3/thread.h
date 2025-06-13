#ifndef THREAD_H
#define THREAD_H
#include <semaphore.h>
#include <stdio.h>

struct hilo_args {
    int start;
    int end;
    char *possibleLetters;
    sem_t *semaphore;
    FILE *output;
};
#endif