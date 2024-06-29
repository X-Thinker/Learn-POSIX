#include <pthread.h>
#define DEBUG
#include "errors.h"

static int counter;

void *thread_routine(void *arg)
{
    DPRINTF(("thread_routine starting\n"));
    for(counter = 0; ; counter++)
        if((counter % 1000) == 0)
        {
            DPRINTF(("calling testcancel at iteration %d\n", counter));
            pthread_testcancel();
        }
}

int main(int argc, char *argv[])
{
    pthread_t thread_id;
    void *result;
    int status;

#ifdef sun
    DPRINTF(("Setting concurrency level to 2\n"));
    thr_setconcurrency(2);
#endif

    status = pthread_create(&thread_id, NULL, thread_routine, NULL);
    if(status != 0)
        err_abort(status, "Create thread");
    sleep(2);

    DPRINTF(("calling cancel\n"));
    status = pthread_cancel(thread_id);
    if(status != 0)
        err_abort(status, "Cancel thread");
    
    DPRINTF(("calling join\n"));
    status = pthread_join(thread_id, &result);
    if(status != 0)
        err_abort(status, "Join thread");
        
    if(result == PTHREAD_CANCELED)
        printf("Thread canceled at iteration %d\n", counter);
    else 
        printf("Thread was not cancel\n");

    return 0;

}