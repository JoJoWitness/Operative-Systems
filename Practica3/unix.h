#include "thread.h"

void createThread(int HILOS, char *possibleLetters, int lettersSize, sem_t *write_semaphore, FILE *output, void *(*cracker)(void *args)){
  
  pthread_t threads[HILOS];
  struct hilo_args args[HILOS];
  for (int i = 0; i < HILOS; i++) {
      args[i].start = (lettersSize / HILOS) * i;
      args[i].end = (i == HILOS - 1) ? lettersSize : args[i].start + (lettersSize / HILOS);
      args[i].possibleLetters = possibleLetters;
      args[i].semaphore = write_semaphore;
      args[i].output = output;
      if(pthread_create(&threads[i], NULL, cracker, &args[i])){
        printf("Error creating thread %d\n", i);
        abort();
      };
  }
  for (int i = 0; i < HILOS; i++) {
      if(pthread_join(threads[i], NULL)){
        printf("Error joining thread %d\n", i);
        abort();
      };    
  };
};
