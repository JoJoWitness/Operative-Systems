#include <stdio.h> 
#include <ctype.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>

void getPIDMaps(char* pidID){
    FILE *file = fopen("pid_info.txt", "w");
    if (!file) {
        perror("No se pudo crear el archivo de salida");
        return;
    }
    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/maps", pidID);
    FILE *f = fopen(path, "r");
    if (f) {
        char header[64];
        char line[256];
        snprintf(header, sizeof(header), "\nLibrerias del proceso %s:\n", pidID);
        fputs(header, file);
        char *libraryToken;
        while (fgets(line, sizeof(line), f)) {
          if (strstr(line, ".so") ){
            char *tokenizedLine = strtok(line, " ");
            while (tokenizedLine != NULL) {
              libraryToken = tokenizedLine;
              tokenizedLine = strtok(NULL, " ");
            }

            fputs(libraryToken,file);
          }
        }
        fclose(f);
        }
      else{
        printf("El proceso no se encontro");
      }
  fclose(file); 
};

void getPIDstatus(char* pidID){
 
  char path[64];
  snprintf(path, sizeof(path), "/proc/%s/status", pidID);
  printf(path);

  FILE *f = fopen(path, "r");
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    printf(line);
  };
  fclose(f);
};

int main(void) {
  
    getPIDMaps("555");
    getPIDstatus("555");
    return 0;
};

