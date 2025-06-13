#include <stdio.h> 
#include <ctype.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h>
#include <unistd.h>

void getPIDMaps(){
    struct dirent *file; 
    DIR *dr = opendir("/proc"); 

    FILE *out = fopen("pid_info.txt", "w");
    if (!out) {
        perror("No se pudo crear el archivo de salida");
        return 1;
    }

    if (dr == NULL){ 
        printf("No se pudo abrir /proc" ); 
        return 1;
    } 
    printf("\nLista de archivos:\n");
    while ((file = readdir(dr)) != NULL) {
      if(isdigit(file->d_name[0])) {
        printf("%s\n", file->d_name);
        char path[64];
        snprintf(path, sizeof(path), "/proc/%s/maps", file->d_name);
        FILE *f = fopen(path, "r");
        if (f) {
            char header[64];
            char line[256];
            snprintf(header, sizeof(header), "\nLibrerias del proceso %s:\n", file->d_name);
            fputs(header, out);
            char *libraryToken;
            while (fgets(line, sizeof(line), f)) {
              if (strstr(line, ".so") ){
                char *tokenizedLine = strtok(line, " ");
                while (tokenizedLine != NULL) {
                  libraryToken = tokenizedLine;
                  tokenizedLine = strtok(NULL, " ");
                }

                fputs(libraryToken, out);
              }
            }
            fclose(f);
        }
    };
  };
  closedir(dr);   
};

int main(void) {
  
    getPIDMaps();
    return 0;
};

