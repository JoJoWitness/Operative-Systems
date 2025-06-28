#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>


typedef struct{
    unsigned long total_memory;
    unsigned long free_memory;
}MemoryConsumeStats;

MemoryConsumeStats memory_consume_info(struct sysinfo *info){
  MemoryConsumeStats stats;
  stats.total_memory = info->totalram * info->mem_unit;
  stats.free_memory = info->freeram * info->mem_unit;
  return stats;
}

unsigned long get_process_memory(pid_t pid) {
  char path[256];
  sprintf(path, "/proc/%d/status", pid);

  FILE *file = fopen(path, "r");
  if (!file) return 0;

  unsigned long memory_kb = 0;
  char line[256];

  while (fgets(line, sizeof(line), file)) {
    if (strstr(line, "VmRSS")) { 
      sscanf(line, "VmRSS: %lu kB", &memory_kb);
      break;
    }
  }
    fclose(file);
    return memory_kb;
}


int process_with_more_memory(FILE *file) {
  char line[256];
  DIR *proc_dir = opendir("/proc");
  if (!proc_dir) {
      perror("Error al abrir /proc");
      return -1;
  }

  pid_t max_pid = 0;
  unsigned long max_memory = 0;
  struct dirent *entry;

  while ((entry = readdir(proc_dir)) != NULL) {
    if (isdigit(entry->d_name[0])) { 
      char *endptr;
      pid_t pid = strtol(entry->d_name, &endptr, 10);
      if (*endptr != '\0') continue; 
      unsigned long memory = get_process_memory(pid);
      if (memory > max_memory) {
        max_memory = memory;
        max_pid = pid;
      }
    }
    else {
      continue; 
    }
  }

  closedir(proc_dir);

  if (max_pid) {
    fputs("Proceso con mayor uso de memoria encontrado:\n", file);
    snprintf(line, sizeof(line), "PID: %d\n", max_pid);
    fputs(line, file);
    snprintf(line, sizeof(line), "memoria usada: %lu KB (aprox. %.2f MB)\n\n", max_memory, max_memory/1024.0);
    fputs(line, file);
    return max_pid;

  } else {
      printf("No se encontraron procesos.\n");
      return -1; 
  }
}



void getPIDMaps(char* pidID, FILE* file) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%s/maps", pidID);
    FILE *f = fopen(path, "r");
    if (f) {
      char header[64];
      char line[256];
      snprintf(header, sizeof(header), "Librerias del proceso %s:\n", pidID);
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
        printf("El proceso no se encontró");
      }
};

void getPIDFiles(char* pidID, FILE* file) {
  char path[64];
  char header[128];
  snprintf(path, sizeof(path), "/proc/%s/fd", pidID);
  snprintf(header, sizeof(header), "\nArchivos abiertos por %s:\n", pidID);
  fputs(header, file);
  struct dirent *dir; 
  DIR *dr = opendir(path); 

  if (dr == NULL) {
    printf("\nNo se pudo abrir el directorio %s\n", path);
    return;
  }
  while ((dir = readdir(dr)) != NULL) {
  if (isdigit(dir->d_name[0])) {
    char linkpath[350];
    char target[350];
    snprintf(linkpath, sizeof(linkpath), "%s/%s", path, dir->d_name);
    readlink(linkpath, target, sizeof(target) - 1);
    char line[1024];
    snprintf(line, sizeof(line), "El archivo %s es: %s\n", dir->d_name, target);
    fputs(line, file);
  }  
  else {
    continue; 
  }
}
  closedir(dr);
};

int main(void) {

    struct sysinfo info;
    if (sysinfo(&info) != 0) {
      perror("sysinfo");
      return EXIT_FAILURE;
    }

    MemoryConsumeStats stats = memory_consume_info(&info);
    FILE *file = fopen("pid_info.txt", "w");
    if (!file) {
        perror("Error al crear el archivo de salida");
        return EXIT_FAILURE;
    }
    char line[256];
    fputs("Consumo de memoria:\n", file);
    snprintf(line, sizeof(line), "Memoria total: %lu bytes (aprox. %.2f Gb)\n", stats.total_memory, (stats.total_memory/1024.0)/1024.0/1024.0);
    fputs(line, file);
    snprintf(line, sizeof(line), "Memoria libre: %lu bytes (aprox. %.2f Gb)\n\n", stats.free_memory, (stats.free_memory/1024.0)/1024.0/1024.0);
    fputs(line, file);

    int max_pid = process_with_more_memory(file);

    char pidID[20];
    snprintf(pidID, sizeof(pidID), "%d", max_pid);

    getPIDMaps(pidID, file);
    getPIDFiles(pidID, file);
    fclose(file);
    return 0;
};

