#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include "errors.h"

timer_t timer_id;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int counter = 0;

void timer_thread(__sigval_t arg)
{
    int status;

    status = pthread_mutex_lock(&mutex);
    if(status != 0)
        err_abort(status, "Lock mutex");
    if(++counter >= 5)
    {
        status = pthread_cond_signal(&cond);
        if(status != 0)
            err_abort(status, "Signal condition");
    }
    status = pthread_mutex_unlock(&mutex);
    if(status != 0)
        err_abort(status, "Unlock mutex");
    printf("Timer %d\n", counter);
}

int main(int argc, char *argv[])
{
    int status;
    struct itimerspec ts;
    struct sigevent se;

#ifdef sun
    fprintf(stderr, "This program cannot compile on Solaris 2.5.\n"
    "To build and run on Solaris 2.6, remove the\n"
    "\"#ifdef sun\" block in main().\n");
#else
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &timer_id;
    se.sigev_notify_function = timer_thread;
    se.sigev_notify_attributes = NULL;

    ts.it_value.tv_sec = 5;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 5;
    ts.it_interval.tv_nsec = 0;

    DPRINTF(("Creating timer\n"));
    status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if(status == -1)
        errno_abort("Create timer");

    DPRINTF(("Setting timer %d for 5-second expiration...\n", timer_id));
    status = timer_settime(timer_id, 0, &ts, 0);
    if(status == -1)
        errno_abort("Set timer");

    status = pthread_mutex_lock(&mutex);
    if(status != 0)
        err_abort(status, "Lock mutex");
    while(counter < 5)
    {
        status = pthread_cond_wait(&cond, &mutex);
        if(status != 0)
            err_abort(status, "Wait on condition");
    }
    status = pthread_mutex_unlock(&mutex);
    if(status != 0)
        err_abort(status, "Unlock mutex");
#endif
    return 0;
}