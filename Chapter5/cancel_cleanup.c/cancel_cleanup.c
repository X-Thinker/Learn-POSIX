#include <pthread.h>
#include "errors.h"

#define THREADS 5

typedef struct control_tag
{
    int             counter, busy;
    pthread_mutex_t mutex;
    pthread_cond_t  cv;
} control_t;

control_t control = {0, 1, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

//取消清除处理函数
void cleanup_handler(void *arg)
{
    control_t *st = (control_t*)arg;
    int status;

    st->counter--;
    printf("cleanup_handler: counter == %d\n", st->counter);
    
    status = pthread_mutex_unlock(&st->mutex);
    if(status != 0)
        err_abort(status, "Unlock in cleanup handler");
}

void *thread_routine(void *arg)
{
    int status;

    pthread_cleanup_push(cleanup_handler, (void*)&control);

    status = pthread_mutex_lock(&control.mutex);
    if(status != 0)
        err_abort(status, "Mutex lock");
    control.counter++;

    //等待谓词busy为0（本例中不会发生）
    while(control.busy)
    {
        status = pthread_cond_wait(&control.cv, &control.mutex);
        if(status != 0)
            err_abort(status, "Wait on condition");
    }
    //设置非0值，即使线程没有被取消也会调用清除函数
    pthread_cleanup_pop(1);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thread_id[THREADS];
    int count;
    void *result;
    int status;

    for(count = 0; count < THREADS; count++)
    {
        status =pthread_create(&thread_id[count], NULL, thread_routine, NULL);
        if(status != 0)
            err_abort(status, "Create thread");
    }

    sleep(2);

    for(count = 0; count < THREADS; count++)
    {
        status = pthread_cancel(thread_id[count]);
        if(status != 0)
            err_abort(status, "Cancel thread");
        
        status = pthread_join(thread_id[count], &result);
        if(status != 0)
            err_abort(status, "Join thread");
        
        if(result == PTHREAD_CANCELED)
            printf("thread %d canceled\n", count);
        else 
            printf("thread %d was not canceled\n", count);
    }

    return 0;
}