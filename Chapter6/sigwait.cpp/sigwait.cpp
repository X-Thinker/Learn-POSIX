#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "errors.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int interrupted = 0;
__sigset_t signal_set;

void *signal_waiter(void *arg)
{
    int sig_number;
    int signal_count = 0;
    int status;

    while(1)
    {
        sigwait(&signal_set, &sig_number);
        if(sig_number == SIGINT)
        {
            printf("Got SIGINT (%d of 5)\n", signal_count + 1);
            if(++signal_count >= 5)
            {
                status = pthread_mutex_lock(&mutex);
                if(status != 0)
                    err_abort(status, "Lock mutex");
                interrupted = 1;
                status = pthread_cond_signal(&cond);
                if(status != 0)
                    err_abort(status, "Signal condition");
                status = pthread_mutex_unlock(&mutex);
                if(status != 0)
                    err_abort(status, "Unlock mutex");
                break;
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t signal_thread_id;
    int status;

    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGINT);
    status = pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
    if(status != 0)
        err_abort(status, "Set signal mask");

    status = pthread_create(&signal_thread_id, NULL, signal_waiter, NULL);
    if(status != 0)
        err_abort(status, "Create sigwaiter");

    status = pthread_mutex_lock(&mutex);
    if(status != 0)
        err_abort(status, "Lock mutex");
    while(!interrupted)
    {
        status = pthread_cond_wait(&cond, &mutex);
        if(status != 0)
            err_abort(status, "Wait for interrupt");
    }
    status = pthread_mutex_unlock(&mutex);
    if(status != 0)
        err_abort(status, "Unlock mutex");
    printf("Main terminating with SIGINT\n");
    return 0;
}