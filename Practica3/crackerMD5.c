#include <stdio.h>
#include <openssl/md5.h>
#include <string.h>
#include <semaphore.h>
#include "thread.h"
#ifdef __linux__
	#include <pthread.h>
	#include <unistd.h>
	#include <stdlib.h>
  #include "unix.h"
#elif _WIN32
	#include <conio.h>
	#include <windows.h>
	#include "windows.h"
#endif

#define MAX_USERS 50
#define MAX_LINE 128

struct user {
    char username[64];
    char password[33];
    char crackedPassword[5];
};



struct user users[MAX_USERS];
int user_count = 0;

void bin_to_hex(const unsigned char *md5,char *hex) {
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(hex + i*2, "%02x", md5[i]);
    hex[32] = '\0';
}

void *cracker(void *arg) {
    struct hilo_args *args = (struct hilo_args *)arg; 
    char guess[5];
    int letters = strlen(args->possibleLetters);
    
    for (int len = 1; len <= 4; len++) {
      for (int j = args->start; j < args->end; j++)
      for (int k = 0; k < letters; k++)
      for (int l = 0; l < letters; l++)
      for (int m = 0; m < letters; m++) {
        if (len == 1) {
                guess[0] = args->possibleLetters[j];
                guess[1] = '\0';
            } else if (len == 2) {
                guess[0] = args->possibleLetters[j];
                guess[1] = args->possibleLetters[k];
                guess[2] = '\0';
            } else if (len == 3) {
                guess[0] = args->possibleLetters[j];
                guess[1] = args->possibleLetters[k];
                guess[2] = args->possibleLetters[l];
                guess[3] = '\0';
            } else if (len == 4) {
                guess[0] = args->possibleLetters[j];
                guess[1] = args->possibleLetters[k];
                guess[2] = args->possibleLetters[l]; 
                guess[3] = args->possibleLetters[m];
                guess[4] = '\0';
            }
        
        unsigned char result[MD5_DIGEST_LENGTH];
        char hex[33];
        MD5((unsigned char*)guess, strlen(guess),result);
        bin_to_hex(result, hex);
        
        for(int n = 0; n < user_count; n++){
          if (strncmp(hex, users[n].password, 32) == 0 && users[n].crackedPassword[0] == '\0') {
              strcpy(users[n].crackedPassword,guess);
              sem_wait(args->semaphore);
              fprintf(args->output, "%s\t%s\n", users[n].username, users[n].crackedPassword);
              sem_post(args->semaphore);
              break;
              }
        }
        if (len == 1) break;
        if (len == 2 && l > 0) break;
        if (len == 3 && m > 0) break;
      }
    }
    return NULL;
}
int main(int argc, char* argv[]) {
  int HILOS = atoi(argv[2]);

  if (HILOS < 1 || HILOS > 6) {
      printf("Number of threads must be between 1 and 6.\n");
      return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
      printf("Error opening file\n");
      return 1;
  }
  char line[MAX_LINE];
    while (fgets(line, sizeof(line), f) && user_count < MAX_USERS) { 
      char* username =strtok(line, ":");
      char* password =strtok(NULL, ":");
      if (username && password) {
        strcpy(users[user_count].username, username);
        strcpy(users[user_count].password, password );
        user_count++;
      }
    }
  fclose(f);
  FILE *output = fopen("resultados.txt", "w");
  
  sem_t write_semaphore;
  sem_init(&write_semaphore, 0, 1);
  char possibleletters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int lettersSize = strlen(possibleletters);
  char guess[5];
  createThread(HILOS, possibleletters, lettersSize, &write_semaphore, output, cracker);
  sem_destroy(&write_semaphore);
  fclose(output);

  return 0;

}

 

