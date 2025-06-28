// C program to demonstrate use of fork() and pipe()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int main()
{
    const int SIZE = 4096;
    const char* sm_name = "shared_memory_string";
    char* ptr_sm;
    int shm_fd = shm_open(sm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

    sem_t semaphore;
    sem_init(&semaphore, 0, 1);
    ptr_sm = mmap(0, SIZE,  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // const char* message_0 = "Hello";
    // const char* message_1 = "World!";

    // char* write_ptr = ptr_sm;
    // sprintf(write_ptr, "%s", message_0);
    // write_ptr += strlen(message_0);
    // sprintf(write_ptr, "%s", message_1);
    // write_ptr += strlen(message_1);

    // printf("%s\n", ptr_sm);
    
    pid_t p1;
    pid_t p2;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

   p1 = fork();
   p2 = fork();

   if(p1 == 0 && p2 > 0) {
      char input_str[100];
      printf("Enter any character string");
      scanf("%s", input_str);

      close(fd[0]);
      write(fd[1], input_str, strlen(input_str) + 1);
      close(fd[1]);
      exit(0);
    } else if (p2 == 0 && p1 > 0) {
        char concat_str[100];
        close(fd[1]);
        read(fd[0], concat_str, sizeof(concat_str));
        printf("Received string: %s\n", concat_str);
        close(fd[0]); 
        exit(0);
    } else if (p1 > 0 && p2 > 0) {
        // Parent process
        wait(NULL); // Wait for child processes to finish
    }
 
    return 0;
}