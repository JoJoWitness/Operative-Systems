#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>


#define SIZE 4096

typedef struct {
    sem_t semaphore1;
    sem_t semaphore2;
    char buffer[SIZE];
} shared_data;

void pipeFunction(){
   const char* sm_name = "shared_memory_string";
    char* ptr_sm;
    int shm_fd = shm_open(sm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_data));
    shared_data* sm = mmap(0, sizeof(shared_data),  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_init(&sm->semaphore1, 1, 0);
    sem_init(&sm->semaphore2, 1, 0);

    pid_t p1;
    pid_t p2;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    p1 = fork();
    if(p1 == 0 ) {
        
        char input_str[100];
        printf("Enter any character string:\n");
        scanf("%s", input_str);
        close(fd[0]);
        write(fd[1], input_str, strlen(input_str) + 1);
        close(fd[1]);
        sem_post(&sm->semaphore1);
        sem_wait(&sm->semaphore2);
        printf("Modified str: %s\n", sm->buffer);
        exit(0);
    }
   p2 = fork();
   if (p2 == 0) {
        sem_wait(&sm->semaphore1);
        char str_toUpper[100];
        close(fd[1]);
        read(fd[0],str_toUpper, sizeof(str_toUpper));

        for(int i=0;str_toUpper[i];i++){
            str_toUpper[i] =toupper(str_toUpper[i]);
        }
        char* write_ptr = sm->buffer;
        sprintf(write_ptr, "%s", str_toUpper);    
        close(fd[0]); 
        sem_post(&sm->semaphore2);
        exit(0);
    } 

    close(fd[0]);
    close(fd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    printf("Final string in shared memory: %s\n", sm->buffer);
    sem_destroy(&sm->semaphore1);
    sem_destroy(&sm->semaphore2);
    munmap(sm, sizeof(shared_data));
    shm_unlink(sm_name);
    close(shm_fd);
}