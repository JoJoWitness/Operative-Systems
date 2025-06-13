#include "thread.h"

void *cracker(void *arg);

DWORD WINAPI crackerWIN(LPVOID arg) {
	cracker(arg);
	return 0;
}

void createThread(int HILOS, char *possibleLetters, int lettersSize, sem_t *write_semaphore, FILE *output, void *(*cracker)(void *args)){

	DWORD threadsId[HILOS];
	HANDLE threads[HILOS];
	struct hilo_args args[HILOS];
	
	for (int i = 0; i < HILOS; i++) {
		args[i].start = (lettersSize / HILOS) * i;
		args[i].end = (i == HILOS - 1) ? lettersSize : args->start + (lettersSize / HILOS);
		args[i].possibleLetters = possibleLetters;
		args[i].semaphore = write_semaphore;
		args[i].output = output;

		threads[i] = CreateThread(
			NULL,       
			0,         
			crackerWIN,    
			(LPVOID)args, 
			0,         
			&threadsId[i]
		);

		if (threads[i] == NULL) {
			printf("Error al crear el hilo %d.\n", i);
			return;
		}
	}
	
	if(WaitForMultipleObjects(HILOS, threads, TRUE, INFINITE)==WAIT_FAILED){
		printf("Error al esperar a los hilos.\n");
		return;
	}

	for (int i = 0; i < HILOS; i++) {
			CloseHandle(threads[i]);
	}
	
}
