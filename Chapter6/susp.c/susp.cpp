#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

#define THREAD_COUNT    20
#define ITERATIONS      40000

unsigned long thread_count = THREAD_COUNT;
unsigned long iterations = ITERATIONS;
pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t sentinel = 0;
pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_t *array = NULL, null_pthread = {0};
int bottom = 0;
int inited = 0;

void suspend_signal_handler(int sig)
{
    __sigset_t signal_set;

    sigfillset(&signal_set);
    sigdelset(&signal_set, SIGUSR2);
    sentinel = 1;
    sigsuspend(&signal_set);

    return;
}

void resume_signal_handler(int sig)
{
    return ;
}

void suspend_init_routine(void)
{
    int status;
    struct sigaction sigusr1, sigusr2;

    bottom = 10;
    array = (pthread_t*)calloc(bottom, sizeof(pthread_t));

    sigusr1.sa_flags = 0;
    sigusr1.sa_handler = suspend_signal_handler;
    sigemptyset(&sigusr1.sa_mask);
    sigaddset(&sigusr1.sa_mask, SIGUSR2);
    sigusr2.sa_flags = 0;
    sigusr2.sa_handler = resume_signal_handler;
    sigemptyset(&sigusr2.sa_mask);

    status = sigaction(SIGUSR1, &sigusr1, NULL);
    if(status == -1)
        errno_abort("Installing suspend handler");
    
    status = sigaction(SIGUSR2, &sigusr2, NULL);
    if(status == -1)
        errno_abort("Installing resume handler");

    inited = 1;
    return;
}

int thd_suspend(pthread_t target_thread)
{
    int status;
    int i = 0;

    status = pthread_once(&once, suspend_init_routine);
    if(status != 0)
        return status;

    status = pthread_mutex_lock(&mut);
    if(status != 0)
        return status;

    while(i < bottom)
        if(pthread_equal(array[i++], target_thread))
        {
            status = pthread_mutex_unlock(&mut);
            return status;
        }

    i = 0;
    while(i < bottom && array[i] != 0)
        i++;

    if(i == bottom)
    {
        array = (pthread_t*)realloc(array, (++bottom * sizeof(pthread_t)));
        if(array == NULL)
        {
            pthread_mutex_unlock(&mut);
            return errno;
        }

        array[bottom] = null_pthread;
    }

    sentinel = 0;
    status = pthread_kill(target_thread, SIGUSR1);
    if(status != 0)
    {
        pthread_mutex_unlock(&mut);
        return status;
    }

    while(sentinel == 0)
        sched_yield();

    array[i] = target_thread;

    status = pthread_mutex_unlock(&mut);
    return status;
}

int thd_continue(pthread_t target_thread)
{
    int status;
    int i = 0;

    status = pthread_once(&once, suspend_init_routine);
    if(status != 0)
        return status;

    status = pthread_mutex_lock(&mut);
    if(status != 0)
        return status;

    while (i < bottom && !pthread_equal(array[i], target_thread))
        i++;

    if(i == bottom)
    {
        pthread_mutex_unlock(&mut);
        return 0;
    }

    status = pthread_kill(target_thread, SIGUSR2);
    if(status != 0)
    {
        pthread_mutex_unlock(&mut);
        return status;
    }

    array[i] = 0;
    status = pthread_mutex_unlock(&mut);
    return status;
}

static void *thread_routine(void *arg)
{
    int number = *(int*)arg;
    delete (int*)arg;
    int status;
    int i;
    char buffer[128];

    for(i = 1; i <= iterations; i++)
    {
        if(i % 2000 == 0)
        {
            sprintf(buffer, "Thread %02d: %d\n", number, i);
            write(1, buffer, strlen(buffer));
        }

        sched_yield();
    }

    return (void*)0;
}

int main(int argc, char *argv[])
{
    pthread_t threads[THREAD_COUNT];
    pthread_attr_t detach;
    int status;
    void *result;
    int i;

    status = pthread_attr_init(&detach);
    if(status != 0)
        err_abort(status, "Init attributes object");
    status = pthread_attr_setdetachstate(&detach, PTHREAD_CREATE_DETACHED);
    if(status != 0)
        err_abort(status, "Set create-detached");

    for(i = 0; i < THREAD_COUNT; i++)
    {
        int *param = new int(i);
        status = pthread_create(&threads[i], &detach, thread_routine, (void*)param);
        if(status != 0)
            err_abort(status, "Create thread");
    }

    printf("Sleeping...\n");
    sleep(2);

    for(i = 0; i < THREAD_COUNT / 2; i++)
    {
        printf("Suspending thread %d.\n", i);
        status = thd_suspend(threads[i]);
        if(status != 0)
            err_abort(status, "Suspend thread");
    }

    printf("Sleeping...\n");
    sleep(2);

    for(i = 0; i < THREAD_COUNT / 2; i++)
    {
        printf("Continuing thread %d.\n", i);
        status = thd_continue(threads[i]);
        if(status != 0)
            err_abort(status, "Suspend thread");
    }

    for(i = THREAD_COUNT / 2; i < THREAD_COUNT; i++)
    {
        printf("Suspending thread %d.\n", i);
        status = thd_suspend(threads[i]);
        if(status != 0)
            err_abort(status, "Suspend thread");
    }

    printf("Sleeping...\n");
    sleep(2);

    for(i = THREAD_COUNT / 2; i < THREAD_COUNT; i++)
    {
        printf("Continuing thread %d.\n", i);
        status = thd_continue(threads[i]);
        if(status != 0)
            err_abort(status, "Continue thread");
    }

    pthread_exit(NULL);    
}
