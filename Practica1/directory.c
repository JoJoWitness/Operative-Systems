#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h> 

void getDirectory() {
     char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Nombre de la ruta actual: %s\n", cwd);
}

void getFiles(){
    struct dirent *de; 
    int numDirectories;
    DIR *dr = opendir("."); 

    if (dr == NULL)
    { 
        printf("No se pudo abrir el directorio" ); 
    } 
    printf("\nLista de archivos:\n");
    while ((de = readdir(dr)) != NULL) {
            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
            numDirectories++;
            printf("%s\n", de->d_name);
            }
    }
    printf("\nCantidad de archivos: %d\n", numDirectories);
    closedir(dr);     
}

int main(void) {
  
    getDirectory();
    getFiles();
    return 0;
}


629
#include <stdio.h> 
#include <ctype.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h>
#include <unistd.h>

void getPIDMaps(char* pidID){
    FILE *file = fopen("pid_info.txt", "w");
    if (!file) {
        perror("No se pudo crear el archivo de salida");
        return;
    }
    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/maps", pidID);
    printf(path);
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

int main(void) {
  
    getPIDMaps("1247");
    return 0;
};

