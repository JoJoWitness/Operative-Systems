#include <windows.h>
#define SIZE 4096


typedef struct {
    char buffer[SIZE];
} shared_data;

int pipeFunction(int argc, char *argv[])  {

    const char* semaphore1 = "semaphore1";
    const char* semaphore2 = "semaphore2";
    const char* sm_name = "shared_memory_string";

    HANDLE hMapFile = CreateFileMapping(
    INVALID_HANDLE_VALUE, 
    NULL, 
    PAGE_READWRITE, 
    0, 
    sizeof(shared_data), 
    sm_name);

    shared_data* sm = (shared_data*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(shared_data));

    sm->buffer[0] = '\0';

    HANDLE hSem1 = CreateSemaphore(NULL, 0, 1, semaphore1);
    HANDLE hSem2 = CreateSemaphore(NULL, 0, 1, semaphore2);

    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);

    STARTUPINFO si1 = { sizeof(STARTUPINFO) }, si2 = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi1, pi2;
    char cmdLine1[256], cmdLine2[256];
    sprintf(cmdLine1, "\"%s\" p1 %llu", argv[0], (unsigned long long)hWritePipe);
    sprintf(cmdLine2, "\"%s\" p2 %llu", argv[0], (unsigned long long)hReadPipe);

    if (argc > 1 && strcmp(argv[1], "p1") == 0) {
        HANDLE hWritePipe = (HANDLE)_strtoui64(argv[2], NULL, 0);
        HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sm_name);
        shared_data* sm = (shared_data*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(shared_data));
        HANDLE hSem1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, semaphore1);
        HANDLE hSem2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, semaphore2);

        char input_str[100];
        printf("Enter any character string:\n");
        fflush(stdout);
        scanf("%s", input_str);

        DWORD written;
        WriteFile(hWritePipe, input_str, (DWORD)strlen(input_str) + 1, &written, NULL);
        CloseHandle(hWritePipe);

        ReleaseSemaphore(hSem1, 1, NULL);
        WaitForSingleObject(hSem2, INFINITE);

        printf("Modified string: %s\n", sm->buffer);
        fflush(stdout);

        UnmapViewOfFile(sm);
        CloseHandle(hMapFile);
        CloseHandle(hSem1);
        CloseHandle(hSem2);
        return 0;
    }

    if (argc > 1 && strcmp(argv[1], "p2") == 0) {
        HANDLE hReadPipe = (HANDLE)_strtoui64(argv[2], NULL, 0);
        HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sm_name);
        shared_data* sm = (shared_data*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(shared_data));
        HANDLE hSem1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, semaphore1);
        HANDLE hSem2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, semaphore2);

        WaitForSingleObject(hSem1, INFINITE);

        char str_toUpper[100] = {0};
        DWORD read;
        ReadFile(hReadPipe, str_toUpper, sizeof(str_toUpper) - 1, &read, NULL);
        str_toUpper[read] = '\0';
        CloseHandle(hReadPipe);

        for(int i=0;str_toUpper[i];i++){
            str_toUpper[i] =toupper(str_toUpper[i]);
        }

        char* write_ptr = sm->buffer;
        sprintf(write_ptr, "%s", str_toUpper);    

        ReleaseSemaphore(hSem2, 1, NULL); 
        UnmapViewOfFile(sm);
        CloseHandle(hMapFile);
        CloseHandle(hSem1);
        CloseHandle(hSem2);
        return 0;
    }    

    CreateProcess(NULL, cmdLine1, NULL, NULL, TRUE, 0, NULL, NULL, &si1, &pi1);
    CreateProcess(NULL, cmdLine2, NULL, NULL, TRUE, 0, NULL, NULL, &si2, &pi2);

    CloseHandle(hWritePipe);
    CloseHandle(hReadPipe);

    HANDLE processes[] = { pi1.hProcess, pi2.hProcess };
    WaitForMultipleObjects(2, processes, TRUE, INFINITE);

    printf("Final string in shared memory: %s\n", sm->buffer);

    UnmapViewOfFile(sm);
    CloseHandle(hMapFile);
    CloseHandle(hSem1);
    CloseHandle(hSem2);
    CloseHandle(pi1.hProcess); CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess); CloseHandle(pi2.hThread);

    printf("\nPress any key to exit...\n");
    getchar();

    return 0;
}