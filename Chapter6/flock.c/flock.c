#include <pthread.h>
#include <stdio.h>
#include "errors.h"

void *prompt_routine(void *arg)
{
    char *prompt = (char*)arg;
    char *string;
    int len;

    string = (char*)malloc(128);
    if(string == NULL)
        errno_abort("Alloc string");

    flockfile(stdin);
    flockfile(stdout);

    printf(prompt);

    if(fgets(string, 128, stdin) == NULL)
        string[0] = '\0';
    else 
    {
        len = strlen(string);
        if(len > 0 && string[len - 1] == '\n')
            string[len - 1] = '\0';
    }

    funlockfile(stdout);
    funlockfile(stdin);

    return (void*)string;
}

int main(int argc, char *argv[])
{
    pthread_t thread1, thread2, thread3;
    void *string;
    int status;

#ifdef sun
    DPRINTF(("Setting concurrency level to 4\n"));
    thr_setconcurrency(4);
#endif

    status = pthread_create(&thread1, NULL, prompt_routine, "Thread 1>");
    if(status != 0)
        err_abort(status, "Create thread 1");
    status = pthread_create(&thread2, NULL, prompt_routine, "Thread 2>");
    if(status != 0)
        err_abort(status, "Create thread 2");
    status = pthread_create(&thread3, NULL, prompt_routine, "Thread 3>");
    if(status != 0)
        err_abort(status, "Create thread 3");

    status = pthread_join(thread1, &string);
    if(status != 0)
        err_abort(status, "Join thread 1");
    printf("Thread 1: \"%s\"\n", (char*)string);
    free(string);
    status = pthread_join(thread2, &string);
    if(status != 0)
        err_abort(status, "Join thread 2");
    printf("Thread 2: \"%s\"\n", (char*)string);
    free(string);
    status = pthread_join(thread3, &string);
    if(status != 0)
        err_abort(status, "Join thread 3");
    printf("Thread 3: \"%s\"\n", (char*)string);
    free(string);

    return 0;
}