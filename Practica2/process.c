
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define CMD_LEN 128

void executeProcess() {
    char input[CMD_LEN];
    char *args[12];
    printf("\nIngrese el comando a ejecutar: ");

    if (!fgets(input, sizeof(input), stdin)) return;
    input[strcspn(input, "\n")] = 0; 
    
    int i = 0;
    char *token = strtok(input, " ");
    while (token && i < 11) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("\nError al ejecutar el comando");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        printf("Proceso lanzado en segundo plano, valor PID: %d\n", pid);
    } else {
        perror("\nError al crear el proceso");
    }
}

void listProcess() {
    struct dirent *de;
    char path[64], name[256];
    DIR *dr = opendir("/proc");

    if (dr == NULL) {
        perror("No se pudo abrir /proc");
        return;
    }

    printf("%-8s %-20s\n", "PID", "Nombre");
    while ((de = readdir(dr)) != NULL) {
        if (isdigit(de->d_name[0])) {      
            snprintf(path, sizeof(path), "/proc/%s/comm", de->d_name);
            FILE *f = fopen(path, "r");
            if (f) {
                if (fgets(name, sizeof(name), f)) {
                    name[strcspn(name, "\n")] = 0;
                    printf("%-8s %-20s\n", de->d_name, name);
                }
                fclose(f);
            }
        }
    }
    closedir(dr);
}

void killProcess() {
    char pidstr[16];
    listProcess();
    
    printf("\nIngrese el codigo PID del proceso a matar: ");
   
    if (!fgets(pidstr, sizeof(pidstr), stdin)) return;
    pid_t pid = (pid_t)atoi(pidstr);
    if (pid > 0) {
        if (kill(pid, SIGKILL) == 0) {
            printf("Proceso %d eliminado.\n", pid);
        } else {
            perror("\nNo se pudo eliminar el proceso");
        }
    } else {
        printf("PID inválido.\n");
    }
}

int main() {
    int opcion;
    char input[8];
    do {
        printf("\n=========================");
        printf("\nGestor de procesos\n");
        printf("1. Ejecutar nuevo proceso\n");
        printf("2. Listar procesos\n");
        printf("3. Matar proceso\n");
        printf("4. Salir\n");
        printf("=========================\n");
        printf("Seleccione una opción: ");
         
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) break;
        opcion = atoi(input);

        switch (opcion) {
            case 1: executeProcess(); 
              break;
            case 2: listProcess(); 
              break;
            case 3: killProcess(); 
              break;
            case 4: printf("Saliendo...\n"); 
              break;
            default: printf("Opción inválida.\n");
        }
    } while (opcion != 4);

    return 0;
}
